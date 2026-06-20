# SmartLock_v2 CubeMX 完整配置手册

> MCU: STM32F407VETx | 封装: LQFP100 | CubeMX 版本兼容 6.x
> 基于 ARCHITECTURE.md 引脚分配和中断优先级方案

---

## 一、GPIO 引脚配置表

> CubeMX 页面：Pinout & Configuration → Pinout view

### 1.1 USART 引脚

| 引脚 | 功能模式 | AF 编号 | 输出类型 | 上下拉 | 输出速度 | 初始电平 | EXTI | 备注 |
|---|---|---|---|---|---|---|---|---|
| PA9 | Alternate Function Push Pull | AF7 (USART1_TX) | Push Pull | No pull-up/pull-down | High | — | 不开启 | AS608 指纹 TX |
| PA10 | Alternate Function Push Pull | AF7 (USART1_RX) | Push Pull | Pull-up | High | — | 不开启 | AS608 指纹 RX |
| PA2 | Alternate Function Push Pull | AF7 (USART2_TX) | Push Pull | No pull-up/pull-down | High | — | 不开启 | ESP8266 WiFi TX |
| PA3 | Alternate Function Push Pull | AF7 (USART2_RX) | Push Pull | Pull-up | High | — | 不开启 | ESP8266 WiFi RX |
| PB10 | Alternate Function Push Pull | AF7 (USART3_TX) | Push Pull | No pull-up/pull-down | High | — | 不开启 | 调试串口 TX |
| PB11 | Alternate Function Push Pull | AF7 (USART3_RX) | Push Pull | Pull-up | High | — | 不开启 | 调试串口 RX |

### 1.2 SPI 引脚

| 引脚 | 功能模式 | AF 编号 | 输出类型 | 上下拉 | 输出速度 | 初始电平 | EXTI | 备注 |
|---|---|---|---|---|---|---|---|---|
| PA5 | Alternate Function Push Pull | AF5 (SPI1_SCK) | Push Pull | No pull-up/pull-down | High | — | 不开启 | RC522 RFID SCK |
| PA6 | Alternate Function Push Pull | AF5 (SPI1_MISO) | Push Pull | Pull-up | High | — | 不开启 | RC522 RFID MISO |
| PA7 | Alternate Function Push Pull | AF5 (SPI1_MOSI) | Push Pull | No pull-up/pull-down | High | — | 不开启 | RC522 RFID MOSI |
| PA4 | Output Push Pull | — (GPIO Output) | Push Pull | No pull-up/pull-down | High | High (SET) | 不开启 | RC522 RFID CS（软件控制） |
| PB13 | Alternate Function Push Pull | AF5 (SPI2_SCK) | Push Pull | No pull-up/pull-down | High | — | 不开启 | W25Q64 Flash SCK |
| PB14 | Alternate Function Push Pull | AF5 (SPI2_MISO) | Push Pull | Pull-up | High | — | 不开启 | W25Q64 Flash MISO |
| PB15 | Alternate Function Push Pull | AF5 (SPI2_MOSI) | Push Pull | No pull-up/pull-down | High | — | 不开启 | W25Q64 Flash MOSI |
| PB12 | Output Push Pull | — (GPIO Output) | Push Pull | No pull-up/pull-down | High | High (SET) | 不开启 | W25Q64 Flash CS（软件控制） |

### 1.3 I2C 引脚

| 引脚 | 功能模式 | AF 编号 | 输出类型 | 上下拉 | 输出速度 | 初始电平 | EXTI | 备注 |
|---|---|---|---|---|---|---|---|---|
| PB6 | Alternate Function Open Drain | AF4 (I2C1_SCL) | Open Drain | No pull-up/pull-down | High | — | 不开启 | OLED SSD1306 SCL |
| PB7 | Alternate Function Open Drain | AF4 (I2C1_SDA) | Open Drain | No pull-up/pull-down | High | — | 不开启 | OLED SSD1306 SDA |

> I2C 引脚使用 Open Drain 模式，外部需 4.7kΩ 上拉电阻。CubeMX 内上下拉选 No pull-up/pull-down（由外部电阻处理）。

### 1.4 GPIO 控制引脚

| 引脚 | 功能模式 | AF 编号 | 输出类型 | 上下拉 | 输出速度 | 初始电平 | EXTI | 备注 |
|---|---|---|---|---|---|---|---|---|
| PE0 | Output Push Pull | — (GPIO Output) | Push Pull | No pull-up/pull-down | Low | High (SET) | 不开启 | 继电器控制（低电平开锁，初始高电平=锁定） |
| PE1 | Output Push Pull | — (GPIO Output) | Push Pull | No pull-up/pull-down | Low | Low (RESET) | 不开启 | 蜂鸣器（低电平发声，初始高电平=静音） |

### 1.5 矩阵键盘 GPIO（4×4 矩阵）

