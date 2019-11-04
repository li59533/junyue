/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_LMT01_H
#define __BSP_LMT01_H

/* 供外部调用的函数声明 */
#define LMT01POWEREnable()  {HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,GPIO_PIN_SET);}
#define LMT01POWERDisable()  {HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,GPIO_PIN_RESET);}
void bsp_InitLMT01(void);
float LMT01_GET_VALUE(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
