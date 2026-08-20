# shared —— 跨应用、与硬件无关的可复用代码
# 包含：平台抽象接口(port.h)、应用框架(app.c)、通用工具(utils)。
# 注意：shared/utils 与业务层一样，不依赖 LVGL。

add_library(shared_app STATIC
    app/app.c
)

target_include_directories(shared_app PUBLIC
    ${CMAKE_SOURCE_DIR}/shared
)

target_link_libraries(shared_app PUBLIC lvgl)
