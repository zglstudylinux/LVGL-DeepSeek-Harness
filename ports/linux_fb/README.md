# ports/linux_fb —— Linux 板卡（framebuffer）移植模板

> 状态：**模板**，port.c 中的显示/输入初始化需按实际板卡补全。

## 适用场景
运行 Linux 的板卡：树莓派、i.MX、全志、RK 等，使用内核 framebuffer（`/dev/fb0`）+ evdev（`/dev/input/event*`）。

## 移植步骤
1. 复制本目录为 `ports/<你的板卡名>/`。
2. 补全 `src/port.c` 中的 `port_init_impl()`，使用 LVGL 内置驱动：
   - 显示：`lvgl/drivers/display/lv_linux_fbdev.h` → `lv_linux_fbdev_create()` / `lv_linux_fbdev_set_file()`
   - 输入：`lvgl/drivers/indev/lv_evdev.h` → `lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/eventX")`
   - 确认 `apps/<app>/lv_conf.h` 中 `LV_USE_LINUX_FBDEV 1`、`LV_USE_EVDEV 1`。
3. 交叉编译（或板卡本机编译）：
   ```bash
   cmake -B build -DPORT=<你的板卡名> -DAPP=demo_widgets \
         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-linux-gnueabihf.cmake
   cmake --build build
   ```
4. 运行：`./bin/<app>`（需要 `/dev/fb0` 与输入设备权限，可能需要 `sudo` 或加入 `input`/`video` 组）。

## 原理
应用代码只依赖 `shared/port/port.h` 的三个函数（init / tick_ms / delay_ms），
因此移植 = 只改本目录，`apps/*` 与 `shared/*` 零改动。
