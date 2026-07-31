#include "i2c.h"
#include "delay.h"

// ========== 内部引脚操作宏 ==========
#define SCL_HIGH(bus)  GPIO_SetBits((bus)->scl_port, (bus)->scl_pin)
#define SCL_LOW(bus)   GPIO_ResetBits((bus)->scl_port, (bus)->scl_pin)
#define SDA_HIGH(bus)  GPIO_SetBits((bus)->sda_port, (bus)->sda_pin)
#define SDA_LOW(bus)   GPIO_ResetBits((bus)->sda_port, (bus)->sda_pin)
#define SDA_READ(bus)  GPIO_ReadInputDataBit((bus)->sda_port, (bus)->sda_pin)

// ========== 微秒延时 (STM32F103 @ 72MHz) ==========
void IIC_delay(uint32_t us) {
    // uint32_t i;
    // for (; us > 0; us--) {
    //     i = 72;   // 72MHz下, 循环72次约1us
    //     while (i--);
    // }
    u8 t=3;
	while(t--);
}

// ========== 总线初始化 ==========
void SoftI2C_Init(SoftI2C_Bus *bus) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(bus->clk_rcc, ENABLE);

    // SCL配置
    GPIO_InitStructure.GPIO_Pin   = bus->scl_pin;
    GPIO_InitStructure.GPIO_Mode  = bus->gpio_mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(bus->scl_port, &GPIO_InitStructure);

    // SDA配置
    GPIO_InitStructure.GPIO_Pin   = bus->sda_pin;
    GPIO_InitStructure.GPIO_Mode  = bus->gpio_mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(bus->sda_port, &GPIO_InitStructure);

    // 总线空闲: SCL和SDA拉高
    // SCL_HIGH(bus);
    // SDA_HIGH(bus);
    delay_ms(200);
    bus->initialized = 1;
}

// ========== 起始信号 ==========
void I2C_Start(SoftI2C_Bus *bus) {
    SDA_HIGH(bus);
    SCL_HIGH(bus);
    IIC_delay(5);
    SDA_LOW(bus);
    IIC_delay(5);
    SCL_LOW(bus);
}

// ========== 停止信号 ==========
void I2C_Stop(SoftI2C_Bus *bus) {
    SDA_LOW(bus);
    SCL_HIGH(bus);
    IIC_delay(5);
    SDA_HIGH(bus);
}

//写入一个字节
void Send_Byte(SoftI2C_Bus *bus, uint8_t dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)//将dat的8位从最高位依次写入
		{
			SDA_HIGH(bus);
    }
		else
		{
			SDA_LOW(bus);
    }
		IIC_delay(5);
		SCL_HIGH(bus);
		IIC_delay(5);
		SCL_LOW(bus);//将时钟信号设置为低电平
		dat<<=1;
  }
}

// ========== 读一个字节 ==========
// is_last=1: 发送NACK (最后一字节)
// is_last=0: 发送ACK (继续读取)
uint8_t I2C_ReadByte(SoftI2C_Bus *bus, uint8_t is_last) {
    uint8_t i, dat = 0;
    SDA_HIGH(bus);  // 释放数据线
    for (i = 0; i < 8; i++) {
        dat <<= 1;
        SCL_HIGH(bus);
        IIC_delay(5);
        if (SDA_READ(bus)) dat |= 0x01;
        SCL_LOW(bus);
        IIC_delay(5);
    }
    // 发送应答位
    if (is_last) {
        SDA_HIGH(bus);  // NACK
    } else {
        SDA_LOW(bus);   // ACK
    }
    SCL_HIGH(bus);
    IIC_delay(5);
    SCL_LOW(bus);
    SDA_HIGH(bus);  // 释放数据线
    return dat;
}

// ========== 等待应答 ==========
// 返回0=成功, 1=超时失败
uint8_t I2C_WaitAck(SoftI2C_Bus *bus) {
    uint8_t errTime = 0;
    SDA_HIGH(bus);
    delay_us(1);
    SCL_HIGH(bus);
    delay_us(1);
    // while (SDA_READ(bus)) {
    //     errTime++;
    //     if (errTime > 250) {
    //         I2C_Stop(bus);
    //         return 1;
    //     }
    // }
    SCL_LOW(bus);
    delay_us(1);
    return 0;
}

