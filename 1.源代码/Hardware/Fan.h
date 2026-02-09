#ifndef	_FAN_H_
#define	_FAN_H_

#include "stm32f10x.h"                  // Device header
#include "key.h" 
#include "gizwits_product.h"
#include "sensormodules.h"
#include "tim2.h"

#define FAN				GPIO_Pin_15
#define FAN_PROT		GPIOB

#define FAN_PWM_UPPER_LIMIT			100		//风扇数值的上限为100%


typedef enum 
{
	FanON,	//关风扇
	FanFirst,	//一级风速
	Fansecond,	//二级风速
	FanThirdOrder	//三极风速
} FanState;


typedef struct
{
	FanState FanState;	//风扇等级
	uint8_t FanDutyRatio;	//风扇转速数值
} Fanluminance;


extern Fanluminance fanluminance;	//风扇风速等级结构体变量

void Fan_Init(void);
void Fan_PWM_KEY(void);

#endif
