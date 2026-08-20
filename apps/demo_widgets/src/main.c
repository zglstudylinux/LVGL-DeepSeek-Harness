/**
 * @file main.c
 * @brief demo_widgets 应用入口 —— 官方控件示例，用于验证环境与脚手架。
 */
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "port/port.h"
#include "app/app.h"

void app_init(void)
{
    /* 运行 LVGL 官方控件示例 */
    lv_demo_widgets();
}

int main(void)
{
    lv_init();
    port_get()->init();
    app_init();
    app_run();          /* 统一主循环：lv_timer_handler + app_update + delay */
    return 0;
}
