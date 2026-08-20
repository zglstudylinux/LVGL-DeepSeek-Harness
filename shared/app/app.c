/**
 * @file app.c
 * @brief 应用统一主循环实现。
 */
#include "app.h"
#include "port/port.h"
#include "lvgl/lvgl.h"

/* 默认空实现，供不关心周期任务的应用使用（弱符号，可被应用覆盖） */
__attribute__((weak)) void app_update(void)
{
    /* 默认无操作 */
}

void app_run(void)
{
    while (1) {
        uint32_t sleep_ms = lv_timer_handler();
        app_update();
        /* LVGL 返回下次需醒来的时间；无待处理定时器时用 5ms 兜底 */
        port_get()->delay_ms(sleep_ms == LV_NO_TIMER_READY ? 5U : sleep_ms);
    }
}
