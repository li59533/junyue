#include "bsp.h"
#include "app.h"

#include "bsp_esp32.h"

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

volatile uint8_t HeadIndex=0;
volatile uint8_t TailIndex=0;

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PB4 (NJTRST)     ------> USART1_CTS
    PB3 (JTDO/TRACESWO)     ------> USART1_RTS
    PA10     ------> USART1_RX
    PA9     ------> USART1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel1;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_USART1_TX;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(huart,hdmatx,hdma_usart1_tx);

		 /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel2;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_USART1_RX;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(huart,hdmarx,hdma_usart1_rx);

		
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  // ʹ�ܴ��ڿ����ж�
    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PB4 (NJTRST)     ------> USART1_CTS
    PB3 (JTDO/TRACESWO)     ------> USART1_RTS
    PA10     ------> USART1_RX
    PA9     ------> USART1_TX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10|GPIO_PIN_9);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmatx);
		HAL_DMA_DeInit(huart->hdmarx);
    /* USART1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }

}

void esp32_gpio_init(void)
{

	GPIO_InitTypeDef gpio_init_structure;

	/* ʹ�� GPIOʱ�� */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/* ���� GPIOB ��ص�IOΪ����������� */
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLDOWN;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	
	gpio_init_structure.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOB, &gpio_init_structure);
	
	gpio_init_structure.Pin = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOD, &gpio_init_structure);

}

void bsp_Esp32_Init(void)
{
	esp32_gpio_init();
   /* USER CODE BEGIN USART1_Init 0 */
/* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 3000000;//115200;//
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;//UART_HWCONTROL_NONE;//
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
//	DisableEsp32Power();
//	RunEsp32();
//	HAL_Delay(100);
//	RunEsp32();
	
//	HAL_Delay(150);
//	ResetEsp32();
//	HAL_Delay(100);
//	RunEsp32();
//	HAL_Delay(50);
	ResetEsp32();
	EnableEsp32Power();
	HAL_Delay(150);
	RunEsp32();
}



void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

  /* USER CODE END DMA1_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
uint8_t receive_buff[IdleRxdBufSize];  

volatile uint16_t esp32_ready=0;
volatile uint16_t WifiConnectedFlag=0;
volatile uint16_t WifiDisconnectedFlag=0;

volatile uint16_t RxdBufHeadIndex=0;
volatile uint16_t RxdBufTailIndex=0;
uint8_t  RxdBuf[RxdBufLine];
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	char cmpbuf[20]; //�����Ƚϵ�buf���ᳬ��20���ֽ�
	// ֹͣ����DMA����
//    HAL_UART_DMAStop(&huart1);  
                                                       
    // ������յ������ݳ���
    uint8_t data_length  = IdleRxdBufSize - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);   
    
	// ������ջ�����
		
		for(uint32_t i=0;i<data_length;i++)
		{
			if(!isRxdBufFull())
			{
					RxdBuf[RxdBufHeadIndex]=receive_buff[i];
					IncreaseRxdBufNum(RxdBufHeadIndex);
			}
		}	
		memcpy(cmpbuf,receive_buff,20);                                                 //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)
		if(strstr ( (const char*)cmpbuf, "ready" ) ? 1 : 0)
		{
			esp32_ready=1;
		}else	if(strstr ( (const char*)cmpbuf, "connectserver" ) ? 1 : 0)
		{
			WifiConnectedFlag=1;
		}else if(strstr ((const char*) cmpbuf, "lostserver" ) ? 1 : 0)
		{
			WifiDisconnectedFlag=1;
		}
		
    // ������ʼDMA���� ÿ��255�ֽ�����
		memset(receive_buff,0,data_length); 
		memset(cmpbuf,0,20); //��ֻ��20���ֽڵĳ���  
    data_length = 0;
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)receive_buff, IdleRxdBufSize);                    
}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{	// �ж��Ƿ��Ǵ���1
    if(USART1 == huart1.Instance)                                   
    {	// �ж��Ƿ��ǿ����ж�
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   
        {	 // ��������жϱ�־�������һֱ���Ͻ����жϣ�
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);                    
//            printf("\r\nUART1 Idle IQR Detected\r\n");
            // �����жϴ�����
            USAR_UART_IDLECallback(huart);                          
        }
    }
}

uint8_t TXDBUF[TxdBufLine][BOARDPOINTS+20];
uint16_t TXDBUFLength[TxdBufLine]={0};
extern osSemaphoreId_t write_data_readyHandle;
uint32_t sendcounter=0;
uint32_t readcounter=0;
uint8_t WriteDataToTXDBUF(uint8_t * source,uint16_t length)
{
	uint8_t result;
	osSemaphoreAcquire(write_data_readyHandle, osWaitForever);
	readcounter++;
	__disable_irq();	   //��Ӧ�����������жϣ�������
	if(isTxdBufFull()) 
	{
//	osSemaphoreRelease(write_data_readyHandle);//�ͷ��ź������
//	__enable_irq();
	result= 0;
	__enable_irq();
	} else{
//	__enable_irq();

	TXDBUFLength[HeadIndex]=length;
	for(uint16_t i=0;i<length;i++)
	TXDBUF[HeadIndex][i]=source[i];

	
//	  __disable_irq();	   //��Ӧ�����������жϣ�������
	 if(HeadIndex==TailIndex)
	 { 
	  Increase(HeadIndex);
   __enable_irq();
//	  __enable_irq();
//		 bsp_LedStatue(1,1);
		HAL_UART_Transmit_DMA(&huart1,TXDBUF[TailIndex],TXDBUFLength[TailIndex]);
		 
	 }else
	 {
		Increase(HeadIndex);
		__enable_irq();

	 }
	 result= 1;
	}
	
	
	osSemaphoreRelease(write_data_readyHandle);//�ͷ��ź������
	return result;
}
void USART1_IRQHandler(void)
{  
	HAL_UART_IRQHandler(&huart1);
    
    /* USER CODE BEGIN USART1_IRQn 1 */
    // ����ӵĺ��������������ڿ����ж�
  USER_UART_IRQHandler(&huart1);                                
    /* USER CODE END USART1_IRQn 1 */
}

