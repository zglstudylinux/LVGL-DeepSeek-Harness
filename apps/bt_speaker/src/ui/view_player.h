/**
 * @file view_player.h
 * @brief 播放器界面 View 层接口：只负责画界面与回调，不写业务规则。
 */
#ifndef APP_UI_VIEW_PLAYER_H
#define APP_UI_VIEW_PLAYER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    void (*on_play_pause)(void);
    void (*on_prev)(void);
    void (*on_next)(void);
    void (*on_volume_changed)(uint8_t vol);   /* 0..100 */
    void (*on_bt_toggle)(void);
    void (*on_seek)(uint32_t ms);             /* 拖动进度条 */
} view_player_events_t;

void view_player_init(const view_player_events_t * ev);

void view_player_set_cover(uint8_t index);                 /* 0..2 */
void view_player_set_track(const char * title, const char * artist);
void view_player_set_playing(bool playing);
void view_player_set_progress(uint32_t elapsed_ms, uint32_t total_ms);
void view_player_set_volume(uint8_t vol);
void view_player_set_bt_connected(bool connected);
void view_player_set_battery(uint8_t pct);

#endif /* APP_UI_VIEW_PLAYER_H */
