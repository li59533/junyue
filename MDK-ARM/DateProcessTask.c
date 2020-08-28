/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "app.h"
extern osSemaphoreId_t ad7682_readyHandle;

volatile uint32_t wp[16];
int16_t piz_emu_data[2][32768];
int16_t  mems_emu_data[2][3][8192]; //3��mems���ݴ��
volatile uint8_t jiaoyansum[16];
volatile uint16_t wave_packege_flag=0;
//uint8_t WaveToSend[AD7682_ADCHS][1600];
volatile uint32_t currentSAMPLEblock=0;
//void UpdateWave(float yy,uint8_t i) //????,??yy??????i???
//{   

//	 int16_t senddata=(int16_t)(yy*Parameter.ReciprocalofRange[i]);
//		if(((config.DataToSendChannel>>i)&0x01)) 
//		{
//			WaveToSend[i][wp[i]+14]=senddata;//*floatdata;  //�޸�sendkk;//
//			WaveToSend[i][wp[i]+15]=senddata>>8;
//			jiaoyansum[i]+=(WaveToSend[i][wp[i]+14]+WaveToSend[i][wp[i]+15]);
//			wp[i]=wp[i]+2;			
//		}
//	 if(wp[i]>(BOARDPOINTS-1)) {	

//		  
//		  uint16_t buflength=BOARDPOINTS+7+2+1;
//			WaveToSend[i][0]=0x7e;//TELid;
//			WaveToSend[i][1]=0x40;//TELid>>8;	
//			WaveToSend[i][2]=(uint8_t)buflength;//TELid>>16;
//			WaveToSend[i][3]=buflength>>8;// TELid>>24; //2,3????????482??
//		  WaveToSend[i][4]=g_tRTC.Year;
//		  WaveToSend[i][5]=g_tRTC.Year>>8;
//		  WaveToSend[i][6]=g_tRTC.Mon;
//		  WaveToSend[i][7]=g_tRTC.Day; //ʱ����32λ��ʾ
//		  WaveToSend[i][8]=g_tRTC.Hour;
//		  WaveToSend[i][9]=g_tRTC.Min;
//		  WaveToSend[i][10]=g_tRTC.Sec; //ʱ����32λ��ʾ
//		  WaveToSend[i][11]=(uint8_t)wave_packege_flag; //ʱ����32λ��ʾ
//		  WaveToSend[i][12]=wave_packege_flag>>8; //ʱ����32λ��ʾ
//			WaveToSend[i][13]=i;
//		  for(uint32_t ii=1;ii<14;ii++)
//		  jiaoyansum[i]+=WaveToSend[i][ii];  //adch�Ǵ�0��ʼ��
//			WaveToSend[i][14+BOARDPOINTS]=jiaoyansum[i];  //2,3????????482??
//		 	WaveToSend[i][15+BOARDPOINTS]=0x7e; 
//		  WriteDataToTXDBUF(WaveToSend[i],BOARDPOINTS+16);
//			
//		
//		  wave_packege_flag++;
//		  jiaoyansum[i]=0;			
//			wp[i]=0;
//		}
//	
//}


void UpdateAccelerationData(int16_t yyy,uint8_t channel ,uint32_t DataIndex)
{
	switch(channel){
		case 0:
			piz_emu_data[currentSAMPLEblock][DataIndex]=yyy;
		break;
		case 1:
			mems_emu_data[currentSAMPLEblock][0][DataIndex]=yyy;
		break;
		case 2:
			mems_emu_data[currentSAMPLEblock][1][DataIndex]=yyy;
		break;
		case 3:
			mems_emu_data[currentSAMPLEblock][2][DataIndex]=yyy;
		break;
		default:
			break;
	}
}

int32_t 	halfref=0;
uint32_t 	ActualIndex=0;
int32_t 	AD_ZERO[AD7682_ADCHS],AD_ZEROlowpass[AD7682_ADCHS],AD_INTER[AD7682_ADCHS],lastdata[AD7682_ADCHS],filtercounter[AD7682_ADCHS];

extern volatile uint32_t CurrentAD7682DataCounter;

uint32_t sprase_counter[12],StoreDateIndex[12];
const uint32_t ADfrequence[4]={16384,8192,8192,8192};
void emu_sprase_index()
{
	uint32_t i=0;
	uint32_t ch[4]={0,1,2,3};
	for(i=0;i<AD7682_ADCHS;i++){  //ĳ���������˻�׼������Ϊ51200����Ϊ8ͨ������ͬ������������8��ͨ��ֻ��һ����׼������
			
		  Parameter.sparse_index[i]=ADfrequence[i]/config.channel_freq[ch[i]];
		}
}

extern uint16_t ad7682_date[SAMPLE_ADCH*SAMPLEPOINTS*2]; //˫����
extern const uint16_t ad7682_ch_data_queue[12];
extern osSemaphoreId_t seconds_sample_data_readyHandle;
volatile uint32_t ActualSampleCH=0;
int16_t wave_jscope[8];

