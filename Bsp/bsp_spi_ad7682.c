/*
*********************************************************************************************************
*
*	模块名称 : AD7606数据采集模块
*	文件名称 : bsp_ad7606.c
*	版    本 : V1.0
*	说    明 : AD7606挂在STM32的FMC总线上。
*
*			本例子使用了 TIM3 作为硬件定时器，定时启动ADC转换
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-10-11 armfly  正式发布
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

/*
	STM32-V7开发板 + AD7606模块， 控制采集的IO:
	
	PC6/TIM3_CH1/TIM8_CH1     ----> AD7606_CONVST  (和摄像头复用),  输出PWM方波，作为ADC启动信号
	PE5/DCMI_D6/AD7606_BUSY   <---- AD7606_BUSY    , CPU在BUSY中断服务程序中读取采集结果
*/

#include "bsp.h"
#include "bsp_spi_ad7682.h"
#include "cmsis_os.h"
#include "app.h"
//#include "app.h"
SPI_HandleTypeDef SpiHandle;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim3;
/* CONVST 启动ADC转换的GPIO = PC6 */
#define CONVST_RCC_GPIO_CLK_ENABLE	__HAL_RCC_GPIOB_CLK_ENABLE
#define CONVST_GPIO		GPIOB
#define CONVST_PIN		GPIO_PIN_0
#define CONVST_TIMX		TIM8
#define CONVST_TIMCH	1


void StopSample(void)
{
	HAL_TIM_Base_Stop_IT(&htim8);
}

void StartSample(void)
{
	HAL_TIM_Base_Start_IT(&htim8);
}

/* 启动AD转换的GPIO : PC6 */
#define CONVST_1()	HAL_GPIO_WritePin((GPIO_TypeDef *)CONVST_GPIO,CONVST_PIN, GPIO_PIN_SET)
#define CONVST_0()	HAL_GPIO_WritePin((GPIO_TypeDef *)CONVST_GPIO,CONVST_PIN, GPIO_PIN_RESET)


int16_t sample_ad7682[4];

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  if (hspi->Instance == SPIx)
  {
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    SPIx_SCK_GPIO_CLK_ENABLE();
    SPIx_MISO_GPIO_CLK_ENABLE();
    SPIx_MOSI_GPIO_CLK_ENABLE();
    /* Enable SPI1 clock */
    SPIx_CLK_ENABLE();
    /* Enable DMA clock */

    /*##-2- Configure peripheral GPIO ##########################################*/  
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;//GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = SPIx_SCK_AF;
    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MISO_PIN;
    GPIO_InitStruct.Alternate = SPIx_MISO_AF;
    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

    

    /*##-5- Configure the NVIC for SPI #########################################*/ 
    /* NVIC configuration for SPI transfer complete interrupt (SPI1) */
    HAL_NVIC_SetPriority(SPIx_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPIx_IRQn);
  }
}

void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 3661;//1831=120000000/65526的采样率;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 119;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 499; //500us一次
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
	HAL_TIM_Base_Start_IT(&htim3);
  /* USER CODE END TIM3_Init 2 */

}







uint32_t emu_times=0;
int64_t s2emu[4]={-1,-1,-1,-1};
int32_t emu[4]={0,0,0,0};
uint16_t cur_ch=0;
uint16_t next_ch=2;

extern int16_t n_pid(int32_t nset,int32_t n);
extern float butterworth(float x0);



//  0010 10XX X100 1001  0x2849 
////  0011 11XX X111 0001  0x3A71 //实际使用
//  0011 11XX X100 0001  0x3A81 //2.5v内部基准 
#define SAMPLE_CFG  0x3A41

volatile uint16_t ad7682_rec[8];
uint16_t ad7682_cfg=0;
//const uint16_t ad7682_ch_queue[12]={1,2,1,3,1,0,1,1}; //原始
//const uint16_t ad7682_ch_data_queue[12]={0,4,0,1,0,1,0,2};
//const uint16_t ad7682_ch_queue[12]={1,2,1,3,1,2,1,3}; //降采样率到16384 8192
									//2 1 2 3 2 1 2 3
									//2 3 2 1 2 3 2 1
                                  //z  y  z  x  z  y  z  x
const uint16_t ad7682_ch_queue[12]={2 ,1, 2, 3, 2, 1, 2, 3}; //降采样率到16384 8192

const uint16_t ad7682_ch_data_queue[12]={0,1,0,2,0,1,0,2};
//const uint16_t ad7682_ch_queue[12]={2,2,2,2,2,2,2,2};
//const uint16_t ad7682_ch_data_queue[12]={1,4,1,0,1,2,1,3};

