# ADR-0001: main.h 拆分为 4 个 App 头文件

> 状态：已采纳 | 日期：2026-06-19

## 背景

SmartLock v1 的 `Core/Inc/main.h` 承担了 4 种职责：HAL 外设句柄声明、系统宏配置、业务类型定义、IPC 对象声明。CubeMX 每次重新生成 .ioc 会覆盖 main.h，导致业务代码丢失。v2 必须解决 CubeMX 生成代码与手写代码的物理隔离问题。

## 决策

将 main.h 按职责拆分为 4 个独立头文件，放入 `App/Inc/`（CubeMX 不触碰的目录）：

| 原内容 | 去向 | 维护方 |
|---|---|---|
| HAL 外设句柄 extern 声明 | `Core/Inc/main.h` | CubeMX |
| 系统宏（MAX_USER_COUNT、栈大小等） | `App/Inc/app_config.h` | 手写 |
| 业务类型（user_record_t、auth_request_t 等） | `App/Inc/app_types.h` | 手写 |
| IPC 对象声明 + 事件掩码位 | `App/Inc/app_ipc.h` | 手写 |

每个文件有严格的最小依赖：
- `app_config.h` → 仅 `stdint.h`
- `app_types.h` → `stdint.h` + `stdbool.h`
- `app_ipc.h` → `FreeRTOS.h`
- `app_init.h` → 无依赖

## 备选方案

### 方案 A：保持单文件，CubeMX 重生成后手动合并 —— 不采纳

- v1 的问题根因。即使写了合并脚本，字段增减仍需人工判断，容易遗漏。

### 方案 B：4 文件拆分 —— 已采纳

- 优点：CubeMX 重生成零影响；每个文件职责单一、依赖最小；新成员能快速定位"类型去哪里找"
- 缺点：文件数增加，需要团队统一认知（通过本文档和 AGENTS.md 约束解决）

## 后果

- AGENTS.md §2 必须明确"App 层禁止 include stm32f4xx_hal.h"——防止有人为取 HAL 类型而绕过 app_types.h
- 新增业务类型时，必须放入 app_types.h 而非 app_config.h——两文件职责不可混淆
- CubeMX 重生成后的 6 步检查（AGENTS.md §8）中，第一步就是确认 app_init() 未被覆盖