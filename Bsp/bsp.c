/*
*********************************************************************************************************
*
*	模块名称 : BSP模块(For STM32L4R9)
*	文件名称 : bsp.c
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h" 来包含所有的外设驱动模块。
*			   bsp = Borad surport packet 板级支持包
*	修改记录 :
*		版本号  日期         作者       说明
*		V1.0    2019-07-29  shao   正式发布
*

*
*********************************************************************************************************
*/
#include "bsp.h"



/*
*********************************************************************************************************
*	                                   函数声明
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void SENSOR_Peripheral_Power_config_gpio(void)
{

	GPIO_InitTypeDef gpio_init_structure;

	/* 使能 GPIO时钟 */
	SENSOR_Peripheral_Power_CLK_ENABLE();

	/* 设置 GPIOB 相关的IO为复用推挽输出 */
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLDOWN;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	
	gpio_init_structure.Pin = SENSOR_Peripheral_Power_Pin;
	HAL_GPIO_Init(SENSOR_Peripheral_Power_Port, &gpio_init_structure);

}

void bsp_Init(void)
{
  
//	bsp_InitUart();		/* 初始化串口 */
//		bsp_InitExtSDRAM();
//		bsp_InitAD7606();
		bsp_InitRTC();
	  bsp_InitLed();    	/* 初始化LED */	
		SENSOR_Peripheral_Power_config_gpio();
		SENSOR_Peripheral_Power_Enable();
		bsp_InitKX220();
		bsp_InitAD7682();
		bsp_Esp32_Init();
		bsp_InitLMT01();
		initEEPROM();
	
}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
