/**
 * @file presenter_player.c
 * @brief 播放器 Presenter 实现（不依赖 lvgl.h，只依赖 port / model / view 接口）。
 *
 * 数据流：
 *   用户点按钮  -> view 回调 -> presenter -> model 改状态 -> presenter 读回 -> view 刷新
 *   时间流逝    -> app_update -> presenter.update -> model.tick -> view 刷新进度
 *   屏幕导航    -> view 事件 -> presenter 决定切换 -> view_xxx_show()
 */
#include "presenter/presenter_player.h"
#include "port/port.h"
#include "model/model_player.h"
#include "model/model_playlist.h"
#include "model/model_bt.h"
#include "ui/view_player.h"
#include "ui/view_devices.h"

static player_model_t s_model;
static bt_model_t s_bt;
static uint32_t s_last_tick = 0;
static uint32_t s_last_shown_sec = 0;
static uint8_t  s_bt_shown_count = 0;
static bool     s_last_scanning = false;
static uint8_t  s_last_count = 0;

/* 刷新当前曲目相关显示（歌名/歌手/封面/进度） */
static void refresh_track(void)
{
    const track_t * t = playlist_get(s_model.track_index);
    view_player_set_track(t->title, t->artist);
    view_player_set_cover(t->cover);
    view_player_set_progress(s_model.elapsed_ms, t->duration_ms);
    s_last_shown_sec = s_model.elapsed_ms / 1000;
}

/* 蓝牙连接状态联动（正在播放屏状态栏 + 设备列表徽标） */
static void refresh_bt_state(void)
{
    s_model.bt_connected = bt_model_is_any_connected(&s_bt);
    view_player_set_bt_connected(s_model.bt_connected);

    int8_t conn = bt_model_get_connected(&s_bt);
    uint8_t count = bt_model_get_count(&s_bt);
    for(uint8_t i = 0; i < count; i++) {
        view_devices_set_connected(i, (int8_t)i == conn);
    }
}

/* 扫描结果同步到设备列表视图（只推送新发现的设备） */
static void refresh_scan_state(void)
{
    uint8_t count = bt_model_get_count(&s_bt);
    view_devices_set_scanning(bt_model_is_scanning(&s_bt), count);
    for(uint8_t i = s_bt_shown_count; i < count; i++) {
        const bt_device_t * d = bt_model_get(&s_bt, i);
        view_devices_set_device(i, d->name, d->addr, d->paired);
        view_devices_set_connected(i, d->connected);
    }
    s_bt_shown_count = count;
}

static void refresh_all(void)
{
    refresh_track();
    view_player_set_playing(s_model.playing);
    view_player_set_volume(s_model.volume);
    view_player_set_bt_connected(s_model.bt_connected);
    view_player_set_battery(s_model.battery);
}

/* ---------- 「正在播放」屏事件 ---------- */
static void on_play_pause(void)
{
    player_model_toggle_play(&s_model);
    view_player_set_playing(s_model.playing);
}

static void on_prev(void)
{
    player_model_prev(&s_model);
    refresh_track();
}

static void on_next(void)
{
    player_model_next(&s_model);
    refresh_track();
}

static void on_volume_changed(uint8_t vol)
{
    player_model_set_volume(&s_model, vol);
}

static void on_seek(uint32_t ms)
{
    player_model_seek(&s_model, ms);
    const track_t * t = playlist_get(s_model.track_index);
    view_player_set_progress(s_model.elapsed_ms, t->duration_ms);
}

static void on_bt_open(void)
{
    bt_model_start_scan(&s_bt);
    s_bt_shown_count = 0;
    view_devices_clear();
    view_devices_set_scanning(true, 0);
    view_devices_show();
}

/* ---------- 设备列表屏事件 ---------- */
static void on_back(void)
{
    bt_model_stop_scan(&s_bt);
    view_player_show();
}

static void on_scan_toggle(void)
{
    bt_model_start_scan(&s_bt);
    s_bt_shown_count = 0;
    view_devices_clear();
    view_devices_set_scanning(true, 0);
}

static void on_device_click(uint8_t index)
{
    bt_model_connect(&s_bt, index);
    refresh_bt_state();
}

void presenter_player_init(void)
{
    player_model_init(&s_model);
    bt_model_init(&s_bt);
    s_last_tick = port_get()->tick_ms();

    /* view_player 先初始化（内含深色主题），再初始化第二屏 */
    view_player_events_t pev = {
        .on_play_pause     = on_play_pause,
        .on_prev           = on_prev,
        .on_next           = on_next,
        .on_volume_changed = on_volume_changed,
        .on_bt_open        = on_bt_open,
        .on_seek           = on_seek,
    };
    view_player_init(&pev);

    view_devices_events_t dev = {
        .on_back          = on_back,
        .on_scan_toggle   = on_scan_toggle,
        .on_device_click  = on_device_click,
    };
    view_devices_init(&dev);

    view_player_show();
    refresh_all();
}

void presenter_player_update(void)
{
    uint32_t now = port_get()->tick_ms();
    uint32_t dt = now - s_last_tick;
    s_last_tick = now;

    /* 蓝牙扫描推进 */
    bt_model_tick(&s_bt, dt);
    bool scanning = bt_model_is_scanning(&s_bt);
    uint8_t count = bt_model_get_count(&s_bt);
    if(scanning != s_last_scanning || count != s_last_count) {
        refresh_scan_state();
        s_last_scanning = scanning;
        s_last_count = count;
    }

    /* 播放进度推进 */
    if(!s_model.playing) {
        return;
    }

    uint32_t idx_before = s_model.track_index;
    player_model_tick(&s_model, dt);

    /* 自动切歌时刷新封面/歌名 */
    if(s_model.track_index != idx_before) {
        refresh_track();
        return;
    }

    /* 仅在秒变化时刷新进度显示（节流，避免每帧刷 UI） */
    uint32_t sec = s_model.elapsed_ms / 1000;
    if(sec != s_last_shown_sec) {
        s_last_shown_sec = sec;
        const track_t * t = playlist_get(s_model.track_index);
        view_player_set_progress(s_model.elapsed_ms, t->duration_ms);
    }
}
