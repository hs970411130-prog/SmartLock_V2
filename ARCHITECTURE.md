# SmartLock_v2 架构设计文档

> STM32F407 + FreeRTOS 智能门锁 · 最终确定版
> 对应项目：D:\Users\hesheng\Desktop\SmartLock_v2
> 原始备份：D:\Users\hesheng\Desktop\SmartLock（不动）
> 术语定义见 [CONTEXT.md](../CONTEXT.md)；架构决策记录见 [docs/adr/](adr/)

---

## 一、标准化工程目录结构

```
SmartLock_v2/
├── SmartLock.ioc              # CubeMX 配置源文件
├── Core/                      # CubeMX 独占层：HAL、FreeRTOS、main 入口
│   ├── Inc/                   # FreeRTOSConfig.h, stm32f4xx_hal_conf.h, main.h ...
│   └── Src/                   # stm32f4xx_hal_*.c, freertos.c, main.c
├── App/                       # 手写应用层，与生成代码物理隔离，重生成不覆盖
│   ├── Inc/                   # app_config.h, app_types.h, app_ipc.h, app_init.h
│   │                          # + task_auth.h, task_cloud.h, task_logger.h
│   │                          # + task_display.h, task_keypad.h, task_supervisor.h
│   └── Src/                   # app_init.c + task_*.c
├── Drivers/                   # 手写驱动层，结构不变
│   ├── Inc/                   # drv_as608.h, drv_rc522.h, drv_keypad.h,
│   │                          # drv_oled.h, drv_esp8266.h, drv_relay.h,
│   │                          # drv_buzzer.h, drv_w25q64.h
│   └── Src/                   # drv_*.c
├── Docs/
├── README.md
└── AGENTS.md
```

**分层调用规则：** App → Drivers → HAL（CubeMX），仅允许单向调用。驱动层不使用 FreeRTOS API（FreeRTOS Timer 回调除外）。

---

## 二、main.h 解耦拆分

原 `main.h` 承担 4 种职责耦合过重，拆分后各归其位：

| 原内容 | 去向 | 维护方 |
|---|---|---|
| HAL 外设句柄 extern 声明 | `Core/Inc/main.h` | CubeMX |
| 系统宏配置（MAX_USER_COUNT 等） | `App/Inc/app_config.h` | 手写 |
| 业务类型定义（typedef enum/struct） | `App/Inc/app_types.h` | 手写 |
| IPC 对象声明 + 事件掩码位 | `App/Inc/app_ipc.h` | 手写 |

**收益：** CubeMX 重生成覆盖 main.h 时，业务代码完全不受影响。

---

## 三、App 层与 CubeMX main() 集成入口

`App/Inc/app_init.h` 对外暴露唯一接口：

```c
void app_init(void);
```

### app_init() 职责（仅纯软件动作）

- 创建所有 Queue / Semaphore / EventGroup
- 创建 6 个业务任务（Auth/Cloud/Logger/Display/Keypad/Supervisor）
- 运行时时钟频率自检（见下文）

**禁止**在 `app_init()` 中做外设初始化（`as608_init` 等）。原因：外设可能不在线导致初始化超时，此时调度器未启动没有超时保护。外设初始化放在各自任务的入口第一行。

### CubeMX 调用位置（必须精确）

```c
HAL_Init();
SystemClock_Config();
MX_GPIO_Init();
MX_USART1_UART_Init();   // ... 等所有外设初始化
MX_FREERTOS_Init();       // FreeRTOS 内核就绪
app_init();               // ← 在此调用，调度器未启动
vTaskStartScheduler();    // 调度器启动
while(1);
```

> 约束：`app_init()` 必须在 `MX_FREERTOS_Init()` 之后、`vTaskStartScheduler()` 之前。

### 运行时时钟自检（兜底安全机制）

不自动适配晶振频率（依赖硬件确认），但做自检报警：

```c
// app_init() 末尾
if (HAL_RCC_GetSysClockFreq() != 168000000) {
    // 时钟异常：记录日志，蜂鸣器长响，拒绝进入正常运行
    // 避免在错误的时钟下操作外设导致不可预期的行为
}
```

> 设计原则：软件不猜测硬件，晶振频率以原理图/丝印为准。自检只做异常报警，不做自动适配。

---

## 四、业务类型头文件依赖约束

`App/Inc/app_types.h` 仅允许引入标准 C 头文件：

