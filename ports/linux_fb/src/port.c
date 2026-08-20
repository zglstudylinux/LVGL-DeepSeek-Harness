/**
 * @file port.c
 * @brief Linux framebuffer 平台模板（未验证，按实际板卡调整）。
 *
 * 目标：运行 Linux 的板卡（如 i.MX / 全志 / 树莓派），用 framebuffer 显示 + evdev 输入。
 * 实现要点：使用 LVGL 内置驱动（无需自写）：
 *   - 显示：lvgl/drivers/display/lv_linux_fbdev.h   -> lv_linux_fbdev_create()
 *   - 输入：lvgl/drivers/indev/lv_evdev.h            -> lv_evdev_create()
 *   - tick：clock_gettime(CLOCK_MONOTONIC)
 *
 * 移植步骤见同目录 README.md。
 */
#include "port/port.h"
#include "lvgl/lvgl.h"
#include <time.h>

#define FB_DEV      "/dev/fb0"
#define TOUCH_DEV   "/dev/input/event0"   /* 按实际触摸/鼠标设备调整 */

static void port_init_impl(void)
{
    /* TODO: 按板卡填充（参考 lv_linux_fbdev / lv_evdev 官方驱动用法） */
    /*
    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, FB_DEV);

    lv_indev_t * ts = lv_evdev_create(LV_INDEV_TYPE_POINTER, TOUCH_DEV);
    lv_indev_set_display(ts, disp);
    */
}

static uint32_t port_tick_ms_impl(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000UL + ts.tv_nsec / 1000000UL);
}

static void port_delay_ms_impl(uint32_t ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
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
