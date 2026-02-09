#include "stm32f10x.h"                  // Device header
#include "oled.h"
#include "adcx.h"
#include "sensormodules.h"
#include "dht11.h"
#include "key.h"
#include "tim2.h"
#include "tim3.h"
#include "flash.h"
#include "motor.h"
#include "pwm.h"
#include "usart.h"
#include "iwdg.h"
#include "relay.h"
#include "usart3.h"	
#include "BH1750.h"
#include "fan.h"

SensorModules sensorData;			//声明传感器模块的结构体变量
SensorThresholdValue Sensorthreshold;	//声明传感器阈值结构体变量
SystemState	systemState;	//声明系统状态结构体变量

#define FLASH_START_ADDR	0x0801f000	//写入的起始地址

uint8_t oledPages = 1;	//系统显示页面

typedef enum 
{
	DISPLAY_PAGE1 = 1,
	SETTINGS_PAGE
} OLED_PAGES;

/**
  * @brief  显示菜单1的固定内容
  * @param  无
  * @retval 无
  */
void OLED_Menu1(void)
{
	//显示“温度：  C”
	OLED_ShowChinese(1,1,0);
	OLED_ShowChinese(1,2,2);
	OLED_ShowChar(1,5,':');
	OLED_ShowChar(1,8,'C');
	
	//显示“湿度:   %”
	OLED_ShowChinese(1,5,1);
	OLED_ShowChinese(1,6,2);
	OLED_ShowChar(1,13,':');	
	OLED_ShowChar(1,16,'%');
	 
	
	//显示“系统模式：”
	OLED_ShowChinese(4, 1, 11);
	OLED_ShowChinese(4, 2, 12);
	OLED_ShowChinese(4, 3, 13);
	OLED_ShowChinese(4, 4, 14);	
	OLED_ShowChar(4, 9, ':');
}

/**
  * @brief  显示菜单1的传感器数据
  * @param  无
  * @retval 无
  */
void SensorDataDisplay1(void)
{
	//显示温度数据
	OLED_ShowNum(1, 6, sensorData.temp, 2);
	
	 
	
	//显示系统状态数据
	if (!systemState.mode)
	{
		OLED_ShowChinese(4, 6, 15);
		OLED_ShowChinese(4, 7, 16);		
	}
	else
	{
		OLED_ShowChinese(4, 6, 17);
		OLED_ShowChinese(4, 7, 18);		
		
	}
}

/**
  * @brief  显示系统阈值设置界面1
  * @param  无
  * @retval 无
  */
void OLED_SetInterfacevoid(void)
{
	 

	//显示“湿度阈值”
	OLED_ShowChinese(3, 2, 1);
	OLED_ShowChinese(3, 3, 2);	
	OLED_ShowChinese(3, 4, 21);
	OLED_ShowChinese(3, 5, 22);	
	OLED_ShowChar(3, 11, ':');
	
	//显示“光照阈值”
	OLED_ShowChinese(4, 2, 3);
	OLED_ShowChinese(4, 3, 4);	
	OLED_ShowChinese(4, 4, 21);
	OLED_ShowChinese(4, 5, 22);	
	OLED_ShowChar(4, 11, ':');
	
	//显示烟雾阈值数值
	OLED_ShowNum(1, 12, Sensorthreshold.smokeValue, 3);
	
	//显示温度阈值数值
	OLED_ShowNum(2, 12, Sensorthreshold.tempValue, 2);
 
}



/**
  * @brief  记录阈值界面下按KEY1的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetSelection(void)
{
	static uint8_t count = 1;
	if(KeyNum == KEY_1)
	{
		KeyNum = 0;
		count++;
		if (count >= 5)
		{
			count = 1;
		}
	}
	return count;
}

/**
  * @brief  显示阈值界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_Option(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(1,1,'>');
			OLED_ShowChar(2,1,' ');
			OLED_ShowChar(3,1,' ');
			OLED_ShowChar(4,1,' ');
			break;
		
		case 2:	
			OLED_ShowChar(1,1,' ');
			OLED_ShowChar(2,1,'>');
			OLED_ShowChar(3,1,' ');
			OLED_ShowChar(4,1,' ');
			break;
		
		 

		default: 
			break;
	}
}

/**
  * @brief  对阈值界面的传感器阈值进行修改
  * @param  num 为当前用户需要更改的传感器阈值位置
  * @retval 无
  */
void ThresholdModification(uint8_t num)
{
	switch (num)
	{
		case 1:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.smokeValue += 10;
				if (Sensorthreshold.smokeValue > 500)
				{
					Sensorthreshold.smokeValue = 0;
				}
			}
		 
			break;
			
		case 2:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.tempValue ++;
				if (Sensorthreshold.tempValue > 99)
				{
					Sensorthreshold.tempValue = 0;
				}
			}
			 
			break;
	 
					
		default: 
			break;		
	}
}

/**
  * @brief  获取语音模块数据
  * @param  无
  * @retval 无
  */
