# tools/ —— 资源转换与开发辅助

集中放置图片/字体转换等工具，避免散落在各应用中。

## 常用工具（LVGL 官方脚本，位于 `lvgl/scripts/`）
- 图片转换：`python lvgl/scripts/img_conv.py <输入图片> --of C`
  - 例：`python lvgl/scripts/img_conv.py icon.png --of C --output-file icon.c`
  - 生成 `LV_IMAGE_DECLARE(icon)` 的 `.c`，放到 `shared/ui/` 或 `apps/<app>/src/ui/`。
- 字体转换：`python lvgl/scripts/font_conv.py --font <ttf> --size 16 --format lvgl ...`

> 说明：`lvgl/scripts/built_in_font/` 体积较大，vendor 时已随 lvgl 保留；
> 如仅需转换工具、想瘦身，可自行裁剪该目录。
