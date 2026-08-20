/**
 * @file presenter_player.c
 * @brief 播放器 Presenter 实现（不依赖 lvgl.h，只依赖 port / model / view 接口）。
 *
 * 数据流：
 *   用户点按钮 -> view 回调 -> presenter -> model 改状态 -> presenter 读回 -> view 刷新
 *   时间流逝   -> app_update -> presenter.update -> model.tick -> view 刷新进度
 */
#include "presenter/presenter_player.h"
#include "port/port.h"
#include "model/model_player.h"
#include "model/model_playlist.h"
#include "ui/view_player.h"

static player_model_t s_model;
static uint32_t s_last_tick = 0;
static uint32_t s_last_shown_sec = 0;

/* 刷新当前曲目相关的显示（歌名/歌手/封面/进度） */
static void refresh_track(void)
{
    const track_t * t = playlist_get(s_model.track_index);
    view_player_set_track(t->title, t->artist);
    view_player_set_cover(t->cover);
    view_player_set_progress(s_model.elapsed_ms, t->duration_ms);
    s_last_shown_sec = s_model.elapsed_ms / 1000;
}

static void refresh_all(void)
{
    refresh_track();
    view_player_set_playing(s_model.playing);
    view_player_set_volume(s_model.volume);
    view_player_set_bt_connected(s_model.bt_connected);
    view_player_set_battery(s_model.battery);
}

/* ---------- View 事件回调 ---------- */
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

static void on_bt_toggle(void)
{
    player_model_toggle_bt(&s_model);
    view_player_set_bt_connected(s_model.bt_connected);
}

static void on_seek(uint32_t ms)
{
    player_model_seek(&s_model, ms);
    const track_t * t = playlist_get(s_model.track_index);
    view_player_set_progress(s_model.elapsed_ms, t->duration_ms);
}

void presenter_player_init(void)
{
    player_model_init(&s_model);
    s_last_tick = port_get()->tick_ms();

    view_player_events_t ev = {
        .on_play_pause      = on_play_pause,
        .on_prev            = on_prev,
        .on_next            = on_next,
        .on_volume_changed  = on_volume_changed,
        .on_bt_toggle       = on_bt_toggle,
        .on_seek            = on_seek,
    };
    view_player_init(&ev);
    refresh_all();
}

void presenter_player_update(void)
{
    uint32_t now = port_get()->tick_ms();
    uint32_t dt = now - s_last_tick;
    s_last_tick = now;

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
