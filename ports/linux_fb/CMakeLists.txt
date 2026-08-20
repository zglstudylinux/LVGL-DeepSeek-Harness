# ports/linux_fb —— Linux framebuffer 平台（模板，需按板卡补全 port.c）
add_library(port STATIC
    src/port.c
)
target_include_directories(port PUBLIC ${CMAKE_SOURCE_DIR}/shared)
target_link_libraries(port PUBLIC lvgl)
