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

#ifndef __BSP_KX220_H
#define __BSP_KX220_H

#define KX220_PORT_ST  GPIOA
#define KX220_PIN_ST		GPIO_PIN_4
#define RCC_KX220_CLK_ENABLE  __HAL_RCC_GPIOA_CLK_ENABLE

#define KX220_PORT_ENABLE  GPIOA
#define KX220_PIN_ENABLE		GPIO_PIN_3


/* 供外部调用的函数声明 */
void bsp_InitKX220(void);
#define KX220Enable()  {HAL_GPIO_WritePin(KX220_PORT_ENABLE, KX220_PIN_ENABLE,GPIO_PIN_SET);}
#define KX220Disnable()  {HAL_GPIO_WritePin(KX220_PORT_ENABLE, KX220_PIN_ENABLE,GPIO_PIN_RESET);}

#define KX220ST_ON()  {HAL_GPIO_WritePin(KX220_PORT_ST, KX220_PIN_ST,GPIO_PIN_SET);}
#define KX220ST_OFF()  {HAL_GPIO_WritePin(KX220_PORT_ST, KX220_PIN_ST,GPIO_PIN_RESET);}


#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
