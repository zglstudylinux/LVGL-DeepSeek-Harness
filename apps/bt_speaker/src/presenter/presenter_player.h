/**
 * @file presenter_player.h
 * @brief 播放器 Presenter 层接口：组装 Model 与 View，翻译 UI 事件与业务调用。
 */
#ifndef APP_PRESENTER_PLAYER_H
#define APP_PRESENTER_PLAYER_H

void presenter_player_init(void);
void presenter_player_update(void);   /* 每轮主循环由 app_update() 调用 */

#endif /* APP_PRESENTER_PLAYER_H */
