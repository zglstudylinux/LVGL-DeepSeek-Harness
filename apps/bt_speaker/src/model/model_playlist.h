/**
 * @file model_playlist.h
 * @brief 曲目列表模型（纯 C，零 LVGL 依赖）。
 */
#ifndef APP_MODEL_PLAYLIST_H
#define APP_MODEL_PLAYLIST_H

#include <stdint.h>

typedef struct {
    const char * title;
    const char * artist;
    uint32_t     duration_ms;
    uint8_t      cover;   /* 0..2 对应 demo 封面 */
} track_t;

uint32_t playlist_count(void);
const track_t * playlist_get(uint32_t index);

#endif /* APP_MODEL_PLAYLIST_H */
