# AGENTS.md — SmartLock_v2

项目结构约束与编码规范。编码行为准则见 GUIDELINES.md，领域术语定义见 CONTEXT.md，架构决策记录见 docs/adr/。

## 0. CubeMX 生成代码——AI 禁止修改（最高优先级）

> ⛔ **硬性禁令**：AI 不得以任何理由修改 CubeMX 生成的任何文件。

### CubeMX 生成文件清单（.ioc 重生成后逐项核对）

| 文件/目录 | 说明 |
|---|---|
| `SmartLock.ioc` | CubeMX 项目配置文件 |
| `.mxproject` | CubeMX 项目元数据 |
| `startup_stm32f407xx.s` | 启动汇编 |
| `STM32F407VETx_FLASH.ld` | 链接脚本 |
| `Core/Inc/main.h` | 外设句柄 extern 声明 |
| `Core/Inc/stm32f4xx_hal_conf.h` | HAL 模块开关 |
| `Core/Inc/stm32f4xx_it.h` | 中断处理函数声明 |
| `Core/Inc/FreeRTOSConfig.h` | FreeRTOS 配置 |
| `Core/Src/main.c` | 主函数入口（含外设初始化） |
| `Core/Src/freertos.c` | FreeRTOS 内核初始化 |
| `Core/Src/stm32f4xx_hal_msp.c` | HAL 外设 MSP 初始化 |
| `Core/Src/stm32f4xx_it.c` | 中断服务函数 |
| `Core/Src/system_stm32f4xx.c` | 系统时钟初始化 |
| `Drivers/CMSIS/` | CMSIS 核心头文件 |
| `Drivers/STM32F4xx_HAL_Driver/` | HAL 库全部 .c/.h |
| `Middlewares/` | FreeRTOS 源码（CubeMX 拷贝） |

### 参数调整流程

1. AI 发现需要调整外设参数（时钟、引脚、中断优先级、DMA 配置等）→ 向用户说明原因和影响
2. 用户确认后，在 CubeMX 中修改 `SmartLock.ioc` 配置
3. 用户重新生成代码
4. AI 执行 §8 的 6 步重生成后强制检查，合并手写代码（仅限 `App/`、`Drivers/Inc/`、`Drivers/Src/`）

### 唯一例外

`Core/Src/main.c` 中 `app_init()` 的调用位置——在 `MX_FREERTOS_Init()` 之后、`vTaskStartScheduler()` 之前。若 CubeMX 重生成覆盖了此调用，AI 可补回——但仅限这一行。

---
## 0.5 目录领土划分——CubeMX 与 AI 代码物理隔离

> **原则**：AI 代码与 CubeMX 代码共用目录时，精确到子目录级别的领土边界。

### 领土划分

| 目录 | 归属 | 规则 |
|---|---|---|
| `Core/` | ⛔ **CubeMX 全部领土** | AI 只读，不新增/修改任何文件 |
| `Drivers/CMSIS/` | ⛔ **CubeMX** | HAL 库依赖，AI 不触碰 |
| `Drivers/STM32F4xx_HAL_Driver/` | ⛔ **CubeMX** | HAL 库本体，AI 不触碰 |
| `Drivers/Inc/`、`Drivers/Src/` | ✅ **AI** | 手写驱动：`drv_as608`、`drv_rc522`、`drv_keypad`、`drv_oled`、`drv_esp8266`、`drv_relay`、`drv_buzzer`、`drv_w25q64` |
| `App/` | ✅ **AI** | 业务任务：`task_*.c` + `app_init.c` + 头文件（`app_*.h`） |

### 为什么不会冲突

- CubeMX 重生成覆盖 `Core/` 全部 + 可能更新 `Drivers/CMSIS/` 和 `Drivers/STM32F4xx_HAL_Driver/`
- `Drivers/Inc/` 和 `Drivers/Src/` 不在 CubeMX 管辖范围——手写驱动文件不会被删除或覆盖
- `App/` 完全独立，通过 CubeMX Include Paths 加入编译

### 唯一桥接点

`Core/Src/main.c` 中 `app_init()` 调用。CubeMX 重生成覆盖后 AI 可补回（仅限这一行）。

---

## 1. 目录结构

```
SmartLock_v2/
├── Core/          ← CubeMX 独占，禁止手改。重生成后合并应用代码
├── App/           ← 手写应用层（task_*.c/h + app_*.h）
├── Drivers/       ← 手写驱动层（drv_*.c/h）
└── Docs/
```

## 2. 分层规则

- 应用层 → 驱动层 → HAL 层，只能从上往下调
- 任务间只用 FreeRTOS IPC（Queue / Semaphore / EventGroup），禁用全局变量跨任务传数据
- App 层禁止 include stm32f4xx_hal.h
- App/Inc/ 下的 app_*.h 按职责严格拆分：

