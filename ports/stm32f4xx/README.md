# ports/stm32f4xx —— 单片机平台移植模板

> 状态：**模板**。MCU 项目依赖厂商 HAL 与 IDE 工程，不走本仓库顶层 CMake，
> 采用"拷贝清单"方式把源码加入你的 Keil / IAR / STM32CubeIDE 工程。

## 拷贝清单（加入你的 MCU 工程）
| 源码目录 | 作用 |
|---------|------|
| `lvgl/src/` | LVGL 库核心 |
| `lvgl/include/` | LVGL 头文件（加入 include 路径） |
| `apps/<你的应用>/` | 你的 UI 应用源码（含 `apps/<你的应用>/lv_conf.h`） |
| `shared/` | 平台接口 + 应用框架 + utils（加入 include 路径） |
| `ports/stm32f4xx/src/port.c` | 平台实现（补全 LCD/触摸/tick） |

## 移植步骤
1. 复制本目录为 `ports/<你的芯片>/`，补全 `src/port.c`：
   - `init()`：初始化 LCD（`lv_display_create()` + 自写 `flush_cb`，或用 `lv_st7789` / `lv_ili9341` 等内置驱动）与触摸。
   - `tick_ms()`：返回 `HAL_GetTick()` 等单调毫秒。
   - `delay_ms()`：裸机用 `HAL_Delay`，RTOS 用 `vTaskDelay`。
2. 设置 include 路径（都用相对路径，不要写绝对路径）：
   - `lvgl/include`
   - `shared`
   - `apps/<你的应用>/src`
3. 设置宏：
   - `LV_CONF_INCLUDE_SIMPLE`（并把 `apps/<你的应用>/lv_conf.h` 所在目录加入 include 路径）
   - 按你的 RTOS 设置 `LV_USE_OS`（裸机保持 0）
4. 主循环：`apps/<你的应用>/src/main.c` 的骨架可直接照搬进 RTOS 任务或裸机 `while(1)`；
   唯一区别是 `port_get()->delay_ms()` 在 RTOS 下换成任务让出。

## 原理
MCU 移植只改本目录的 `port.c`；你的 UI（`apps/*`）与业务（`model/service`）一行不改。
