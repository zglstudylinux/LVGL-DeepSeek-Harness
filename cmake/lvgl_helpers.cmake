# lvgl_helpers.cmake —— 应用构建辅助
#
# lvgl_add_app(<app_name> <src1> [src2 ...])
#   创建应用可执行文件，统一链接 lvgl + shared_app + port，并设置输出目录与平台选项。
#   每个 apps/<name>/CMakeLists.txt 只需一行调用即可。

function(lvgl_add_app APP_NAME)
    add_executable(${APP_NAME} ${ARGN})

    # 依赖：LVGL 库 + 应用框架 + 当前平台
    target_link_libraries(${APP_NAME} PRIVATE shared_app port)

    # LVGL 各子库（条件存在，避免配置差异导致 target not found）
    foreach(_tgt lvgl lvgl::examples lvgl::demos lvgl::thorvg)
        if(TARGET ${_tgt})
            target_link_libraries(${APP_NAME} PRIVATE ${_tgt})
        endif()
    endforeach()

    # 头文件搜索：
    #   ${CMAKE_SOURCE_DIR}     -> "lvgl/..."、"lvgl/demos/..."、"lvgl/examples/..."
    #   <app>/src              -> "ui/..."、"presenter/..."、"model/..."、"service/..."
    target_include_directories(${APP_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # 输出到统一 bin/ 目录
    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    )

    # 非 MSVC：链接 math 与 pthread（GCC/MinGW 需要）
    if(NOT MSVC)
        target_link_libraries(${APP_NAME} PRIVATE m pthread)
    endif()

    # Windows 下保留控制台以便打印日志
    if(WIN32)
        if(MSVC)
            target_link_options(${APP_NAME} PRIVATE "/SUBSYSTEM:CONSOLE")
        else()
            target_link_options(${APP_NAME} PRIVATE "-mconsole")
        endif()
    endif()
endfunction()
