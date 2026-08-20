/**
 * @file model_player.h
 * @brief 播放器业务模型（纯 C，零 LVGL 依赖）。
 */
#ifndef APP_MODEL_PLAYER_H
#define APP_MODEL_PLAYER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool     playing;        /* 是否正在播放 */
    uint32_t track_index;    /* 当前曲目下标 */
    uint32_t elapsed_ms;     /* 当前曲目已播放毫秒 */
    uint8_t  volume;         /* 0..100 */
    bool     bt_connected;   /* 蓝牙连接状态 */
    uint8_t  battery;        /* 0..100 */
} player_model_t;

void player_model_init(player_model_t * m);

void player_model_toggle_play(player_model_t * m);
void player_model_next(player_model_t * m);
void player_model_prev(player_model_t * m);
void player_model_set_volume(player_model_t * m, uint8_t vol);
void player_model_toggle_bt(player_model_t * m);
void player_model_seek(player_model_t * m, uint32_t ms);

/* 播放时推进进度，到尾自动切下一首 */
void player_model_tick(player_model_t * m, uint32_t dt_ms);

bool     player_model_is_playing(const player_model_t * m);
uint32_t player_model_get_track_index(const player_model_t * m);
uint32_t player_model_get_elapsed_ms(const player_model_t * m);
uint8_t  player_model_get_volume(const player_model_t * m);
bool     player_model_is_bt_connected(const player_model_t * m);
uint8_t  player_model_get_battery(const player_model_t * m);

#endif /* APP_MODEL_PLAYER_H */
