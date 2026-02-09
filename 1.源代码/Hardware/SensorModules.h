#ifndef	__SENSORMODULES_H_
#define __SENSORMODULES_H_

#include "stm32f10x.h"                  // Device header
#include "motor.h"                  // Device header

typedef struct
{
	uint8_t humi;
	uint8_t temp;
	uint16_t lux;	
	uint16_t smoke;
}SensorModules;

typedef struct
{
	uint8_t tempValue;
	uint8_t humiValue;
	uint8_t flameValue;
	uint16_t luxValue;	
	uint16_t smokeValue;
	
}SensorThresholdValue;

typedef struct
{
	uint8_t mode;					//系统模式
	uint8_t led;					//灯状态
	uint8_t fan;					//风扇状态
	uint8_t humidifier;				//加湿器状态
	MotorCommand motorCommand;	//声明步进电机控制的结构体变量
} SystemState;

extern SensorModules sensorData;			//声明传感器模块的结构体变量
extern SensorThresholdValue Sensorthreshold;	//声明传感器阈值结构体变量
extern SystemState	systemState;	//声明系统状态结构体变量

#endif
