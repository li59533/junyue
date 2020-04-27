/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_LMT01_H
#define __BSP_LMT01_H

/* ���ⲿ���õĺ������� */
#define LMT01POWEREnable()  {HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,GPIO_PIN_SET);}
#define LMT01POWERDisable()  {HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0,GPIO_PIN_RESET);}
void bsp_InitLMT01(void);
float LMT01_GET_VALUE(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
