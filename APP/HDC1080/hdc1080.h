#ifndef __HDC1080_H
#define __HDC1080_H

#include "stm32f10x.h"
#include <stdint.h>

//------------------ ????? ------------------
#define HDC1080_ADDRESS        0x80        // 7?????[reference:0][reference:1]
#define HDC1080_TEMP_REG       0x00        // ?????[reference:2][reference:3]
#define HDC1080_HUMID_REG      0x01        // ?????[reference:4][reference:5]
#define HDC1080_CONFIG_REG     0x02        // ?????[reference:6][reference:7]

//------------------ ???????? ------------------
#define HDC1080_CONFIG_DEFAULT 0x1000      // ????: 14-bit, ????[reference:8]

//------------------ ???? ------------------
void HDC1080_Init(void);
void HDC1080_ReadTempHum(float *temperature, float *humidity);
void I2C_Delay_us(uint32_t us);
void Read_Temp_Humi_Reg(uint8_t *data);
void HDC1080_HeaterOn(void);
void HDC1080_HeaterOff(void);
float HDC1080_ReadTemp_Smooth(float get_temp);

#endif

