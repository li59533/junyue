/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.c
*	��    �� : V1.0
*	˵    �� : ����LEDָʾ��
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-09-05 armfly  ��ʽ����
*
*	Copyright (C), 2015-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#include "bsp_KX220.h"
/*
	suozhang
	2019-4-1 11:00:24
	STM32H743 Nucleo-144 
	
	led red PB14

*/	

static void KX220_config_gpio(void)
{

	GPIO_InitTypeDef gpio_init_structure;

	/* ʹ�� GPIOʱ�� */
	RCC_KX220_CLK_ENABLE();

	/* ���� GPIOB ��ص�IOΪ����������� */
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	
	/* ����GPIOA */
	gpio_init_structure.Pin = KX220_PIN_ST;
	HAL_GPIO_Init(KX220_PORT_ST, &gpio_init_structure);
	
	gpio_init_structure.Pin = KX220_PIN_ENABLE;
	HAL_GPIO_Init(KX220_PORT_ENABLE, &gpio_init_structure);

}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLed
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKX220(void)
{
	KX220_config_gpio();
	KX220ST_OFF();
	KX220Enable();

}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
