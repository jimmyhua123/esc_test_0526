// dshot_decoder.c
#include "dshot_decoder.h"
#include "main.h" // for HAL libs or motor control APIs

#define MAX_IQ_REF 1024
#define THROTTLE_MIN 48
#define THROTTLE_MAX 2047

volatile uint16_t dshot_rx_packet = 0;

// === CRC 計算函式 ===
uint8_t DShot_CalculateCRC(uint16_t packet)
{
    uint8_t crc = 0;
    packet >>= 4;  // 只取前 12 bits

    for (int i = 0; i < 3; i++) {
        crc ^= (packet >> (i * 4)) & 0xF;
    }

    return crc & 0xF;
}

// === 主解碼函式 ===
void DShot_ProcessPacket(uint16_t packet)
{
    uint8_t telemetry = (packet >> 15) & 0x01;
    uint16_t value = (packet >> 4) & 0x7FF;
    uint8_t crc = packet & 0x0F;

    if (DShot_CalculateCRC(packet) != crc) {
        return; // CRC 錯誤
    }

    if (value >= THROTTLE_MIN) {
        uint16_t throttle_val = value - THROTTLE_MIN;
        ApplyThrottle(throttle_val);
    } else {
        ExecuteCommand(value);
    }

    if (telemetry) {
        StartTelemetryRequest();
    }
}

// === throttle 映射至 IqRef ===
void ApplyThrottle(uint16_t throttle_val)
{
    int16_t iq_target = (int32_t)throttle_val * MAX_IQ_REF / (THROTTLE_MAX - THROTTLE_MIN);

    if (iq_target > MAX_IQ_REF)
        iq_target = MAX_IQ_REF;

    MC_ProgramTorqueRampMotor1(iq_target, 0); // ST Motor SDK API
}

// === 命令執行區 ===
void ExecuteCommand(uint16_t cmd)
{
    switch (cmd)
    {
        case 0:
            break;
        case 1:
            MotorArm();
            break;
        case 2:
            MotorDisarm();
            break;
        case 3:
            StartBeepPattern();
            break;
        default:
            break;
    }
}

// === Stub 可擴充函數 ===
void MotorArm(void) {}
void MotorDisarm(void) {}
void StartBeepPattern(void) {}
void StartTelemetryRequest(void) {}

// === DMA 回呼函式（示意） ===
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) // 假設用 TIM3 接收
    {
        DShot_ProcessPacket(dshot_rx_packet);
    }
}


