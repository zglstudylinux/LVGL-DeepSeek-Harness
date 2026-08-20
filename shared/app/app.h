/**
 * @file app.h
 * @brief 应用生命周期接口 —— 每个 UI 应用（apps/<name>/）都必须实现 app_init()。
 *
 * 应用层通过本接口与平台(port)解耦：main.c 骨架对所有应用完全一致。
 */
#ifndef SHARED_APP_APP_H
#define SHARED_APP_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 应用初始化：构建界面(View) + 组装业务(Presenter/Model/Service)。
 * 在 port_get()->init() 之后调用。
 */
void app_init(void);

/**
 * @brief 可选的应用周期任务，每轮主循环被调用一次（用于非 UI 的业务轮询）。
 * 简单应用可不实现（由编译期弱符号提供默认空实现）。
 */
void app_update(void);

/**
 * @brief 统一主循环：lv_timer_handler() -> app_update() -> 按需延时。
 * 所有应用共用同一骨架；MCU 上可把本函数体搬进 RTOS 任务。
 */
void app_run(void);

#ifdef __cplusplus
}
#endif

#endif /* SHARED_APP_APP_H */
