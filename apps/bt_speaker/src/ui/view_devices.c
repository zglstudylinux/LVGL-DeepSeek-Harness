/**
 * @file view_devices.c
 * @brief 蓝牙设备列表屏 View 实现（UI 层，唯一直接使用 LVGL 的代码之一）。
 */
#include "ui/view_devices.h"
#include "lvgl/lvgl.h"
#include <stdint.h>
#include <stdio.h>

/* 与 view_player 一致的深色配色 */
#define COLOR_BG       0x101418
#define COLOR_CARD     0x1C2128
#define COLOR_ACCENT   0x4FC3F7
#define COLOR_TEXT     0xE6EDF3
#define COLOR_TEXT_DIM 0x8B949E

typedef struct {
    lv_obj_t * btn;
    lv_obj_t * name;
    lv_obj_t * addr;
    lv_obj_t * status;
    bool       paired;
    bool       connected;
} row_t;

static view_devices_events_t s_ev;
static lv_obj_t * s_screen;
static lv_obj_t * s_status;
static row_t s_rows[VIEW_DEVICES_MAX];

/* ---------- 事件回调 ---------- */
static void back_cb(lv_event_t * e)
{
    (void)e;
    if(s_ev.on_back) s_ev.on_back();
}

static void scan_cb(lv_event_t * e)
{
    (void)e;
    if(s_ev.on_scan_toggle) s_ev.on_scan_toggle();
}

static void row_click_cb(lv_event_t * e)
{
    uint8_t i = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    if(s_ev.on_device_click) s_ev.on_device_click(i);
}

/* 刷新某行的右侧状态徽标 */
static void update_row_status(uint8_t index)
{
    row_t * r = &s_rows[index];
    if(r->connected) {
        lv_label_set_text(r->status, LV_SYMBOL_BLUETOOTH " Connected");
        lv_obj_set_style_text_color(r->status, lv_color_hex(COLOR_ACCENT), 0);
    }
    else if(r->paired) {
        lv_label_set_text(r->status, "Paired");
        lv_obj_set_style_text_color(r->status, lv_color_hex(COLOR_TEXT_DIM), 0);
    }
    else {
        lv_label_set_text(r->status, LV_SYMBOL_RIGHT);
        lv_obj_set_style_text_color(r->status, lv_color_hex(COLOR_TEXT_DIM), 0);
    }
}

static void create_row(uint8_t i)
{
    lv_obj_t * btn = lv_button_create(s_screen);
    lv_obj_set_size(btn, 448, 76);
    lv_obj_set_pos(btn, 16, 124 + i * 84);
    lv_obj_set_style_radius(btn, 14, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_0, 0);
    lv_obj_add_event_cb(btn, row_click_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);

    lv_obj_t * name = lv_label_create(btn);
    lv_label_set_text(name, "");
    lv_obj_set_style_text_font(name, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(name, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_set_pos(name, 16, 14);

    lv_obj_t * addr = lv_label_create(btn);
    lv_label_set_text(addr, "");
    lv_obj_set_style_text_font(addr, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(addr, lv_color_hex(COLOR_TEXT_DIM), 0);
    lv_obj_set_pos(addr, 16, 44);

    lv_obj_t * status = lv_label_create(btn);
    lv_label_set_text(status, "");
    lv_obj_set_style_text_font(status, &lv_font_montserrat_14, 0);
    lv_obj_align(status, LV_ALIGN_RIGHT_MID, -16, 0);

    s_rows[i].btn = btn;
    s_rows[i].name = name;
    s_rows[i].addr = addr;
    s_rows[i].status = status;
    s_rows[i].paired = false;
    s_rows[i].connected = false;
    lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
}

void view_devices_init(const view_devices_events_t * ev)
{
    s_ev = *ev;

    /* 第二块屏幕：用独立 lv_obj，主题由 view_player_init 统一设置 */
    s_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(s_screen, lv_color_hex(COLOR_BG), 0);
    lv_obj_set_style_bg_opa(s_screen, LV_OPA_COVER, 0);

    /* ---- 头部：返回 + 标题 ---- */
    lv_obj_t * back = lv_button_create(s_screen);
    lv_obj_set_size(back, 48, 48);
    lv_obj_set_pos(back, 16, 16);
    lv_obj_set_style_radius(back, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(back, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_opa(back, LV_OPA_0, 0);
    lv_obj_add_event_cb(back, back_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * back_icon = lv_label_create(back);
    lv_label_set_text(back_icon, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_font(back_icon, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(back_icon, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_center(back_icon);

    lv_obj_t * title = lv_label_create(s_screen);
    lv_label_set_text(title, "Bluetooth Devices");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 26);

    /* ---- 扫描状态 + 扫描按钮 ---- */
    s_status = lv_label_create(s_screen);
    lv_label_set_text(s_status, "");
    lv_obj_set_style_text_font(s_status, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_status, lv_color_hex(COLOR_TEXT_DIM), 0);
    lv_obj_set_pos(s_status, 24, 92);

    lv_obj_t * scan_btn = lv_button_create(s_screen);
    lv_obj_set_size(scan_btn, 44, 44);
    lv_obj_set_pos(scan_btn, 420, 80);
    lv_obj_set_style_radius(scan_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(scan_btn, lv_color_hex(COLOR_ACCENT), 0);
    lv_obj_set_style_bg_opa(scan_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_opa(scan_btn, LV_OPA_0, 0);
    lv_obj_add_event_cb(scan_btn, scan_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * scan_icon = lv_label_create(scan_btn);
    lv_label_set_text(scan_icon, LV_SYMBOL_REFRESH);
    lv_obj_set_style_text_font(scan_icon, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(scan_icon, lv_color_hex(0x0B1220), 0);
    lv_obj_center(scan_icon);

    /* ---- 设备行（预创建 6 行，默认隐藏） ---- */
    for(uint8_t i = 0; i < VIEW_DEVICES_MAX; i++) {
        create_row(i);
    }
}

void view_devices_show(void)
{
    lv_scr_load(s_screen);
}

void view_devices_clear(void)
{
    for(uint8_t i = 0; i < VIEW_DEVICES_MAX; i++) {
        lv_obj_add_flag(s_rows[i].btn, LV_OBJ_FLAG_HIDDEN);
        s_rows[i].paired = false;
        s_rows[i].connected = false;
        lv_label_set_text(s_rows[i].name, "");
        lv_label_set_text(s_rows[i].addr, "");
        lv_label_set_text(s_rows[i].status, "");
    }
}

void view_devices_set_scanning(bool scanning, uint8_t found_count)
{
    if(scanning) {
        lv_label_set_text(s_status, "Scanning...");
    }
    else if(found_count == 0) {
        lv_label_set_text(s_status, "No devices found");
    }
    else {
        char buf[32];
        snprintf(buf, sizeof(buf), "%u device%s found",
                 (unsigned)found_count, found_count > 1 ? "s" : "");
        lv_label_set_text(s_status, buf);
    }
}

void view_devices_set_device(uint8_t index, const char * name, const char * addr, bool paired)
{
    if(index >= VIEW_DEVICES_MAX) return;
    row_t * r = &s_rows[index];
    lv_label_set_text(r->name, name);
    lv_label_set_text(r->addr, addr);
    r->paired = paired;
    lv_obj_clear_flag(r->btn, LV_OBJ_FLAG_HIDDEN);
    update_row_status(index);
}

void view_devices_set_connected(uint8_t index, bool connected)
{
    if(index >= VIEW_DEVICES_MAX) return;
    s_rows[index].connected = connected;
    update_row_status(index);
}
