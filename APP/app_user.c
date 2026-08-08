#include "sys.h"
#include "delay.h"
#include "app_user.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "oled.h"
#include "hdc1080.h"
#include "stepper_motor.h"
#include "wtk6900.h"
#include "bh1750.h"
#include "light.h"

static uint8_t light_control = 1;

void switch_func(void)
{
    static uint8_t bak_receive_data = 0;
    uint64_t timestamp = xTaskGetTickCount();

    if(receive_data != bak_receive_data)
    {
        switch (receive_data)
        {
            case 0x01:
                LIGHT = 1;
                light_control = 0;
                break;

            case 0x02:
                LIGHT = 0;
                light_control = 0;
                break;

            case 0x03:
                StepperMotor_SetDirection(CLOCKWISE);
                StepperMotor_SetSpeed(5000);
                StepperMotor_RotateContinuous();
                break;

            case 0x04:
                StepperMotor_Stop();  // ÓÅÑÅÍ£Ö¹
                break;
            
            default:
                break;
        }
        bak_receive_data = receive_data;
    }
}


void check_lighting(void)
{
    uint64_t timestamp = xTaskGetTickCount();
    if((timestamp >= 5000) && (light_control == 1))//5sºó
    {
        set_led_on();
    }
}

