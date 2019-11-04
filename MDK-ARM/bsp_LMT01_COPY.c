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
#include "cmsis_os.h"
#include "bsp_LMT01.h"
#include "app.h"
/*
	suozhang
	2019-4-1 11:00:24
	STM32H743 Nucleo-144 
	
	led red PB14

*/	
TIM_HandleTypeDef htim4;
void LMT01_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();


  /*Configure GPIO pin : PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 10, 0);
//  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/



uint32_t LMT01_RUN_COUNTER=0; //该变量用来计时LMT01运行时间，正常是54ms conv 50ms data ,所以放104ms一次转换
#define LMT01_LIFE_TIME 10600 //10600*10us=106ms
uint32_t LMT01_TEMP_CNT=0;
uint32_t LMT01_TEMP_VALUE=0;
uint32_t LMT01_PULSE_WIDTH=0;
typedef enum {LMT01_ERR=0,LMT01_OK=1,} LMT01_Condition;
LMT01_Condition LMT01_CURRENT_CONDITION;



void LMT01_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */
	 __HAL_RCC_TIM4_CLK_ENABLE();
    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 9, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
//	HAL_NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 119;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */

}



void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
	 setLMT01Running();
	 LMT01_TEMP_CNT++;
	 if(LMT01_PULSE_WIDTH>2) //||(LMT01_PULSE_WIDTH<2)
		 LMT01_CURRENT_CONDITION=LMT01_ERR;
	 LMT01_PULSE_WIDTH=0;
  /* USER CODE END EXTI9_5_IRQn 1 */
}
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 1 */
	bsp_LedToggle(1);
  /* USER CODE END TIM1_BRK_TIM15_IRQn 1 */

	if(isLMT01Running()&&(LMT01_TEMP_CNT>2))//
	{
	LMT01_PULSE_WIDTH++;
	}
	LMT01_RUN_COUNTER++;//
	if(LMT01_RUN_COUNTER>LMT01_LIFE_TIME)
	{
		LMT01_TEMP_VALUE=LMT01_TEMP_CNT;
		LMT01_RUN_COUNTER=0;
		LMT01POWERDisable();
	}
	
}

void bsp_InitLMT01(void)
{
	LMT01_GPIO_Init();
	LMT01_TIM15_Init();
//	bsp_LedOff(1);

}

void  Init_temp_parameter(void)
{
	LMT01_TEMP_CNT=0;
	LMT01_RUN_COUNTER=0;
	LMT01_PULSE_WIDTH=0;	
}

float LMT01_GET_VALUE(void)
{
	Init_temp_parameter();
	
	LMT01_CURRENT_CONDITION=LMT01_OK;
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);  //外部中断使能
	HAL_NVIC_EnableIRQ(TIM4_IRQn);           //TPM中断使能
	clrLMT01Running();	
	HAL_TIM_Base_Start_IT(&htim4);   //start the counter
	
	LMT01POWEREnable();     //打开电源
	osDelay(120); 
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);    //外部中断使能
	HAL_NVIC_DisableIRQ(TIM4_IRQn);           //TPM中断使能	//
	HAL_TIM_Base_Stop_IT(&htim4);         //关闭计数 
	LMT01POWERDisable();    //关闭电源
	if(LMT01_CURRENT_CONDITION!=LMT01_OK)
	{
		Init_temp_parameter();
		HAL_TIM_Base_Start_IT(&htim4);    //start the counter 
		LMT01_CURRENT_CONDITION=LMT01_OK;
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);   //外部中断使能
		HAL_NVIC_EnableIRQ(TIM4_IRQn);           //TPM中断使能
		clrLMT01Running();	
		LMT01POWEREnable() ;					 //打开电源
		osDelay(120); 
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);    //外部中断使能
		HAL_NVIC_DisableIRQ(TIM4_IRQn);            //TPM中断使能	//
		HAL_TIM_Base_Stop_IT(&htim4);          //关闭计数 
		LMT01POWERDisable();     //关闭电源
		if(LMT01_CURRENT_CONDITION!=LMT01_OK)
			return 110;
		else 
			return  ((float)LMT01_TEMP_VALUE*0.0625f-50);
	}else 
	
	return  ((float)LMT01_TEMP_VALUE*0.0625f-50);
	
}
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
