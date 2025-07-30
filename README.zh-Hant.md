![專案版本](https://img.shields.io/github/release/z3lx/genshin-fov-unlock?label=release)
![下載次數](https://img.shields.io/github/downloads/z3lx/genshin-fov-unlock/total?label=downloads)

# 原神解鎖器

[English](README.md) | [简体中文](README.zh-Hans.md) | **繁體中文**

> ⚠️ 此文為自動翻譯，僅供參考，請以英文原文為準。

一個提升遊戲體驗的模組，解鎖遊戲的幀率（FPS）和視野（FOV），並包含其他多種優化。如果您覺得這個項目有用，請考慮給倉庫點個星 🌟！

https://github.com/user-attachments/assets/56a11762-ebd2-4093-8c1d-768f913bd063

## 功能

- 定期更新以支援《原神》PC客戶端的全球及中國版本
- 解鎖幀率（FPS）和視野（FOV）
- 當遊戲未處於焦點時，限制幀率並降低遊戲進程優先權
- 與遊戲內序列化視野變化整合（例如爆發、過場動畫）
- 透過鍵位綁定快速切換視角解鎖狀態及預設模式
- 自訂遊戲啟動參數和顯示設定
- 相容[GIMI](https://github.com/SilentNightSound/GI-Model-Importer)及相關模組
- 不完全相容於PC手把佈局

## 免責聲明

儘管我自該模組發布以來一直使用且未遇到問題，但需注意第三方軟體修改遊戲行為可能違反遊戲服務條款，並存在帳號封鎖風險。建議您查看原始程式碼並自行建立專案以確保透明度並評估潛在風險。

## 安裝

由於該模組的實現方式，可能被殺毒軟體誤報為病毒。若在安裝過程中遇到此類問題，請考慮將模組新增至防毒軟體的例外清單。

1. 從 [最新版本](https://github.com/z3lx/genshin-fov-unlock/releases/latest) 下載 `mod.zip` 並解壓
2. 執行解壓縮後的資料夾中的 `loader.exe`
3. 等待遊戲啟動
4. （可選）透過編輯設定檔調整模組行為

模組在遊戲中啟動後：
- 使用 <kbd>←</kbd> 和 <kbd>→</kbd> 鍵循環切換 FOV 預設
- 使用 <kbd>↓</kbd> 鍵切換 FOV 解鎖狀態

與 [GIMI](https://github.com/SilentNightSound/GI-Model-Importer) 搭配使用：
1. 使用文字編輯器開啟 `loader_config.json` 文件
2. 將 GIMI 的 `d3d11.dll` 路徑加入 `dllPaths` 陣列中
```json
"dllPaths": [
    "plugin.dll",
    "C:\\path\\to\\GIMI\\d3d11.dll"
],
```

## 配置

模組的行為可透過 `loader_config.json` 和 `plugin_config.json` 檔案進行自訂，具體如下。

### 載入器

| 鍵                | 類型         | 描述                     |
|------------------|------------|------------------------|
| `checkUpdates`   | `bool`     | 檢查模組的新版本               |
| `gamePath`       | `string`   | Genshin Impact 可執行檔的路徑 |
| `overrideArgs`   | `bool`     | 覆蓋遊戲的預設啟動參數            |
| `monitorIndex`   | `int`      | 用於遊戲視窗的監視器索引（從 1 開始）   |
| `displayMode`    | `string`   | 遊戲視窗的顯示模式              |
| `screenWidth`    | `int`      | 遊戲視窗的寬度（以像素為單位）        |
| `screenHeight`   | `int`      | 遊戲視窗的高度（以像素為單位）        |
| `mobilePlatform` | `bool`     | 啟用行動裝置 UI              |
| `additionalArgs` | `string`   | 傳遞給遊戲執行檔的額外參數          |
| `dllPaths`       | `string[]` | 要注入到遊戲中的 DLL 檔案清單      |
| `suspendLoad`    | `bool`     | 暫停遊戲進程直至模組完全載入         |

<details>

<summary>有效顯示模式</summary>

| 值            | 說明       |
|--------------|----------|
| `Windowed`   | 視窗化模式    |
| `Fullscreen` | 全螢幕獨佔模式  |
| `Borderless` | 無邊框視窗化模式 |

</details>

預設載入器配置：

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

| 鍵                  | 類型       | 描述                     |
|--------------------|----------|------------------------|
| `unlockFps`        | `bool`   | 啟用幀率（FPS）解鎖            |
| `targetFps`        | `int`    | 解鎖幀率時的目標幀率             |
| `autoThrottle`     | `bool`   | 當外掛程式未被聚焦時限制幀率並降低進程優先權 |
| `unlockFov`        | `bool`   | 啟用視野角（FOV）解鎖           |
| `targetFov`        | `int`    | 啟動時套用的預設FOV            |
| `fovPresets`       | `int[]`  | 要循環的FOV值清單             |
| `fovSmoothing`     | `float`  | FOV過渡時的平滑因子（以秒為單位）     |
| `unlockFovKey`     | `string` | 用於切換視場解鎖的鍵綁定           |
| `nextFovPresetKey` | `string` | 用於循環到下一個視場預設的鍵綁定       |
| `prevFovPresetKey` | `string` | 用於循環到上一個視場預設的鍵綁定       |

<details>

<summary>有效鍵綁定</summary>

| 常數                | 說明                     |
|-------------------|------------------------|
| `LeftMouse`       | 左鍵                     |
| `RightMouse`      | 右鍵                     |
| `MiddleMouse`     | 中鍵                     |
| `X1Mouse`         | X1鍵                    |
| `X2Mouse`         | X2鍵                    |
| `Backspace`       | 退格鍵                    |
| `Tab`             | 製表鍵                    |
| `Clear`           | 清除鍵                    |
| `Enter`           | 回車鍵                    |
| `Shift`           | 換頁鍵                    |
| `Ctrl`            | Ctrl 鍵                 |
| `Alt`             | Alt 鍵                  |
| `Pause`           | 暫停鍵                    |
| `CapsLock`        | 大寫鎖定鍵                  |
| `Esc`             | 逃逸鍵                    |
| `Space`           | 空白鍵                    |
| `PageUp`          | 頁面向上鍵                  |
| `PageDown`        | 頁面向下鍵                  |
| `End`             | 末尾鍵                    |
| `Home`            | 首頁鍵                    |
| `LeftArrow`       | 左箭頭鍵                   |
| `UpArrow`         | 上箭頭鍵                   |
| `RightArrow`      | 右箭頭鍵                   |
| `DownArrow`       | 下箭頭鍵                   |
| `PrintScreen`     | 列印螢幕鍵                  |
| `Insert`          | 插入鍵                    |
| `Delete`          | 刪除鍵                    |
| `0`               | 0 鍵                    |
| `1`               | 1 鍵                    |
| `2`               | 2 鍵                    |
| `3`               | 3 鍵                    |
| `4`               | 4 鍵                    |
| `5`               | 5 鍵                    |
| `6`               | 6 鍵                    |
| `7`               | 7 鍵                    |
| `8`               | 8 鍵                    |
| `9`               | 9 鍵                    |
| `A`               | A 鍵                    |
| `B`               | B 鍵                    |
| `C`               | C 鍵                    |
| `D`               | D 鍵                    |
| `E`               | E 鍵                    |
| `F`               | F 鍵                    |
| `G`               | G 鍵                    |
| `H`               | H 鍵                    |
| `I`               | I 鍵                    |
| `J`               | J 鍵                    |
| `K`               | K 鍵                    |
| `L`               | L 鍵                    |
| `M`               | M 鍵                    |
| `N`               | N 鍵                    |
| `O`               | O 鍵                    |
| `P`               | P 鍵                    |
| `Q`               | Q 鍵                    |
| `R`               | R 鍵                    |
| `S`               | S 鍵                    |
| `T`               | T 鍵                    |
| `U`               | U 鍵                    |
| `V`               | V 鍵                    |
| `W`               | W 鍵                    |
| `X`               | X 鍵                    |
| `Y`               | Y 鍵                    |
| `Z`               | Z 鍵                    |
| `LeftWindows`     | 左 Windows 標誌鍵          |
| `RightWindows`    | 右 Windows 標誌鍵          |
| `Apps`            | 應用程式鍵                  |
| `Numpad0`         | 數字小鍵盤 0 鍵              |
| `Numpad1`         | 數字小鍵盤 1 鍵              |
| `Numpad2`         | 數字小鍵盤 2 鍵              |
| `Numpad3`         | 數字小鍵盤 3 鍵              |
| `Numpad4`         | 數字小鍵盤 4 鍵              |
| `Numpad5`         | 數字小鍵盤 5 鍵              |
| `Numpad6`         | 數字小鍵盤 6 鍵              |
| `Numpad7`         | 數字小鍵盤 7 鍵              |
| `Numpad8`         | 數字小鍵盤 8 鍵              |
| `Numpad9`         | 數字小鍵盤 9 鍵              |
| `NumpadMultiply`  | 乘法鍵                    |
| `NumpadAdd`       | 加法鍵                    |
| `NumpadSeparator` | 分隔符號鍵                  |
| `NumpadSubtract`  | 減法鍵                    |
| `NumpadDecimal`   | 小數鍵                    |
| `NumpadDivide`    | 除號鍵                    |
| `F1`              | F1 鍵                   |
| `F2`              | F2 鍵                   |
| `F3`              | F3 鍵                   |
| `F4`              | F4 鍵                   |
| `F5`              | F5 鍵                   |
| `F6`              | F6 鍵                   |
| `F7`              | F7 鍵                   |
| `F8`              | F8 鍵                   |
| `F9`              | F9 鍵                   |
| `F10`             | F10 鍵                  |
| `F11`             | F11 鍵                  |
| `F12`             | F12 鍵                  |
| `F13`             | F13 鍵                  |
| `F14`             | F14 鍵                  |
| `F15`             | F15 鍵                  |
| `F16`             | F16 鍵                  |
| `F17`             | F17 鍵                  |
| `F18`             | F18 鍵                  |
| `F19`             | F19 鍵                  |
| `F20`             | F20 鍵                  |
| `F21`             | F21 鍵                  |
| `F22`             | F22 鍵                  |
| `F23`             | F23 鍵                  |
| `F24`             | F24 鍵                  |
| `NumLock`         | Num Lock 鍵             |
| `ScrollLock`      | 滾動鎖定鍵                  |
| `LeftShift`       | 左 Shift 鍵              |
| `RightShift`      | 右 Shift 鍵              |
| `LeftCtrl`        | 左 Ctrl 鍵               |
| `RightCtrl`       | 右 Ctrl 鍵               |
| `LeftAlt`         | 左Alt鍵                  |
| `RightAlt`        | 右Alt鍵                  |
| `Oem1`            | 對於美國ANSI鍵盤，分號與冒號鍵      |
| `Plus`            | 對於任何國家和地區，等號和加號鍵       |
| `Comma`           | 對於任何國家和地區，逗號和小於號鍵      |
| `Minus`           | 適用於任何國家和地區，破折號和下劃線鍵    |
| `Period`          | 適用於任何國家和地區，句點和大於號鍵     |
| `Oem2`            | 適用於美國 ANSI 鍵盤，正斜線與問號鍵  |
| `Oem3`            | 適用於美國 ANSI 鍵盤，重音符和波浪號鍵 |
| `Oem4`            | 適用於美國 ANSI 鍵盤，左大括號鍵    |
| `Oem5`            | 適用於美國 ANSI 鍵盤，反斜線與垂直線鍵 |
| `Oem6`            | 適用於美國 ANSI 鍵盤，右大括號鍵    |
| `Oem7`            | 適用於美國 ANSI 鍵盤，撇號與雙引號鍵  |
| `Oem8`            | 加拿大 CSA 鍵盤的右 Ctrl 鍵    |
| `Oem102`          | 歐洲 ISO 鍵盤的反斜線與垂直線鍵     |
| `OemClear`        | 清除鍵                    |

</details>

預設插件配置：

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

## 建構

如果您希望直接使用該模組，可在 [最新版本](https://github.com/z3lx/genshin-fov-unlock/releases/latest) 中取得預編譯二進位檔。若需要從原始碼建置專案，您必須使用 Windows 系統並已安裝 MSVC v143（Visual Studio 2022）、Windows 11 SDK、CMake 和 Git。然後，請按照以下步驟操作：

1. 克隆倉庫
```bash
git clone https://github.com/z3lx/genshin-fov-unlock.git
cd genshin-fov-unlock
```
2. 配置項目及其相依性
```bash
cmake . -G "Visual Studio 17 2022"
```
3. 建構項目
```bash
cmake --build . --config Release
```

## 致謝

本專案使用了以下程式庫和資源：
- [`cpr`](https://github.com/libcpr/cpr)，以 MIT 授權授權
- [`glaze`](https://github.com/stephenberry/glaze)，採用 MIT 許可證
- [`minhook`](https://github.com/TsudaKageyu/minhook)，採用 BSD-2-Clause 許可證
- [`wil`](https://github.com/microsoft/wil)，採用 MIT 許可證
- [`genshin-fps-unlock`](https://github.com/34736384/genshin-fps-unlock)，用於實現細節的參考
- [`genshin-utility`](https://github.com/lanylow/genshin-utility)，用於模組初始構思的參考
- [`gurh32`](https://x.com/gurh32/status/1944266962496106662)，載入器圖示所用藝術作品的來源

## 許可證

本項目採用 MIT 許可證。請參閱 [LICENSE](LICENSE) 檔案以取得更多資訊。
