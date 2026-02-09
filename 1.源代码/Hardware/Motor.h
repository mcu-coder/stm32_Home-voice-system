#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"                  // Device header
#include "delay.h"


#define MOTOR_A GPIO_Pin_8
#define MOTOR_B GPIO_Pin_11
#define MOTOR_C GPIO_Pin_12
#define MOTOR_D GPIO_Pin_15
#define MOTOR_PROT GPIOA

#define MOTOR_A_HIGH GPIO_SetBits(MOTOR_PROT,MOTOR_A)
#define MOTOR_A_LOW GPIO_ResetBits(MOTOR_PROT,MOTOR_A)

#define MOTOR_B_HIGH GPIO_SetBits(MOTOR_PROT,MOTOR_B)
#define MOTOR_B_LOW GPIO_ResetBits(MOTOR_PROT,MOTOR_B)

#define MOTOR_C_HIGH GPIO_SetBits(MOTOR_PROT,MOTOR_C)
#define MOTOR_C_LOW GPIO_ResetBits(MOTOR_PROT,MOTOR_C)

#define MOTOR_D_HIGH GPIO_SetBits(MOTOR_PROT,MOTOR_D)
#define MOTOR_D_LOW GPIO_ResetBits(MOTOR_PROT,MOTOR_D)


typedef enum
{
	motorLocation_OFF,
	motorLocation_ON,
} MotorLocation_t;

typedef struct
{
	MotorLocation_t motorLocation;
	uint8_t motorAnterogradeFlag;
	uint8_t motorReversalFlag;
} MotorCommand;

//extern MotorCommand		motorCommand;	//声明步进电机控制的结构体变量

void MOTOR_Init(void);

/**
  * @brief  根据标志位控制步进电机的运行
  * @param  无
  * @retval 无
  */
void MotorOperation(void);
#endif
