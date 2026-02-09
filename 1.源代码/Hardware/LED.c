#include "led.h"

LEDluminance ledluminance;	//LED亮度等级结构体变量

void LED_Init(void)
{
	//开启GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//配置LED引脚为模拟输出模式
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = LED;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_PROT, &GPIO_InitStructure);
	GPIO_ResetBits(LED_PROT, LED);
}

void LED_PWM_KEY(void)
{
	if (KeyNum == 2)
	{
		KeyNum = 0;
		ledluminance.ledState++;
		if (ledluminance.ledState > 3)
		{
			ledluminance.ledState = lightsOut;
		}
		
		switch (ledluminance.ledState)
		{
			case lightsOut:
				
				ledluminance.ledDutyRatio = lightsOut;
				break;
			
			case ledFirst:
				/*一级亮度30%*/
				ledluminance.ledDutyRatio = 30;
				break;
			
			case ledsecond:
				/*二级亮度60%*/
				ledluminance.ledDutyRatio = 60;	
				break;
			
			case ledThirdOrder:
				/*三级亮度100%*/
				ledluminance.ledDutyRatio = 100;		
				break;
			
			default:
				break;
		}
		
		currentDataPoint.valueLED = ledluminance.ledDutyRatio;	//LED亮度上传云平台
		TIM_SetCompare2(TIM1, ledluminance.ledDutyRatio);	//显示亮度				
	}
	


}




