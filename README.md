![Project Version](https://img.shields.io/github/release/z3lx/genshin-fov-unlock?label=release)
![Download Count](https://img.shields.io/github/downloads/z3lx/genshin-fov-unlock/total?label=downloads)

# Genshin Impact Unlocker

**English** | [简体中文](README.zh-Hans.md) | [繁體中文](README.zh-Hant.md)

A quality of life mod for Genshin Impact that unlocks the game's frame rate (FPS) and field of view (FOV), along with various other enhancements. If you find this project useful, consider giving the repository a star 🌟!

https://github.com/user-attachments/assets/56a11762-ebd2-4093-8c1d-768f913bd063

## Features

- Regularly updated to support the PC client for both global and Chinese versions of Genshin Impact
- Unlocks frame rate (FPS) and field of view (FOV)
- Throttles FPS and lowers game process priority when unfocused
- Integrates with in-game sequenced FOV changes (e.g., bursts, cutscenes)
- Uses keybindings to easily toggle FOV unlock and switch between presets
- Customizable game launch arguments and display settings
- Compatible with [GIMI](https://github.com/SilentNightSound/GI-Model-Importer) and associated mods
- Not fully compatible with the PC controller layout

## Disclaimer

While I have used this mod since its conception without issues, it is important to note that third-party software that alters the game's behavior may violate the game's Terms of Service and may carry a risk of account suspension. You are encouraged to review the source code and build the project yourself to ensure transparency and assess potential risks.

## Installation

Due to the nature of the mod's implementation, it may be flagged by antivirus software as a false positive. If you encounter such issues during the installation process, consider adding the mod to your antivirus's exclusion list.

1. Download `mod.zip` from the [latest release](https://github.com/z3lx/genshin-fov-unlock/releases/latest) and extract it
2. Run `loader.exe` from the extracted folder
3. Wait for the game to launch
4. (Optional) Configure the mod's behavior by editing the configuration files

Once the mod is active in-game:
- Use the <kbd>←</kbd> and <kbd>→</kbd> keys to cycle through the FOV presets
- Use the <kbd>↓</kbd> key to toggle the FOV unlocking

To use the mod with [GIMI](https://github.com/SilentNightSound/GI-Model-Importer):
1. Open the `loader_config.json` file with a text editor
2. Add GIMI's `d3d11.dll` path to the `dllPaths` array
```json
"dllPaths": [
    "plugin.dll",
    "C:\\path\\to\\GIMI\\d3d11.dll"
],
```

## Configuration

The mod's behavior can optionally be customized through the `loader_config.json` and `plugin_config.json` files, described below.

### Loader

| Key              | Type       | Description                                             |
|------------------|------------|---------------------------------------------------------|
| `checkUpdates`   | `bool`     | Checks for new versions of the mod                      |
| `gamePath`       | `string`   | Path to the Genshin Impact executable                   |
| `overrideArgs`   | `bool`     | Overrides the game's default launch arguments           |
| `monitorIndex`   | `int`      | 1-based index of the monitor used for the game window   |
| `displayMode`    | `string`   | Display mode for the game window                        |
| `screenWidth`    | `int`      | Width of the game window in pixels                      |
| `screenHeight`   | `int`      | Height of the game window in pixels                     |
| `mobilePlatform` | `bool`     | Enables mobile UI                                       |
| `additionalArgs` | `string`   | Additional arguments to pass to the game executable     |
| `dllPaths`       | `string[]` | List of DLLs to inject with the game                    |
| `suspendLoad`    | `bool`     | Suspends the game process until the mod is fully loaded |

<details>

<summary>Valid display modes</summary>

| Value        | Description               |
|--------------|---------------------------|
| `Windowed`   | Windowed mode             |
| `Fullscreen` | Fullscreen exclusive mode |
| `Borderless` | Borderless windowed mode  |

</details>

Default loader configuration:

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

### Plugin

| Key                | Type     | Description                                           |
|--------------------|----------|-------------------------------------------------------|
| `unlockFps`        | `bool`   | Enables frame rate (FPS) unlocking                    |
| `targetFps`        | `int`    | Target frame rate when FPS is unlocked                |
| `autoThrottle`     | `bool`   | Limits FPS and lowers process priority when unfocused |
| `unlockFov`        | `bool`   | Enables field of view (FOV) unlocking                 |
| `targetFov`        | `int`    | Default FOV applied at startup                        |
| `fovPresets`       | `int[]`  | List of FOV values to cycle through                   |
| `fovSmoothing`     | `float`  | Smoothing factor (in seconds) for FOV transitions     |
| `unlockFovKey`     | `string` | Keybinding to toggle FOV unlock                       |
| `nextFovPresetKey` | `string` | Keybinding to cycle to the next FOV preset            |
| `prevFovPresetKey` | `string` | Keybinding to cycle to the previous FOV preset        |

<details>

<summary>Valid keybindings</summary>

| Constant          | Description                                                            |
|-------------------|------------------------------------------------------------------------|
| `LeftMouse`       | Left mouse button                                                      |
| `RightMouse`      | Right mouse button                                                     |
| `MiddleMouse`     | Middle mouse button                                                    |
| `X1Mouse`         | X1 mouse button                                                        |
| `X2Mouse`         | X2 mouse button                                                        |
| `Backspace`       | Backspace key                                                          |
| `Tab`             | Tab key                                                                |
| `Clear`           | Clear key                                                              |
| `Enter`           | Enter key                                                              |
| `Shift`           | Shift key                                                              |
| `Ctrl`            | Ctrl key                                                               |
| `Alt`             | Alt key                                                                |
| `Pause`           | Pause key                                                              |
| `CapsLock`        | Caps lock key                                                          |
| `Esc`             | Esc key                                                                |
| `Space`           | Spacebar key                                                           |
| `PageUp`          | Page up key                                                            |
| `PageDown`        | Page down key                                                          |
| `End`             | End key                                                                |
| `Home`            | Home key                                                               |
| `LeftArrow`       | Left arrow key                                                         |
| `UpArrow`         | Up arrow key                                                           |
| `RightArrow`      | Right arrow key                                                        |
| `DownArrow`       | Down arrow key                                                         |
| `PrintScreen`     | Print screen key                                                       |
| `Insert`          | Insert key                                                             |
| `Delete`          | Delete key                                                             |
| `0`               | 0 key                                                                  |
| `1`               | 1 key                                                                  |
| `2`               | 2 key                                                                  |
| `3`               | 3 key                                                                  |
| `4`               | 4 key                                                                  |
| `5`               | 5 key                                                                  |
| `6`               | 6 key                                                                  |
| `7`               | 7 key                                                                  |
| `8`               | 8 key                                                                  |
| `9`               | 9 key                                                                  |
| `A`               | A key                                                                  |
| `B`               | B key                                                                  |
| `C`               | C key                                                                  |
| `D`               | D key                                                                  |
| `E`               | E key                                                                  |
| `F`               | F key                                                                  |
| `G`               | G key                                                                  |
| `H`               | H key                                                                  |
| `I`               | I key                                                                  |
| `J`               | J key                                                                  |
| `K`               | K key                                                                  |
| `L`               | L key                                                                  |
| `M`               | M key                                                                  |
| `N`               | N key                                                                  |
| `O`               | O key                                                                  |
| `P`               | P key                                                                  |
| `Q`               | Q key                                                                  |
| `R`               | R key                                                                  |
| `S`               | S key                                                                  |
| `T`               | T key                                                                  |
| `U`               | U key                                                                  |
| `V`               | V key                                                                  |
| `W`               | W key                                                                  |
| `X`               | X key                                                                  |
| `Y`               | Y key                                                                  |
| `Z`               | Z key                                                                  |
| `LeftWindows`     | Left Windows logo key                                                  |
| `RightWindows`    | Right Windows logo key                                                 |
| `Apps`            | Application key                                                        |
| `Numpad0`         | Numeric keypad 0 key                                                   |
| `Numpad1`         | Numeric keypad 1 key                                                   |
| `Numpad2`         | Numeric keypad 2 key                                                   |
| `Numpad3`         | Numeric keypad 3 key                                                   |
| `Numpad4`         | Numeric keypad 4 key                                                   |
| `Numpad5`         | Numeric keypad 5 key                                                   |
| `Numpad6`         | Numeric keypad 6 key                                                   |
| `Numpad7`         | Numeric keypad 7 key                                                   |
| `Numpad8`         | Numeric keypad 8 key                                                   |
| `Numpad9`         | Numeric keypad 9 key                                                   |
| `NumpadMultiply`  | Multiply key                                                           |
| `NumpadAdd`       | Add key                                                                |
| `NumpadSeparator` | Separator key                                                          |
| `NumpadSubtract`  | Subtract key                                                           |
| `NumpadDecimal`   | Decimal key                                                            |
| `NumpadDivide`    | Divide key                                                             |
| `F1`              | F1 key                                                                 |
| `F2`              | F2 key                                                                 |
| `F3`              | F3 key                                                                 |
| `F4`              | F4 key                                                                 |
| `F5`              | F5 key                                                                 |
| `F6`              | F6 key                                                                 |
| `F7`              | F7 key                                                                 |
| `F8`              | F8 key                                                                 |
| `F9`              | F9 key                                                                 |
| `F10`             | F10 key                                                                |
| `F11`             | F11 key                                                                |
| `F12`             | F12 key                                                                |
| `F13`             | F13 key                                                                |
| `F14`             | F14 key                                                                |
| `F15`             | F15 key                                                                |
| `F16`             | F16 key                                                                |
| `F17`             | F17 key                                                                |
| `F18`             | F18 key                                                                |
| `F19`             | F19 key                                                                |
| `F20`             | F20 key                                                                |
| `F21`             | F21 key                                                                |
| `F22`             | F22 key                                                                |
| `F23`             | F23 key                                                                |
| `F24`             | F24 key                                                                |
| `NumLock`         | Num lock key                                                           |
| `ScrollLock`      | Scroll lock key                                                        |
| `LeftShift`       | Left Shift key                                                         |
| `RightShift`      | Right Shift key                                                        |
| `LeftCtrl`        | Left Ctrl key                                                          |
| `RightCtrl`       | Right Ctrl key                                                         |
| `LeftAlt`         | Left Alt key                                                           |
| `RightAlt`        | Right Alt key                                                          |
| `Oem1`            | For the US ANSI keyboard, the Semiсolon and Colon key                  |
| `Plus`            | For any country/region, the Equals and Plus key                        |
| `Comma`           | For any country/region, the Comma and Less Than key                    |
| `Minus`           | For any country/region, the Dash and Underscore key                    |
| `Period`          | For any country/region, the Period and Greater Than key                |
| `Oem2`            | For the US ANSI keyboard, the Forward Slash and Question Mark key      |
| `Oem3`            | For the US ANSI keyboard, the Grave Accent and Tilde key               |
| `Oem4`            | For the US ANSI keyboard, the Left Brace key                           |
| `Oem5`            | For the US ANSI keyboard, the Backslash and Pipe key                   |
| `Oem6`            | For the US ANSI keyboard, the Right Brace key                          |
| `Oem7`            | For the US ANSI keyboard, the Apostrophe and Double Quotation Mark key |
| `Oem8`            | For the Canadian CSA keyboard, the Right Ctrl key                      |
| `Oem102`          | For the European ISO keyboard, the Backslash and Pipe key              |
| `OemClear`        | Clear key                                                              |

</details>

Default plugin configuration:

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

## Building

If you're looking to use the mod directly, a pre-built binary is available in the [latest release](https://github.com/z3lx/genshin-fov-unlock/releases/latest). To build the project from source, you must be on Windows and have MSVC v143 (Visual Studio 2022), Windows 11 SDK, CMake, and Git installed. Then, follow these steps:

1. Clone the repository
```bash
git clone https://github.com/z3lx/genshin-fov-unlock.git
cd genshin-fov-unlock
```
2. Configure the project and its dependencies
```bash
cmake . -G "Visual Studio 17 2022"
```
3. Build the project
```bash
cmake --build . --config Release
```

## Attributions

This project makes use of the following libraries and resources:
- [`cpr`](https://github.com/libcpr/cpr), licensed under the MIT License
- [`glaze`](https://github.com/stephenberry/glaze), licensed under the MIT License
- [`minhook`](https://github.com/TsudaKageyu/minhook), licensed under the BSD-2-Clause License
- [`wil`](https://github.com/microsoft/wil), licensed under the MIT License
- [`genshin-fps-unlock`](https://github.com/34736384/genshin-fps-unlock), referenced for implementation details
- [`genshin-utility`](https://github.com/lanylow/genshin-utility), referenced for the initial conception of the mod
- [`gurh32`](https://x.com/gurh32/status/1944266962496106662), source of the artwork used for the loader icon

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