extern volatile uint16_t cur_ad_ch;
extern volatile uint32_t cur_ad_index;
extern volatile uint8_t datareadyprocess;
extern volatile uint16_t ad7682_rec[8];
volatile uint8_t FirstBlood=1;
void DataProcessFunction(void *argument)
{
    
	uint32_t i,j;
	uint16_t *p;  //????????16???
	int32_t y;
	float volatile vy=0;//�ٶ��м����
	//	uint32_t si=0;
	volatile float yy=0;
	halfref=32768;
		
	for(i=0;i<AD7682_ADCHS;i++)
	{
		AD_ZERO[i]=(uint64_t)32768*4096;  // tao 10s 
		AD_ZEROlowpass[i]=0u;
		AD_INTER[i]=0;
		Parameter.vs[i]=0;
		lastdata[i]=0;
		wp[i]=0;
		jiaoyansum[i]=0;		
		filtercounter[i]=0;
		sprase_counter[i]=0;
		StoreDateIndex[i]=0;
	//	Parameter.ReciprocalofRange[i]=32768/config.floatrange[i];
	//	Parameter.sparse_index[i]=ADfrequence[i]/config.channel_freq[i];
	}
	osDelay(50); //���ؿ����и���ѹ�ȶ�ʱ�䣬������Ҫ�ⲿ��
	StartSample();
	osDelay(100);
	StopSample();
	cur_ad_ch=0;
	cur_ad_index=0;
	CurrentAD7682DataCounter=0;
	datareadyprocess=0;
	AD_ZERO[0]=32768*4096;   //ѹ�����ȶ�ʱ�䣬����׼��ѹ��OK�ģ����Կ���ֱ��ȡֵ��ʡ�ĵ����ȶ�
	AD_ZERO[1]=ad7682_rec[1]*8192;   //�����ѹ�磬����ܺ�ȥȷ����ֵ������mems�����������йأ��б�Ҫ���²ɼ�ȷ����ֵ
	AD_ZERO[2]=ad7682_rec[3]*8192;  //���������ʲ�һ��
	emu_sprase_index();
	
	if(Parameter.wakeupsourec==VLLS)
	{	
		
		StartSample();//		
	}
	
	while(1)
	{		
		osSemaphoreAcquire(ad7682_readyHandle, osWaitForever);//
		if(FirstBlood)  //Ϊ���ܿ����������ӿ��˲��ȶ�
		{
			ad7682_date[0]=32768;
			ad7682_date[1]=AD_ZERO[1]>>13;
			ad7682_date[2]=32768;
			ad7682_date[3]=AD_ZERO[2]>>13;
			ad7682_date[4]=32768;
			ad7682_date[5]=AD_ZERO[1]>>13;
			ad7682_date[6]=32768;
			ad7682_date[7]=AD_ZERO[2]>>13;
			FirstBlood=0;
		}

		p=(uint16_t *)&ad7682_date[CurrentAD7682DataCounter];

		for(j=0;j<SAMPLEPOINTS;j++)
		{	
			for(i=0;i<SAMPLE_ADCH;i++)
			{

				y=(int32_t)((*(uint16_t*)p));//-(int32_t)((*(uint16_t*)(p+2)));  // 
				ActualSampleCH=	ad7682_ch_data_queue[i]; //ת��Ϊʵ��ADCͨ��
				wave_jscope[ActualSampleCH]=y;
				//					if(i==2) continue; //��һ��ͨ������Ҫ��������������Ϊ�˳�Ϊ4�ı���
				if(config.interface_type[ActualSampleCH] == TYPE_IEPE)
				{
					
					switch(ActualSampleCH)
					{

						case 0:
						//							y=y>>1;
						AD_ZERO[ActualSampleCH] = y + (((int64_t)AD_ZERO[ActualSampleCH]*4095)>>12); // >> 16 /65536  >> 15 /32768  >> 14 / 16384  >> 13 / 8192
						y=y-(int32_t)(AD_ZERO[ActualSampleCH]>>12); //AD_ZEROlowpass[i]
						
						if(config.channel_freq[ActualSampleCH]==16384)
						{
							// 6000Hz
							AD_ZEROlowpass[ActualSampleCH]=((int64_t)y+lastdata[ActualSampleCH])*45292 - AD_ZEROlowpass[ActualSampleCH]*25049; //5
							AD_ZEROlowpass[ActualSampleCH]=AD_ZEROlowpass[ActualSampleCH]>>16;			
							lastdata[ActualSampleCH]=y;
							y=AD_ZEROlowpass[ActualSampleCH];

						}else if(config.channel_freq[ActualSampleCH]==8192)
						{
							/***********************?����?2��?��?��32768��?����?��3��?��8192��?�̨�����2500 0.19638846199800786 -0.60722307600398429*******/			
							//				?����?2��?��?��16384��?����?��3��?��8192��?�̨�����2500			0.34202261737325917       -0.31595476525348171      
							//								AD_ZEROlowpass[ActualSampleCH]=((int64_t)y+lastdata[ActualSampleCH])*15767+AD_ZEROlowpass[ActualSampleCH]*34002;
//							AD_ZEROlowpass[ActualSampleCH] = ((int64_t)y + lastdata[ActualSampleCH]) * 22414 + AD_ZEROlowpass[ActualSampleCH] * 20706;
//							AD_ZEROlowpass[ActualSampleCH] = AD_ZEROlowpass[ActualSampleCH]>>16;			
//							lastdata[ActualSampleCH] = y;
//							y = AD_ZEROlowpass[ActualSampleCH];
						}else if(config.channel_freq[ActualSampleCH]==2048)
						{
							/*********************?����?2��?��?��32768��?����?��3��?��2048��?�̨�����625 0.056597493815809335      -0.88680501236838138      *******/			  													
							//	*?����?2��?��?��16384��?����?��3��?��2048��?�̨�����625 		0.1074769920231831        -0.78504601595363377      
							//								AD_ZEROlowpass[ActualSampleCH]=((int64_t)y+lastdata[ActualSampleCH])*10286+AD_ZEROlowpass[ActualSampleCH]*45648;
							AD_ZEROlowpass[ActualSampleCH] = ((int64_t)y+lastdata[ActualSampleCH])*7043+AD_ZEROlowpass[ActualSampleCH]*51448;
							AD_ZEROlowpass[ActualSampleCH] = AD_ZEROlowpass[ActualSampleCH]>>16;	
							lastdata[ActualSampleCH] = y;
							y = AD_ZEROlowpass[ActualSampleCH];
						}	
						break;
						default :
							AD_ZERO[ActualSampleCH] = y + (((int64_t)AD_ZERO[ActualSampleCH] * 8191) >> 13);
							y = y - (int32_t)(AD_ZERO[ActualSampleCH] >> 13);
						
							if(config.channel_freq[ActualSampleCH] == 8192)
							{
//								/************************?����?2��?��?��8192��?����?��3��?��8192��?�̨�����2500 0.58754852395358581       0.17509704790717162       *************/			
								AD_ZEROlowpass[ActualSampleCH]=((int64_t)y+lastdata[ActualSampleCH])*38505-AD_ZEROlowpass[ActualSampleCH]*11475; //6.2K
								AD_ZEROlowpass[ActualSampleCH]=AD_ZEROlowpass[ActualSampleCH]>>16;			
								lastdata[ActualSampleCH]=y;
								y=AD_ZEROlowpass[ActualSampleCH];

							}else if(config.channel_freq[ActualSampleCH]==2048)
							{
								/***********************?����?2��?��?��8192��?����?��3��?��2048��?�̨�����625 0.19638846199800786        -0.60722307600398429      *******/			

								AD_ZEROlowpass[ActualSampleCH]=((int64_t)y+lastdata[ActualSampleCH])*12870+AD_ZEROlowpass[ActualSampleCH]*39794;
								AD_ZEROlowpass[ActualSampleCH]=AD_ZEROlowpass[i]>>16;			
								lastdata[ActualSampleCH]=y;
								y=AD_ZEROlowpass[ActualSampleCH];
							}
						break;
					}

					/**************************��������************************/

					sprase_counter[ActualSampleCH]++;
					if(sprase_counter[ActualSampleCH] >= Parameter.sparse_index[ActualSampleCH]) 
					{
						sprase_counter[ActualSampleCH] = 0;
						//������¼�������ݵ��±�
						//						y=((float)y*0.30517578f-config.floatadjust[ActualSampleCH])*config.floatadc[ActualSampleCH]*config.floatscale[ActualSampleCH];//*0.1f;//-config.floatscale[i];//-500.0f; // mv???0.038146f						
						UpdateAccelerationData((int16_t)y,ActualSampleCH,StoreDateIndex[ActualSampleCH]);
						StoreDateIndex[ActualSampleCH]++;
					}
				}else if(config.interface_type[ActualSampleCH]==TYPE_NONE)
				{
					halfref=y;
				}


				p++;
				ActualIndex++;   //ԭ����Ӧ�üӸ��ж����ģ��ر��ǵ�����ֵģʽ�£�д����±����Ϊ0ʱ����������һ������				

			}



			if(ActualIndex>=32768)
			{ //config.ADfrequence,���һ�����ݣ������˲�����

				RTC_ReadClock();	/* ��ʱ�ӣ������ g_tRTC */
				for(i=0;i<AD7682_ADCHS;i++)
				{
					StoreDateIndex[i]=0;
					sprase_counter[i]=0; //ȫ����0
				}
				ActualIndex=0;
				currentSAMPLEblock=(currentSAMPLEblock+1)%2;

				osSemaphoreRelease(seconds_sample_data_readyHandle);
			}
		}
		//			bsp_LedStatue(1,1);

	}
		
}
