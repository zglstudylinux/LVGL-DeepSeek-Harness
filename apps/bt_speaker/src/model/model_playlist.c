/**
 * @file model_playlist.c
 * @brief 曲目列表（占位数据；接真实蓝牙音频时由 service 层提供）。
 */
#include "model/model_playlist.h"

static const track_t s_tracks[] = {
    { "Midnight Drive", "Neon Waves",  3 * 60 * 1000 + 30 * 1000, 0 },  /* 3:30 */
    { "Ocean Eyes",     "Coastal",     4 * 60 * 1000 + 5 * 1000,  1 },  /* 4:05 */
    { "Golden Hour",    "Sunset Club", 2 * 60 * 1000 + 55 * 1000, 2 },  /* 2:55 */
};

uint32_t playlist_count(void)
{
    return (uint32_t)(sizeof(s_tracks) / sizeof(s_tracks[0]));
}

const track_t * playlist_get(uint32_t index)
{
    if (index >= playlist_count()) {
        index = 0;
    }
    return &s_tracks[index];
}
