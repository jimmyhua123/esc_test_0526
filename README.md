
---

# STM32G431 BLDC Motor Control (MCSDK 6.3.2)

此專案為使用 STMicroelectronics MCSDK v6.3.2 建構的 BLDC 三相電機驅動器範例，核心平台為 STM32G431，搭配 ST 的 B-G431B-ESC1 開發板進行驅動與通訊實驗，使用 Field Oriented Control（FOC）為控制架構。

## 硬體需求

* STM32G431 開發板（建議 B-G431B-ESC1）
* 三相無刷直流馬達（BLDC Motor）
* 馬達電源（建議 12V）
* ST-Link V3 或其他相容的 Debugger
* UART to USB 模組（用於 MCP 通訊）

## 主要功能

* 無感 FOC 控制架構
* 支援速度與轉矩雙控制模式
* PID 速度迴授、Iq/Id 電流控制
* 虛擬速度感測器（VSS）
* 相電流偵測（3-shunt）
* UART + MCP 通訊協定支援
* 支援 ST Motor Pilot 工具進行參數調校與監控

## 軟體架構

* **main.c**：初始化所有 HAL 和 MotorControl 相關模組。
* **motorcontrol.c**：呼叫 `MCboot()` 啟動馬達控制子系統。
* **mc\_interface.c / mc\_api.c**：定義上層控制 API，例如 `MC_StartMotor1()`、`MC_ProgramSpeedRampMotor1()` 等。
* **mc\_tasks.c / mc\_tasks\_foc.c**：週期性執行中頻與高頻任務（PID、FOC 控制邏輯）。
* **mcp\_config.c / mcp.c**：MCP 協定封裝，使用 UART 傳遞控制命令與回報資料。

## 控制流程

1. 初始化 MCU 和周邊。
2. 呼叫 `MX_MotorControl_Init()` → `MCboot()` → `FOC_Init()`。
3. 在主迴圈中呼叫 `MC_RunMotorControlTasks()` 處理通訊與中頻任務。
4. 透過 UART（MCP 協定）傳送指令控制馬達狀態。

## 常用 API

```c
MC_StartMotor1();
MC_StopMotor1();
MC_ProgramSpeedRampMotor1(int16_t speed, uint16_t duration_ms);
MC_SetCurrentReferenceMotor1(qd_t iqd);
```

## MCP 指令範例

* 設定速度參考（32-bit）
* 設定控制模式（8-bit）
* 讀取即時電壓、電流、速度等狀態值（16-bit）

## 編譯工具

* STM32CubeIDE v1.14 或以上
* STM32CubeMX 搭配 MCSDK v6.3.2
* GCC ARM 編譯器

## 使用說明

1. 使用 STM32CubeIDE 匯入本專案。
2. 編譯與燒錄至 STM32G431。
3. 使用 ST Motor Pilot 或串列工具透過 UART 操作 MCP 命令。

## 參考資料

* [ST Motor Control SDK v6.3.2](https://www.st.com/en/embedded-software/x-cube-mcsdk.html)
* [B-G431B-ESC1 說明頁面](https://www.st.com/en/evaluation-tools/b-g431b-esc1.html)
* [Motor Pilot Tool](https://www.st.com/en/development-tools/motor-pilot.html)

---