```c
#include <stdint.h>
#include <stdbool.h>
```

- 禁止引入 `stm32f4xx_hal.h` 或任何 HAL 头文件
- 所有业务结构体（`user_record_t`、`auth_request_t`、`log_entry_t` 等）只使用 `uint` 标准类型
- 外设句柄封死驱动层，不向应用层泄露

---

## 五、硬件外设引脚分配

| 外设 | 总线 | 引脚 |
|---|---|---|
| AS608 指纹 | USART1 | PA9(TX) / PA10(RX) |
| ESP8266 WiFi | USART2 | PA2(TX) / PA3(RX) |
| 调试串口 | USART3 | PB10(TX) / PB11(RX) |
| RC522 RFID | SPI1 | PA5(SCK) PA6(MISO) PA7(MOSI) / CS=PA4 |
| W25Q64 Flash | SPI2 | PB13(SCK) PB14(MISO) PB15(MOSI) / CS=PB12 |
| OLED 128x64 | I2C1 | PB6(SCL) / PB7(SDA) |
| 继电器 | GPIO | PE0（低电平开锁） |
| 蜂鸣器 | GPIO | PE1（高电平发声） |
| 矩阵键盘 4x4 | GPIO | ROW: PC0-3, COL: PC4-7 |

---

## 六、时钟树基线

| 参数 | 值 |
|---|---|
| HSE 晶振 | 8MHz（⚠ 必须从原理图或丝印确认，不要靠软件猜测） |
| SYSCLK | 168MHz（HSE → PLL → 168MHz） |
| APB2 总线 | 84MHz（预分频 2），挂载 SPI1、USART1 |
| APB1 总线 | 42MHz（预分频 4），挂载 SPI2、USART2/3、I2C1 |

---

## 七、FreeRTOS 核心参数

| 配置项 | 值 | 说明 |
|---|---|---|
| 总堆大小 | 20480 (20KB) | 6 任务栈约 6KB + IPC 开销 |
| Tick 频率 | 1000Hz | 毫秒级超时 |
| 最大优先级 | 8 | 使用 6 级，预留 2 级 |
| 空闲任务栈 | 128 words | 默认值 |

---

## 八、中断优先级分配（⚠ 关键）

### NVIC 优先级分组选择：Group 4（4 位抢占，0 位子优先级）

选择理由：

1. **FreeRTOS 只需要抢占优先级。** `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` 和 `taskENTER_CRITICAL` 的关中断掩码只认抢占优先级，子优先级在 FreeRTOS 临界区保护面前无实际作用。
2. **本项目无子优先级需求。** 中断表中没有任何两个中断共用同一抢占优先级，子优先级排序功能用不上。
3. **Group 4 最简单，最不容易出错。**

### FreeRTOS 参数（`FreeRTOSConfig.h`）

```c
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
```

> 规则：抢占优先级 < 5 的中断，禁止调用任何 RTOS ISR API（`xSemaphoreGiveFromISR` 等）。

### 中断优先级表

| 中断源 | 抢占优先级 | 说明 |
|---|---|---|
| 独立看门狗 IWDG | 0 | 最高优先级，脱离 RTOS 管辖，系统卡死强制复位 |
| USART1 RX（指纹） | 7 | 可调用 RTOS ISR API |
| USART2 RX（WiFi） | 7 | 可调用 RTOS ISR API |
| SPI1 DMA（RFID） | 8 | 传输量小，实时性要求中等 |
| SPI2 DMA（Flash） | 8 | 大块数据传输，频繁触发，不宜过高 |
| USART3 RX（调试） | 8 | 实时要求最低 |
| I2C1（OLED） | 9 | 屏幕刷新，允许较大延迟 |
| SysTick | 15 | FreeRTOS 心跳基准 |

> ⚠ CubeMX 生成后必须对照此表检查 NVIC 配置和 FreeRTOSConfig.h。优先级不匹配是 STM32 + FreeRTOS 项目最频繁的死机原因。

---

## 九、6 个 FreeRTOS 任务

