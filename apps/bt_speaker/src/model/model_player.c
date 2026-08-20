/**
 * @file model_player.c
 * @brief 播放器业务逻辑实现（纯 C，无 LVGL）。
 */
#include "model/model_player.h"
#include "model/model_playlist.h"

void player_model_init(player_model_t * m)
{
    m->playing = true;       /* 启动即播放，便于演示进度推进 */
    m->track_index = 0;
    m->elapsed_ms = 0;
    m->volume = 60;
    m->bt_connected = true;
    m->battery = 80;
}

void player_model_toggle_play(player_model_t * m)
{
    m->playing = !m->playing;
}

void player_model_next(player_model_t * m)
{
    uint32_t n = playlist_count();
    m->track_index = (m->track_index + 1) % n;
    m->elapsed_ms = 0;
}

void player_model_prev(player_model_t * m)
{
    uint32_t n = playlist_count();
    m->track_index = (m->track_index + n - 1) % n;
    m->elapsed_ms = 0;
}

void player_model_set_volume(player_model_t * m, uint8_t vol)
{
    m->volume = (vol > 100) ? 100 : vol;
}

void player_model_toggle_bt(player_model_t * m)
{
    m->bt_connected = !m->bt_connected;
}

void player_model_seek(player_model_t * m, uint32_t ms)
{
    const track_t * t = playlist_get(m->track_index);
    m->elapsed_ms = (ms > t->duration_ms) ? t->duration_ms : ms;
}

void player_model_tick(player_model_t * m, uint32_t dt_ms)
{
    if(!m->playing) {
        return;
    }
    m->elapsed_ms += dt_ms;

    const track_t * t = playlist_get(m->track_index);
    if(m->elapsed_ms >= t->duration_ms) {
        player_model_next(m);   /* 到尾自动下一首 */
    }
}

bool player_model_is_playing(const player_model_t * m)
{
    return m->playing;
}

uint32_t player_model_get_track_index(const player_model_t * m)
{
    return m->track_index;
}

uint32_t player_model_get_elapsed_ms(const player_model_t * m)
{
    return m->elapsed_ms;
}

uint8_t player_model_get_volume(const player_model_t * m)
{
    return m->volume;
}

bool player_model_is_bt_connected(const player_model_t * m)
{
    return m->bt_connected;
}

uint8_t player_model_get_battery(const player_model_t * m)
{
    return m->battery;
}
