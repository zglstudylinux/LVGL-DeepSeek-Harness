/**
 * @file presenter_main.c
 * @brief Presenter 层实现：唯一同时接触 Model 与 View 的"翻译层"。
 *
 * 数据流：
 *   View 按钮点击 -> on_increment_clicked() -> Model 自增 -> View 刷新显示
 */
#include "presenter/presenter_main.h"
#include "model/model_counter.h"
#include "ui/view_main.h"

static counter_model_t s_model;

static void on_increment_clicked(void)
{
    counter_model_increment(&s_model);
    view_main_set_count(counter_model_get(&s_model));
}

void presenter_main_init(void)
{
    counter_model_init(&s_model);
    view_main_init(on_increment_clicked);
    view_main_set_count(counter_model_get(&s_model));
}