| 优先级 | 任务 | 文件 | 栈 | 周期 | 职责 |
|---|---|---|---|---|---|
| 5 | Auth | task_auth.c | 2048w | 50ms | 指纹/RFID/密码认证、开锁、5 次失败触发锁定 |
| 4 | Cloud | task_cloud.c | 1024w | 事件驱动 | WiFi + MQTT 远程开锁/报警（当前 ENABLE_WIFI=0） |
| 3 | Logger | task_logger.c | 1024w | 事件驱动 | 日志写入 W25Q64、日志查询 |
| 2 | Display | task_display.c | 512w | 事件驱动 | OLED 页面渲染 |
| 1 | Keypad | task_keypad.c | 512w | 50ms | 键盘扫描去抖、密码输入 |
| 0 | Supervisor | task_supervisor.c | 256w | 1s | 看门狗喂狗、锁定兜底解除、系统监控 |

### Supervisor 锁定兜底机制


### 模块深度评估（codebase-design 词汇）

> **Deep module** = 小接口 + 深实现（调用方 leverage 高，维护方 locality 好）。
> **Shallow module** = 大接口 + 薄实现（透传为主，删除后复杂度不集中）。

| 模块 | 深度 | 评估依据 |
|---|---|---|
| `task_auth.c` | **Deep** | 接口仅 auth_request → auth_result；内部封装指纹比对、RFID 查表、密码验证、锁定状态机——认证逻辑高度内聚 |
| `drv_w25q64.c` | **Deep** | SPI Flash 操作封装为 read/write/erase 三接口；内部隐藏页对齐、回读校验、坏块跳过 |
| `drv_as608.c` | **Deep** | 指纹模块 UART 协议完全封装；上层只调 enroll/verify/delete，无需了解指令包格式 |
| `drv_rc522.c` | **Deep** | RFID SPI 寄存器操作封装；上层只读卡号 UID |
| `drv_esp8266.c` | **Medium** | AT 指令集接口较宽（connect/send/subscribe...），但内部封装了重连和超时 |
| `drv_oled.c` | **Deep** | I2C 显存操作封装为打点/字符串接口；上层只关心"显示什么" |
| `task_logger.c` | **Medium** | 接口为 log_write/log_query；实现含 Flash 循环写入和日志检索 |
| `task_keypad.c` | **Medium** | 接口返回按键事件；内部封装扫描/去抖/长按检测 |
| `task_display.c` | **Shallow** | UI 渲染层，接口较多（多页面），实现偏向透传——预期保持浅层 |
| `task_supervisor.c` | **Shallow** | 看门狗喂狗 + 超时检查——逻辑简单但关键 |
| `drv_keypad.c` | **Shallow** | GPIO 扫描，无复杂协议——简单但正确性关键 |
| `drv_buzzer.c` | **Shallow（有意为之）** | 单 GPIO 控制——接口即实现 |
| `drv_relay.c` | **Shallow（有意为之）** | 单 GPIO 控制——接口即实现，但安全关键（开锁） |
| `app_init.c` | **N/A（入口层）** | 纯 IPC 创建 + 任务启动，无业务逻辑——不评估深度 |

> **使用原则**：新增功能时优先判断目标模块的深度定位——Deep 模块可承载更多内部复杂度，Shallow 模块应保持简单。


即使 Auth 任务因未预期的 bug（死锁、堆栈溢出、高优先级 ISR 永久抢占）卡死，Supervisor 也能在 1 秒内解除锁定状态，确保门锁不会永久不可用：

```c
// Supervisor 1s 循环内（零 IPC 开销，只读全局变量）
if (g_sys_state == STATE_LOCKED_OUT) {
    if (HAL_GetTick() >= lockout_until) {
        system_set_state(STATE_IDLE);
        retry = 0;
        buzzer_stop();
    }
}
```

---

## 十、IPC 通道

| 对象 | 类型 | 深度 | 用途 |
|---|---|---|---|
| g_auth_queue | Queue | 10 | 认证请求 → Auth 任务 |
| g_log_queue | Queue | 20 | 日志条目 → Logger 任务 |
| g_display_queue | Queue | 10 | 显示指令 → Display 任务 |
| g_cloud_queue | Queue | 10 | 云端事件 → Cloud 任务 |
| g_cmd_queue | Queue | 5 | 通用命令 |
| g_sys_events | EventGroup | - | 系统事件（开锁/锁定/WiFi 状态） |
| g_flash_mutex | Mutex | - | Flash 读写互斥 |
| g_user_mutex | Mutex | - | 用户数据库互斥 |
| g_display_mutex | Mutex | - | OLED 显示互斥 |

---

## 十一、数据存储 (W25Q64)