| 引脚 | 功能模式 | AF 编号 | 输出类型 | 上下拉 | 输出速度 | 初始电平 | EXTI | 备注 |
|---|---|---|---|---|---|---|---|---|
| PC0 | Output Push Pull | — (GPIO Output) | Push Pull | No pull-up/pull-down | Low | High (SET) | 不开启 | 行 0 |
| PC1 | Output Push Pull | — (GPIO Output) | Push Pull | No pull-up/pull-down | Low | High (SET) | 不开启 | 行 1 |
| PC2 | Output Push Pull | — (GPIO Output) | Push Pull | No pull-up/pull-down | Low | High (SET) | 不开启 | 行 2 |
| PC3 | Output Push Pull | — (GPIO Output) | Push Pull | No pull-up/pull-down | Low | High (SET) | 不开启 | 行 3 |
| PC4 | Input mode | — (GPIO Input) | — | Pull-down | — | — | 不开启 | 列 0 |
| PC5 | Input mode | — (GPIO Input) | — | Pull-down | — | — | 不开启 | 列 1 |
| PC6 | Input mode | — (GPIO Input) | — | Pull-down | — | — | 不开启 | 列 2 |
| PC7 | Input mode | — (GPIO Input) | — | Pull-down | — | — | 不开启 | 列 3 |

> 行输出高电平，列输入下拉。扫描时逐行拉低，读列状态。此为初代代码方式，实际可根据硬件电路调整。

---

## 二、USART 外设配置

> CubeMX 页面：Pinout & Configuration → Connectivity → USART1/2/3
> 中断优先级配置：Pinout & Configuration → System Core → NVIC

### 2.1 USART1（AS608 指纹模块）— 参数设置页

| 配置项 | 值 | 说明 |
|---|---|---|
| Mode | ☑ Asynchronous | 异步模式 |
| Hardware Flow Control (RS232) | ☐ 不勾选 | 无硬件流控 |
| Hardware Flow Control (RS485) | ☐ 不勾选 | 无 RS485 |
| **Parameter Settings 页** | | |
| Baud Rate | 57600 Bits/s | AS608 默认波特率 |
| Word Length | 8 Bits (including Parity) | 8 位数据 |
| Parity | None | 无校验 |
| Stop Bits | 1 | 1 停止位 |
| Data Direction | Receive and Transmit | 收发 |
| Over Sampling | 16 Samples | 默认过采样 |

| NVIC 中断配置 | 值 |
|---|---|
| USART1 global interrupt | ☑ Enabled |
| Preemption Priority | 7 |
| Sub Priority | 0 |
| DMA 使能 | ☐ 不使用 DMA |

### 2.2 USART2（ESP8266 WiFi）— 参数设置页

| 配置项 | 值 | 说明 |
|---|---|---|
| Mode | ☑ Asynchronous | 异步模式 |
| Hardware Flow Control (RS232) | ☐ 不勾选 | 无硬件流控 |
| **Parameter Settings 页** | | |
| Baud Rate | 115200 Bits/s | ESP8266 默认波特率 |
| Word Length | 8 Bits (including Parity) | 8 位数据 |
| Parity | None | 无校验 |
| Stop Bits | 1 | 1 停止位 |
| Data Direction | Receive and Transmit | 收发 |
| Over Sampling | 16 Samples | 默认过采样 |

| NVIC 中断配置 | 值 |
|---|---|
| USART2 global interrupt | ☑ Enabled |
| Preemption Priority | 7 |
| Sub Priority | 0 |
| DMA 使能 | ☐ 不使用 DMA |

### 2.3 USART3（调试串口）— 参数设置页

| 配置项 | 值 | 说明 |
|---|---|---|
| Mode | ☑ Asynchronous | 异步模式 |
| Hardware Flow Control (RS232) | ☐ 不勾选 | 无硬件流控 |
| **Parameter Settings 页** | | |
| Baud Rate | 115200 Bits/s | 调试用 |
| Word Length | 8 Bits (including Parity) | 8 位数据 |
| Parity | None | 无校验 |
| Stop Bits | 1 | 1 停止位 |
| Data Direction | Receive and Transmit | 收发 |
| Over Sampling | 16 Samples | 默认过采样 |

| NVIC 中断配置 | 值 |
|---|---|
| USART3 global interrupt | ☐ Disabled（调试串口可用轮询） |
| Preemption Priority | —（未使能） |
| Sub Priority | —（未使能） |
| DMA 使能 | ☐ 不使用 DMA |

---

## 三、SPI 外设配置

> CubeMX 页面：Pinout & Configuration → Connectivity → SPI1/2

### 3.1 SPI1（RC522 RFID）— 参数设置页