void Asrpro(void)
{
	if (Usart3_RxFlag == 1)
	{
		Usart3_RxFlag = 0;
		switch (Usart3_RxPacket[3])
		{
			/*控制开关灯*/
			case 1:
				if (!systemState.mode)
				{
					if (Usart3_RxPacket[4])
					{
						ledluminance.ledDutyRatio = 100;
						currentDataPoint.valueLED = ledluminance.ledDutyRatio;	//LED亮度上传云平台
						TIM_SetCompare2(TIM1, ledluminance.ledDutyRatio);	//显示亮度	
					}
					else
					{
						ledluminance.ledDutyRatio = 0;
						currentDataPoint.valueLED = ledluminance.ledDutyRatio;	//LED亮度上传云平台
						TIM_SetCompare2(TIM1, ledluminance.ledDutyRatio);	//显示亮度						
					}
				}
				break;
				
			/*控制开关风扇*/
			case 2:
				if (!systemState.mode)
				{
					if (Usart3_RxPacket[4])
					{
						fanluminance.FanDutyRatio = 100;
						currentDataPoint.valuefan = fanluminance.FanDutyRatio;	//风速数据上传至云平台
						TIM_SetCompare3(TIM1, fanluminance.FanDutyRatio);	//显示风速
					}
					else
					{
						fanluminance.FanDutyRatio = 0;
						currentDataPoint.valuefan = fanluminance.FanDutyRatio;	//风速数据上传至云平台
						TIM_SetCompare3(TIM1, fanluminance.FanDutyRatio);	//显示风速					
					}
				}
				break;
			 
 
				
			default:
				break;
		}
	}
}

/**
  * @brief  传感器数据扫描
  * @param  无
  * @retval 无
  */
void SensorScan(void)
{
	DHT11_Read_Data(&sensorData.humi, &sensorData.temp);
	Get_Average_MQ2_PPM(&sensorData.smoke);
	sensorData.lux = (uint16_t)read_BH1750();
}


int main(void)
{
	OLED_Init();
	DHT11_Init();
	Key_Init();
	MOTOR_Init(); 
	
	ADCX_Init();
	Timer2_Init(9, 14398);
	Uart2_Init(9600);
	Uart1_Init(115200);  
	
	Sensorthreshold.tempValue = FLASH_R(FLASH_START_ADDR);	//从指定页的地址读FLASH
	Sensorthreshold.humiValue = FLASH_R(FLASH_START_ADDR+2);	//从指定页的地址读FLASH
	Sensorthreshold.luxValue = FLASH_R(FLASH_START_ADDR+4);	//从指定页的地址读FLASH
	Sensorthreshold.smokeValue = FLASH_R(FLASH_START_ADDR+6);	//从指定页的地址读FLASH
	Sensorthreshold.flameValue = FLASH_R(FLASH_START_ADDR+8);	//从指定页的地址读FLASH
	
	GENERAL_TIM_Init();
	userInit();		//完成机智云初始赋值
	gizwitsInit();	//开辟一个环形缓冲区
	
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	Delay_ms(1000);

	while (1)
	{
			
		IWDG_ReloadCounter();	//使能重装载寄存器计数
		SensorScan();	//获取传感器数据
		
		switch (oledPages)
		{
			case DISPLAY_PAGE1:
				OLED_Menu1();	//显示主页面1固定信息
				SensorDataDisplay1();//显示传感器1数据
			
				/* 手动模式下按键控制执行器的运行 */
				if (!systemState.mode)
				{
					LED_PWM_KEY();	//LED按键控制
					Fan_PWM_KEY();	//风扇按键控制
					
					/* 按键控制步进电机的运行 */
					if(KeyNum == KEY_4 && systemState.motorCommand.motorLocation == motorLocation_OFF)
					{
						KeyNum = 0;
						systemState.motorCommand.motorAnterogradeFlag = 1;
					}
					else if ((KeyNum == KEY_4 && systemState.motorCommand.motorLocation == motorLocation_ON))
					{
						KeyNum = 0;
						systemState.motorCommand.motorReversalFlag = 1;
					}
				}
			
				/*按键按下时切换模式*/
				if (KeyNum == KEY_1)
				{
					KeyNum = 0;
					systemState.mode = !systemState.mode;
					if (!systemState.mode)
					{
						Buzzer_OFF();
						Relay_OFF();
					}
				}
				
				/*按键按下时进入阈值设计界面*/
				if (KeyNum == KEY_Long1)
				{
					KeyNum = 0;
					oledPages = SETTINGS_PAGE;
					OLED_Clear();
				}
				
				Asrpro();	//语音控制
				MotorOperation();	//步进电机控制
				break;
				
			case SETTINGS_PAGE:
				
				OLED_Option(SetSelection());	//实现阈值设置页面的选择功能
				OLED_SetInterfacevoid();	//显示阈值设置界面1的固定内容
				ThresholdModification(SetSelection());	//实现阈值调节功能	
			
				//判断是否退出阈值设置界面
				if (KeyNum == KEY_2)
				{
					KeyNum = 0;
					oledPages = DISPLAY_PAGE1;	//跳转到主界面
					OLED_Clear();	//清屏
					
					//存储修改的传感器阈值至flash内	
					FLASH_W(FLASH_START_ADDR, Sensorthreshold.tempValue,Sensorthreshold.humiValue,
					Sensorthreshold.luxValue, Sensorthreshold.smokeValue, Sensorthreshold.flameValue);
				}
				break;
			default: break;
		}
		 					
	}
}
