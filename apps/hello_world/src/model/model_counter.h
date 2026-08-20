/**
 * @file model_counter.h
 * @brief Model 层示例：一个计数器。
 *
 * ★ 铁律：本层禁止 #include "lvgl.h"，也不触碰任何 LVGL 类型。
 * 这样业务逻辑可跨 LVGL 版本、甚至脱离 LVGL 单独单元测试。
 */
#ifndef APP_MODEL_COUNTER_H
#define APP_MODEL_COUNTER_H

#include <stdint.h>

typedef struct {
    uint32_t count;
} counter_model_t;

void counter_model_init(counter_model_t * m);
void counter_model_increment(counter_model_t * m);
uint32_t counter_model_get(const counter_model_t * m);

#endif /* APP_MODEL_COUNTER_H */