| 配置项 | 值 | 说明 |
|---|---|---|
| Mode | ☑ Full-Duplex Master | 全双工主模式 |
| Hardware NSS Signal | ☐ Disable（软件 NSS） | 用 PA4 GPIO 做片选 |
| **Parameter Settings 页** | | |
| Frame Format | Motorola | 标准 SPI |
| Data Size | 8 Bits | 8 位数据 |
| First Bit | MSB First | 高位先行 |
| Clock Prescaler | 64 | 84MHz / 64 = 1.3125 MHz（RC522 最大 10MHz） |
| Clock Polarity (CPOL) | Low | 空闲时 SCK 低电平 |
| Clock Phase (CPHA) | 1 Edge | 第 1 个边沿采样 |
| CRC Calculation | ☐ Disabled | 不使能 CRC |
| NSS Signal Type | Software | 软件片选 |

| NVIC 中断配置 | 值 |
|---|---|
| SPI1 global interrupt | ☑ Enabled |
| Preemption Priority | 8 |
| Sub Priority | 0 |
| DMA 使能 | ☐ 不使用 DMA |

### 3.2 SPI2（W25Q64 Flash）— 参数设置页

| 配置项 | 值 | 说明 |
|---|---|---|
| Mode | ☑ Full-Duplex Master | 全双工主模式 |
| Hardware NSS Signal | ☐ Disable（软件 NSS） | 用 PB12 GPIO 做片选 |
| **Parameter Settings 页** | | |
| Frame Format | Motorola | 标准 SPI |
| Data Size | 8 Bits | 8 位数据 |
| First Bit | MSB First | 高位先行 |
| Clock Prescaler | 4 | 42MHz / 4 = 10.5 MHz（W25Q64 最大 80MHz，保守取 10.5MHz） |
| Clock Polarity (CPOL) | Low | 空闲时 SCK 低电平（W25Q64 Mode 0） |
| Clock Phase (CPHA) | 1 Edge | 第 1 个边沿采样（W25Q64 Mode 0） |
| CRC Calculation | ☐ Disabled | 不使能 CRC |
| NSS Signal Type | Software | 软件片选 |

| NVIC 中断配置 | 值 |
|---|---|
| SPI2 global interrupt | ☑ Enabled |
| Preemption Priority | 8 |
| Sub Priority | 0 |
| DMA 使能 | ☐ 不使用 DMA |

---

## 四、I2C 外设配置

> CubeMX 页面：Pinout & Configuration → Connectivity → I2C1

### 4.1 I2C1（OLED SSD1306）— 参数设置页

| 配置项 | 值 | 说明 |
|---|---|---|
| I2C Speed Mode | Fast Mode | 快速模式 400kHz |
| **Parameter Settings 页** | | |
| I2C Speed Mode | Fast Mode | SSD1306 支持 400kHz |
| I2C Speed Frequency | 400 KHz | 由 CubeMX 自动计算时序 |
| Rise Time | 100 ns | 默认值 |
| Fall Time | 100 ns | 默认值 |
| Clock No Stretch Mode | ☐ Disabled | 允许从设备时钟拉伸 |
| Primary Address Length | 7-bit | 7 位地址 |
| Dual Address Acknowledged | ☐ Disabled | 不需要双地址 |
| Primary Slave Address | 0x3C << 1（由程序设置） | SSD1306 默认地址 0x3C |
| General Call Address Detection | ☐ Disabled | 不检测广播地址 |
| Packet Error Checking (PEC) | ☐ Disabled | 不使能 PEC |

| NVIC 中断配置 | 值 |
|---|---|
| I2C1 event interrupt | ☑ Enabled |
| I2C1 error interrupt | ☑ Enabled |
| Preemption Priority（both） | 9 |
| Sub Priority（both） | 0 |
| DMA 使能 | ☐ 不使用 DMA |

> 外部硬件需在 PB6/PB7 各接 4.7kΩ 上拉到 3.3V。

---

## 五、时钟树配置

> CubeMX 页面：Clock Configuration
> 前提：HSE 晶振 = 8MHz（⚠ 需确认实物）

### 5.1 时钟源选择

| 配置项 | 选择 | 说明 |
|---|---|---|
| **RCC 设置（Pinout & Configuration → System Core → RCC）** | | |
| High Speed Clock (HSE) | ☑ Crystal/Ceramic Resonator | 外部 8MHz 晶振 |
| Low Speed Clock (LSE) | ☐ Disable | 不使用外部低速晶振 |

### 5.2 PLL 配置

| 参数 | 值 | 计算公式 |
|---|---|---|
| PLL Source Mux | HSE | 选择 HSE（8MHz）为 PLL 输入源 |
| PLLM | 8 | HSE / 8 = 1MHz → PLL 输入 |
| PLLN | 336 | × 336 → VCO = 1MHz × 336 = 336MHz |
| PLLP | 2 | 336MHz / 2 = 168MHz → SYSCLK |
| PLLQ | 7 | 336MHz / 7 = 48MHz → USB OTG / SDIO / RNG 时钟 |
| PLLR | —（F4 系列不使用） | |

