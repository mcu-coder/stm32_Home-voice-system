#ifndef __ADCX_H
#define __ADCX_H

#include "stm32f10x.h"                  // Device header

/**
  * 函    数：ADC初始化
  * 参    数：无
  * 返 回 值：无
  */
void ADCX_Init(void);

/**
  * 函    数：获取滤波后的MQ2传感器的PPM
  * 参    数：输出ppm数据
  * 返 回 值：无
  */
void Get_Average_MQ2_PPM(uint16_t *ppm);


/**
  * 函    数：获取flame数值
  * 参    数：无
  * 返 回 值：输出百分比数据
  */
uint8_t Get_flame_Percentage(void);
#endif
