# AGENTS.md — 驱动层

本目录存放所有外设驱动（drv_*.c/h）。驱动层只和 HAL 打交道，不依赖应用层和 FreeRTOS。

## 2. 规则

1. **通信类操作必须返回错误**。SPI 收发、I2C 读写、UART 带超时应答——返回 int，0 成功，负数错误码。纯 GPIO 操作、显示绘制可以返回 void。
2. **不调 FreeRTOS API**。禁止 vTaskDelay、xQueueSend 等。唯一例外：FreeRTOS Timer 回调。
3. **静态持句柄**。外设句柄用 static 全局变量持有。
4. **避免忙等**。轮询状态的循环必须带 timeout。
5. **新增驱动不影响现有**。不修改 main.h 中已有的宏和结构体。
