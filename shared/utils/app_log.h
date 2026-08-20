/**
 * @file app_log.h
 * @brief 轻量日志宏（零 LVGL 依赖），供 model/service/shared/utils 使用。
 *
 * 业务层禁止 #include "lvgl.h"，因此不能用 LV_LOG；统一走本模块。
 * 默认输出到标准输出，可通过 APP_LOG_ENABLE 开关。
 */
#ifndef SHARED_UTILS_APP_LOG_H
#define SHARED_UTILS_APP_LOG_H

#include <stdio.h>

#ifndef APP_LOG_ENABLE
#define APP_LOG_ENABLE 1
#endif

#if APP_LOG_ENABLE
#define APP_LOG(level, fmt, ...) \
    printf("[%s] %s:%d " fmt "\n", level, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define APP_LOG(level, fmt, ...) ((void)0)
#endif

#define APP_LOGI(fmt, ...) APP_LOG("I", fmt, ##__VA_ARGS__)
#define APP_LOGW(fmt, ...) APP_LOG("W", fmt, ##__VA_ARGS__)
#define APP_LOGE(fmt, ...) APP_LOG("E", fmt, ##__VA_ARGS__)

#endif /* SHARED_UTILS_APP_LOG_H */
