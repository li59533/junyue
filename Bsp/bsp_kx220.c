/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.c
*	版    本 : V1.0
*	说    明 : 驱动LED指示灯
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-09-05 armfly  正式发布
*
*	Copyright (C), 2015-2030, 安富莱电子 www.armfly.com
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

	/* 使能 GPIO时钟 */
	RCC_KX220_CLK_ENABLE();

	/* 设置 GPIOB 相关的IO为复用推挽输出 */
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	
	/* 配置GPIOA */
	gpio_init_structure.Pin = KX220_PIN_ST;
	HAL_GPIO_Init(KX220_PORT_ST, &gpio_init_structure);
	
	gpio_init_structure.Pin = KX220_PIN_ENABLE;
	HAL_GPIO_Init(KX220_PORT_ENABLE, &gpio_init_structure);

}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKX220(void)
{
	KX220_config_gpio();
	KX220ST_OFF();
	KX220Enable();

}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