### 5.3 系统时钟选择

| 参数 | 值 | 说明 |
|---|---|---|
| System Clock Mux | PLLCLK | 选择 PLL 输出为系统时钟 |
| SYSCLK | **168 MHz** | 系统时钟 |
| AHB Prescaler | 1 | HCLK = SYSCLK = **168 MHz** |
| HCLK | **168 MHz** | AHB 总线时钟 |

### 5.4 APB 总线分频

| 参数 | 预分频值 | 总线时钟 | 挂载外设 |
|---|---|---|---|
| APB1 Prescaler | 4 | **42 MHz**（168/4） | USART2/3、SPI2、I2C1、TIM2-7/12-14 |
| APB2 Prescaler | 2 | **84 MHz**（168/2） | USART1、SPI1、TIM1/8-11 |
| APB1 Timer Clocks | ×2 | **84 MHz**（42×2） | APB1 上的定时器自动倍频 |
| APB2 Timer Clocks | ×2 | **168 MHz**（84×2） | APB2 上的定时器自动倍频 |

> APB 预分频 > 1 时，定时器时钟自动 ×2 补偿，无需手动配置。

### 5.5 专用时钟

| 参数 | 值 | 说明 |
|---|---|---|
| USB / RNG / SDIO Clock Mux | PLLQ | 48MHz |
| RTC Clock Mux | LSI（内部 32kHz） | 不使用外部 LSE 晶振 |
| Independent Watchdog (IWDG) | LSI（内部 32kHz） | 独立看门狗时钟 |
| MCO1 / MCO2 | ☐ Disabled | 不使用时钟输出 |
| Clock Security System (CSS) | ☐ Disabled | 不使能 HSE 监控 |

---


## 六、RTC 配置

> CubeMX 页面：Pinout & Configuration → Timers → RTC
> 为日志时间戳和定时任务提供日历时钟。时钟源为内部 LSI（32kHz），精度 ±5%。如需精确时间，改用外部 LSE 晶振。

### 6.1 RTC Mode and Clock Source 页

| 配置项 | 勾选/选择 | 说明 |
|---|---|---|
| ☑ Activate Clock Source | 勾选 | 激活 RTC 时钟源 |
| ☑ Activate Calendar | 勾选 | 激活日历功能 |
| Clock Source | LSI（下拉选择） | 内部低速 32kHz RC 振荡器 |
| RTC OUT | ☐ Disable（不勾选） | RTC 输出到引脚，不需要 |

### 6.2 RTC Parameter Settings 页

| 配置项 | 值 | 说明 |
|---|---|---|
| **Calendar 节** | | |
| Hour Format | **24 Hour**（下拉选择） | 24 小时制 |
| Asynchronous Predivider value | **127** | LSI=32kHz → 32000/(127+1)=250 Hz |
| Synchronous Predivider value | **249** | 250/(249+1)=1 Hz，日历每秒递增 |
| Output Polarity | High（默认，灰色） | RTC_OUT 未打开时无效 |
| Data Format | **Binary data format**（下拉） | 二进制格式（非 BCD） |
| Daylight Saving | **None**（默认） | 不处理夏令时 |
| Store Operation | **Set**（默认） | |

### 6.3 RTC Calendar Time/Date 页

> 初始日期时间任意填，代码中通过串口命令或 WiFi NTP 校准。

| 配置项 | 值 | 说明 |
|---|---|---|
| **Date 节** | | |
| Week Day | Monday（下拉） | 任意 |
| Month | January | |
| Date | 1 | |
| Year | 26 | 2026 年后两位 |
| **Time 节** | | |
| Hours | 0 | |
| Minutes | 0 | |
| Seconds | 0 | |
| SubSeconds | 0 | |
| Time Format | AM（下拉） | |

### 6.4 RTC Alarm 页（全部不勾选）

| 配置项 | 勾选 | 说明 |
|---|---|---|
| ☑ Activate Alarm | ☐ 不勾选 | 不用闹钟，代码里轮询读时间 |
| ☑ Activate Alarm A | ☐ 不勾选 | |
| ☑ Activate Alarm B | ☐ 不勾选 | |

> 不勾选 Alarm → NVIC 中无 RTC 中断 → 无需配置优先级。时间读取用 `HAL_RTC_GetTime()` + `HAL_RTC_GetDate()` 轮询，不依赖中断。### 6.5 RTC Tamper 页

| 配置项 | 勾选 |
|---|---|
| ☑ Activate Tamper 1/2/3 | ☐ 全部不勾选 |

### 6.6 RTC NVIC Settings

> ⚠ 不勾选 Alarm → 无 RTC 中断 → NVIC 列表中不出现 RTC → 无需配置优先级。

---
### 6.6 RTC NVIC Settings

