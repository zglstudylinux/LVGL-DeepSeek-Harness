/**
 * @file model_counter.c
 * @brief Model 层实现（纯 C，无 LVGL 依赖）。
 */
#include "model/model_counter.h"

void counter_model_init(counter_model_t * m)
{
    m->count = 0;
}

void counter_model_increment(counter_model_t * m)
{
    m->count++;
}

uint32_t counter_model_get(const counter_model_t * m)
{
    return m->count;
}
