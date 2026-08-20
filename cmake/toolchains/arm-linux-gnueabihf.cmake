# cmake/toolchains/arm-linux-gnueabihf.cmake —— ARM Linux 板卡交叉编译模板
#
# 用法：
#   cmake -B build -DAPP=demo_widgets -DPORT=linux_fb \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-linux-gnueabihf.cmake
#
# 需配合 sysroot（含目标板 libSDL2/freetype 等依赖的交叉库）。

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# TODO: 改成你的交叉工具链前缀（例如 arm-linux-gnueabihf- 或 Yocto SDK 的 .../sysroots/.../bin/arm-...-linux-gnueabihf-）
set(TOOLCHAIN_PREFIX "/usr/bin/arm-linux-gnueabihf-")

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

# TODO: 指向你的 sysroot（含目标板依赖库）
# set(CMAKE_SYSROOT "/path/to/sysroot")
# set(CMAKE_FIND_ROOT_PATH "/path/to/sysroot")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