| 配置项 | 勾选/值 |
|---|---|
| RTC global interrupt | ☑ Enabled |
| RTC Alarm interrupt through EXTI line 17 | ☑ Enabled |
| Preemption Priority | **10** |
| Sub Priority | **0** |

### 6.6 分频计算

```
LSI 标称值   = 32,000 Hz
Async 预分频 = 127    →  32,000 / 128 = 250 Hz
Sync 预分频  = 249    →     250 / 250 =   1 Hz
日历时钟     =   1 Hz  → 每秒递增
```

> ⚠ LSI 偏差 ±5%，时间戳会有漂移但不影响开锁。精确计时需 LSE 32.768kHz 晶振（Async=127, Sync=255）。

### 6.7 生成代码确认

`Core/Src/rtc.c` 中 `MX_RTC_Init()`：
```c
hrtc.Instance = RTC;
hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
hrtc.Init.AsynchPrediv = 127;
hrtc.Init.SynchPrediv = 249;
hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
HAL_RTC_Init(&hrtc);
```
## 七、FreeRTOS 配置

> CubeMX 页面：Pinout & Configuration → Middleware → FREERTOS

### 7.1 Interface 页

| 配置项 | 值 |
|---|---|
| ☑ Enabled | 勾选 |
| CMSIS_V1 | 选中 |

### 7.2 Configuration 页 — Config parameters

| 配置项 | 值 | 说明 |
|---|---|---|
| USE_PREEMPTION | ☑ Enabled | 抢占式调度 |
| CPU_CLOCK_HZ | 168000000 | 与 SYSCLK 一致 |
| TICK_RATE_HZ | **1000** | 1ms tick |
| MAX_PRIORITIES | **8** | 6 个任务 + 2 预留 |
| MINIMAL_STACK_SIZE | 128 words | 空闲任务栈 |
| TOTAL_HEAP_SIZE | **20480** | 20KB 总堆 |
| MAX_TASK_NAME_LEN | 16 | 任务名最长 16 字符 |
| USE_16_BIT_TICKS | ☐ Disabled | 使用 32 位 tick |
| IDLE_SHOULD_YIELD | ☑ Enabled | 空闲任务可让出 |
| USE_MUTEXES | ☑ Enabled | 启用互斥信号量 |
| USE_RECURSIVE_MUTEXES | ☐ Disabled | |
| USE_COUNTING_SEMAPHORES | ☑ Enabled | 启用计数信号量 |
| QUEUE_REGISTRY_SIZE | 8 | 队列注册表大小 |
| **Hook 函数** | | |
| USE_IDLE_HOOK | ☐ Disabled | |
| USE_TICK_HOOK | ☐ Disabled | |
| USE_MALLOC_FAILED_HOOK | ☑ Enabled | 内存分配失败回调 |
| USE_DAEMON_TASK_STARTUP_HOOK | ☐ Disabled | |
| **运行时检测** | | |
| CHECK_FOR_STACK_OVERFLOW | **2**（检测溢出 + 检查栈顶标记） | 开发阶段开启 |
| **其他** | | |
| USE_TRACE_FACILITY | ☑ Enabled | 启用跟踪（配合高水位检查） |
| GENERATE_RUN_TIME_STATS | ☐ Disabled | |
| USE_STATS_FORMATTING_FUNCTIONS | ☐ Disabled | |
| INCLUDE_vTaskPrioritySet | ☑ Enabled | |
| INCLUDE_uxTaskPriorityGet | ☑ Enabled | |
| INCLUDE_vTaskDelete | ☐ Disabled | |
| INCLUDE_vTaskSuspend | ☑ Enabled | |
| INCLUDE_xResumeFromISR | ☑ Enabled | |
| INCLUDE_vTaskDelayUntil | ☑ Enabled | |
| INCLUDE_vTaskDelay | ☑ Enabled | |
| INCLUDE_xTaskGetSchedulerState | ☑ Enabled | |
| INCLUDE_xTaskGetCurrentTaskHandle | ☑ Enabled | |
| INCLUDE_uxTaskGetStackHighWaterMark | ☑ Enabled | Supervisor 打印栈余量用 |
| INCLUDE_xTaskGetIdleTaskHandle | ☐ Disabled | |
| INCLUDE_eTaskGetState | ☑ Enabled | |
| INCLUDE_xEventGroupSetBitFromISR | ☑ Enabled | ISR 中操作事件组 |
| INCLUDE_xTimerPendFunctionCall | ☑ Enabled | |
| INCLUDE_xTaskAbortDelay | ☐ Disabled | |
| INCLUDE_xTaskGetHandle | ☑ Enabled | |

### 7.3 Configuration 页 — Include parameters

