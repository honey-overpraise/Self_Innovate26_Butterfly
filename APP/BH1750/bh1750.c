#include "bh1750.h"
#include "delay.h"
#include "led.h"
#include "light.h"

#define LUX_ON_THRESH    20.0f
#define LUX_OFF_THRESH   80.0f
// ========== 底层I2C操作 ==========

// 低通滤波缓存全局静态变量
static float lux_filter_val = 0.0f;

static void I2C_Delay(void)
{
    uint8_t i = 15;
    while (i--) {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}

static void SCL_HIGH(void)
{
    GPIO_SetBits(BH1750_GPIO_PORT, BH1750_SCL_PIN);
}

static void SCL_LOW(void)
{
    GPIO_ResetBits(BH1750_GPIO_PORT, BH1750_SCL_PIN);
}

static void SDA_HIGH(void)
{
    GPIO_SetBits(BH1750_GPIO_PORT, BH1750_SDA_PIN);
}

static void SDA_LOW(void)
{
    GPIO_ResetBits(BH1750_GPIO_PORT, BH1750_SDA_PIN);
}

static uint8_t SDA_READ(void)
{
    return GPIO_ReadInputDataBit(BH1750_GPIO_PORT, BH1750_SDA_PIN);
}

static void I2C_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    I2C_Delay();
    SDA_LOW();
    I2C_Delay();
    SCL_LOW();
    I2C_Delay();
}

static void I2C_Stop(void)
{
    SDA_LOW();
    SCL_HIGH();
    I2C_Delay();
    SDA_HIGH();
    I2C_Delay();
}

static uint8_t I2C_WaitAck(void)
{
    uint16_t timeout = 0;
    
    SDA_HIGH();
    SCL_HIGH();
    I2C_Delay();
    
    while (SDA_READ()) {
        timeout++;
        if (timeout > 1000) {
            SCL_LOW();
            return 1;  // 无应答
        }
    }
    
    SCL_LOW();
    I2C_Delay();
    return 0;  // 应答成功
}

static void I2C_SendAck(void)
{
    SDA_LOW();
    SCL_HIGH();
    I2C_Delay();
    SCL_LOW();
    I2C_Delay();
    SDA_HIGH();
}

static void I2C_SendNack(void)
{
    SDA_HIGH();
    SCL_HIGH();
    I2C_Delay();
    SCL_LOW();
    I2C_Delay();
}

static void I2C_WriteByte(uint8_t data)
{
    uint8_t i;
    
    for (i = 0; i < 8; i++) {
        if (data & 0x80) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        data <<= 1;
        SCL_HIGH();
        I2C_Delay();
        SCL_LOW();
        I2C_Delay();
    }
    SDA_HIGH();
}

static uint8_t I2C_ReadByte(void)
{
    uint8_t i, data = 0;
    
    SDA_HIGH();
    for (i = 0; i < 8; i++) {
        data <<= 1;
        SCL_HIGH();
        I2C_Delay();
        if (SDA_READ()) {
            data |= 0x01;
        }
        SCL_LOW();
        I2C_Delay();
    }
    return data;
}

// ========== BH1750 操作 ==========

static uint8_t BH1750_WriteCmd(uint8_t cmd)
{
    I2C_Start();
    I2C_WriteByte(BH1750_ADDR_WR);
    if (I2C_WaitAck()) {
        I2C_Stop();
        return 1;
    }
    I2C_WriteByte(cmd);
    if (I2C_WaitAck()) {
        I2C_Stop();
        return 1;
    }
    I2C_Stop();
    return 0;
}

static uint16_t BH1750_ReadRaw(void)
{
    uint8_t high, low;
    uint16_t raw;
    
    I2C_Start();
    I2C_WriteByte(BH1750_ADDR_RD);
    I2C_WaitAck();
    
    high = I2C_ReadByte();
    I2C_SendAck();
    
    low = I2C_ReadByte();
    I2C_SendNack();
    
    I2C_Stop();
    
    raw = (high << 8) | low;
    return raw;
}

// ========== 公开API ==========

/**
 * @brief 初始化GPIO (开漏输出)
 */
void BH1750_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(BH1750_RCC_GPIO, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = BH1750_SCL_PIN | BH1750_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BH1750_GPIO_PORT, &GPIO_InitStruct);
    
    // 总线空闲
    SCL_HIGH();
    SDA_HIGH();
    
    // 上电复位
    BH1750_PowerOn();
    delay_ms(10);
    BH1750_Reset();
    delay_ms(10);
}

/**
 * @brief 上电
 */
void BH1750_PowerOn(void)
{
    BH1750_WriteCmd(BH1750_CMD_POWER_ON);
}

/**
 * @brief 掉电
 */
void BH1750_PowerDown(void)
{
    BH1750_WriteCmd(BH1750_CMD_POWER_DOWN);
}

/**
 * @brief 复位
 */
void BH1750_Reset(void)
{
    BH1750_WriteCmd(BH1750_CMD_RESET);
}

/**
 * @brief 设置测量模式
 * @param mode: BH1750_CMD_CONT_H_MODE 等
 */
void BH1750_SetMode(uint8_t mode)
{
    BH1750_WriteCmd(mode);
    delay_ms(150);  // 等待首次测量完成
}

/**
 * @brief 读取光照强度 (必须已设置为连续模式)
 * @retval 光照值 (单位: lx)
 */
float BH1750_ReadLight(void)
{
    uint16_t raw;
    float lux;
    
    raw = BH1750_ReadRaw();
    lux = (float)raw / 1.2f;
    
    return lux;
}

/**
 * @brief 一次性测量 (上电→测量→读取→掉电)
 * @retval 光照值 (单位: lx)
 */
float BH1750_ReadOnce(void)
{
    float lux;
    
    // 上电
    BH1750_PowerOn();
    delay_ms(5);
    
    // 发送一次性测量指令
    BH1750_WriteCmd(BH1750_CMD_ONE_H_MODE);
    delay_ms(150);  // 等待测量完成
    
    // 读取数据
    lux = BH1750_ReadLight();
    
    // 掉电
    BH1750_PowerDown();
    
    return lux;
}

float set_led_on(void)
{
    static float lux;
    static uint8_t led_state = 0; // 0关 1开

    lux = BH1750_ReadLight_Filter();
    if(led_state == 0)
    {
        // 当前灯关，低于开灯阈值则打开
        if(lux <= LUX_ON_THRESH)
        {
            LED0 = 0;
            LIGHT = 1;
            led_state = 1;
        }
    }
    else
    {
        // 当前灯开，高于关灯阈值则关闭
        if(lux >= LUX_OFF_THRESH)
        {
            LED0 = 1;
            LIGHT = 0;
            led_state = 0;
        }
    }

    return lux;
}

// 对外：带低通滤波读取光照
float BH1750_ReadLight_Filter(void)
{
    float raw_lux = BH1750_ReadLight();
    // 滤波系数alpha：0~1
    // 0.2 强滤波，非常平滑，反应慢一点
    // 0.3 均衡平滑+响应（自动开灯推荐）
    // 0.4 轻微滤波，响应快，还有小幅跳动
    const float alpha = 0.3f;

    lux_filter_val = alpha * raw_lux + (1 - alpha) * lux_filter_val;
    return lux_filter_val;
}