| 文件 | 内容 | 依赖 |
|---|---|---|
| app_config.h | 系统宏（MAX_USER_COUNT、栈大小、优先级等） | stdint.h |
| app_types.h | 业务枚举和结构体（user_record_t、auth_request_t 等） | stdint.h + stdbool.h |
| app_ipc.h | IPC 对象 extern 声明 + 事件掩码位 | FreeRTOS.h |
| app_init.h | `void app_init(void)` | 无 |

## 3. 全局变量使用管控

- **允许：** 只读状态标志（g_sys_state、lockout_until、g_boot_time）
- **禁止：** 业务数据、指纹缓存、日志缓存放全局变量——一律 IPC 队列或任务私有 static
- **禁止：** 中断与任务同时读写同一全局变量。必须共享时配套互斥信号量保护

## 4. 硬件状态传递规范

App 层需要区分硬件故障类型时，由 Drivers 层在自身头文件内提供纯标准 C 的故障枚举（不依赖 HAL）。App 层只 include 该驱动头文件，不触碰 HAL。禁止 App 层直接使用 HAL 状态码。

## 5. 驱动层约束

- 通信类操作返回 int（0 成功，负数错误码）。每驱动在自身头文件定义私有错误码枚举，不强求全局统一
- 驱动内阻塞轮询统一上限 10ms。长等待交由 App 层用 vTaskDelay
- 不调 FreeRTOS API（Timer 回调除外）
- 外设句柄用 static 全局变量持有
- 新增驱动不修改 app_config.h、app_types.h、app_ipc.h 中已有内容

详见 `Drivers/AGENTS.md`。

## 6. FreeRTOS 栈溢出防护

1. 所有任务栈深度预留 20% 冗余（如测算需要 400w → 配置 512w）
2. 开发阶段开启 `configCHECK_FOR_STACK_OVERFLOW`（设为 2）
3. Supervisor 上电后打印各任务剩余栈空间（`uxTaskGetStackHighWaterMark`），日志留存
4. 禁止在任务内定义 ≥128 字节的局部数组——放到堆或全局静态区

## 7. app_init() 入口约束

- **只做纯软件动作：** IPC 创建 + xTaskCreate
- **禁止外设初始化**（外设 init 放各任务入口——任务可自行 vTaskDelay 处理超时，避免卡住调度器启动）
- 末尾时钟频率自检：`if (HAL_RCC_GetSysClockFreq() != 168000000)` → 蜂鸣器长响 → **死循环停机**，拒绝进入正常运行

## 8. CubeMX 重生成后强制操作

每次 `.ioc` 重新生成代码后，必须执行：

1. 检查 Core/Src/main.c 中 `app_init()` 是否存在
2. 确认调用位置在 `MX_FREERTOS_Init()` 之后、`vTaskStartScheduler()` 之前
3. 若被覆盖丢失，立即补回
4. 核对 HSE 晶振配置与硬件丝印一致
5. 核对 APB1/APB2 分频正确，总线时钟不超限
6. 核对 PLL 输出严格 168MHz

## 9. 中断优先级

- NVIC PriorityGroup = 4（4 位抢占，0 位子优先级）
- configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5
- 抢占优先级 < 5 的中断禁止调用 RTOS ISR API
- SystemClock_Config() 由 CubeMX 生成，不自写

完整中断优先级表见 `ARCHITECTURE.md` 第八章。

### 运行时校验

Supervisor 任务**启动后仅执行一次**：读取 NVIC 分组配置和各中断抢占优先级，与标准表比对。任一不匹配 → 记录日志 → 蜂鸣器告警。

## 10. 命名规范

- 文件: `drv_xxx`（驱动）、`task_xxx`（任务）
- 函数: `模块_动作()`，如 `as608_init()`、`relay_unlock()`
- 常量: 全大写_下划线，如 `MAX_USER_COUNT`
- 注释: 中文，可读性优先。解释"为什么"优先于"做了什么"

## 11. CubeMX 与 HAL

- 改外设通过 CubeMX 编辑 `.ioc`，生成后合并应用代码
- 禁止手动修改 `stm32f4xx_hal_*.c` 和 `freertos.c`
- HAL 句柄名禁止改名

## 12. Supervisor 兜底校验（按频率分离）

| 校验项 | 频率 | 说明 |
|---|---|---|
| 锁定超时解除 | 每 1s | 检查 `STATE_LOCKED_OUT` 超时后强制恢复 IDLE |
| Flash 状态检测 | 每 30s | W25Q64 读状态寄存器，异常则告警 |
| NVIC 中断优先级校验 | 仅上电 1 次 | 与标准表比对，不匹配告警 |

