# bt_speaker —— 竖屏 480×640 蓝牙音箱（「正在播放」+「蓝牙设备列表」双屏）
lvgl_add_app(bt_speaker
    src/main.c
    src/ui/view_player.c
    src/ui/view_devices.c
    src/presenter/presenter_player.c
    src/model/model_player.c
    src/model/model_playlist.c
    src/model/model_bt.c
)
