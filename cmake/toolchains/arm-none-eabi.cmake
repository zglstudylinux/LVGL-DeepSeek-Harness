# cmake/toolchains/arm-none-eabi.cmake —— 通用 ARM Cortex-M 交叉编译模板
#
# 用法：
#   cmake -B build -DAPP=demo_widgets -DPORT=stm32f4xx \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake
#
# 说明：MCU 项目通常依赖厂商 HAL 与 IDE 工程（见 ports/stm32f4xx/README.md 的拷贝清单），
# 本文件仅作参考模板；按你的芯片与工具链路径修改后再使用。

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# TODO: 改成你的 arm-none-eabi-gcc 路径（例如 STM32CubeIDE 或 xpack 工具链）
set(TOOLCHAIN_PREFIX "/opt/gcc-arm-none-eabi/bin/arm-none-eabi-")

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_OBJDUMP      ${TOOLCHAIN_PREFIX}objdump)
set(CMAKE_SIZE         ${TOOLCHAIN_PREFIX}size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
