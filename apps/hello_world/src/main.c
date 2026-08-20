/**
 * @file main.c
 * @brief hello_world 应用入口 —— 最小四层结构模板（View / Presenter / Model）。
 *
 * 所有应用共用此极薄骨架，平台差异完全由 port 屏蔽。
 */
#include "lvgl/lvgl.h"
#include "port/port.h"
#include "app/app.h"
#include "presenter/presenter_main.h"

void app_init(void)
{
    presenter_main_init();
}

int main(void)
{
    lv_init();
    port_get()->init();
    app_init();
    app_run();          /* 统一主循环 */
    return 0;
}
