/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块
*	文件名称 : main.c
*	版    本 : V1.1
*	说    明 : 
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2018-12-12   Eric2013     1. CMSIS软包版本 V5.4.0
*                                     2. HAL库版本 V1.3.0
*
*   V1.1    2019-04-01   suozhang     1. add FreeRTOS V10.20
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "main.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "app.h"
#include "arm_math.h"
extern volatile uint8_t currentblock ;






arm_rfft_fast_instance_f32 S;
uint32_t fftSize,ifftFlag;
void Init_FFT(void)
{
 
 fftSize = config.ADfrequence; //????

 ifftFlag = 0;//???

// arm_rfft_fast_init_f32(&S, fftSize);
}



void updateParameter(uint8_t i)
{

	if(config.interface_type[i]==TYPE_IEPE)
	{
		Parameter.Mean[i]=Parameter.S_sum[i]*Parameter.ReciprocalofEMUnumber[i];//??
		float tt=Parameter.SS_sum[i]*Parameter.ReciprocalofEMUnumber[i];  //
		if(Parameter.SS_sum[i]==0) Parameter.SS_sum[i]=0.01f;
		if(tt<0) tt=-tt;
		arm_sqrt_f32(tt,(float *)Parameter.EffectiveValue+i);
#ifdef USE_FLITER
		if(Parameter.EffectiveValue[i]<0.25f)
		{
		Parameter.PeakValue[i]=0;
		Parameter.Inter_Skew[i]=0;
		Parameter.Kurtosis[i]=0; 		
		Parameter.Abs_average[i]=0;
		Parameter.MaxValue[i]=0;			
		Parameter.InterIIMarginIndex[i]=0;
		Parameter.WaveformIndex[i]=0; //????
		Parameter.PeakIndex[i]=0;//????
		Parameter.PulseIndex[i]=0;//????
		Parameter.Skew[i]=0;//????	 
		Parameter.KurtosisIndex[i]=0;//????
		Parameter.MarginIndex[i]=0;
					
		}
		else
#endif
		{		
		//Parameter.ReciprocalofEMUnumber ?????? Parameter.ReciprocalofEMUnumber=1/config.ADfrequence; 
		Parameter.PeakValue[i]=Parameter.InterMAX[i]-Parameter.InterMIN[i];
		Parameter.Inter_Skew[i]=Parameter.SSS_sum[i]*Parameter.ReciprocalofEMUnumber[i];
		Parameter.Kurtosis[i]=Parameter.SSSS_sum[i]*Parameter.ReciprocalofEMUnumber[i]; //??
		
		Parameter.Abs_average[i]=Parameter.Abs_S_average[i]*Parameter.ReciprocalofEMUnumber[i];//?????
		
		Parameter.InterIIMarginIndex[i]=(Parameter.Inter_MarginIndex[i]*Parameter.ReciprocalofEMUnumber[i])*(Parameter.Inter_MarginIndex[i]*Parameter.ReciprocalofEMUnumber[i]);
		if(Parameter.InterMAX[i]<(-Parameter.InterMIN[i])) Parameter.MaxValue[i]=-Parameter.InterMIN[i];
		else Parameter.MaxValue[i]=Parameter.InterMAX[i]; //????
		if(Parameter.Abs_average[i]==0) Parameter.Abs_average[i]=0.01f;
		Parameter.WaveformIndex[i]=Parameter.EffectiveValue[i]/Parameter.Abs_average[i]; //????
		Parameter.PeakIndex[i]=Parameter.MaxValue[i]/Parameter.EffectiveValue[i];//????
		Parameter.PulseIndex[i]=Parameter.MaxValue[i]/Parameter.Abs_average[i];//????
		Parameter.Skew[i]=Parameter.Inter_Skew[i]/(Parameter.EffectiveValue[i]*Parameter.EffectiveValue[i]*Parameter.EffectiveValue[i]);//????	 
		Parameter.KurtosisIndex[i]=Parameter.Kurtosis[i]/(Parameter.EffectiveValue[i]*Parameter.EffectiveValue[i]*Parameter.EffectiveValue[i]*Parameter.EffectiveValue[i]);//????
		Parameter.MarginIndex[i]=Parameter.MaxValue[i]/Parameter.InterIIMarginIndex[i];
		}
	}else 
		{  //
		 Parameter.Abs_average[i]=Parameter.S_sum[i]*Parameter.ReciprocalofEMUnumber[i];
		}
}



