# AGENTS.md — 应用层

本目录存放 FreeRTOS 任务（task_*.c/h）和 main.c 入口。

## 规则

1. **任务间只走 IPC**。Queue / Semaphore / EventGroup，禁止共享全局变量。
2. **优先级自上而下**。认证任务最高（5），监控任务最低（0）。
3. **公开接口判空**。被中断回调或队列接收后调用的函数，第一个参数必须做 NULL 检查。
4. **不直接操作寄存器**。所有硬件操作通过驱动层。
5. **vTaskDelayUntil 优先于 vTaskDelay**。周期性任务用 vTaskDelayUntil。
6. **栈大小保守设置**。新任务先设 512 words，运行后用 uxTaskGetStackHighWaterMark 检查实际用量。
