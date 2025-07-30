![项目版本](https://img.shields.io/github/release/z3lx/genshin-fov-unlock?label=release)
![下载次数](https://img.shields.io/github/downloads/z3lx/genshin-fov-unlock/total?label=downloads)

# 原神解锁器

[English](README.md) | **简体中文** | [繁體中文](README.zh-Hant.md)

> ⚠️ 此文为自动翻译，仅供参考，请以英文原文为准。

一个提升游戏体验的模组，解锁游戏的帧率（FPS）和视野（FOV），并包含其他多种优化。如果您觉得这个项目有用，请考虑给仓库点个星 🌟！

https://github.com/user-attachments/assets/56a11762-ebd2-4093-8c1d-768f913bd063

## 功能

- 定期更新以支持《原神》PC客户端的全球及中国版本
- 解锁帧率（FPS）和视野（FOV）
- 当游戏未处于焦点时，限制帧率并降低游戏进程优先级
- 与游戏内序列化视野变化集成（例如爆发、过场动画）
- 通过键位绑定快速切换视角解锁状态及预设模式
- 自定义游戏启动参数和显示设置
- 兼容[GIMI](https://github.com/SilentNightSound/GI-Model-Importer)及相关模组
- 不完全兼容PC手柄布局

## 免责声明

尽管我自该模组发布以来一直使用且未遇到问题，但需注意第三方软件修改游戏行为可能违反游戏服务条款，并存在账号封禁风险。建议您查看源代码并自行构建项目以确保透明度并评估潜在风险。

## 安装

由于该模组的实现方式，可能被杀毒软件误报为病毒。若在安装过程中遇到此类问题，请考虑将模组添加至杀毒软件的例外列表。

1. 从 [最新版本](https://github.com/z3lx/genshin-fov-unlock/releases/latest) 下载 `mod.zip` 并解压
2. 运行解压后的文件夹中的 `loader.exe`
3. 等待游戏启动
4. （可选）通过编辑配置文件调整模组行为

模组在游戏中激活后：
- 使用 <kbd>←</kbd> 和 <kbd>→</kbd> 键循环切换 FOV 预设
- 使用 <kbd>↓</kbd> 键切换 FOV 解锁状态

与 [GIMI](https://github.com/SilentNightSound/GI-Model-Importer) 配合使用：
1. 使用文本编辑器打开 `loader_config.json` 文件
2. 将 GIMI 的 `d3d11.dll` 路径添加到 `dllPaths` 数组中
```json
"dllPaths": [
    "plugin.dll",
    "C:\\path\\to\\GIMI\\d3d11.dll"
],
```

## 配置

模组的行为可通过 `loader_config.json` 和 `plugin_config.json` 文件进行自定义，具体如下。

### 加载器

| 键                | 类型         | 描述                      |
|------------------|------------|-------------------------|
| `checkUpdates`   | `bool`     | 检查模组的新版本                |
| `gamePath`       | `string`   | Genshin Impact 可执行文件的路径 |
| `overrideArgs`   | `bool`     | 覆盖游戏的默认启动参数             |
| `monitorIndex`   | `int`      | 用于游戏窗口的监视器索引（从 1 开始）    |
| `displayMode`    | `string`   | 游戏窗口的显示模式               |
| `screenWidth`    | `int`      | 游戏窗口的宽度（以像素为单位）         |
| `screenHeight`   | `int`      | 游戏窗口的高度（以像素为单位）         |
| `mobilePlatform` | `bool`     | 启用移动端 UI                |
| `additionalArgs` | `string`   | 传递给游戏可执行文件的额外参数         |
| `dllPaths`       | `string[]` | 要注入到游戏中的 DLL 文件列表       |
| `suspendLoad`    | `bool`     | 暂停游戏进程直至模组完全加载          |

<details>

<summary>有效显示模式</summary>

| 值            | 描述       |
|--------------|----------|
| `Windowed`   | 窗口化模式    |
| `Fullscreen` | 全屏独占模式   |
| `Borderless` | 无边框窗口化模式 |

</details>

默认加载器配置：

```json
{
    "checkUpdates": true,
    "gamePath": "",
    "overrideArgs": false,
    "monitorIndex": 1,
    "displayMode": "Fullscreen",
    "screenWidth": 1920,
    "screenHeight": 1080,
    "mobilePlatform": false,
    "additionalArgs": "",
    "dllPaths": [
        "plugin.dll"
    ],
    "suspendLoad": false
}
```

### 插件

| 键                  | 类型       | 描述                   |
|--------------------|----------|----------------------|
| `unlockFps`        | `bool`   | 启用帧率（FPS）解锁          |
| `targetFps`        | `int`    | 解锁帧率时的目标帧率           |
| `autoThrottle`     | `bool`   | 当插件未被聚焦时限制帧率并降低进程优先级 |
| `unlockFov`        | `bool`   | 启用视场角（FOV）解锁         |
| `targetFov`        | `int`    | 启动时应用的默认FOV          |
| `fovPresets`       | `int[]`  | 要循环的FOV值列表           |
| `fovSmoothing`     | `float`  | FOV过渡时的平滑因子（以秒为单位）   |
| `unlockFovKey`     | `string` | 用于切换视场解锁的键绑定         |
| `nextFovPresetKey` | `string` | 用于循环到下一个视场预设的键绑定     |
| `prevFovPresetKey` | `string` | 用于循环到上一个视场预设的键绑定     |

<details>

<summary>有效键绑定</summary>

| 常量                | 描述                     |
|-------------------|------------------------|
| `LeftMouse`       | 左键                     |
| `RightMouse`      | 右键                     |
| `MiddleMouse`     | 中键                     |
| `X1Mouse`         | X1键                    |
| `X2Mouse`         | X2键                    |
| `Backspace`       | 退格键                    |
| `Tab`             | 制表键                    |
| `Clear`           | 清除键                    |
| `Enter`           | 回车键                    |
| `Shift`           | 换页键                    |
| `Ctrl`            | Ctrl 键                 |
| `Alt`             | Alt 键                  |
| `Pause`           | 暂停键                    |
| `CapsLock`        | 大写锁定键                  |
| `Esc`             | 逃逸键                    |
| `Space`           | 空格键                    |
| `PageUp`          | 页面向上键                  |
| `PageDown`        | 页面向下键                  |
| `End`             | 末尾键                    |
| `Home`            | 首页键                    |
| `LeftArrow`       | 左箭头键                   |
| `UpArrow`         | 上箭头键                   |
| `RightArrow`      | 右箭头键                   |
| `DownArrow`       | 下箭头键                   |
| `PrintScreen`     | 打印屏幕键                  |
| `Insert`          | 插入键                    |
| `Delete`          | 删除键                    |
| `0`               | 0 键                    |
| `1`               | 1 键                    |
| `2`               | 2 键                    |
| `3`               | 3 键                    |
| `4`               | 4 键                    |
| `5`               | 5 键                    |
| `6`               | 6 键                    |
| `7`               | 7 键                    |
| `8`               | 8 键                    |
| `9`               | 9 键                    |
| `A`               | A 键                    |
| `B`               | B 键                    |
| `C`               | C 键                    |
| `D`               | D 键                    |
| `E`               | E 键                    |
| `F`               | F 键                    |
| `G`               | G 键                    |
| `H`               | H 键                    |
| `I`               | I 键                    |
| `J`               | J 键                    |
| `K`               | K 键                    |
| `L`               | L 键                    |
| `M`               | M 键                    |
| `N`               | N 键                    |
| `O`               | O 键                    |
| `P`               | P 键                    |
| `Q`               | Q 键                    |
| `R`               | R 键                    |
| `S`               | S 键                    |
| `T`               | T 键                    |
| `U`               | U 键                    |
| `V`               | V 键                    |
| `W`               | W 键                    |
| `X`               | X 键                    |
| `Y`               | Y 键                    |
| `Z`               | Z 键                    |
| `LeftWindows`     | 左 Windows 徽标键          |
| `RightWindows`    | 右 Windows 徽标键          |
| `Apps`            | 应用程序键                  |
| `Numpad0`         | 数字小键盘 0 键              |
| `Numpad1`         | 数字小键盘 1 键              |
| `Numpad2`         | 数字小键盘 2 键              |
| `Numpad3`         | 数字小键盘 3 键              |
| `Numpad4`         | 数字小键盘 4 键              |
| `Numpad5`         | 数字小键盘 5 键              |
| `Numpad6`         | 数字小键盘 6 键              |
| `Numpad7`         | 数字小键盘 7 键              |
| `Numpad8`         | 数字小键盘 8 键              |
| `Numpad9`         | 数字小键盘 9 键              |
| `NumpadMultiply`  | 乘法键                    |
| `NumpadAdd`       | 加法键                    |
| `NumpadSeparator` | 分隔符键                   |
| `NumpadSubtract`  | 减法键                    |
| `NumpadDecimal`   | 小数键                    |
| `NumpadDivide`    | 除号键                    |
| `F1`              | F1 键                   |
| `F2`              | F2 键                   |
| `F3`              | F3 键                   |
| `F4`              | F4 键                   |
| `F5`              | F5 键                   |
| `F6`              | F6 键                   |
| `F7`              | F7 键                   |
| `F8`              | F8 键                   |
| `F9`              | F9 键                   |
| `F10`             | F10 键                  |
| `F11`             | F11 键                  |
| `F12`             | F12 键                  |
| `F13`             | F13 键                  |
| `F14`             | F14 键                  |
| `F15`             | F15 键                  |
| `F16`             | F16 键                  |
| `F17`             | F17 键                  |
| `F18`             | F18 键                  |
| `F19`             | F19 键                  |
| `F20`             | F20 键                  |
| `F21`             | F21 键                  |
| `F22`             | F22 键                  |
| `F23`             | F23 键                  |
| `F24`             | F24 键                  |
| `NumLock`         | Num Lock 键             |
| `ScrollLock`      | 滚动锁定键                  |
| `LeftShift`       | 左 Shift 键              |
| `RightShift`      | 右 Shift 键              |
| `LeftCtrl`        | 左 Ctrl 键               |
| `RightCtrl`       | 右 Ctrl 键               |
| `LeftAlt`         | 左Alt键                  |
| `RightAlt`        | 右Alt键                  |
| `Oem1`            | 对于美国ANSI键盘，分号和冒号键      |
| `Plus`            | 对于任何国家和地区，等号和加号键       |
| `Comma`           | 对于任何国家和地区，逗号和小于号键      |
| `Minus`           | 适用于任何国家和地区，破折号和下划线键    |
| `Period`          | 适用于任何国家和地区，句点和大于号键     |
| `Oem2`            | 适用于美国 ANSI 键盘，正斜杠和问号键  |
| `Oem3`            | 适用于美国 ANSI 键盘，重音符和波浪号键 |
| `Oem4`            | 适用于美国 ANSI 键盘，左大括号键    |
| `Oem5`            | 适用于美国 ANSI 键盘，反斜杠和竖线键  |
| `Oem6`            | 适用于美国 ANSI 键盘，右大括号键    |
| `Oem7`            | 适用于美国 ANSI 键盘，撇号和双引号键  |
| `Oem8`            | 加拿大 CSA 键盘的右 Ctrl 键    |
| `Oem102`          | 欧洲 ISO 键盘的反斜杠和竖线键      |
| `OemClear`        | 清除键                    |

</details>

默认插件配置：

```json
{
    "unlockFps": true,
    "targetFps": 120,
    "autoThrottle": true,
    "unlockFov": true,
    "targetFov": 90,
    "fovPresets": [
        30,
        45,
        60,
        75,
        90,
        110
    ],
    "fovSmoothing": 0.125,
    "unlockFovKey": "DownArrow",
    "nextFovPresetKey": "RightArrow",
    "prevFovPresetKey": "LeftArrow"
}
```

## 构建

如果您希望直接使用该模组，可在 [最新版本](https://github.com/z3lx/genshin-fov-unlock/releases/latest) 中获取预编译二进制文件。若需从源代码构建项目，您必须使用 Windows 系统并已安装 MSVC v143（Visual Studio 2022）、Windows 11 SDK、CMake 和 Git。然后，按照以下步骤操作：

1. 克隆仓库
```bash
git clone https://github.com/z3lx/genshin-fov-unlock.git
cd genshin-fov-unlock
```
2. 配置项目及其依赖项
```bash
cmake . -G "Visual Studio 17 2022"
```
3. 构建项目
```bash
cmake --build . --config Release
```

## 致谢

本项目使用了以下库和资源：
- [`cpr`](https://github.com/libcpr/cpr)，采用 MIT 许可证授权
- [`glaze`](https://github.com/stephenberry/glaze)，采用 MIT 许可证
- [`minhook`](https://github.com/TsudaKageyu/minhook)，采用 BSD-2-Clause 许可证
- [`wil`](https://github.com/microsoft/wil)，采用 MIT 许可证
- [`genshin-fps-unlock`](https://github.com/34736384/genshin-fps-unlock)，用于实现细节的参考
- [`genshin-utility`](https://github.com/lanylow/genshin-utility)，用于模组初始构思的参考
- [`gurh32`](https://x.com/gurh32/status/1944266962496106662)，加载器图标所用艺术作品的来源

## 许可证

本项目采用 MIT 许可证。请参阅 [LICENSE](LICENSE) 文件以获取更多信息。