/*实际接受顺序 0 1 1 1 2 1 3 1*/
volatile uint16_t cur_ad_ch=0;
void TIM8_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_UP_IRQn 0 */
if (TIM8->SR & TIM_IT_UPDATE) 
	{
		TIM8->SR &= (uint16_t)~TIM_IT_UPDATE;
		
//		next_ch = (cur_ch+2) & 0x3;
		next_ch=ad7682_ch_queue[cur_ad_ch];
//		bsp_LedToggle(1);
		{
			ad7682_cfg = (uint16_t)SAMPLE_CFG |	(next_ch<<7);
			ad7682_cfg=ad7682_cfg<<2;
//			ad7682_cfg=0x55aa;
			HAL_GPIO_WritePin(CONVST_GPIO, CONVST_PIN,GPIO_PIN_RESET);
			HAL_SPI_TransmitReceive_IT(&SpiHandle,(uint8_t *)&ad7682_cfg,(uint8_t *)(&ad7682_rec[cur_ad_ch]),1);
//			HAL_SPI_TransmitReceive(&SpiHandle,(uint8_t *)&ad7682_cfg,(uint8_t *)(&ad7682_rec[cur_ad_ch]),1,10);
//			HAL_GPIO_WritePin(CONVST_GPIO, CONVST_PIN,GPIO_PIN_SET);
		}

	}
  /* USER CODE END TIM8_UP_IRQn 0 */
 
  /* USER CODE BEGIN TIM8_UP_IRQn 1 */

  /* USER CODE END TIM8_UP_IRQn 1 */
}

int64_t emu_zeros[4]={0,0,0,0};
uint16_t ad7682_date[SAMPLE_ADCH*SAMPLEPOINTS*2]; //双缓冲
volatile uint32_t cur_ad_index=0;

volatile uint32_t CurrentAD7682DataCounter;
extern osSemaphoreId_t ad7682_readyHandle;
volatile uint8_t datareadyprocess=0;
void bsp_InitAD7682(void){
	
	{
	GPIO_InitTypeDef gpio_init_structure;

	/* 使能 GPIO时钟 */
	CONVST_RCC_GPIO_CLK_ENABLE();

	/* 设置 GPIOB 相关的IO为复用推挽输出 */
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	/* 配置GPIOA */
	gpio_init_structure.Pin = CONVST_PIN;
	HAL_GPIO_Init(CONVST_GPIO, &gpio_init_structure);
	}
	{	
	/*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
	SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_16BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
//  SpiHandle.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE; 
  SpiHandle.Init.Mode = SPI_MODE_MASTER;

	}

  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    /* Initialization Error */
//    Error_Handler();
  }	
	MX_TIM8_Init();	
	MX_TIM3_Init();  //低等级定时器释放信号量
	
}



void SPI1_IRQHandler(void){	
	
		HAL_SPI_IRQHandler(&SpiHandle);
	  

		if(HAL_SPI_GetState(&SpiHandle) == HAL_SPI_STATE_READY)
		{		
	   
			HAL_GPIO_WritePin(CONVST_GPIO, CONVST_PIN,GPIO_PIN_SET);	
			ad7682_date[cur_ad_index++]=ad7682_rec[cur_ad_ch];
			if(cur_ad_index==SAMPLE_ADCH*SAMPLEPOINTS)	
			{

				CurrentAD7682DataCounter=0;
				datareadyprocess=1;
	//			 osSemaphoreRelease(ad7682_readyHandle);
			}
			else if(cur_ad_index>=SAMPLE_ADCH*SAMPLEPOINTS*2)
			{
				cur_ad_index=0;			
				CurrentAD7682DataCounter=SAMPLE_ADCH*SAMPLEPOINTS;
				datareadyprocess=1;
	//			osSemaphoreRelease(ad7682_readyHandle);
			}
		
			cur_ad_ch++;
			cur_ad_ch=cur_ad_ch%SAMPLE_ADCH;
			if(((cur_ad_index-cur_ad_ch)%SAMPLE_ADCH)!=0)  //串包的话，这包丢掉
			{
				cur_ad_ch=0;
				cur_ad_index=0;
			}
		}
}

void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_UP_IRQn 0 */
if (TIM3->SR & TIM_IT_UPDATE) 
	{
//		bsp_LedToggle(1);
		TIM3->SR &= (uint16_t)~TIM_IT_UPDATE;
		if(datareadyprocess==1)
		{
			osSemaphoreRelease(ad7682_readyHandle);
			datareadyprocess=0;
		}

	}
  /* USER CODE END TIM8_UP_IRQn 0 */
 
  /* USER CODE BEGIN TIM8_UP_IRQn 1 */

  /* USER CODE END TIM8_UP_IRQn 1 */
}
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
