#include "relay.h"


void Relay_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = Relay1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Relay_PROT, &GPIO_InitStructure);
	
}

void Relay_ON(void)
{

	GPIO_SetBits(GPIOB, Relay1);		


}

void Relay_OFF(void)
{
	
	GPIO_ResetBits(GPIOB, Relay1);		

}