void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */
//	if(hdma_usart1_tx.State==HAL_DMA_STATE_READY)   
//	{	 // ��������жϱ�־�������һֱ���Ͻ����жϣ�
//	 Increase(TailIndex);
//	 if(!isTxdBufEmpty())		{
//			HAL_UART_Transmit_DMA(&huart1,TXDBUF[TailIndex],TXDBUFLength[TailIndex]);    	 
//		}                        
//	}
	
  /* USER CODE END DMA1_Channel1_IRQn 1 */
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
//�ص�����
	sendcounter++;
//	bsp_LedStatue(0,1);
	 Increase(TailIndex);
	 if(!isTxdBufEmpty())		{
		 
			HAL_UART_Transmit_DMA(&huart1,TXDBUF[TailIndex],TXDBUFLength[TailIndex]);    	 
		}
//huart->gState=HAL_UART_STATE_READY; 


}

extern uint8_t CmdBuf[RX_BUFFER_SIZE];
extern uint16_t CmdBufLength;

bool Esp32_SetIP_1(char *localIP,char *LocalMASK,char *LocalGATEWAY,uint8_t dhcp)  //����IP��ַ
{
	 uint8_t localIPlength=strlen(localIP);
	 uint8_t LocalMASKlength=strlen(LocalMASK);
	 uint8_t LocalGATEWAYlength=strlen(LocalGATEWAY);
	 CmdBufLength=15+localIPlength+LocalMASKlength+LocalGATEWAYlength;
	 CmdBuf[0]=0X7E;
	 CmdBuf[1]=0xe4;   //T
	 CmdBuf[2]=CmdBufLength-6;
	 CmdBuf[3]=(CmdBufLength-6)>>8;   //L
	 CmdBuf[4]=0X00;   //V_IP
	 CmdBuf[5]=localIPlength;
	{
		for(uint8_t i=0;i<localIPlength;i++)
		CmdBuf[6+i]=localIP[i];
	 
	 }
   CmdBuf[6+localIPlength]=0X01;   //V_MASK
	 CmdBuf[7+localIPlength]=LocalMASKlength;
	{
		for(uint8_t i=0;i<LocalMASKlength;i++)
		CmdBuf[8+localIPlength+i]=LocalMASK[i];
	 
	 }
	 CmdBuf[8+localIPlength+LocalMASKlength]=0X02;   //V_GATEWAY
	 CmdBuf[9+localIPlength+LocalMASKlength]=LocalGATEWAYlength;
	{
		for(uint8_t i=0;i<LocalGATEWAYlength;i++)
		CmdBuf[10+localIPlength+LocalMASKlength+i]=LocalGATEWAY[i];
	 
	 }
	 CmdBuf[10+localIPlength+LocalMASKlength+LocalGATEWAYlength]=0X03;
	 CmdBuf[11+localIPlength+LocalMASKlength+LocalGATEWAYlength]=0X01;
	 CmdBuf[12+localIPlength+LocalMASKlength+LocalGATEWAYlength]=dhcp;
	 CmdBuf[13+localIPlength+LocalMASKlength+LocalGATEWAYlength]=0;
	 for(uint8_t i=1;i<(13+localIPlength+LocalMASKlength+LocalGATEWAYlength);i++)
	 CmdBuf[13+localIPlength+LocalMASKlength+LocalGATEWAYlength]+=CmdBuf[i];//��AP�ַ�����ֵ��config
	 CmdBuf[14+localIPlength+LocalMASKlength+LocalGATEWAYlength]=0x7e;
	 CmdBufLength=15+localIPlength+LocalMASKlength+LocalGATEWAYlength;
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
//	 return Esp32_wait_response( "OK", NULL, 10);
}

