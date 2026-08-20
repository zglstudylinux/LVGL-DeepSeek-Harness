# LVGL-DeepSeek-Harness

一个可长期演进的 **LVGL 多项目脚手架**：遵循「模拟先行，移植在后」，把 UI 与业务逻辑解耦，
在 PC 上开发调试，再按需移植到 MCU 或 Linux 板卡。

- **多应用**：`apps/` 下每个目录是一个独立 UI 应用，互不干扰（各自 `lv_conf.h`）。
- **多平台**：`ports/` 下每个目录是一个目标平台，`APP × PORT` 正交组合构建。
- **分层解耦**：View / Presenter / Model，业务层零 LVGL 依赖。
- **版本可管理**：LVGL 源码 vendor 在 `lvgl/`，按需升级（见 `docs/05-LVGL版本升级.md`）。

## 快速开始（PC 模拟器）

前置：Windows 需安装 MSYS2 UCRT64 工具链（GCC / CMake / Ninja / SDL2）。
安装步骤见 `docs/` 与旧环境搭建指南 `LVGL_PC模拟器环境搭建指南.md`（在上一级目录）。

```powershell
# 构建并运行官方控件示例
.\build.ps1 -App demo_widgets -Run

# 构建并运行最小四层模板
.\build.ps1 -App hello_world -Run
```

Linux / macOS 直接用 CMake：

```bash
cmake -B build -DAPP=demo_widgets -DPORT=pc_sdl
cmake --build build -j
./bin/demo_widgets
```

## 目录结构

```
LVGL-DeepSeek-Harness/
├── apps/                  # 每个 UI 应用一个自包含目录（各含 lv_conf.h）
│   ├── demo_widgets/      # 官方控件示例（验证环境）
│   └── hello_world/       # 最小四层模板（新建应用请复制它）
├── ports/                 # 每个目标平台一个目录（实现 shared/port/port.h）
│   ├── pc_sdl/            # ★ PC 模拟器（SDL2，开发主战场）
│   ├── linux_fb/          # Linux framebuffer 模板
│   └── stm32f4xx/         # 单片机模板（厂商 IDE 拷贝清单）
├── shared/                # 跨应用复用：平台接口 / 应用框架 / 工具
│   ├── port/port.h        # ★ 唯一硬件契约
│   ├── app/               # 应用生命周期 + 统一主循环
│   └── utils/             # 日志等（零 LVGL 依赖）
├── lvgl/                  # LVGL 源码（vendor，pin 到已验证版本）
├── cmake/                 # toolchains 交叉编译模板 + lvgl_helpers
├── docs/                  # 架构 / 开发流程 / 移植指南
├── build.ps1              # PC 一键构建脚本
└── CMakeLists.txt         # 顶层：-DAPP= -DPORT= 选择应用与平台
```

## 核心概念

### APP × PORT 正交组合

一次只构建一个应用 + 一个平台：

```bash
cmake -B build -DAPP=<应用> -DPORT=<平台>
```

- 应用：`apps/<APP>/`（含该应用自己的 `lv_conf.h`）。
- 平台：`ports/<PORT>/`（实现 `shared/port/port.h`）。

### 三层解耦（铁律）

| 层 | 目录 | 允许 | 禁止 |
|----|------|------|------|
| View | `src/ui/` | 操作 LVGL 控件、回调 Presenter | 写业务规则 |
| Presenter | `src/presenter/` | 翻译 UI 事件 ↔ Model | 持有大量布局 |
| Model/Service | `src/model/`、`src/service/` | 纯 C 业务、可单测 | **`#include "lvgl.h"`** |

### 平台抽象

应用只依赖 `shared/port/port.h` 的三个函数（`init` / `tick_ms` / `delay_ms`），
对平台零感知。移植 = 只改 `ports/*`，应用代码一行不动。

## 文档

| 文档 | 内容 |
|------|------|
| [docs/01-架构设计.md](docs/01-架构设计.md) | 设计原则、分层规则、接口契约 |
| [docs/02-开发流程.md](docs/02-开发流程.md) | 新建应用 / 新增平台的标准步骤 |
| [docs/03-移植指南-单片机.md](docs/03-移植指南-单片机.md) | MCU（Keil/IAR/CubeIDE）移植 |
| [docs/04-移植指南-Linux.md](docs/04-移植指南-Linux.md) | Linux 板卡移植 |
| [docs/05-LVGL版本升级.md](docs/05-LVGL版本升级.md) | LVGL 升级与多版本管理 |

## 说明：LVGL 为何 vendor 而非 submodule

本仓库把 LVGL 源码直接纳入版本控制（`lvgl/`），原因：
1. 网络受限时 `git clone` / `git submodule` 到 github.com 常超时；
2. vendor 后 clone 本仓库即可直接构建，无需任何 submodule 拉取。

如需升级/改回 submodule，见 `docs/05-LVGL版本升级.md`。