void InitIntermediateVariable(uint8_t i) // ???????,??????? ???????
{ 
	Parameter.S_average[i]=0;
	Parameter.alarm[i]=0;
	Parameter.backlash[i]=0;
	Parameter.gate[i]=0;
	Parameter.s[i]=0;
	Parameter.as[i]=0;
	Parameter.InterMAX[i]=0;
//	Parameter.MaxValue[i]=0;
	Parameter.InterMIN[i]=0;
	Parameter.Inter_MarginIndex[i]=0;
	Parameter.Abs_S_average[i]=0;
	//Parameter.Abs_average[i]=0;
	Parameter.S_sum[i]=0;
	Parameter.SS_sum[i]=0;
	Parameter.SSS_sum[i]=0;
	Parameter.SSSS_sum[i]=0;
	
}
float (*p)(double,uint32_t);

double (*p_highpass)(double,uint32_t);
extern uint32_t SAMPLEblock;

#define numStages 2 /* 2?×IIR??2¨μ???êy */
float testOutput[32768] ;//__attribute__ ((section ("RW_IRAM2"),  zero_init));//__attribute__((at(0x24000000)));//·??úaxiram /* ??2¨oóμ?ê?3? */
float IIRStateF32[4*numStages]; /* ×′ì??o′?￡?′óD?numTaps + blockSize - 1*/

//10 --- 1000 Hz
const float IIRCoeffs32768_10HP[5*numStages] = {
1 , 0 , -1 , 1.9972893096263979  , -0.99729305452039707   ,    
1 , 0 , -1 , 1.7348868152338506  , -0.76663771630244748      
};
const float sacle32768_10HP=0.08901975074448544f*0.08901975074448544f;


const float IIRCoeffs8192_10HP[5*numStages] = {
1 , 0 , -1 , 1.9891560834074471   , -0.98921570487111776    ,    
1 , 0 , -1 , 0.97731223759508934  , -0.34894057425861719          
};
const float sacle8192_10HP=0.30393869227125503f*0.30393869227125503f;


const float IIRCoeffs2048_10HP[5*numStages] = {
1 , 0 , -1 , 1.9566180839771632   , - 0.95754030895711184 ,    
1 , 0 , -1 , -1.8959514679392964   , -0.90110841687019083         
};
const float sacle2048_10HP=0.96379067842185728f*0.96379067842185728f;


