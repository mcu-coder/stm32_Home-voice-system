#include "fan.h"

Fanluminance fanluminance;	//风扇等级结构体变量

void Fan_Init(void)
{
	//开启GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//配置LED引脚为模拟输出模式
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = FAN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FAN_PROT, &GPIO_InitStructure);
	GPIO_ResetBits(FAN_PROT, FAN);
}

void Fan_PWM_KEY(void)
{
	if (KeyNum == 3)
	{
		KeyNum = 0;
		fanluminance.FanState++;
		if (fanluminance.FanState > 3)
		{
			fanluminance.FanState = FanON;
		}
		
		switch (fanluminance.FanState)
		{
			case FanON:
				
				fanluminance.FanDutyRatio = FanON;
				break;
			
			case FanFirst:
				/*一级风速40%*/
				fanluminance.FanDutyRatio = 40;
				break;
			
			case Fansecond:
				/*二级风速60%*/
				fanluminance.FanDutyRatio = 60;	
				break;
			
			case FanThirdOrder:
				/*三级风速100%*/
				fanluminance.FanDutyRatio = 100;		
				break;
			
			default:
				break;
		}
		
		currentDataPoint.valuefan = fanluminance.FanDutyRatio;	//风速数据上传至云平台
		TIM_SetCompare3(TIM1, fanluminance.FanDutyRatio);	//显示风速		
	}
	


}
