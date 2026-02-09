#ifndef	_LED_H_
#define	_LED_H_

#include "stm32f10x.h"                  // Device header
#include "key.h" 
#include "gizwits_product.h"
#include "sensormodules.h"
#include "tim2.h"

#define LED				GPIO_Pin_14
#define LED_PROT		GPIOB

#define LED_PWM_UPPER_LIMIT			100		//LED亮度数值的上限为100%



typedef enum 
{
	lightsOut,	//关灯
	ledFirst,	//一级亮度
	ledsecond,	//二级亮度
	ledThirdOrder	//三极亮度
} LedState;


typedef struct
{
	LedState ledState;	//LED等级
	uint8_t ledDutyRatio;	//LED亮度数值
} LEDluminance;


extern LEDluminance ledluminance;	//LED亮度等级结构体变量

void LED_Init(void);
void LED_PWM_KEY(void);

#endif