arm_biquad_casd_df1_inst_f32 S_test;
volatile float hp_yy[6],SpeedInter[6];
extern volatile uint32_t currentSAMPLEblock;
extern int16_t piz_emu_data[2][32768];
float inputdata[32768];
extern  int16_t  mems_emu_data[2][3][8192]; //3轴mems数据存放
void EmuData(void)
{ 
	float absyy=0;
  float INTERsqrt=0;
	float yy;
	float * Adata;
	uint32_t SAMPLEblock=(currentSAMPLEblock+1)%2;;
	float ScaleValue=1;
		
		for(uint32_t j=0;j<AD7682_ADCHS;j++)
		{  
			Parameter.ReciprocalofEMUnumber[j]=1.0f/config.channel_freq[j]*config.ADtime;  //2é?ù?μ?êμ?μ1êy
			Parameter.ReciprocalofADfrequence[j]=1.0f/config.channel_freq[j]; 
			if(((config.DataToSendChannel>>j)&0x01)==0)  //?????????
			{
				continue;
			}
			Parameter.vs[j]=0;
			hp_yy[j]=0;
		  /* 3?ê??ˉ */
			switch(j)
			{
				case 0: 
					for(uint32_t i=0;i<config.channel_freq[j];i++)
						inputdata[i]=piz_emu_data[SAMPLEblock][i];
					break;
				case 1: 
					for(uint32_t i=0;i<config.channel_freq[j];i++)
						inputdata[i]=mems_emu_data[SAMPLEblock][0][0];
					break;
				case 2: 
					for(uint32_t i=0;i<config.channel_freq[j];i++)
						inputdata[i]=mems_emu_data[SAMPLEblock][1][0];
					break;
				case 3: 
					for(uint32_t i=0;i<config.channel_freq[j];i++)
						inputdata[i]=mems_emu_data[SAMPLEblock][2][0];
					break;
				default:
					break;
			}	
			switch(config.channel_freq[j])
				{
				
				case 32768:
					arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float *)&IIRCoeffs32768_10HP[0], (float
				*)&IIRStateF32[0]);
				/* IIR??2¨ */
				arm_biquad_cascade_df1_f32(&S_test,inputdata, testOutput,  config.channel_freq[j]);
				/*·????μêy */
				 ScaleValue = sacle32768_10HP;
					break;
				case 16384:
					arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float *)&IIRCoeffs32768_10HP[0], (float
				*)&IIRStateF32[0]);
				/* IIR??2¨ */
				arm_biquad_cascade_df1_f32(&S_test,inputdata, testOutput,  config.channel_freq[j]);
				/*·????μêy */
				 ScaleValue = sacle32768_10HP;
					break;
				case 8192:
					arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float *)&IIRCoeffs8192_10HP[0], (float
				*)&IIRStateF32[0]);
				/* IIR??2¨ */
				arm_biquad_cascade_df1_f32(&S_test,inputdata, testOutput, config.channel_freq[j]);
	//			arm_biquad_cascade_df1_f32(&S_test,(float *)testOutput1, testOutput, config.ADfrequence*2);
				
				/*·????μêy */
				ScaleValue = sacle8192_10HP;
					break;
				case 2048:
					arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float *)&IIRCoeffs2048_10HP[0], (float
				*)&IIRStateF32[0]);
				/* IIR??2¨ */
				arm_biquad_cascade_df1_f32(&S_test,inputdata, testOutput,  config.channel_freq[j]);
				/*·????μêy */
				 ScaleValue = sacle2048_10HP;
					break;
				default:
				break;
				}
			for(uint32_t i=0;i<config.channel_freq[j];i++)	 
			{
				hp_yy[j]+=testOutput[i]*ScaleValue*1000*Parameter.ReciprocalofADfrequence[j];
//				hp_yy[j]=testOutput[i];
				hp_yy[j]=hp_yy[j]*0.9999f;//D1·??±á÷·?á?
				if(i>=0.5f*config.channel_freq[j])
				{	
//				ReceiveSamplesPeriod[SAMPLEblock][j][i]=testOutput[i];					
//				hp_yy[j]=testOutput[i]*ScaleValue;//1000*Parameter.ReciprocalofADfrequence;
//				ReceiveSamplesPeriod[SAMPLEblock][j][i]=hp_yy[j];
					Parameter.vs[j]+=hp_yy[j]*hp_yy[j];	
				}
		}
		for(uint32_t j=0;j<AD7682_ADCHS;j++)
		{
				int16_t *p;
				switch(j){
					case 0: 
					p=&piz_emu_data[SAMPLEblock][0];
					break;
				case 1: 
					p=&mems_emu_data[SAMPLEblock][j][0];
					break;
				case 2: 
					p=&mems_emu_data[SAMPLEblock][j][1];
					break;
				case 3: 
					p=&mems_emu_data[SAMPLEblock][j][2];
					break;
				default:
					break;
				 }
				if(((config.DataToSendChannel>>j)&0x01)==0)  //?????????
				{
					continue;
				}
				 InitIntermediateVariable(j); //???????
				 for(uint32_t i=0;i<config.channel_freq[j];i++)	 {				
				 Parameter.S_average[j]+=*p;
				 p++;
			 }
			
		}
		for(uint32_t j=0;j<AD7682_ADCHS;j++)
		{
			Parameter.average[j]=Parameter.S_average[j]*Parameter.ReciprocalofEMUnumber[j]*config.floatadc[j]*config.floatscale[j];//?????
			if(0)
			{
				NeedRestartCollect();
			}
		}
		for(uint32_t j=0;j<AD7682_ADCHS;j++)	 {
		 	if(((config.DataToSendChannel>>j)&0x01)==0)  //?????????
			{
				continue;
			}
     for(uint32_t i=0;i<config.channel_freq[j];i++)
	   {
       switch(j){
						 case 0:
							  yy=(float)piz_emu_data[SAMPLEblock][i]*config.floatadc[j]*config.floatscale[j];
							 break;
						 case 1:
							  yy=(float)mems_emu_data[SAMPLEblock][0][i]*config.floatadc[j]*config.floatscale[j];
							 break;
						 case 2:
							  yy=(float)mems_emu_data[SAMPLEblock][1][i]*config.floatadc[j]*config.floatscale[j];
							 break;
						 case 3:
							  yy=(float)mems_emu_data[SAMPLEblock][2][i]*config.floatadc[j]*config.floatscale[j];
							 break;
						 default:
							 break;
					 }
			if(Parameter.InterMAX[j]<yy) Parameter.InterMAX[j]=yy;
			if(Parameter.InterMIN[j]>yy) Parameter.InterMIN[j]=yy;
			if(yy<0) absyy=-yy;
			 else absyy=yy;
			Parameter.Abs_S_average[j]+=absyy;
			arm_sqrt_f32(absyy,&INTERsqrt);
			Parameter.Inter_MarginIndex[j]+=INTERsqrt;
			Parameter.S_sum[j]+=yy;
			Parameter.SS_sum[j]+=yy*yy;
			Parameter.SSS_sum[j]+=(absyy*absyy*absyy);
			Parameter.SSSS_sum[j]+=(yy*yy*yy*yy);  	 
	   }
			
		}
		for(uint32_t j=0;j<AD7682_ADCHS;j++)
		{
			updateParameter(j);
//			if(Parameter.EffectiveValue[j]>25)
//			{
//				NeedRestartCollect();
//			}
			InitIntermediateVariable(j); 
		}
	}

}

