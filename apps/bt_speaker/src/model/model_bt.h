/**
 * @file model_bt.h
 * @brief 蓝牙设备列表模型（纯 C，零 LVGL 依赖）。模拟扫描/发现/连接流程。
 */
#ifndef APP_MODEL_BT_H
#define APP_MODEL_BT_H

#include <stdbool.h>
#include <stdint.h>

#define BT_DEVICE_MAX 6

typedef struct {
    char     name[32];
    char     addr[18];   /* "AA:BB:CC:DD:EE:FF" */
    bool     paired;
    bool     connected;
} bt_device_t;

typedef struct {
    bool         scanning;
    bt_device_t  devices[BT_DEVICE_MAX];
    uint8_t      device_count;
    int8_t       connected_index;   /* -1 表示无连接 */
    uint32_t     reveal_acc_ms;     /* 扫描计时 */
    uint8_t      next_reveal;       /* 下一个待发现设备下标 */
} bt_model_t;

void bt_model_init(bt_model_t * m);
void bt_model_start_scan(bt_model_t * m);
void bt_model_stop_scan(bt_model_t * m);
void bt_model_connect(bt_model_t * m, uint8_t index);
void bt_model_disconnect(bt_model_t * m);
void bt_model_tick(bt_model_t * m, uint32_t dt_ms);   /* 扫描时逐个「发现」设备 */

uint8_t            bt_model_get_count(const bt_model_t * m);
const bt_device_t * bt_model_get(const bt_model_t * m, uint8_t index);
bool               bt_model_is_scanning(const bt_model_t * m);
int8_t             bt_model_get_connected(const bt_model_t * m);
bool               bt_model_is_any_connected(const bt_model_t * m);

#endif /* APP_MODEL_BT_H */
