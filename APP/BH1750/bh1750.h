#ifndef __BH1750_H
#define __BH1750_H

#include "stm32f10x.h"
#include <stdint.h>

// ========== 引脚定义 (可根据需要修改) ==========
// 使用 PB6=SCL, PB7=SDA
#define BH1750_SCL_PIN   GPIO_Pin_0
#define BH1750_SDA_PIN   GPIO_Pin_1
#define BH1750_GPIO_PORT GPIOA
#define BH1750_RCC_GPIO  RCC_APB2Periph_GPIOA

// ========== 设备地址 ==========
// ADDR接GND: 0x23, ADDR接VCC: 0x5C
#define BH1750_ADDR      0x23
#define BH1750_ADDR_WR  (BH1750_ADDR << 1)      // 0x46
#define BH1750_ADDR_RD  ((BH1750_ADDR << 1) | 1) // 0x47

// ========== 指令 ==========
#define BH1750_CMD_POWER_DOWN   0x00
#define BH1750_CMD_POWER_ON     0x01
#define BH1750_CMD_RESET        0x07
#define BH1750_CMD_CONT_H_MODE  0x10    // 连续高分辨率 1lx 120ms
#define BH1750_CMD_CONT_H_MODE2 0x11    // 连续高分辨率2 0.5lx
#define BH1750_CMD_CONT_L_MODE  0x13    // 连续低分辨率 4lx 16ms
#define BH1750_CMD_ONE_H_MODE   0x20    // 一次性高分辨率
#define BH1750_CMD_ONE_H_MODE2  0x21    // 一次性高分辨率2
#define BH1750_CMD_ONE_L_MODE   0x23    // 一次性低分辨率

// ========== API ==========
void BH1750_Init(void);
void BH1750_PowerOn(void);
void BH1750_PowerDown(void);
void BH1750_Reset(void);
void BH1750_SetMode(uint8_t mode);
float BH1750_ReadLight(void);
float BH1750_ReadOnce(void);
float set_led_on(void);
float BH1750_ReadLight_Filter(void);

#endif


