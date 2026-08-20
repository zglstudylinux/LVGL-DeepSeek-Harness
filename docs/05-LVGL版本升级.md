# 05 · LVGL 版本升级

## 当前状态

- LVGL 源码 **vendor** 在 `lvgl/`（直接纳入版本控制，非 git submodule）。
- 当前版本：**LVGL 9.6-dev**（已 pin 到验证过的提交 `c65e112`，见 `lvgl/include/lvgl/lv_version.h`）。

## 为什么 vendor 而不是 submodule

1. 网络受限时 `git clone` / `git submodule update` 到 github.com 常超时（本环境实测）。
2. vendor 后，clone 本仓库即可直接构建，零 submodule 拉取。
3. 升级/降级 = 换 `lvgl/` 目录内容 + 提交，操作简单且历史可追溯。

## 升级步骤

1. 下载新版本 LVGL 源码（任选其一）：
   ```bash
   # 方式 A：tarball（网络不稳时推荐）
   curl -L -o lvgl.tar.gz https://codeload.github.com/lvgl/lvgl/tar.gz/<tag或提交>
   tar -xzf lvgl.tar.gz
   # 方式 B：git clone
   git clone --depth 1 --branch v9.x https://github.com/lvgl/lvgl.git
   ```
2. 替换 `lvgl/` 内容（删除旧目录，移入新目录；保留 lvgl 子目录本身）。
3. 对比新旧 `lv_conf_template.h` 差异，同步到各应用的 `apps/*/lv_conf.h`（新增/改名/废弃项）。
4. 重建验证：
   ```powershell
   .\build.ps1 -App demo_widgets -Clean
   .\build.ps1 -App hello_world
   ```
5. 按错误清单修复：**LVGL 版本相关 API 差异只可能出现在**：
   - `ports/*`（display/indev 驱动，v8↔v9 差异最大）；
   - `apps/*/src/ui`（控件 API）。
   `model/service/shared/utils` 零依赖，天然免疫。
6. 提交：`git add lvgl apps && git commit -m "bump lvgl to <版本>"`。

## 多版本并行（如有需要）

不搞目录并存，用 git 分支承载旧版本维护：

```bash
git branch release/lvgl-v8     # 冻结 v8 版本
git checkout -b release/lvgl-v9 # 维护 v9 版本
```

主分支始终跟一个版本；旧版本需要时切分支维护。

## 改回 submodule（可选）

若以后网络稳定、想恢复 submodule：

```bash
git rm -r --cached lvgl
rm -rf lvgl
git submodule add https://github.com/lvgl/lvgl.git lvgl
git -C lvgl checkout <pin提交>
git add .gitmodules lvgl
git commit -m "switch lvgl to submodule"
```

## 已知坑位备忘

- CMake 4.x：LVGL 顶层需 `$<BUILD_INTERFACE:...>` 包裹源目录 include（已在本仓库 CMake 规避）。
- SDL2 链接：只连 `SDL2::SDL2`（不含 SDL2main），否则 `undefined reference to SDL_main`（已规避）。
- `lvgl/scripts/` 为构建所需（生成 lv_conf_internal.h / CMake 变量），vendor 时保留；
  `lvgl/tests/`、`lvgl/docs/` 已剔除（与构建无关，可省 ~86MB）。
