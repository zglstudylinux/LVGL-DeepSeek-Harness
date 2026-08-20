# ports/pc_sdl —— PC 模拟器（SDL2）平台库
#
# 统一平台目标名 "port"：顶层 CMake 只会 add_subdirectory 一个 port，
# 应用通过链接 "port" 获得当前平台的显示/输入/tick 实现。
# 依赖：SDL2（MSYS2 或 apt/brew 安装）。

find_package(SDL2 REQUIRED)

# 模拟显示分辨率（竖屏 480×640 为当前项目默认；其他应用可 -D 覆盖）
set(LVGL_SIM_HOR_RES 480 CACHE STRING "Simulated display horizontal resolution")
set(LVGL_SIM_VER_RES 640 CACHE STRING "Simulated display vertical resolution")

add_library(port STATIC
    src/port.c
    src/mouse_cursor_icon.c
)

target_compile_definitions(port PUBLIC
    LVGL_SIM_HOR_RES=${LVGL_SIM_HOR_RES}
    LVGL_SIM_VER_RES=${LVGL_SIM_VER_RES}
)

target_include_directories(port PUBLIC
    ${CMAKE_SOURCE_DIR}/shared
    ${SDL2_INCLUDE_DIRS}
)

# port.c 既调用 lvgl（lv_sdl_*）也直接调用 SDL（SDL_GetTicks/SDL_Delay）。
# 链接 SDL2 时只连 SDL2 本体（SDL2::SDL2），**不连 SDL2main**：
#   应用入口使用普通 main()（app 层不依赖 SDL，可移植到 MCU/Linux）。
#   若连入 SDL2main，它会用 WinMain 包装器重定向 main -> SDL_main，导致
#   "undefined reference to SDL_main" 链接错误。
if(TARGET SDL2::SDL2)
    target_link_libraries(port PUBLIC lvgl SDL2::SDL2)
else()
    target_link_libraries(port PUBLIC lvgl ${SDL2_LIBRARIES})
endif()

# 防御性：即使某 TU 包含 SDL.h，也不重定义 main（app 自行提供 main）
target_compile_definitions(port PUBLIC SDL_MAIN_HANDLED)