| 配置项 | 值 | 说明 |
|---|---|---|
| vTaskPrioritySet | ☑ Enabled | |
| uxTaskPriorityGet | ☑ Enabled | |
| vTaskDelete | ☐ Disabled（不动态删任务） | |
| vTaskCleanUpResources | ☐ Disabled | |
| vTaskSuspend | ☑ Enabled | |
| vTaskDelayUntil | ☑ Enabled | |
| vTaskDelay | ☑ Enabled | |
| uxTaskGetStackHighWaterMark | ☑ Enabled | |
| xTaskGetSchedulerState | ☑ Enabled | |
| xTaskGetCurrentTaskHandle | ☑ Enabled | |
| eTaskGetState | ☑ Enabled | |
| xEventGroupSetBitFromISR | ☑ Enabled | |
| xTimerPendFunctionCall | ☑ Enabled | |
| xTaskAbortDelay | ☐ Disabled | |

### 7.4 Tasks and Queues 页

> ⚠ 不要在 CubeMX 中创建任务和队列——全部由 `app_init()` 在代码中创建。
> 以下仅确认 CubeMX 不自动生成任务：

| 配置项 | 值 |
|---|---|
| Tasks 表格 | 空（不创建任何任务） |
| Queues 表格 | 空（不创建任何队列） |
| Timers 表格 | 空 |
| Semaphores 表格 | 空 |
| Mutexes 表格 | 空 |
| Event Groups 表格 | 空 |

---

## 八、NVIC 中断优先级配置

> CubeMX 页面：Pinout & Configuration → System Core → NVIC

### 8.1 NVIC Settings

| 中断源 | 使能状态 | Preemption Priority | Sub Priority | 说明 |
|---|---|---|---|---|
| Non maskable interrupt | ☑ Enabled（默认） | — | — | NMI |
| Hard fault interrupt | ☑ Enabled（默认） | — | — | |
| Memory management fault | ☑ Enabled（默认） | — | — | |
| Bus fault | ☑ Enabled（默认） | — | — | |
| Usage fault | ☑ Enabled（默认） | — | — | |
| SVCall | ☑ Enabled（默认） | — | — | |
| Debug monitor | ☑ Enabled（默认） | — | — | |
| PendSV | ☑ Enabled（默认） | — | — | FreeRTOS 使用 |
| SysTick | ☑ Enabled（默认） | 15 | 0 | FreeRTOS 心跳 |
| USART1 global interrupt | ☑ Enabled | **7** | 0 | 指纹模块 RX |
| USART2 global interrupt | ☑ Enabled | **7** | 0 | WiFi 模块 RX |
| USART3 global interrupt | ☐ Disabled | — | — | 调试串口轮询 |
| SPI1 global interrupt | ☑ Enabled | **8** | 0 | RFID RX |
| SPI2 global interrupt | ☑ Enabled | **8** | 0 | Flash RX |
| I2C1 event interrupt | ☑ Enabled | **9** | 0 | OLED 通信事件 |
| I2C1 error interrupt | ☑ Enabled | **9** | 0 | OLED 通信错误 |
| RTC global interrupt | ☑ Enabled | 10 | 0 | RTC 闹钟 |

### 8.2 NVIC Priority Group

| 配置项 | 值 | CubeMX 设置 |
|---|---|---|
| Priority Group | **4 bits for preemption priority, 0 bits for subpriority** | NVIC Settings 页面 Priority Group 下拉选择 `4` |

> 确认 HAL_Init() 内部调用 `HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4)`。

---

## 九、GPIO 通用配置

> CubeMX 页面：Pinout & Configuration → System Core → GPIO

### 9.1 GPIO 配置（非通信引脚）

| 引脚 | 配置项 | 值 |
|---|---|---|
| **PA4（RC522 CS）** | | |
| | GPIO output level | High |
| | GPIO mode | Output Push Pull |
| | GPIO Pull-up/Pull-down | No pull-up/pull-down |
| | Maximum output speed | High |
| | User Label | RC522_CS |
| **PB12（W25Q64 CS）** | | |
| | GPIO output level | High |
| | GPIO mode | Output Push Pull |
| | GPIO Pull-up/Pull-down | No pull-up/pull-down |
| | Maximum output speed | High |
| | User Label | FLASH_CS |
| **PE0（继电器）** | | |
| | GPIO output level | High |
| | GPIO mode | Output Push Pull |
| | GPIO Pull-up/Pull-down | No pull-up/pull-down |
| | Maximum output speed | Low |
| | User Label | RELAY |
| **PE1（蜂鸣器）** | | |
| | GPIO output level | Low |
| | GPIO mode | Output Push Pull |
| | GPIO Pull-up/Pull-down | No pull-up/pull-down |
| | Maximum output speed | Low |
| | User Label | BUZZER |
| **PC0-3（键盘行）** | | |
| | GPIO output level | High |
| | GPIO mode | Output Push Pull |
| | GPIO Pull-up/Pull-down | No pull-up/pull-down |
| | Maximum output speed | Low |
| | User Label | KEY_ROW0 ~ KEY_ROW3 |
| **PC4-7（键盘列）** | | |
| | GPIO mode | Input mode |
| | GPIO Pull-up/Pull-down | Pull-down |
| | User Label | KEY_COL0 ~ KEY_COL3 |

