#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include <stdint.h>

// ========== 软件I2C总线结构体 ==========
// 每条I2C总线只需实例化一个此结构体，即可调用所有I2C原语
typedef struct {
    GPIO_TypeDef *scl_port;   // SCL GPIO端口
    uint16_t      scl_pin;    // SCL GPIO引脚
    GPIO_TypeDef *sda_port;   // SDA GPIO端口
    uint16_t      sda_pin;    // SDA GPIO引脚
    uint32_t      clk_rcc;    // RCC时钟使能位 (RCC_APB2Periph_GPIOx)
    GPIOMode_TypeDef  gpio_mode;  // GPIO模式: GPIO_Mode_Out_OD 或 GPIO_Mode_Out_PP
    uint8_t       initialized;// 是否已初始化
} SoftI2C_Bus;

// ========== I2C原语函数 ==========

// 总线初始化 (配置GPIO)
void OLED_I2C_Init(SoftI2C_Bus *bus);
void HDC1080_I2C_Init(SoftI2C_Bus *bus);

// 起始信号
void I2C_Start(SoftI2C_Bus *bus);

// 停止信号
void I2C_Stop(SoftI2C_Bus *bus);

// 写一个字节, 返回从机应答 (0=ACK, 1=NACK)
void Send_Byte(SoftI2C_Bus *bus, uint8_t dat);

// 读一个字节, is_last=1时发送NACK, is_last=0时发送ACK
uint8_t I2C_ReadByte(SoftI2C_Bus *bus, uint8_t is_last);

// 等待应答, 返回0=成功, 1=超时失败
uint8_t I2C_WaitAck(SoftI2C_Bus *bus);

// 微秒延时
void IIC_delay(uint32_t us);

#endif
