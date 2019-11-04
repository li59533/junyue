/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��(For STM32L4R9)
*	�ļ����� : bsp.c
*	��    �� : V1.0
*	˵    �� : ����Ӳ���ײ�������������ļ���ÿ��c�ļ����� #include "bsp.h" ���������е���������ģ�顣
*			   bsp = Borad surport packet �弶֧�ְ�
*	�޸ļ�¼ :
*		�汾��  ����         ����       ˵��
*		V1.0    2019-07-29  shao   ��ʽ����
*

*
*********************************************************************************************************
*/
#include "bsp.h"



/*
*********************************************************************************************************
*	                                   ��������
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void SENSOR_Peripheral_Power_config_gpio(void)
{

	GPIO_InitTypeDef gpio_init_structure;

	/* ʹ�� GPIOʱ�� */
	SENSOR_Peripheral_Power_CLK_ENABLE();

	/* ���� GPIOB ��ص�IOΪ����������� */
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLDOWN;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	
	gpio_init_structure.Pin = SENSOR_Peripheral_Power_Pin;
	HAL_GPIO_Init(SENSOR_Peripheral_Power_Port, &gpio_init_structure);

}

void bsp_Init(void)
{
  
//	bsp_InitUart();		/* ��ʼ������ */
//		bsp_InitExtSDRAM();
//		bsp_InitAD7606();
		bsp_InitRTC();
	  bsp_InitLed();    	/* ��ʼ��LED */	
		SENSOR_Peripheral_Power_config_gpio();
		SENSOR_Peripheral_Power_Enable();
		bsp_InitKX220();
		bsp_InitAD7682();
		bsp_Esp32_Init();
		bsp_InitLMT01();
		initEEPROM();
	
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
