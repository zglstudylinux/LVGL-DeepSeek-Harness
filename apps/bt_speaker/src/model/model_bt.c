/**
 * @file model_bt.c
 * @brief 蓝牙设备列表业务逻辑（纯 C）。占位数据；接真实蓝牙栈时由 service 层替换。
 */
#include "model/model_bt.h"
#include <string.h>

/* 预置周边设备（演示用） */
static const bt_device_t s_all[BT_DEVICE_MAX] = {
    { "AirPods Pro",   "AA:BB:CC:11:22:33", true,  false },
    { "SoundCore 2",   "AA:BB:CC:44:55:66", true,  false },
    { "JBL Flip 6",    "AA:BB:CC:77:88:99", false, false },
    { "Bose Mini II",  "AA:BB:CC:AA:BB:CC", true,  false },
    { "Sony SRS-XB33", "AA:BB:CC:DD:EE:01", false, false },
    { "Anker Rave",    "AA:BB:CC:DD:EE:02", false, false },
};

#define REVEAL_INTERVAL_MS 650   /* 每隔 650ms「发现」一个设备 */

void bt_model_init(bt_model_t * m)
{
    memset(m, 0, sizeof(*m));
    m->connected_index = -1;
}

void bt_model_start_scan(bt_model_t * m)
{
    m->scanning = true;
    m->device_count = 0;
    m->reveal_acc_ms = 0;
    m->next_reveal = 0;
}

void bt_model_stop_scan(bt_model_t * m)
{
    m->scanning = false;
}

void bt_model_connect(bt_model_t * m, uint8_t index)
{
    if(index >= m->device_count) {
        return;
    }
    /* 先断开旧连接 */
    if(m->connected_index >= 0) {
        m->devices[m->connected_index].connected = false;
    }
    m->devices[index].connected = true;
    m->connected_index = (int8_t)index;
}

void bt_model_disconnect(bt_model_t * m)
{
    if(m->connected_index >= 0) {
        m->devices[m->connected_index].connected = false;
        m->connected_index = -1;
    }
}

void bt_model_tick(bt_model_t * m, uint32_t dt_ms)
{
    if(!m->scanning) {
        return;
    }
    m->reveal_acc_ms += dt_ms;
    while(m->next_reveal < BT_DEVICE_MAX && m->reveal_acc_ms >= REVEAL_INTERVAL_MS) {
        m->reveal_acc_ms -= REVEAL_INTERVAL_MS;
        m->devices[m->next_reveal] = s_all[m->next_reveal];
        /* 重扫后保留已连接设备的标记 */
        if((int8_t)m->next_reveal == m->connected_index) {
            m->devices[m->next_reveal].connected = true;
        }
        m->next_reveal++;
        m->device_count++;
    }
    if(m->next_reveal >= BT_DEVICE_MAX) {
        m->scanning = false;
    }
}

uint8_t bt_model_get_count(const bt_model_t * m)
{
    return m->device_count;
}

const bt_device_t * bt_model_get(const bt_model_t * m, uint8_t index)
{
    if(index >= m->device_count) {
        return NULL;
    }
    return &m->devices[index];
}

bool bt_model_is_scanning(const bt_model_t * m)
{
    return m->scanning;
}

int8_t bt_model_get_connected(const bt_model_t * m)
{
    return m->connected_index;
}

bool bt_model_is_any_connected(const bt_model_t * m)
{
    return m->connected_index >= 0;
}
