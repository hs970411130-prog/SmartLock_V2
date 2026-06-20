# AGENTS.md — 应用层 (App/)

本目录存放 FreeRTOS 任务（task_*.c/h）和 app_init.c 入口。

> 通用约束（分层、IPC、栈、CubeMX 等）见根 `AGENTS.md`；业务术语见根 `CONTEXT.md`。

## 本层专属规则

1. **任务间只走 IPC。** Queue / Semaphore / EventGroup，禁止共享全局变量。
2. **公开接口判空。** 被中断回调或队列接收后调用的函数，第一个参数必须做 NULL 检查。
3. **不直接操作寄存器。** 所有硬件操作通过 Drivers 层。
4. **vTaskDelayUntil 优先于 vTaskDelay。** 周期性任务用 vTaskDelayUntil 保证周期稳定。