/**
 * @file view_player.c
 * @brief 播放器界面 View 实现（本应用唯一直接使用 LVGL 的 UI 层）。
 */
#include "ui/view_player.h"
#include "lvgl/lvgl.h"
#include <stdio.h>
#include <time.h>

/* 深色主题配色 */
#define COLOR_BG       0x101418
#define COLOR_CARD     0x1C2128
#define COLOR_ACCENT   0x4FC3F7
#define COLOR_TEXT     0xE6EDF3
#define COLOR_TEXT_DIM 0x8B949E
#define COLOR_TRACK    0x2A3139

/* demo 自带专辑封面（LV_DEMO_MUSIC_LARGE=1 时为 428×428 大图） */
LV_IMAGE_DECLARE(img_lv_demo_music_cover_1);
LV_IMAGE_DECLARE(img_lv_demo_music_cover_2);
LV_IMAGE_DECLARE(img_lv_demo_music_cover_3);

static const lv_image_dsc_t * s_covers[3] = {
    &img_lv_demo_music_cover_1,
    &img_lv_demo_music_cover_2,
    &img_lv_demo_music_cover_3,
};

static view_player_events_t s_ev;
static bool s_refreshing = false;   /* 程序化刷新时抑制回调，避免回环 */

static lv_obj_t * s_screen;
static lv_obj_t * s_bt_label;
static lv_obj_t * s_time_label;
static lv_obj_t * s_battery_label;
static lv_obj_t * s_cover;
static lv_obj_t * s_title;
static lv_obj_t * s_artist;
static lv_obj_t * s_progress;
static lv_obj_t * s_elapsed_label;
static lv_obj_t * s_total_label;
static lv_obj_t * s_play_icon;
static lv_obj_t * s_volume_slider;

static void fmt_mmss(uint32_t ms, char * buf, size_t n)
{
    uint32_t s = ms / 1000;
    snprintf(buf, n, "%u:%02u", (unsigned)(s / 60), (unsigned)(s % 60));
}

/* ---------- 事件回调 ---------- */
static void play_pause_cb(lv_event_t * e)
{
    (void)e;
    if(s_ev.on_play_pause) s_ev.on_play_pause();
}

static void prev_cb(lv_event_t * e)
{
    (void)e;
    if(s_ev.on_prev) s_ev.on_prev();
}

static void next_cb(lv_event_t * e)
{
    (void)e;
    if(s_ev.on_next) s_ev.on_next();
}

static void bt_open_cb(lv_event_t * e)
{
    (void)e;
    if(s_ev.on_bt_open) s_ev.on_bt_open();
}

static void volume_cb(lv_event_t * e)
{
    if(s_refreshing) return;
    lv_obj_t * slider = lv_event_get_target(e);
    uint8_t vol = (uint8_t)lv_slider_get_value(slider);
    if(s_ev.on_volume_changed) s_ev.on_volume_changed(vol);
}

static void seek_cb(lv_event_t * e)
{
    if(s_refreshing) return;
    lv_obj_t * slider = lv_event_get_target(e);
    uint32_t ms = (uint32_t)lv_slider_get_value(slider);
    if(s_ev.on_seek) s_ev.on_seek(ms);
}

static void clock_timer_cb(lv_timer_t * t)
{
    (void)t;
    time_t now = time(NULL);
    struct tm * tm = localtime(&now);
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", tm->tm_hour, tm->tm_min);
    lv_label_set_text(s_time_label, buf);
}

/* 创建圆形按钮，内含一个符号 label */
static lv_obj_t * create_round_btn(lv_obj_t * parent, const char * symbol, const lv_font_t * font,
                                   int32_t size, lv_color_t bg)
{
    lv_obj_t * btn = lv_button_create(parent);
    lv_obj_set_size(btn, size, size);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn, bg, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_20, 0);
    lv_obj_set_style_shadow_color(btn, lv_color_hex(0x000000), 0);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, symbol);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_center(label);
    return btn;
}