兜底范围原则：**只覆盖影响基础开锁功能的阻塞逻辑**。WiFi 离线（非阻塞）、按键卡死（硬件层）不在范围内。

## 13. 日志与存储约束

### Flash 持久化结构体

- 第一个成员固定 `uint32_t data_ver;`
- 版本号变更规则：业务字段增删、类型修改 → 升级；仅注释调整不升级
- 读取到不匹配版本 → 清空对应分区，打印告警，不兼容旧数据

### Flash 写入

- 每次页写入后必须回读校验
- 失败重试 1 次，仍失败 → 记录故障、跳过当前页

### 串口调试

- 分四级：DEBUG / INFO / WARN / ERROR，编译宏 `LOG_DEBUG_EN` 开关 DEBUG
- 上线版本强制关闭 DEBUG
- **安全脱敏：** 禁止打印用户指纹 ID、RFID 卡号、开锁记录明文
- **中断上下文禁止调用 printf**

## 14. 禁止操作清单

- 中断上下文调用 vTaskDelay 等阻塞 API
- App 层 include stm32f4xx_hal.h
- app_init() 中做外设初始化
- 顺手统一格式化
- 中断与任务同时读写同一全局变量（无信号量保护时）
- 任务内定义 ≥128 字节局部数组
- 串口打印指纹/卡号/密码等敏感信息
- 中断上下文调用 printf
- 手动修改 stm32f4xx_hal_*.c 和 freertos.c

## 15. 自我纠错与规则溯源

同类错误 2 次必须更新本文档加规则。已有规则能覆盖的不重复加。一次修改禁止连续追加超过 3 条新规则。

新增规则时附带触发场景：`// 触发: 具体故障场景`

---

## 变更记录

