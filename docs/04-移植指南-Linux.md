# 04 · 移植指南 —— Linux 板卡

> 模板参考：`ports/linux_fb/`（framebuffer + evdev）。

## 适用场景

树莓派、i.MX、全志、RK 等运行 Linux 的板卡，使用内核 framebuffer（`/dev/fb0`）+ evdev（`/dev/input/event*`）。

## 移植步骤

1. 复制 `ports/linux_fb` → `ports/<板卡名>`。
2. 补全 `src/port.c` 的 `port_init_impl()`，使用 LVGL 内置驱动（无需自写）：
   - 显示：`lv_linux_fbdev_create()` / `lv_linux_fbdev_set_file(disp, "/dev/fb0")`
   - 输入：`lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/eventX")`
3. 确认 `apps/<应用>/lv_conf.h` 开启 `LV_USE_LINUX_FBDEV 1`、`LV_USE_EVDEV 1`。
4. 交叉编译或板卡本机编译：
   ```bash
   cmake -B build -DPORT=<板卡名> -DAPP=demo_widgets \
         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-linux-gnueabihf.cmake
   cmake --build build -j
   ```
5. 运行 `./bin/<应用>`（需要 `/dev/fb0` 与输入设备权限，可能需要 `sudo` 或加入 `input`/`video` 组）。

## 高级：DRM / Wayland / X11

LVGL 内置 `LV_USE_LINUX_DRM`、`LV_USE_WAYLAND`、`LV_USE_X11` 等驱动，切换方法：
1. 在 lv_conf.h 打开对应 `LV_USE_*`。
2. 在 `ports/<板卡>/src/port.c` 改用对应驱动 API。
3. 依赖由 LVGL 的 CMake 自动解析（find_package / pkg-config）。

## 调试技巧

- 无显示设备时可用 `LV_USE_SDL` 在桌面 Linux 直接模拟（等同 pc_sdl）。
- 帧率/内存占用可用 lv_conf.h 的 `LV_USE_PERF_MONITOR`、`LV_USE_MEM_MONITOR`。