uint8_t sendbuf[812];
uint8_t BoardParameter_withtime(void)  //发送特征值
{

	uint32_t empty_Acceleration_ADCHS=0;   //未发送通道
	sendbuf[0]=0x7e;
	sendbuf[1]=0x42;
	uint32_t iii=0;
	
	sendbuf[4]=g_tRTC.Year;
	sendbuf[5]=g_tRTC.Year>>8;
	sendbuf[6]=g_tRTC.Mon;
	sendbuf[7]=g_tRTC.Day; //时间用32位表示
	sendbuf[8]=g_tRTC.Hour;
	sendbuf[9]=g_tRTC.Min;
	sendbuf[10]=g_tRTC.Sec; //时间用32位表示
		
	for(uint32_t ii=0;ii<Acceleration_ADCHS;ii++)
	{
		if(((config.DataToSendChannel>>ii)&0x01)==0)  //未使能的通道不发送
		{
		 empty_Acceleration_ADCHS++;
		 continue;
		}

		sendbuf[11+50*iii]=ii+1;
		sendbuf[12+50*iii]=0x0c;
		

		uint8_t * floatdata=(uint8_t *)&Parameter.PeakValue[ii];//[0];
		sendbuf[13+50*iii]=*floatdata;
		sendbuf[14+50*iii]=*(floatdata+1);
		sendbuf[15+50*iii]=*(floatdata+2);
		sendbuf[16+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.MaxValue[ii];
		sendbuf[17+50*iii]=*floatdata;
		sendbuf[18+50*iii]=*(floatdata+1);
		sendbuf[19+50*iii]=*(floatdata+2);
		sendbuf[20+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.Abs_average[ii];
		sendbuf[21+50*iii]=*floatdata;
		sendbuf[22+50*iii]=*(floatdata+1);
		sendbuf[23+50*iii]=*(floatdata+2);
		sendbuf[24+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.EffectiveValue[ii];
		sendbuf[25+50*iii]=*floatdata;
		sendbuf[26+50*iii]=*(floatdata+1);
		sendbuf[27+50*iii]=*(floatdata+2);
		sendbuf[28+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.Kurtosis[ii];
		sendbuf[29+50*iii]=*floatdata;
		sendbuf[30+50*iii]=*(floatdata+1);
		sendbuf[31+50*iii]=*(floatdata+2);
		sendbuf[32+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.WaveformIndex[ii];
		sendbuf[33+50*iii]=*floatdata;
		sendbuf[34+50*iii]=*(floatdata+1);
		sendbuf[35+50*iii]=*(floatdata+2);
		sendbuf[36+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.PeakIndex[ii];
		sendbuf[37+50*iii]=*floatdata;
		sendbuf[38+50*iii]=*(floatdata+1);
		sendbuf[39+50*iii]=*(floatdata+2);
		sendbuf[40+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.PulseIndex[ii];
		sendbuf[41+50*iii]=*floatdata;
		sendbuf[42+50*iii]=*(floatdata+1);
		sendbuf[43+50*iii]=*(floatdata+2);
		sendbuf[44+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.MarginIndex[ii];
		sendbuf[45+50*iii]=*floatdata;
		sendbuf[46+50*iii]=*(floatdata+1);
		sendbuf[47+50*iii]=*(floatdata+2);
		sendbuf[48+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.KurtosisIndex[ii];
		sendbuf[49+50*iii]=*floatdata;
		sendbuf[50+50*iii]=*(floatdata+1);
		sendbuf[51+50*iii]=*(floatdata+2);
		sendbuf[52+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.Skew[ii];
		sendbuf[53+50*iii]=*floatdata;
		sendbuf[54+50*iii]=*(floatdata+1);
		sendbuf[55+50*iii]=*(floatdata+2);
		sendbuf[56+50*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.fv[ii];
		sendbuf[57+50*iii]=*floatdata;
		sendbuf[58+50*iii]=*(floatdata+1);
		sendbuf[59+50*iii]=*(floatdata+2);
		sendbuf[60+50*iii]=*(floatdata+3);
		iii++;
	} 
	iii--;
  uint32_t length=50*(AD7682_ADCHS-empty_Acceleration_ADCHS)+7;
  sendbuf[2]=(uint8_t)length;
	sendbuf[3]=(uint8_t)(length>>8);
	sendbuf[61+50*iii]=0;
	for(uint8_t i=1;i<(61+50*iii);i++)
	sendbuf[61+50*iii]+=sendbuf[i];
	sendbuf[62+50*iii]=0x7e;

	WriteDataToTXDBUF(sendbuf,(63+50*iii));
	//发送加速度跟速度，上面两个，下面发送速度
  
	return 1;
}

uint8_t PeriodWaveToSend[1600];  //这个全局变量开的很草率，以后项目还是用malloc,下次又是什么时候
extern RTC_T send_tRTC;

void BoardAutoPeroidWave(void)
{ 
	uint8_t checksum=0;
	uint32_t wpp=0;
	int16_t sendperioddata=0;
	uint32_t SAMPLEblock=(currentSAMPLEblock+1)%2;;
	float yy;
	int16_t *p;
	uint16_t packege_flag=0;
	uint16_t buflength=PERIODBOARDPOINTS+1+7+2;//1个字节通道号 7个字节时间，2个字节包号
 for(uint32_t ii=0;ii<Acceleration_ADCHS;ii++){
 if(((config.DataToSendChannel>>ii)&0x01)==0)  //未使能的通道不发送
	{
	 continue;
	}
// for(uint32_t i=0;i<(config.channel_freq[ii]);i++) //*config.ADtime
	 
		 switch(ii){
			 case 0:
					p=&piz_emu_data[SAMPLEblock][0];
				 break;
			 case 1:
					p=&mems_emu_data[SAMPLEblock][0][0];
				 break;
			 case 2:
					p=&mems_emu_data[SAMPLEblock][1][0];
				 break;
			 case 3:
					p=&mems_emu_data[SAMPLEblock][2][0];
				 break;
			 default:
				 break;
			}
	for(uint32_t i=0;i<config.channel_freq[ii];i++) //*config.ADtime
	 {
		 
		yy=*p;
		p++;
		sendperioddata=yy*config.floatadc[ii]*config.floatscale[ii];//(int16_t)(yy*Parameter.ReciprocalofRange[ii]);
		
		PeriodWaveToSend[wpp+14]=sendperioddata;
		PeriodWaveToSend[wpp+15]=sendperioddata>>8;
		checksum+=(PeriodWaveToSend[wpp+14]+PeriodWaveToSend[wpp+15]);			 
		wpp=wpp+2;			
  if(wpp>(PERIODBOARDPOINTS-1)) {	
		PeriodWaveToSend[0]=0x7e;//TELid;
		PeriodWaveToSend[1]=0x70;//TELid>>8;	
		PeriodWaveToSend[2]=buflength;//TELid>>16;
		PeriodWaveToSend[3]=(uint8_t)(buflength>>8);// TELid>>24; //2,3????????482??
		PeriodWaveToSend[4]=ii+1;// TELid>>24;
		PeriodWaveToSend[5]=g_tRTC.Year;
		PeriodWaveToSend[6]=g_tRTC.Year>>8;
		PeriodWaveToSend[7]=g_tRTC.Mon;
		PeriodWaveToSend[8]=g_tRTC.Day; //时间用32位表示
		PeriodWaveToSend[9]=g_tRTC.Hour;
		PeriodWaveToSend[10]=g_tRTC.Min;
		PeriodWaveToSend[11]=g_tRTC.Sec; //时间用32位表示		
		PeriodWaveToSend[12]=packege_flag; //时间用32位表示
		PeriodWaveToSend[13]=packege_flag>>8; //时间用32位表示
		for(uint32_t ii=1;ii<14;ii++)
		checksum+=PeriodWaveToSend[ii];  //adch是从1开始的
		PeriodWaveToSend[14+PERIODBOARDPOINTS]=checksum;  //2,3????????482??
		PeriodWaveToSend[15+PERIODBOARDPOINTS]=0x7e; 
		packege_flag++;
		WriteDataToTXDBUF(PeriodWaveToSend,PERIODBOARDPOINTS+16);	
		wpp=0;
		checksum=0;
		osDelay(1);
		}	 
	}
}
  

}

static uint16_t BoardPeroidWave_packege_flag=0;  //请求波形包号

RTC_T Requirdperiodwave_tRTC;

void BoardPeroidWave(void)
{ 
	uint8_t checksum=0;
	static uint32_t wpp=0;
  uint32_t SAMPLEblock=(currentSAMPLEblock+1)%2;
	int16_t send_perioddata=0;
	float yy;
	uint16_t buflength=PERIODBOARDPOINTS+1+7+2;//1个字节通道号 7个字节时间，2个字节包号
	for(uint32_t ii=0;ii<Acceleration_ADCHS;ii++){
	if(((config.DataToSendChannel>>ii)&0x01)==0)  //未使能的通道不发送
	{
		continue;
	}
	if(((config.RequirePeriodChannel>>ii)&0x01)==0)  //未请求的通道不发送
	{
	 continue;
	}
	for(uint32_t i=0;i<(config.channel_freq[ii]);i++) //*config.ADtime
	 {
		switch(i){
			 case 0:
					yy=piz_emu_data[SAMPLEblock][i];
				 break;
			 default:
					yy=mems_emu_data[SAMPLEblock][ii][i];
				 break;
			}
		//ReceiveSamplesPeriod[SAMPLEblock][ii][i]=100;
		send_perioddata=(int16_t)(yy*Parameter.ReciprocalofRange[ii]);
		
		PeriodWaveToSend[wpp+14]=send_perioddata;
		PeriodWaveToSend[wpp+15]=send_perioddata>>8;
		checksum+=(PeriodWaveToSend[wpp+14]+PeriodWaveToSend[wpp+15]);			 
		wpp=wpp+2;			
  if(wpp>(PERIODBOARDPOINTS-1)) {			
		PeriodWaveToSend[0]=0x7e;//TELid;
		PeriodWaveToSend[1]=0x30;//TELid>>8;	
		PeriodWaveToSend[2]=buflength;//TELid>>16;
		PeriodWaveToSend[3]=(uint8_t)(buflength>>8);// TELid>>24; //2,3????????482??
		PeriodWaveToSend[4]=ii;// TELid>>24;
		PeriodWaveToSend[5]=Requirdperiodwave_tRTC.Year;
		PeriodWaveToSend[6]=Requirdperiodwave_tRTC.Year>>8;
		PeriodWaveToSend[7]=Requirdperiodwave_tRTC.Mon;
		PeriodWaveToSend[8]=Requirdperiodwave_tRTC.Day; //时间用32位表示
		PeriodWaveToSend[9]=Requirdperiodwave_tRTC.Hour;
		PeriodWaveToSend[10]=Requirdperiodwave_tRTC.Min;
		PeriodWaveToSend[11]=Requirdperiodwave_tRTC.Sec; //时间用32位表示		
		PeriodWaveToSend[12]=BoardPeroidWave_packege_flag; //时间用32位表示
		PeriodWaveToSend[13]=BoardPeroidWave_packege_flag>>8; //时间用32位表示
		for(uint32_t ii=1;ii<14;ii++)
		checksum+=PeriodWaveToSend[ii];  //adch是从1开始的
		PeriodWaveToSend[14+PERIODBOARDPOINTS]=checksum;  //2,3????????482??
		PeriodWaveToSend[15+PERIODBOARDPOINTS]=0x7e; 
		BoardPeroidWave_packege_flag++;
//		WriteDataToTXDBUF(PeriodWaveToSend,PERIODBOARDPOINTS+16);	
		wpp=0;
		checksum=0;
//		vTaskDelay (pdMS_TO_TICKS(1));   
		}	 
	}
}

}

float PT_Temp[2];
void EMUTemp(void)
{
	int32_t t;
	float df;
	uint8_t TEMPBUF[30];
	unsigned char *floatPdata;
  uint32_t empty_Temp_ADCHS=0;
  uint8_t iii=0;
//	if(!isAbleTempTransmission()) return;

	PT_Temp[0]=Parameter.pdate;  


	 TEMPBUF[0]=0x7E;
	 TEMPBUF[1]=0x45;
	 for(uint32_t i=0;i<Temp_ADCHS;i++){
//	 if(((config.DataToSendChannel>>(i+Acceleration_ADCHS))&0x01)==0)  //未使能的通道不发送
//	{
//	 empty_Temp_ADCHS++;
//	 continue;
//	}
	 
	 floatPdata=(unsigned char *)&PT_Temp[i];
	 TEMPBUF[5+4*iii]=*floatPdata;
	 TEMPBUF[6+4*iii]=*(floatPdata+1);
	 TEMPBUF[7+4*iii]=*(floatPdata+2);
	 TEMPBUF[8+4*iii]=*(floatPdata+3);
	 iii++;
	 }
	 TEMPBUF[2]=0x02+(Temp_ADCHS-empty_Temp_ADCHS)*4;
	 TEMPBUF[3]=0x00;
	 TEMPBUF[4]=(Temp_ADCHS-empty_Temp_ADCHS);
	 iii--;
	 TEMPBUF[9+4*iii]=0XFF;  //电量
	 TEMPBUF[10+4*iii]=0;
	 for(uint16_t i=1;i<(10+4*iii);i++)
	 TEMPBUF[10+4*iii]+=TEMPBUF[i];	 
	 TEMPBUF[11+4*iii]=0x7E;

	 if(Temp_ADCHS!=empty_Temp_ADCHS) 
	 WriteDataToTXDBUF(TEMPBUF,(12+4*iii));
}

extern osSemaphoreId_t seconds_sample_data_readyHandle;
extern RTC_HandleTypeDef RtcHandle;

uint32_t WaritSignalCounter=0;

extern uint16_t command_channelkind(void);
extern uint16_t command_id(void);
extern uint16_t command_reply_SampleParameter(void);
extern uint16_t command_reply_scale(void);
uint32_t timetest=0;
void DataEmuFunction(void *argument)
{ 

 while(1)
 {
  osSemaphoreAcquire(seconds_sample_data_readyHandle, osWaitForever);//              // 		 
//		if(config.DataToBoardMode==PARAMETERMODE)
			if(config.Lowpower_Mode==normalmode)
			{
			
//				/*					请求波形逻辑 */	
//				if(Parameter.PeroidWaveTransmissionCounter!=0)
//				{
//					Parameter.PeroidWaveTransmissionCounter--;
//					BoardPeroidWave();
//				}else 
//				{
//					BoardPeroidWave_packege_flag=0;
//					Parameter.PeroidWaveTransmissionCounter=0;
//				}
//				/*				请求波形逻辑 */		
				EMUTemp();
			
				/*				自动发送波形跟特征值逻辑            */	
				if(config.PeriodTransimissonStatus==TRUE)
				{
					if((Parameter.AutoPeriodTransmissonCounter+1)>=config.PeriodTransimissonCounter)
					{
						EnablePeroidWaveAutoTransmission();
						Parameter.AutoPeriodTransmissonCounter=0;
					}else
					{
						Parameter.AutoPeriodTransmissonCounter++;
					}
					
				}
				/*				自动发送波形跟特征值逻辑            */	
				timetest=RTC_ConvertDatetimeToSeconds(&g_tRTC);		
				if(isAblePeroidWaveAutoTransmission()&&(config.DataToBoardMode==PARAMETERMODE))  //自动上传波形
				{		
//						bsp_LedStatue(0,1);
					
						EmuData();
					 if(Parameter.wakeupsourec==MANUALRESET)
					 {
						BoardParameter_withtime();
						BoardAutoPeroidWave();//BoardAutoPeroidWave();  //仅在特征值模式下生效
						DisablePeroidWaveAutoTransmission();
					 }
//						bsp_LedStatue(1,1);
				}
				
				if((config.Lowpower_Mode==lowpowermode)&&(Parameter.wakeupsourec==VLLS))
				{
					 WaritSignalCounter++;
					if(WaritSignalCounter>config.WaitforIEPEtime) //d等待信号滤波稳定开始发送数据
					 {
						 while(Parameter.Esp32TransmissionMode==BrainTransmission) //确定是透传模式
						 {
							 osDelay(10);
						 }
						 command_id();
						 command_channelkind();
						 command_reply_SampleParameter();
						 command_reply_scale();
//						 osDelay(100);
						 BoardParameter_withtime();
						 EMUTemp();
						 BoardAutoPeroidWave();
						 
					 }
					 
				}
				
			 
   	
  }
 }
}
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