bool Esp32_SetAP_1(char *APssid,char *APpassword )  //����IP��ַ
{  
	//���Ҫ�ж��Ƿ񹤳�����
	 uint8_t APSSIDlength=strlen(APssid);
	 uint8_t APPASSWORDlength=strlen(APpassword);
	 CmdBufLength=10+APSSIDlength+APPASSWORDlength;
	 CmdBuf[0]=0X7E;
	 CmdBuf[1]=0xe5;   //T
	 CmdBuf[2]=CmdBufLength-6;
	 CmdBuf[3]=(CmdBufLength-6)>>8;   //L
	 CmdBuf[4]=0X00;   //V_IP
	 CmdBuf[5]=APSSIDlength;
	{
		for(uint8_t i=0;i<APSSIDlength;i++)
		CmdBuf[6+i]=APssid[i];
	 
	 }
   CmdBuf[6+APSSIDlength]=0X01;   //V_MASK
	 CmdBuf[7+APSSIDlength]=APPASSWORDlength;
	{
		for(uint8_t i=0;i<APPASSWORDlength;i++)
		CmdBuf[8+APSSIDlength+i]=APpassword[i];
	 
	 }
	 CmdBuf[8+APSSIDlength+APPASSWORDlength]=0X00;   //У���
	 
	 for(uint8_t i=1;i<(8+APSSIDlength+APPASSWORDlength);i++)
	 CmdBuf[8+APSSIDlength+APPASSWORDlength]+=CmdBuf[i];//��AP�ַ�����ֵ��config
	 CmdBuf[9+APSSIDlength+APPASSWORDlength]=0x7e;
	 CmdBufLength=10+APSSIDlength+APPASSWORDlength;
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
//	 return Esp32_wait_response( "OK", NULL, 10);
}

bool Esp32_applynetset()  //����IP��ַ
{
	CmdBuf[0]=0x7e;
	CmdBuf[1]=0xff;
	CmdBuf[2]=0x00;
	CmdBuf[3]=0x00;
	CmdBuf[4]=0xff;
	CmdBuf[5]=0x7e;
  CmdBufLength=6;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
//	return Esp32_wait_response( "connectserver", NULL, 400);
}

bool Esp32_SetTCPSERVER_1(char *TcpServer_IP,char *TcpServer_Port)  //����IP��ַ
{
	 uint8_t TcpServer_IPlength=strlen(TcpServer_IP);
	 uint8_t TcpServer_Portlength=strlen(TcpServer_Port);
	 CmdBufLength=10+TcpServer_Portlength+TcpServer_IPlength;
	 CmdBuf[0]=0X7E;
	 CmdBuf[1]=0xe7;   //T
	 CmdBuf[2]=CmdBufLength-6;
	 CmdBuf[3]=(CmdBufLength-6)>>8;   //L
	 CmdBuf[4]=0X00;   //V_IP
	 CmdBuf[5]=TcpServer_IPlength;
	{
		for(uint8_t i=0;i<TcpServer_IPlength;i++)
		CmdBuf[6+i]=TcpServer_IP[i];
	 
	 }
   CmdBuf[6+TcpServer_IPlength]=0X01;   //V_MASK
	 CmdBuf[7+TcpServer_IPlength]=TcpServer_Portlength;
	{
		for(uint8_t i=0;i<TcpServer_Portlength;i++)
		CmdBuf[8+TcpServer_IPlength+i]=TcpServer_Port[i];
	 
	 }
	 CmdBuf[8+TcpServer_IPlength+TcpServer_Portlength]=0X00;   //У���
	 
	 for(uint8_t i=1;i<(8+TcpServer_IPlength+TcpServer_Portlength);i++)
	 CmdBuf[8+TcpServer_IPlength+TcpServer_Portlength]+=CmdBuf[i];//��AP�ַ�����ֵ��config
	 CmdBuf[9+TcpServer_IPlength+TcpServer_Portlength]=0x7e;
	 CmdBufLength=10+TcpServer_IPlength+TcpServer_Portlength;
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
//	 return Esp32_wait_response( "OK", NULL, 10);
}