| 区域 | 起始地址 | 内容 |
|---|---|---|
| 日志区 | 0x00000 | 500 条 log_entry_t，环形覆盖写入 |
| 指纹区 | 0x40000 | 最多 50 枚指纹模板 |
| 用户区 | 0x80000 | 最多 100 条 user_record_t |

**日志策略：** 环形覆盖，无需磨损均衡。W25Q64 单扇区擦写寿命 10 万次，日擦除 ≤1 次，理论寿命 270+ 年，远超产品生命周期。

---

## 十二、认证方式

| 方式 | 驱动 | 接口 | 凭证 |
|---|---|---|---|
| 指纹 | drv_as608 | UART | fingerprint_id (uint16) |
| RFID | drv_rc522 | SPI | uid[4] |
| 密码 | drv_keypad | GPIO | password[16] |
| 管理员 | drv_keypad | GPIO | 固定密码 "88888888" |

---

## 十三、用户角色

| 角色 | 枚举值 | 权限 |
|---|---|---|
| Guest | ROLE_GUEST | 限时访问（可设过期时间） |
| User | ROLE_USER | 普通永久访问 |
| Admin | ROLE_ADMIN | 管理配置 |

---

## 十四、系统状态机

```
STATE_IDLE ──认证请求──→ STATE_AUTHENTICATING
    ↑                        │
    │                   ┌────┴────┐
    │             认证成功        认证失败 (≥5次)
    │                │              │
    │                ↓              ↓
    │         STATE_UNLOCKED   STATE_LOCKED_OUT
    │         (5s 自动上锁)    (30s 后 → IDLE)
    │                │         (Supervisor 兜底解除)
    │                │              │
    └────────────────┴──────────────┘
```

---

## 十五、编译开关

```c
#define ENABLE_FINGERPRINT  1
#define ENABLE_RFID         1
#define ENABLE_KEYPAD       1
#define ENABLE_WIFI         0  // 当前关闭
```

---

## 十六、WiFi AT 异步改造设计（待启用时实现）

当 `ENABLE_WIFI=1` 时，ESP8266 AT 指令交互采用 **switch-case 状态机**：

```c
typedef enum { AT_IDLE, AT_SEND, AT_WAIT_OK, AT_RETRY, AT_DONE } at_state_t;

switch (state) {
case AT_SEND:    esp8266_send_cmd(cmd); t0 = HAL_GetTick(); state = AT_WAIT_OK; break;
case AT_WAIT_OK: if (uart_has_line("OK")) state = AT_DONE;
                 else if (timeout) { retry++; state = retry < 3 ? AT_SEND : AT_IDLE; }
                 break;
case AT_DONE:    /* next command */ break;
}
```

选择 switch-case 而非函数指针表的理由：状态数量少（4-5 步），每个状态超时逻辑各异，switch-case 就地处理超时更清晰，调试时可打印状态名。

---

## 十七、未决 / 延后事项

| 事项 | 结论 | 理由 |
|---|---|---|
| W25Q64 磨损均衡 | ❌ 不需要 | 环形覆盖寿命足够 |
| OLED Canvas 帧缓冲解耦 | 🔜 TODO，非必须 | 128x64 性能不是瓶颈 |
| WiFi AT 异步改造 | 🔜 TODO，待 WiFi 开启 | 状态机方案已设计（第十六章），当前 ENABLE_WIFI=0 |

---

## 十八、开工前置检查清单

| # | 检查项 | 状态 |
|---|---|---|
| 1 | 确认板载 HSE 晶振频率（原理图或丝印，不要靠软件猜） | ⬜ |
| 2 | CubeMX 中 NVIC Priority Group 设为 4 | ⬜ |
| 3 | CubeMX 生成后检查 FreeRTOSConfig.h 中 MAX_SYSCALL=5、LOWEST=15 | ⬜ |
| 4 | 中断优先级按第八章表格逐一配置 | ⬜ |
| 5 | 主堆栈大小 ≥ 20KB | ⬜ |
| 6 | `app_init()` 调用位置在 MX_FREERTOS_Init() 之后、vTaskStartScheduler() 之前 | ⬜ |
| 7 | `app_init()` 不做外设初始化，只做 IPC 创建 + xTaskCreate | ⬜ |

---

## 十九、命名规范（延续初代约定）

- **文件：** `drv_xxx`（驱动）、`task_xxx`（任务）
- **函数：** `模块_动作()`，如 `as608_init()`、`relay_unlock()`
- **常量：** 全大写 + 下划线
- **注释：** 中文，"为什么"优先于"做什么"
