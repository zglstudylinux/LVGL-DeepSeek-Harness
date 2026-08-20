/**
 * @file view_main.c
 * @brief View 层实现：只负责画界面、把用户操作转成回调，不写业务规则。
 */
#include "ui/view_main.h"
#include "lvgl/lvgl.h"
#include <stdio.h>

static lv_obj_t * s_count_label = NULL;
static view_main_increment_cb_t s_increment_cb = NULL;

static void btn_clicked_cb(lv_event_t * e)
{
    (void)e;
    if (s_increment_cb) {
        s_increment_cb();
    }
}

void view_main_init(view_main_increment_cb_t on_increment)
{
    s_increment_cb = on_increment;
    lv_obj_t * scr = lv_screen_active();

    lv_obj_t * title = lv_label_create(scr);
    lv_label_set_text(title, "Hello LVGL!");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

    s_count_label = lv_label_create(scr);
    lv_label_set_text(s_count_label, "Count: 0");
    lv_obj_align(s_count_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * btn = lv_button_create(scr);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_obj_add_event_cb(btn, btn_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Increment");
    lv_obj_center(btn_label);
}

void view_main_set_count(uint32_t count)
{
    if (s_count_label) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Count: %u", (unsigned)count);
        lv_label_set_text(s_count_label, buf);
    }
}
