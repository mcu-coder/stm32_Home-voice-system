#include "tim2.h"                  // Device header

static uint8_t smokeBuzzerSelfHold;		//烟雾浓度蜂鸣器自锁标志位
static uint8_t stepperMotorSelfHold;	//步进电机自锁标志位

void Timer2_Init(u16 Prescaler, u16 Period)	//0-65535
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = Period;
	TIM_TimeBaseInitStructure.TIM_Prescaler = Prescaler;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)	//2ms定时中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET ) 
	{
		
		Key_scan();	//按键扫描函数
		
		/*自动模式下自动控制*/
		if (systemState.mode)
		{
			/*温度大于阈值开启风扇*/
			if (sensorData.temp > Sensorthreshold.tempValue)
			{
				uint8_t tempDifference;
				tempDifference = sensorData.temp - Sensorthreshold.tempValue;
				
				tempDifference = tempDifference * 10 + 30;	//30为基础风速，tempDifference * 10是因为温差而增加的风速
				
				/*风速数值限位*/
				if (tempDifference > 100)
				{
					tempDifference = 100;
				}
				
				fanluminance.FanDutyRatio = tempDifference;
				currentDataPoint.valuefan = fanluminance.FanDutyRatio;	//风速数据上传至云平台
				TIM_SetCompare3(TIM1, fanluminance.FanDutyRatio);	//显示风速
			}
			else
			{
				fanluminance.FanDutyRatio = RESET;
				currentDataPoint.valuefan = fanluminance.FanDutyRatio;	//风速数据上传至云平台
				TIM_SetCompare3(TIM1, fanluminance.FanDutyRatio);	//显示风速			
			}
			
			/*光照强度小于阈值开启窗帘*/
			if (sensorData.lux < Sensorthreshold.luxValue && stepperMotorSelfHold == RESET)
			{
				stepperMotorSelfHold = SET;
				systemState.motorCommand.motorLocation = motorLocation_ON;
				systemState.motorCommand.motorAnterogradeFlag = 1;
				currentDataPoint.valuestepperMotor = systemState.motorCommand.motorLocation;
				
				TIM_SetCompare2(TIM1, 100);	//显示亮度	
			}
			else if (sensorData.lux > Sensorthreshold.luxValue && stepperMotorSelfHold == SET)
			{
				stepperMotorSelfHold = RESET;
				systemState.motorCommand.motorLocation = motorLocation_OFF;
				systemState.motorCommand.motorReversalFlag = 1;
				currentDataPoint.valuestepperMotor = systemState.motorCommand.motorLocation;				
				
				TIM_SetCompare2(TIM1, 0);	//显示亮度	
			}
			
			/*湿度低于阈值加湿器开启*/
			if (sensorData.humi < Sensorthreshold.humiValue)
			{
				Relay_ON();	//加湿器开启
				currentDataPoint.valuehumidifier = SET;
			}
			else
			{
				Relay_OFF();	//加湿器关闭
				currentDataPoint.valuehumidifier = RESET;
			}
			
			
			/*烟雾浓度超出阈值蜂鸣器报警*/
			if (sensorData.smoke > Sensorthreshold.smokeValue && smokeBuzzerSelfHold == RESET)
			{
				smokeBuzzerSelfHold = SET;
				currentDataPoint.valuesmokeAlarm = SET;
				Buzzer_ON();
			}
			else if (sensorData.smoke < Sensorthreshold.smokeValue && smokeBuzzerSelfHold == SET)
			{
				smokeBuzzerSelfHold = RESET;
				currentDataPoint.valuesmokeAlarm = RESET;
				Buzzer_OFF();
			}
		}		

		//机智云配网按键，可以重新进行配网。	
		if(KeyNum == 44 && (oledPages == 1)) 
		{
			KeyNum = 0;
			gizwitsSetMode(2);
			Buzzer_ON();
			Delay_ms(300);
			Buzzer_OFF();
		}		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);				
	}	
}

