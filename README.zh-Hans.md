![版本](https://img.shields.io/badge/版本-5.7-brightgreen)
![下载量](https://img.shields.io/github/downloads/z3lx/genshin-fov-unlock/total?label=下载量)

# 原神视野（FOV）解锁插件

[English](README.md) | **简体中文** | [繁體中文](README.zh-Hant.md)

> ⚠️ 此文为自动翻译，仅供参考，请以英文原文为准。

这是一个用于原神（Genshin Impact）的插件，可将相机默认45°的视野（FOV）解锁到更高数值。插件包含一个 DLL 文件，需配合 [**genshin-fps-unlock**](https://github.com/34736384/genshin-fps-unlock) 一起使用。如果你觉得这个插件好用，别忘了给项目点个 ⭐！

本插件支持国际服（GL）和国服（CN），但暂不完全兼容手柄控制。

https://github.com/user-attachments/assets/56a11762-ebd2-4093-8c1d-768f913bd063

## 免责声明

我个人使用该插件未遇到任何问题，也未被封号。但请注意，此类第三方插件违反游戏服务条款。建议你自行审查源代码并编译，以确保透明可控并了解相关风险。

虽然插件本身很轻量，但提升视野会让游戏渲染更多物体，从而明显影响性能。在极端数值下，还可能出现图像瑕疵。

## 安装

1. 下载最新版 **genshin-fps-unlock** 工具：
   1. 前往项目的 [**最新发布**](https://github.com/34736384/genshin-fps-unlock/releases/latest)。
   2. 从 **Assets** 区域下载 **unlockfps_nc.exe** 可执行文件。
   3. 将其放置在任意文件夹中（后续步骤均以此文件所在目录为例）。
2. 下载最新版 **genshin-fov-unlock** 插件：
   1. 前往项目的 [**最新发布**](https://github.com/z3lx/genshin-fov-unlock/releases/latest)。
   2. 从 **Assets** 区域下载 **plugin.zip** 压缩包。
   3. 解压到任意文件夹（**不要放在 genshin-fps-unlock 根目录**）。压缩包包含 **genshin_fov_unlock.dll** 和 **fov_config.json**。
3. 配置 **genshin-fps-unlock** 加载插件：
   1. 运行 **unlockfps_nc.exe**。
   2. 依次点击 **Options** → **Settings** → **DLLs** → **Add**。
   3. 添加 **genshin_fov_unlock.dll** 的完整路径。
4. （可选）修改插件配置：
   1. 使用文本编辑器打开 **fov_config.json**。
   2. 按需修改参数，详情见下方【配置】章节。

完成上述步骤后，通过 **unlockfps_nc.exe** 启动游戏时，插件将自动加载。如需了解更多工具使用方式，请参阅该工具的 [**README**](https://github.com/34736384/genshin-fps-unlock/blob/netcore/README.md)。

## 使用

- <kbd>←</kbd> / <kbd>→</kbd>：切换预设视野（FOV）。
- <kbd>↓</kbd>：启用/禁用插件。

## 配置

插件的行为可通过与 **genshin_fov_unlock.dll** 同目录的 **fov_config.json** 自定义。以下为可配置项：

- `enabled`（布尔值）：游戏启动时是否默认启用。
- `fov`（整数）：默认视野值。
- `fov_presets`（整数数组）：可切换的 FOV 预设列表。
- `smoothing`（浮点数）：指数滤波时间常数（秒），数值越小响应越快，设定过高可能触发完整性验证。
- `enable_key`（整数）：启用/禁用插件的按键代码。
- `next_key`（整数）：切换到下一个预设的按键代码。
- `prev_key`（整数）：切换到上一个预设的按键代码。

> **注意**：所有按键代码需使用十进制格式。参考 [虚拟按键码文档](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes) 获取合法值。

**默认配置**：

```json
{
    "enabled": true,
    "fov": 75,
    "fov_presets": [
        30,
        45,
        60,
        75,
        90,
        110
    ],
    "smoothing": 0.125,
    "enable_key": 40,
    "next_key": 39,
    "prev_key": 37
}
```

## 源代码构建

如果只想使用插件，建议参阅【安装】章节。若要从源码自行构建：

1. 在 Windows 环境下，确保已安装 git、CMake 和 MSVC。
2. 克隆项目并进入目录：
   ```bash
   git clone https://github.com/z3lx/genshin-fov-unlock.git
   cd genshin-fov-unlock
   ```
3. 配置项目（需联网获取依赖）：
   ```bash
   cmake . -G "Visual Studio 17 2022"
   ```
4. 构建项目：
   ```bash
   cmake --build . --config Release
   ```

构建完成后，**Release** 目录下会生成 **genshin_fov_unlock.dll**。

## 致谢

* 使用了 [**minhook**](https://github.com/TsudaKageyu/minhook)（BSD-2-Clause）；
* 使用了 [**nlohmann/json**](https://github.com/nlohmann/json)（MIT）；
* 灵感来源于 [**genshin-utility**](https://github.com/lanylow/genshin-utility)。

## 许可证

本项目采用 MIT 许可证。详情请参见仓库中的 [LICENSE](LICENSE) 文件。
