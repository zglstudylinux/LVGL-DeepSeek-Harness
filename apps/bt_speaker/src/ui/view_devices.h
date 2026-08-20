/**
 * @file view_devices.h
 * @brief 蓝牙设备列表 View 层接口：负责绘制设备列表屏与回调。
 */
#ifndef APP_UI_VIEW_DEVICES_H
#define APP_UI_VIEW_DEVICES_H

#include <stdbool.h>
#include <stdint.h>

#define VIEW_DEVICES_MAX 6

typedef struct {
    void (*on_back)(void);
    void (*on_scan_toggle)(void);
    void (*on_device_click)(uint8_t index);
} view_devices_events_t;

void view_devices_init(const view_devices_events_t * ev);
void view_devices_show(void);   /* 切换到设备列表屏 */
void view_devices_clear(void);  /* 清空列表（开始新扫描前调用） */
void view_devices_set_scanning(bool scanning, uint8_t found_count);
void view_devices_set_device(uint8_t index, const char * name, const char * addr, bool paired);
void view_devices_set_connected(uint8_t index, bool connected);

#endif /* APP_UI_VIEW_DEVICES_H */
