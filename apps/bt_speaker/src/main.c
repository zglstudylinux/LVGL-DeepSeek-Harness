/**
 * @file main.c
 * @brief bt_speaker 应用入口 —— 竖屏 480×640 蓝牙音箱「正在播放」界面。
 */
#include "lvgl/lvgl.h"
#include "port/port.h"
#include "app/app.h"
#include "presenter/presenter_player.h"

void app_init(void)
{
    presenter_player_init();
}

/* 每轮主循环被 app_run() 调用：推进模拟播放进度并刷新界面 */
void app_update(void)
{
    presenter_player_update();
}

int main(void)
{
    lv_init();
    port_get()->init();
    app_init();
    app_run();          /* 统一主循环 */
    return 0;
}
