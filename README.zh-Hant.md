![版本](https://img.shields.io/badge/版本-5.7-brightgreen)
![下載次數](https://img.shields.io/github/downloads/z3lx/genshin-fov-unlock/total?label=下載次數)

# 原神視野（FOV）解鎖插件

[English](README.md) | [简体中文](README.zh-Hans.md) | **繁體中文**

> ⚠️ 此文為自動翻譯，僅供參考，請以英文原文為準。

這是一個用於原神（Genshin Impact）的插件，可將相機預設45°的視野（FOV）解鎖至更高數值。插件包含一個 DLL 檔案，需配合 [**genshin-fps-unlock**](https://github.com/34736384/genshin-fps-unlock) 一同使用。如果你覺得這個插件好用，別忘了給專案按個 ⭐！

本插件支援國際服（GL）和國服（CN），但尚不完整支援手把控制。

https://github.com/user-attachments/assets/56a11762-ebd2-4093-8c1d-768f913bd063

## 免責聲明

我個人使用該插件未遇過任何問題，也未被停權。但請注意，此類第三方插件違反遊戲服務條款。建議你自行審查原始碼並編譯，以確保透明可控並了解相關風險。

雖然插件本身很輕量，但提升視野會讓遊戲渲染更多物體，從而明顯影響效能。在極端數值下，還可能出現影像瑕疵。

## 安裝

1. 下載最新版 **genshin-fps-unlock** 工具：
   1. 前往專案的 [**最新發佈**](https://github.com/34736384/genshin-fps-unlock/releases/latest)。
   2. 從 **Assets** 區域下載 **unlockfps_nc.exe** 可執行檔。
   3. 將其置於任意資料夾中（後續步驟均以此檔案所在目錄為例）。
2. 下載最新版 **genshin-fov-unlock** 插件：
   1. 前往專案的 [**最新發佈**](https://github.com/z3lx/genshin-fov-unlock/releases/latest)。
   2. 從 **Assets** 區域下載 **plugin.zip** 壓縮檔。
   3. 解壓縮至任意資料夾（**勿放在 genshin-fps-unlock 根目錄**）。壓縮檔包含 **genshin_fov_unlock.dll** 和 **fov_config.json**。
3. 設定 **genshin-fps-unlock** 載入插件：
   1. 執行 **unlockfps_nc.exe**。
   2. 依序點選 **Options** → **Settings** → **DLLs** → **Add**。
   3. 新增 **genshin_fov_unlock.dll** 的完整路徑。
4. （選用）修改插件設定：
   1. 使用文字編輯器開啟 **fov_config.json**。
   2. 按需修改參數，詳情見下方【設定】章節。

完成上述步驟後，透過 **unlockfps_nc.exe** 啟動遊戲時，插件將自動載入。如需了解更多工具使用方式，請參閱該工具的 [**README**](https://github.com/34736384/genshin-fps-unlock/blob/netcore/README.md)。

## 使用方式

- <kbd>←</kbd> / <kbd>→</kbd>：切換預設視野（FOV）。
- <kbd>↓</kbd>：啟用/停用插件。

## 設定

插件的行為可透過與 **genshin_fov_unlock.dll** 同目錄的 **fov_config.json** 自訂。以下為可設定項：

- `enabled`（布林值）：遊戲啟動時是否預設啟用。
- `fov`（整數）：預設視野值。
- `fov_presets`（整數陣列）：可切換的 FOV 預設清單。
- `smoothing`（浮點數）：指數濾波時間常數（秒），數值越小響應越快，設定過高可能觸發完整性驗證。
- `enable_key`（整數）：啟用/停用插件的按鍵代碼。
- `next_key`（整數）：切換至下一個預設的按鍵代碼。
- `prev_key`（整數）：切換至上一個預設的按鍵代碼。

> **注意**：所有按鍵代碼需使用十進位格式。參考 [虛擬按鍵碼文件](https://learn.microsoft.com/zh-tw/windows/win32/inputdev/virtual-key-codes) 取得合法值。

**預設設定**：

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

## 原始碼建構

若只想使用插件，建議參閱【安裝】章節。若要從原始碼自行建構：

1. 在 Windows 環境下，確保已安裝 git、CMake 和 MSVC。
2. 複製儲存庫並進入目錄：
   ```bash
   git clone https://github.com/z3lx/genshin-fov-unlock.git
   cd genshin-fov-unlock
   ```
3. 設定專案（需連網取得相依項）：
   ```bash
   cmake . -G "Visual Studio 17 2022"
   ```
4. 建構專案：
   ```bash
   cmake --build . --config Release
   ```

建構完成後，**Release** 目錄下會產生 **genshin_fov_unlock.dll**。

## 致謝

* 使用了 [**minhook**](https://github.com/TsudaKageyu/minhook)（BSD-2-Clause）；
* 使用了 [**nlohmann/json**](https://github.com/nlohmann/json)（MIT）；
* 靈感來源於 [**genshin-utility**](https://github.com/lanylow/genshin-utility)。

## 授權許可

本專案採用 MIT 授權條款。詳情請參見儲存庫中的 [LICENSE](LICENSE) 檔案。
