# LVGL-DeepSeek-Harness —— 顶层构建脚本
#
# 用法：
#   cmake -B build -G Ninja -DAPP=demo_widgets -DPORT=pc_sdl
#   cmake --build build
#
# 一次只构建一个 APP 与一个 PORT（不同 app 使用各自的 lv_conf.h）。

cmake_minimum_required(VERSION 3.16)
project(LVGL-DeepSeek-Harness C CXX)

# ===== 选择应用与目标平台 =====
set(APP "demo_widgets" CACHE STRING "Application to build (apps/<APP>)")
set(PORT "pc_sdl" CACHE STRING "Target platform (ports/<PORT>)")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/apps/${APP}")
    message(FATAL_ERROR "Unknown APP '${APP}': apps/${APP}/ does not exist")
endif()
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/ports/${PORT}")
    message(FATAL_ERROR "Unknown PORT '${PORT}': ports/${PORT}/ does not exist")
endif()

message(STATUS "Building APP=${APP}  PORT=${PORT}")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 每个应用使用自己的 lv_conf.h（apps/<APP>/lv_conf.h）
set(LV_BUILD_CONF_PATH "${CMAKE_SOURCE_DIR}/apps/${APP}/lv_conf.h" CACHE PATH "lv_conf.h location" FORCE)

# ===== 构建顺序：lvgl -> shared -> port -> app =====
add_subdirectory(lvgl)            # LVGL 库（lvgl / lvgl::examples / lvgl::demos / lvgl::thorvg）
add_subdirectory(shared)          # shared_app（port/app 框架 + utils）

# 平台实现：CMake 型 port 直接 add_subdirectory；MCU 型 port 走厂商 IDE 拷贝清单
if(EXISTS "${CMAKE_SOURCE_DIR}/ports/${PORT}/CMakeLists.txt")
    add_subdirectory(ports/${PORT})   # 目标名统一为 port
else()
    message(FATAL_ERROR "ports/${PORT} has no CMakeLists.txt. "
                        "MCU ports are built via vendor IDE copy-list (see ports/${PORT}/README.md).")
endif()

include(cmake/lvgl_helpers.cmake)
add_subdirectory(apps/${APP})     # 应用可执行文件