| 日期 | 新增/变更 | 触发场景 |
|---|---|---|
| 2026-06-19 | 初始版本 | 初代 SmartLock 返工 → v2 架构重建 |
| 2026-06-20 | 歧义判定标准、紧急 bug 例外 | 需求模糊时主观判定、门锁阻断 bug 延误 |
| 2026-06-20 | 驱动错误码私有枚举、短超时统一 10ms | 多人开发返回值混乱 |
| 2026-06-20 | 禁止顺手格式化、验收失败流程 | 无意义 diff、无回滚规范 |
| 2026-06-20 | 3 文件例外规则 | 公共头文件联动误判 |
| 2026-06-20 | 硬件状态传递规范 | App 层为取 HAL 状态码直接 include HAL |
| 2026-06-20 | 时钟自检停机、CubeMX 重生成还原流程 | 时钟异常仅告警不阻止、遗漏 app_init() |
| 2026-06-20 | 中断优先级运行时校验 | CubeMX 重生成后优先级被默认值覆盖 |
| 2026-06-20 | Supervisor Flash 检测 + 校验频率分离 | Flash 故障未检测、1s 周期过重 |
| 2026-06-20 | 存储 version 字段规范 | 跨版本 Flash 结构体无兼容逻辑 |
| 2026-06-20 | FreeRTOS 栈溢出防护 | 多任务栈溢出死机无检测 |
| 2026-06-20 | 全局变量使用管控 | 中断与任务竞态读写 |
| 2026-06-20 | 串口调试分级 + 安全脱敏 | 上线遗留调试打印、泄露敏感信息 |
| 2026-06-20 | 低电压供电异常边缘案例 | 电池欠压导致外设通信断续 |
| 2026-06-20 | 拆分为 GUIDELINES.md + AGENTS.md 双文档 | 行为准则与项目约束耦合，不便复用`n| 2026-06-20 | 任务崩溃与硬故障处理 | 任务卡死后无处理规范 |`n| 2026-06-20 | `.ioc` 版本控制规范 | `.ioc` 是否纳入版本控制未说明 |`n| 2026-06-20 | `data_ver` 命名统一 + App/AGENTS.md 去重 | 多文档命名不一致、表格重复维护 |


## 16. 代码生成后自检清单（逐条核对）

> 代码提交或 CubeMX 重生成合并后，逐条对照本清单验证。全部通过方可认为本次改动合格。

| # | 检查项 | 关联条款 |
|:--:|------|:--:|
| 1 | 未修改 CubeMX 生成的文件（Core/、SmartLock.ioc）？仅有 `app_init()` 调用被补回？ | §0 |
| 2 | App 层未 include `stm32f4xx_hal.h`？ | §2 |
| 3 | 任务间无全局变量共享业务数据，IPC 已正确使用（Queue/Semaphore/EventGroup）？ | §2, §3 |
| 4 | App 层取硬件状态使用驱动故障枚举，未直接使用 HAL 状态码？ | §4 |
| 5 | 驱动层无 FreeRTOS API 调用（Timer 回调除外）？ | §5 |
| 6 | 驱动层阻塞轮询 ≤10ms？ | §5 |
| 7 | 各任务栈深度有 20% 冗余（如测算 400w → 配置 512w），无 ≥128 字节局部数组？ | §6 |
| 8 | `app_init()` 仅做 IPC 创建 + `xTaskCreate` + 时钟自检，无外设初始化？ | §7 |
| 9 | CubeMX 重生成后 `app_init()` 未丢失、在 `MX_FREERTOS_Init()` 之后 `vTaskStartScheduler()` 之前调用？ | §8 |
| 10 | 中断优先级配置与标准表一致？抢占优先级 <5 的中断未调用 RTOS ISR API？ | §9 |
| 11 | 命名符合 `drv_xxx` / `task_xxx` / 全大写常量规范？ | §10 |
| 12 | Flash 持久化结构体首成员为 `uint32_t data_ver`，写入后已回读校验？ | §13 |
| 13 | 未打印指纹 ID、RFID 卡号、密码等敏感信息？中断上下文未调用 `printf`？ | §13, §14 |
| 14 | 未手动修改 `stm32f4xx_hal_*.c` 和 `freertos.c`？ | §11 |
| 15 | `app_config.h` / `app_types.h` / `app_ipc.h` 之间职责未混淆，依赖最小？ | ADR-0001 |

> **使用说明**：本清单对应 `verification-before-completion` skill。写完代码后 invoke 该 skill 逐条核验。


## 17. Skill 路由表

> 编码哲学见 [GUIDELINES.md](../GUIDELINES.md)；本表负责将具体任务场景路由到对应 skill。
> 生效闭环：GUIDELINES.md（哲学 + 嵌入式场景）→ 本表（调度）→ skill（执行）→ §16 自检清单（验证）

> Agent 进入项目时，根据任务类型自动匹配对应 skill。本表仅列出高频场景；未覆盖场景仍可通过 skill 描述自动发现。

### 动手前（设计、规划、决策）

| 触发场景 | Skill | 理由 |
|---|---|---|
| "开始返工"、"下一步做什么" | `writing-plans` | CubeMX 建 .ioc → 生成 HAL → 合入代码 → 验证 |
| "v1→v2 迁移策略怎么定" | `request-refactor-plan` | 访谈式确认迁移步骤，安全增量提交 |
| "drv_w25q64 的接口怎么设计"、"App/Inc 四个文件怎么拆"、"驱动错误码枚举怎么定" | `codebase-design` | deep module 方法论，对齐 ARCHITECTURE.md 的分层与接口设计 |
| "Flash 分区方案"、"版本号方案"、"日志结构怎么定" | `decision-mapping` | 多个存储架构决策点，逐个追踪到结论 |

### 动手时（编码、实现、拆分）

| 触发场景 | Skill | 理由 |
|---|---|---|
| "写 drv_as608.c"、"实现 task_auth" | `tdd` | 驱动层返回错误码、Flash 写入后回读校验——天然适合 TDD |
| "把迁移计划拆成独立子任务" | `to-issues` | 拆分：CubeMX 搭建、驱动移植、任务创建、Supervisor 校验等分步 issue |
| "按这个 issue 实现" | `implement` | 按 PRD/issue 驱动实现 |
| "用户/指纹/RFID 这些类型怎么定义" | `domain-modeling` | user_record_t、auth_request_t 等业务类型统一 |

### 动手后（验证、审查、合入）

| 触发场景 | Skill | 理由 |
|---|---|---|
| "CubeMX 重生成完了，检查"、"代码写完了，自查" | `verification-before-completion` | 逐条跑 AGENTS.md §16 自检清单（含 §8 的 CubeMX 重生成检查） |
| "review 这段代码" | `review` | 双轴审查：标准合规（AGENTS.md 规则）+ 规格匹配（ARCHITECTURE.md 设计） |
| "准备合入" | `finishing-a-development-branch` | 合并策略决策 |

### 出问题时（调试、排障）

| 触发场景 | Skill | 理由 |
|---|---|---|
| "指纹认证超时"、"Flash 读写异常"、"WiFi MQTT 断连"、"栈溢出"、"中断优先级错配" | `diagnosing-bugs` | 结构化诊断——AGENTS.md 有 20+ 条规则，每条背后都是踩过的坑 |
| 备选 | `systematic-debugging` | 全局已安装，与 diagnosing-bugs 互为 backup |

> **注意**：`grill-me`、`grilling`、`handoff`、`triage`、`prototype` 等低频/不适用 skill 未列入路由表，但 agent 仍可通过名称和描述自动匹配。