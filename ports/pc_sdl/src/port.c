/**
 * @file port.c
 * @brief PC (SDL2) 平台实现 —— LVGL 模拟器开发主战场。
 *
 * 使用 LVGL 内置 SDL 驱动：显示窗口 + 鼠标 + 滚轮 + 键盘 + tick。
 * 本文件是"平台差异"的收口点：移植到 MCU/Linux 时，新增 ports/<name>/ 实现同样的 port_get() 即可。
 */
#include "port/port.h"
#include "lvgl/lvgl.h"
#include <SDL.h>

/* 鼠标光标图标（由 tools/img_conv 从 PNG 生成） */
LV_IMAGE_DECLARE(mouse_cursor_icon);

/* 模拟显示分辨率由 CMake 传入（LVGL_SIM_HOR_RES / LVGL_SIM_VER_RES，默认 480×640）。
 * 通过 ports/pc_sdl/CMakeLists.txt 的 target_compile_definitions 注入。 */
#ifndef LVGL_SIM_HOR_RES
#define LVGL_SIM_HOR_RES 480
#endif
#ifndef LVGL_SIM_VER_RES
#define LVGL_SIM_VER_RES 640
#endif

static void port_init_impl(void)
{
    /* 默认输入组 */
    lv_group_set_default(lv_group_create());

    /* 显示：创建 SDL 窗口并设为默认显示 */
    lv_display_t * disp = lv_sdl_window_create(LVGL_SIM_HOR_RES, LVGL_SIM_VER_RES);
    lv_display_set_default(disp);

    /* 鼠标 + 光标图标 */
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, disp);

    lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse, cursor_obj);

    /* 滚轮 */
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_group(mousewheel, lv_group_get_default());
    lv_indev_set_display(mousewheel, disp);

    /* 键盘 */
    lv_indev_t * kb = lv_sdl_keyboard_create();
    lv_indev_set_group(kb, lv_group_get_default());
    lv_indev_set_display(kb, disp);
}

static uint32_t port_tick_ms_impl(void)
{
    return (uint32_t)SDL_GetTicks();
}

static void port_delay_ms_impl(uint32_t ms)
{
    SDL_Delay(ms);
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
