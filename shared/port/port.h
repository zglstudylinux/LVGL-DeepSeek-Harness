/**
 * @file port.h
 * @brief 平台抽象接口 —— 所有目标平台（PC / Linux / MCU）都必须实现本接口。
 *
 * 应用层与业务层只依赖本接口，不直接触碰任何平台或硬件细节。
 * 移植新平台时：只需新增 ports/<name>/ 并实现 port_get()，应用代码零改动。
 */
#ifndef SHARED_PORT_PORT_H
#define SHARED_PORT_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /**
     * @brief 初始化平台显示与输入：创建并注册 LVGL 显示设备、输入设备、tick 源。
     * 调用前必须先 lv_init()。
     */
    void (*init)(void);

    /**
     * @brief 返回单调递增的毫秒计数（自启动起）。
     * 供业务逻辑、动画、超时判断使用；不允许回绕到 0。
     */
    uint32_t (*tick_ms)(void);

    /**
     * @brief 阻塞延时/让出指定毫秒，用于主循环节奏控制。
     * MCU 裸机上为延时；RTOS 上可为任务让出（vTaskDelay）。
     */
    void (*delay_ms)(uint32_t ms);
} port_api_t;

/**
 * @brief 获取当前选中平台（ports/<name>/）实现的 API 实例。
 */
const port_api_t *port_get(void);

#ifdef __cplusplus
}
#endif

#endif /* SHARED_PORT_PORT_H */
