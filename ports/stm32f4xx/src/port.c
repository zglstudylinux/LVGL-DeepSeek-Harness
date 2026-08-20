/**
 * @file port.c
 * @brief STM32F4xx 单片机平台模板（裸机 + 可选 RTOS）。
 *
 * 本文件是给 Keil / IAR / STM32CubeIDE 等厂商 IDE 的**拷贝清单模板**，
 * 不参与顶层 CMake 构建（MCU 走 IDE 工程，见同目录 README.md）。
 *
 * 实现要点：
 *   - init()     : 初始化 LCD（SPI/FSMC）与触摸，用 lv_lcd_generic_mipi / lv_st7789 /
 *                  lv_ili9341 等 LVGL 内置驱动，或 lv_display_create() 自写 flush_cb。
 *   - tick_ms()  : 用 SysTick / HAL_GetTick()。
 *   - delay_ms() : 裸机用 HAL_Delay / DWT；RTOS 用 vTaskDelay(pdMS_TO_TICKS(ms))。
 */
#include "port/port.h"
#include "lvgl/lvgl.h"

static void port_init_impl(void)
{
    /* TODO: 初始化显示与输入（按你的 LCD 控制器/触摸芯片实现） */
    /*
    lv_display_t * disp = lv_display_create(320, 480);
    lv_display_set_flush_cb(disp, my_lcd_flush_cb);   // 用户实现：打点刷屏
    lv_display_set_resolution(disp, 320, 480);
    ...
    */
}

static uint32_t port_tick_ms_impl(void)
{
    /* TODO: 返回单调毫秒，例如 HAL_GetTick() 或 SysTick 计数 */
    return 0;
}

static void port_delay_ms_impl(uint32_t ms)
{
    /* TODO: HAL_Delay(ms); 或 RTOS: vTaskDelay(pdMS_TO_TICKS(ms)); */
    (void)ms;
}

static const port_api_t s_port = {
    .init     = port_init_impl,
    .tick_ms  = port_tick_ms_impl,
    .delay_ms = port_delay_ms_impl,
};

const port_api_t * port_get(void)
{
    return &s_port;
}