---

## 十、RCC 时钟配置

> CubeMX 页面：Pinout & Configuration → System Core → RCC

| 配置项 | 选择 | 说明 |
|---|---|---|
| High Speed Clock (HSE) | ☑ Crystal/Ceramic Resonator | HSE 旁路选 Disable |
| Low Speed Clock (LSE) | ☐ Disable | 不使用外部低速晶振 |
| Master Clock Output 1 | ☐ Disable | |
| Master Clock Output 2 | ☐ Disable | |

---

## 十一、SYS 调试配置

> CubeMX 页面：Pinout & Configuration → System Core → SYS

| 配置项 | 值 | 说明 |
|---|---|---|
| Debug | Serial Wire | SWD 调试（占用 PA13/PA14，不与外设冲突） |
| Timebase Source | TIM6 | 或 SysTick（CubeMX 一般推荐非 SysTick 定时器避免与 FreeRTOS 冲突） |
| System Wake-Up | ☐ Disable | |

> ⚠ **重要**：如果 FreeRTOS 使用 SysTick 作为 tick，则 HAL 时基必须使用独立定时器（如 TIM6），避免 HAL_Delay 与 FreeRTOS tick 争抢 SysTick。

---

## 十二、独立看门狗 IWDG（可选）

> CubeMX 页面：Pinout & Configuration → System Core → IWDG
> 看门狗是安全门锁的必要配置

| 配置项 | 值 | 说明 |
|---|---|---|
| ☑ Activate | 勾选 | 激活 IWDG |
| IWDG counter clock prescaler | 32 (32kHz / 32 = 1kHz) | 预分频 = 32 |
| IWDG down-counter reload value | 4000 | 超时时间 = 4000 × 1ms = 4s |

> Supervisor 任务需在 1 秒周期内调用 `HAL_IWDG_Refresh(&hiwdg)`。4 秒超时给出 4 倍余量。

---

## 十三、外设时钟使能

> CubeMX 在 Pinout 视图中选中引脚并配置为 AF 功能后，自动使能对应外设时钟。无需手动操作，但需核对：

| 外设 | 时钟使能状态（自动） |
|---|---|
| GPIOA/B/C/D/E | ☑ Enabled |
| USART1 | ☑ Enabled |
| USART2 | ☑ Enabled |
| USART3 | ☑ Enabled |
| SPI1 | ☑ Enabled |
| SPI2 | ☑ Enabled |
| I2C1 | ☑ Enabled |
| RTC | ☑ Enabled |
| IWDG | ☑ Enabled |

---

## 十四、CubeMX 工程生成设置

> CubeMX 页面：Project Manager

### 14.1 Project 页

| 配置项 | 值 |
|---|---|
| Project Name | SmartLock |
| Project Location | D:\Users\hesheng\Desktop\SmartLock_v2 |
| Application Structure | Advanced（推荐）或 Basic |
| Toolchain / IDE | **STM32CubeIDE**（推荐）或 MDK-ARM |
| ☑ Use Default Firmware Location | 勾选 |

### 14.2 Code Generator 页

| 配置项 | 勾选状态 | 说明 |
|---|---|---|
| **STM32Cube Firmware Library Package** | — | 自动选择 |
| Copy only the necessary library files | ☑ 勾选 | 只拷贝用到的 HAL 文件 |
| **Generated files** | | |
| Generate peripheral initialization as a pair of `.c/.h` files per peripheral | ☑ 勾选 | 每个外设独立成对文件 |
| Keep User Code when re-generating | ☐ 不勾选 | ⚠ CubeMX 覆盖 Core/ 时，此选项保护的是 User Code 段（`/* USER CODE BEGIN */` 内）。我们不做用户代码段保护——所有手写代码在 App/ 和 Drivers/，不在 Core/ 内 |
| Delete previously generated files when not re-generated | ☐ 不勾选 | 不自动删除 |
| **HAL settings** | | |
| Set all free pins as analog (to optimize the power consumption) | ☐ 不勾选（避免影响调试） | |
| Enable Full Assert | ☑ 勾选 | 开发阶段启用 assert（对应 `configASSERT`） |
| Generate full assertion code | ☑ 勾选 | |

### 14.3 Advanced Settings 页

| 外设 | 驱动选择 | 说明 |
|---|---|---|
| USART1/2/3 | HAL | 标准 HAL 驱动 |
| SPI1/SPI2 | HAL | 标准 HAL 驱动 |
| I2C1 | HAL | 标准 HAL 驱动 |
| RCC | HAL | |
| GPIO | HAL | |
| RTC | HAL | |
| IWDG | HAL | |
| FREERTOS | — | 自动 |
| DMA（未使用） | — | |

### 14.4 堆栈大小

> Project Manager → Project → Linker Settings

