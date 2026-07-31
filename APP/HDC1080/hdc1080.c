#include "hdc1080.h"
#include "delay.h"      // ?????/???????
#include "sys.h"
#include "math.h"
#include "i2c.h"
#include "FreeRTOS.h"
#include "task.h"

// ========== 1. 修改这里的引脚定义 ==========
#define I2C_SCL_PORT   GPIOA
#define I2C_SCL_PIN    GPIO_Pin_9

#define I2C_SDA_PORT   GPIOA
#define I2C_SDA_PIN    GPIO_Pin_10

SoftI2C_Bus hdc1080_bus_info = 
{
	GPIOA,
	GPIO_Pin_9,
	GPIOA,
	GPIO_Pin_10,
	RCC_APB2Periph_GPIOA,
	GPIO_Mode_Out_PP,
	0
};
// ========== 2. 初始化函数（在 main 中调用） ==========
void HDC1080_Init(void) {
    HDC1080_I2C_Init(&hdc1080_bus_info);
}

void HDC1080_ReadTempHum(float *temperature, float *humidity)
{
    uint8_t data[4];
    /* 采集温湿度 */
    Read_Temp_Humi_Reg(data);
    /* 温度转换 */
    *temperature=(float)(data[0]<<8|data[1]);
    *temperature=(*temperature/pow(2,16))*165.0f-40.0f;
    /* 湿度转换 */
    *humidity=0;
    *humidity=(float)(data[2]<<8|data[3]);
    *humidity=(*humidity/pow(2,16))*100.0f;

    if (*humidity < 0) *humidity = 0;
}

void Read_Temp_Humi_Reg(uint8_t *data)
{
    data[0]=0;
    data[1]=0;
    data[2]=0;
    data[3]=0;
    I2C_Start(&hdc1080_bus_info);   //起始信号
    Send_Byte(&hdc1080_bus_info,HDC1080_ADDRESS);   //发送HDC1080设备地址+写信号
    // I2C_WaitAck();
    Send_Byte(&hdc1080_bus_info,HDC1080_TEMP_REG);   //触发测量
    I2C_WaitAck(&hdc1080_bus_info);
    vTaskDelay(pdMS_TO_TICKS(20));
    // delay_ms(20);   /* Temp转换时间 + Humi转换时间 = 20ms */
    I2C_Start(&hdc1080_bus_info);   //起始信号
    Send_Byte(&hdc1080_bus_info,HDC1080_ADDRESS+1);   //发送HDC1080设备地址+读信号
    // I2C_WaitAck();
    data[0]=I2C_ReadByte(&hdc1080_bus_info,0);   /* 读取温度寄存器数值 */
    data[1]=I2C_ReadByte(&hdc1080_bus_info,0);
    data[2]=I2C_ReadByte(&hdc1080_bus_info,0);   /* 读取湿度寄存器数值 */
    data[3]=I2C_ReadByte(&hdc1080_bus_info,1);
    I2C_Stop(&hdc1080_bus_info);   //停止信号

}

// 开启加热器（配置寄存器 Bit13 = 1）
void HDC1080_HeaterOn(void)
{
    // 加热配置值：默认配置 0x1000 | (1 << 13) = 0x2000
    uint16_t config = 0x2000;

    I2C_Start(&hdc1080_bus_info);
    Send_Byte(&hdc1080_bus_info,HDC1080_ADDRESS);   // 写地址
    Send_Byte(&hdc1080_bus_info,HDC1080_CONFIG_REG);     // 配置寄存器地址
    Send_Byte(&hdc1080_bus_info,(config >> 8) & 0xFF);   // 高字节
    Send_Byte(&hdc1080_bus_info,config & 0xFF);          // 低字节
    I2C_Stop(&hdc1080_bus_info);

    // 等待配置生效（至少 1ms）
    delay_ms(2);
}

// 关闭加热器（恢复默认配置 0x1000）
void HDC1080_HeaterOff(void)
{
    uint16_t config = 0x1000;   // 默认 14-bit 顺序采样

    I2C_Start(&hdc1080_bus_info);
    Send_Byte(&hdc1080_bus_info,HDC1080_ADDRESS);
    Send_Byte(&hdc1080_bus_info,HDC1080_CONFIG_REG);
    Send_Byte(&hdc1080_bus_info,(config >> 8) & 0xFF);
    Send_Byte(&hdc1080_bus_info,config & 0xFF);
    I2C_Stop(&hdc1080_bus_info);

    delay_ms(2);
}
