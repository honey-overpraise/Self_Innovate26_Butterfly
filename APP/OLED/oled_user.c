#include "oled.h"
#include "delay.h"
#include "bmp.h"

const unsigned char *GIFList[] = {
    gImage_1,
    gImage_2,
    gImage_3,
    gImage_4,
    gImage_5,
    gImage_6,
    gImage_7,
    gImage_8,
    gImage_9,
    gImage_10,
    gImage_11,
    gImage_12,
    gImage_13,
    gImage_14,
    gImage_15,
    gImage_16,
    gImage_17,
    gImage_18
};

void OLED_Display_Temp(float temp,float humi)
{
    uint16_t dec_temp = 0;
    uint16_t dec_humi = 0;
    /*ÏÔÊ¾ÎÂ¶È*/
    dec_temp = (uint16_t)(temp*10);
    dec_humi = (uint16_t)(humi*10);
    OLED_ShowString(76,0,"Temp:",16,1);
    OLED_ShowNum(72,20,temp,2,16,1);
    OLED_ShowString(88,20,".",16,1);
    OLED_ShowNum(92,20,(dec_temp%10),1,16,1);
    OLED_ShowChinese(104,20,4,16,1);//??
    OLED_Display_Butterfly();
}

void OLED_Display_Butterfly(void)
{
   static uint8_t printf_cnt = 0;
   if(printf_cnt >= 18)
   {
       printf_cnt = 0;
   }
   OLED_ShowPicture(0,0,64,64,GIFList[printf_cnt],1);
	// OLED_Refresh();
   // delay_ms(100);
   printf_cnt++;
}