| 配置项 | 值 | 说明 |
|---|---|---|
| Minimum Heap Size | **0x2000**（8KB） | 堆起始大小。FreeRTOS 通过 TOTAL_HEAP_SIZE 自行分配，此堆仅 HAL 库内部少量使用 |
| Minimum Stack Size | **0x1000**（4KB） | 主栈（main 函数 + 中断栈上下文切换使用） |

> ⚠ FreeRTOS 任务栈不使用此栈，任务栈从 FreeRTOS 堆（TOTAL_HEAP_SIZE=20KB）中分配。

---

## 十五、CubeMX 操作步骤概览

| 步骤 | 页面 | 操作 |
|---|---|---|
| 1 | 新建工程 | 选择 MCU: STM32F407VETx |
| 2 | Pinout & Configuration → RCC | HSE = Crystal/Ceramic Resonator |
| 3 | Pinout & Configuration → SYS | Debug = Serial Wire, Timebase = TIM6 |
| 4 | Pinout view | 按 §一 表格逐个配置引脚功能 |
| 5 | Pinout & Configuration → Connectivity → USART1/2/3 | 按 §二 逐个配置 |
| 6 | Pinout & Configuration → Connectivity → SPI1/2 | 按 §三 逐个配置 |
| 7 | Pinout & Configuration → Connectivity → I2C1 | 按 §四 配置 |
| 8 | Pinout & Configuration → Timers → RTC | 按 §六 配置 |
| 9 | Pinout & Configuration → System Core → GPIO | 按 §九 配置 User Label |
| 10 | Pinout & Configuration → Middleware → FREERTOS | 按 §七 配置 |
| 11 | Pinout & Configuration → System Core → NVIC | 按 §八 配置中断优先级 |
| 12 | Pinout & Configuration → System Core → IWDG | 按 §十二 配置看门狗 |
| 13 | Clock Configuration | 按 §五 配置时钟树 |
| 14 | Project Manager → Project | 工程名 SmartLock，工具链 STM32CubeIDE |
| 15 | Project Manager → Code Generator | 按 §十四 勾选 |
| 16 | Project Manager → Advanced Settings | 确认驱动选择 |
| 17 | GENERATE CODE | 生成代码 |

---

## 十六、HAL 初始化代码关键参数含义

> 以下说明 CubeMX 生成代码中的关键函数和参数，便于理解生成后的代码结构。

### 16.1 SystemClock_Config()

```c
RCC_OscInitTypeDef RCC_OscInitStruct = {0};
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSI;
RCC_OscInitStruct.HSEState = RCC_HSE_ON;          // HSE 开启
RCC_OscInitStruct.LSIState = RCC_LSI_ON;           // LSI 开启（IWDG + RTC 用）
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;       // PLL 开启
RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE; // PLL 源 = HSE
RCC_OscInitStruct.PLL.PLLM = 8;                    // HSE/8 = 1MHz
RCC_OscInitStruct.PLL.PLLN = 336;                  // ×336 = 336MHz VCO
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;        // /2 = 168MHz SYSCLK
RCC_OscInitStruct.PLL.PLLQ = 7;                    // /7 = 48MHz USB/RNG
HAL_RCC_OscConfig(&RCC_OscInitStruct);

RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;  // 系统时钟 = PLL
RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;         // AHB = SYSCLK/1 = 168MHz
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;          // APB1 = HCLK/4 = 42MHz
RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;          // APB2 = HCLK/2 = 84MHz
HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);  // 168MHz 需 5 等待周期
```

### 16.2 HAL_Init() 中的 NVIC 配置

```c
HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);  // 4 位抢占，0 位子优先级
HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);            // SysTick 优先级 = 15
```

### 16.3 FreeRTOSConfig.h 关键确认项

```c
#define configCPU_CLOCK_HZ                       ( 168000000UL )
#define configTICK_RATE_HZ                       ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                     ( 8 )
#define configMINIMAL_STACK_SIZE                 ( ( uint16_t ) 128 )
#define configTOTAL_HEAP_SIZE                    ( ( size_t ) 20480 )
#define configCHECK_FOR_STACK_OVERFLOW           2
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       15
```

---

## 附录：引脚占用汇总

| 端口 | 已占用引脚 | 用途 |
|---|---|---|
| PA | 2, 3, 4, 5, 6, 7, 9, 10, 13, 14 | USART1/2, SPI1, SWD |
| PB | 6, 7, 10, 11, 12, 13, 14, 15 | I2C1, USART3, SPI2 |
| PC | 0, 1, 2, 3, 4, 5, 6, 7 | 矩阵键盘（全部 PC） |
| PD | —（未使用） | 保留 |
| PE | 0, 1 | 继电器、蜂鸣器 |
| **总计** | **24 引脚** | |

> 剩余可用引脚：约 58 个 GPIO（含部分复用冲突限制），足够后续扩展。



