/**
 * @file view_main.h
 * @brief View 层接口：界面对外只暴露"初始化"和"更新数据"两个能力，
 *        事件通过回调交给 Presenter 处理，不包含任何业务规则。
 */
#ifndef APP_UI_VIEW_MAIN_H
#define APP_UI_VIEW_MAIN_H

#include <stdint.h>

/** 点击"加一"按钮时的回调（由 Presenter 提供） */
typedef void (*view_main_increment_cb_t)(void);

void view_main_init(view_main_increment_cb_t on_increment);
void view_main_set_count(uint32_t count);

#endif /* APP_UI_VIEW_MAIN_H */
