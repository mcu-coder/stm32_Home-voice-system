#include "stm32f10x.h"                  // Device header
#include <math.h>
#include "delay.h"

static uint16_t AD_Value[2];					//存放AD转换结果


#define MQ2				GPIO_Pin_4
#define flame			GPIO_Pin_6
#define ADC_PROT		GPIOA





/**
  * 函    数：ADC初始化
  * 参    数：无
  * 返 回 值：无
  */
void ADCX_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//开启ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		//开启DMA1的时钟
	
	/*设置ADC时钟*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);						//选择6分频
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = MQ2 | flame;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_PROT, &GPIO_InitStructure);					
						
	/*规则组通道配置*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_55Cycles5);	
	
	/*ADC初始化*/
	ADC_InitTypeDef ADC_InitStructure;											//定义结构体变量
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;							//独立模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;			//软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//连续转换
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;								//扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 2;										//扫描规则组的前4个通道
	ADC_Init(ADC1, &ADC_InitStructure);											//配置ADC1
	
	/*DMA初始化*/
	DMA_InitTypeDef DMA_InitStructure;											//定义结构体变量
	 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//循环模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//存储器到存储器，选择失能
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级，选择中等
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);								//配置DMA1的通道1
	
	/*DMA和ADC使能*/
	DMA_Cmd(DMA1_Channel1, ENABLE);							//DMA1的通道1使能
	ADC_DMACmd(ADC1, ENABLE);								//ADC1触发DMA1的信号使能
	ADC_Cmd(ADC1, ENABLE);									//ADC1使能
	
	/*ADC校准*/
	ADC_ResetCalibration(ADC1);								
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	/*ADC触发*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//软件触发ADC开始工作
}


/**
  * 函    数：获取MQ2传感器PPM
  * 参    数：无
  * 返 回 值：输出ppm数据
  */
static uint16_t Get_MQ2_PPM(void)
{
	uint16_t adc_value = 0;	//这是从MQ-2传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
	float voltage = 0;	//MQ-2传感器模块的电压输出
	adc_value = AD_Value[0];

    voltage  = ((3.3f/4096.f)*(adc_value)) * 2;	//计算出当前电压
	
	return (uint16_t)(voltage * 100.f);	//转换成ppm
}

/**
  * 函    数：获取flame数值
  * 参    数：无
  * 返 回 值：输出百分比数据
  */
uint8_t Get_flame_Percentage(void)
{
	float voltage = 0;		
	voltage = AD_Value[1];
	voltage  = (1 - (voltage / 4096)) * 100;	//计算出当前百分比
	
	return voltage;
}


/**
  * 函    数：获取滤波后的MQ2传感器的PPM
  * 参    数：输出ppm数据
  * 返 回 值：无
  */
void Get_Average_MQ2_PPM(uint16_t *ppm)
{
	uint32_t temp;

	for (uint8_t i = 0; i < 10; i++)
	{
		temp += Get_MQ2_PPM();
		Delay_ms(5);
	}
	temp /= 10;
	*ppm = temp;
}