void view_player_init(const view_player_events_t * ev)
{
    s_ev = *ev;

    /* 深色主题（幂等，重复调用会刷新样式） */
    lv_theme_default_init(lv_display_get_default(),
                          lv_palette_main(LV_PALETTE_LIGHT_BLUE),
                          lv_palette_main(LV_PALETTE_BLUE_GREY),
                          true, LV_FONT_DEFAULT);

    s_screen = lv_screen_active();
    lv_obj_t * scr = s_screen;
    lv_obj_set_style_bg_color(scr, lv_color_hex(COLOR_BG), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    /* ---- 状态栏 ---- */
    s_bt_label = lv_label_create(scr);
    lv_label_set_text(s_bt_label, LV_SYMBOL_BLUETOOTH " Connected");
    lv_obj_set_style_text_color(s_bt_label, lv_color_hex(COLOR_ACCENT), 0);
    lv_obj_set_style_bg_color(s_bt_label, lv_color_hex(COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(s_bt_label, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(s_bt_label, 6, 0);
    lv_obj_set_style_radius(s_bt_label, 12, 0);
    lv_obj_set_pos(s_bt_label, 16, 8);
    lv_obj_add_flag(s_bt_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(s_bt_label, bt_open_cb, LV_EVENT_CLICKED, NULL);

    s_time_label = lv_label_create(scr);
    lv_label_set_text(s_time_label, "00:00");
    lv_obj_set_style_text_font(s_time_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(s_time_label, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_align(s_time_label, LV_ALIGN_TOP_MID, 0, 12);

    s_battery_label = lv_label_create(scr);
    lv_label_set_text(s_battery_label, LV_SYMBOL_BATTERY_FULL " 80%");
    lv_obj_set_style_text_color(s_battery_label, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_align(s_battery_label, LV_ALIGN_TOP_RIGHT, -16, 12);

    /* ---- 专辑封面 ---- */
    s_cover = lv_image_create(scr);
    lv_image_set_src(s_cover, s_covers[0]);
    lv_obj_set_size(s_cover, 340, 340);
    lv_image_set_inner_align(s_cover, LV_IMAGE_ALIGN_CONTAIN_DOWNSCALE);
    lv_obj_set_pos(s_cover, 70, 54);
    lv_obj_set_style_radius(s_cover, 16, 0);
    lv_obj_set_style_clip_corner(s_cover, true, 0);

    /* ---- 歌名 / 歌手 ---- */
    s_title = lv_label_create(scr);
    lv_label_set_text(s_title, "");
    lv_obj_set_style_text_font(s_title, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_color(s_title, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_align(s_title, LV_ALIGN_TOP_MID, 0, 404);

    s_artist = lv_label_create(scr);
    lv_label_set_text(s_artist, "");
    lv_obj_set_style_text_font(s_artist, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(s_artist, lv_color_hex(COLOR_TEXT_DIM), 0);
    lv_obj_align(s_artist, LV_ALIGN_TOP_MID, 0, 438);

    /* ---- 进度条 ---- */
    s_progress = lv_slider_create(scr);
    lv_obj_set_size(s_progress, 432, 14);
    lv_obj_set_pos(s_progress, 24, 470);
    lv_slider_set_range(s_progress, 0, 100);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(COLOR_TRACK), LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(COLOR_ACCENT), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(COLOR_ACCENT), LV_PART_KNOB);
    lv_obj_add_event_cb(s_progress, seek_cb, LV_EVENT_VALUE_CHANGED, NULL);

    s_elapsed_label = lv_label_create(scr);
    lv_label_set_text(s_elapsed_label, "0:00");
    lv_obj_set_style_text_font(s_elapsed_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_elapsed_label, lv_color_hex(COLOR_TEXT_DIM), 0);
    lv_obj_set_pos(s_elapsed_label, 24, 492);

    s_total_label = lv_label_create(scr);
    lv_label_set_text(s_total_label, "0:00");
    lv_obj_set_style_text_font(s_total_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_total_label, lv_color_hex(COLOR_TEXT_DIM), 0);
    lv_obj_align(s_total_label, LV_ALIGN_TOP_RIGHT, -24, 492);

    /* ---- 控制按钮 ---- */
    lv_obj_t * prev_btn = create_round_btn(scr, LV_SYMBOL_PREV, &lv_font_montserrat_24, 56, lv_color_hex(COLOR_CARD));
    lv_obj_set_pos(prev_btn, 124, 520);
    lv_obj_add_event_cb(prev_btn, prev_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * next_btn = create_round_btn(scr, LV_SYMBOL_NEXT, &lv_font_montserrat_24, 56, lv_color_hex(COLOR_CARD));
    lv_obj_set_pos(next_btn, 300, 520);
    lv_obj_add_event_cb(next_btn, next_cb, LV_EVENT_CLICKED, NULL);

    /* 播放/暂停（主按钮，高亮圆钮） */
    lv_obj_t * play_btn = lv_button_create(scr);
    lv_obj_set_size(play_btn, 72, 72);
    lv_obj_set_pos(play_btn, 204, 512);
    lv_obj_set_style_radius(play_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(play_btn, lv_color_hex(COLOR_ACCENT), 0);
    lv_obj_set_style_bg_opa(play_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_opa(play_btn, LV_OPA_40, 0);
    lv_obj_set_style_shadow_color(play_btn, lv_color_hex(COLOR_ACCENT), 0);
    lv_obj_set_style_shadow_width(play_btn, 16, 0);
    lv_obj_add_event_cb(play_btn, play_pause_cb, LV_EVENT_CLICKED, NULL);

    s_play_icon = lv_label_create(play_btn);
    lv_label_set_text(s_play_icon, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_font(s_play_icon, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(s_play_icon, lv_color_hex(0x0B1220), 0);
    lv_obj_center(s_play_icon);

    /* ---- 音量 ---- */
    lv_obj_t * vol_icon = lv_label_create(scr);
    lv_label_set_text(vol_icon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_set_style_text_font(vol_icon, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(vol_icon, lv_color_hex(COLOR_TEXT), 0);
    lv_obj_set_pos(vol_icon, 24, 606);

    s_volume_slider = lv_slider_create(scr);
    lv_obj_set_size(s_volume_slider, 392, 14);
    lv_obj_set_pos(s_volume_slider, 64, 604);
    lv_slider_set_range(s_volume_slider, 0, 100);
    lv_obj_set_style_bg_color(s_volume_slider, lv_color_hex(COLOR_TRACK), LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_volume_slider, lv_color_hex(COLOR_ACCENT), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(s_volume_slider, lv_color_hex(COLOR_TEXT), LV_PART_KNOB);
    lv_obj_add_event_cb(s_volume_slider, volume_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* 时钟（每 1s 刷新） */
    lv_timer_create(clock_timer_cb, 1000, NULL);
    clock_timer_cb(NULL);
}

void view_player_set_cover(uint8_t index)
{
    if(index > 2) index = 0;
    lv_image_set_src(s_cover, s_covers[index]);
}

void view_player_set_track(const char * title, const char * artist)
{
    lv_label_set_text(s_title, title);
    lv_label_set_text(s_artist, artist);
}

void view_player_set_playing(bool playing)
{
    lv_label_set_text(s_play_icon, playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
}

void view_player_set_progress(uint32_t elapsed_ms, uint32_t total_ms)
{
    s_refreshing = true;
    lv_slider_set_range(s_progress, 0, (int32_t)total_ms);
    lv_slider_set_value(s_progress, (int32_t)elapsed_ms, LV_ANIM_OFF);
    s_refreshing = false;

    char buf[16];
    fmt_mmss(elapsed_ms, buf, sizeof(buf));
    lv_label_set_text(s_elapsed_label, buf);
    fmt_mmss(total_ms, buf, sizeof(buf));
    lv_label_set_text(s_total_label, buf);
}

void view_player_set_volume(uint8_t vol)
{
    s_refreshing = true;
    lv_slider_set_value(s_volume_slider, vol, LV_ANIM_OFF);
    s_refreshing = false;
}

void view_player_set_bt_connected(bool connected)
{
    if(connected) {
        lv_label_set_text(s_bt_label, LV_SYMBOL_BLUETOOTH " Connected");
        lv_obj_set_style_text_color(s_bt_label, lv_color_hex(COLOR_ACCENT), 0);
    }
    else {
        lv_label_set_text(s_bt_label, LV_SYMBOL_BLUETOOTH " Disconnected");
        lv_obj_set_style_text_color(s_bt_label, lv_color_hex(COLOR_TEXT_DIM), 0);
    }
}

void view_player_show(void)
{
    lv_scr_load(s_screen);
}

void view_player_set_battery(uint8_t pct)
{
    const char * icon = LV_SYMBOL_BATTERY_EMPTY;
    if(pct >= 80) icon = LV_SYMBOL_BATTERY_FULL;
    else if(pct >= 30) icon = LV_SYMBOL_BATTERY_1;

    char buf[16];
    snprintf(buf, sizeof(buf), "%s %u%%", icon, (unsigned)pct);
    lv_label_set_text(s_battery_label, buf);
}
