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
#include "Esp32ProcessTask.h"
#include "main.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "app.h"
#include "arm_math.h"
#include "clog.h"
extern volatile uint8_t currentblock ;
#include "double_integral.h"





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


volatile float hp_yy[6],SpeedInter[6];
extern volatile uint32_t currentSAMPLEblock;
extern int16_t piz_emu_data[2][32768];
float inputdata[32768];
extern  int16_t  mems_emu_data[2][3][8192]; //3轴mems数据存放
float testin[4096];
float testout[4096];
uint32_t test11[4]={1000,80,1000,1};
float testin[4096];
float emu_inter_data[16384];
float fft_data[4096];
float fft_inter_data[4096];

#define numStages 2 /* 2阶IIR滤波的个数 */

//emu_data输入
float32_t testOutput[16384] ;//////输出结果
float32_t IIRStateF32[4*numStages]; /* 状态缓存，大小numTaps + blockSize - 1*/
/* 巴特沃斯低通滤波器系数 140Hz */
const float32_t IIRCoeffs16384_1600HP[5*numStages] = {
1 , 2 , 1 , 1.3399160934903298         ,  -0.63887106504007884        ,    
1 , 2 , 1 , 1.0673588418587563         ,  -0.30550228513217675            
};
const float sacle16384_1600HP=0.074738742887437259f*0.05953586081835513f;


const float32_t IIRCoeffs8192_1600HP[5*numStages] = {
1 , 2 , 1 , 0.49531214578230898        ,  -0.47024952160149391           ,    
1 , 2 , 1 , 0.36033441950676542        ,  -0.069591190944092607         
};
const float sacle8192_1600HP=0.24373434395479621f*0.17731419285933178f;


const float32_t IIRCoeffs4096_1600HP[5*numStages] = {
1 , 2 , 1 , -1.2440102900675862          , -0.60930590867938939          ,    
1 , 2 , 1 , -0.97472921694631054         , -0.26095218079659854           
};
const float sacle4096_1600HP=0.71332904968674393f*0.55892034943572722f;

const float IIRCoeffs16384_1000_6000BP[5*numStages] = {
1,  0,  -1,  1.461284132969656,       -0.59524384411993081, 
1,  0,  -1,  -0.83352188313723552,   -0.33796843110898939   
};
const float sacle16384_1000_6000BP= 0.63429186127528914f*0.63429186127528914f;

const float IIRCoeffs16384_1000LP[5*numStages] = {
1 , 2 , 1 , 1.6224170187429059        , - 0.74949598875049617          ,    
1 , 2 , 1 , 1.3782783266582523        , - 0.48623465854586218             
};
const float sacle16384_1000LP=0.031769742501897576f*0.026989082971902483f;

arm_biquad_casd_df1_inst_f32 S_test;

void EmuData(void)
{ 
	float absyy=0;
	float INTERsqrt=0;
	float yy;
	float * Adata;
	uint32_t SAMPLEblock=(currentSAMPLEblock+1)%2;;
	float ScaleValue=1;
	float avage[4];
	uint32_t maxindex;
	float EnvelopMax[4];
	
	for(uint32_t j=0;j<Acceleration_ADCHS;j++)
	{
		float inter_factor = config.floatscale[j] * config.floatadc[j] * 0.03814755f ;//2500 / 65535;
		switch(j)
		{
			case 0: 
				for(uint32_t i=0;i<config.channel_freq[j];i++)
				emu_inter_data[i]=inter_factor * piz_emu_data[SAMPLEblock][i];
			break;
			default:
				for(uint32_t i=0;i<config.channel_freq[j];i++)
				{
					emu_inter_data[i]=inter_factor*mems_emu_data[SAMPLEblock][j -1][i];
				}
			break;
		}

		/*
		for(uint32_t i=0;i<config.channel_freq[j];i++)
			emu_inter_data[i] = test11[0]*arm_sin_f32(2*3.1415926f*test11[1]*i/config.channel_freq[j]);
		*/
		
		switch(config.channel_freq[j])
		{
			case 16384:
				{
					arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float32_t *)&IIRCoeffs16384_1600HP[0], (float32_t
					*)&IIRStateF32[0]);
					/* IIR滤波 */
					arm_biquad_cascade_df1_f32(&S_test,emu_inter_data, testOutput, config.channel_freq[j]);
					/*放缩系数 */
					ScaleValue = sacle16384_1600HP;
				}
				for(uint32_t i=0;i<4096;i++)
				{
					fft_data[i]=testOutput[4*i]*ScaleValue;
				}
			break;
			case 8192:
				arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float32_t *)&IIRCoeffs8192_1600HP[0], (float32_t
				*)&IIRStateF32[0]);
				/* IIR滤波 */
				arm_biquad_cascade_df1_f32(&S_test,emu_inter_data, testOutput,  config.channel_freq[j]);
				/*放缩系数 */
				ScaleValue = sacle8192_1600HP;
				for(uint32_t i=0;i<4096;i++)
				{
					fft_data[i]=testOutput[2*i]*ScaleValue;
				}
			break;
			case 4096:
				arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float32_t *)&IIRCoeffs4096_1600HP[0], (float32_t
				*)&IIRStateF32[0]);
				/* IIR滤波 */
				arm_biquad_cascade_df1_f32(&S_test,emu_inter_data, testOutput, config.channel_freq[j]);
				/*放缩系数 */
				ScaleValue = sacle4096_1600HP;
				for(uint32_t i=0;i<4096;i++)
				{
					fft_data[i]=testOutput[i]*ScaleValue;
				}   
			break;
			default:

			break;
		}         
		
		arm_rms_f32(fft_data, 4096, &Parameter.Arms[j]);
		integ_init(4096,4096,1000,1,4,1000);  //速度到4
		frq_domain_integral(fft_data,fft_inter_data);
		arm_rms_f32(fft_inter_data, 4096, &Parameter.Vrms[j]);
		integ_init(4096,4096,1000000,2,10,1000);//位移到10
		frq_domain_integral(fft_data,fft_inter_data);
		arm_rms_f32(fft_inter_data, 4096, &Parameter.Drms[j]);
		
		if(0)
		{
			integ_init(4096,4096,1,1,10,1000);//位移到10
			frq_envelop_integral(fft_data,fft_inter_data);
			arm_max_f32(fft_inter_data, 4096, &EnvelopMax[j],&maxindex);
			arm_sqrt_f32(EnvelopMax[j],&Parameter.Envelop[j]);
		}

		arm_mean_f32(emu_inter_data, config.channel_freq[j], &avage[j]); //求均值

		for(uint32_t i=0;i<config.channel_freq[j];i++)
		{
			emu_inter_data[i]=(emu_inter_data[i]-avage[j]); //去下均值
			testOutput[i]=emu_inter_data[i];
		}

		arm_rms_f32(emu_inter_data, config.channel_freq[j], &Parameter.EffectiveValue[j]);	
		for(uint32_t i=0;i<config.channel_freq[j];i++)
		{
			emu_inter_data[i]=emu_inter_data[i]*emu_inter_data[i]*emu_inter_data[i]*emu_inter_data[i];
		}
		arm_rms_f32(emu_inter_data, config.channel_freq[j], &Parameter.Kurtosis[j]);
		Parameter.KurtosisIndex[j]=Parameter.Kurtosis[j]/(Parameter.EffectiveValue[j]*Parameter.EffectiveValue[j]*Parameter.EffectiveValue[j]*Parameter.EffectiveValue[j]);


		if((j==0)&&(config.channel_freq[j]==16384)) //对主轴进行包络计算
		{
			arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float32_t *)&IIRCoeffs16384_1000_6000BP[0], (float32_t
			*)&IIRStateF32[0]);
			/* IIR滤波 */
			arm_biquad_cascade_df1_f32(&S_test,testOutput, emu_inter_data, config.channel_freq[j]);
			/*放缩系数 */
			ScaleValue = sacle16384_1000_6000BP;
			for(uint32_t i=0;i<16384;i++)
			{
				emu_inter_data[i]=emu_inter_data[i]*ScaleValue;
			}  
			arm_abs_f32(emu_inter_data,testOutput,16384);
			arm_biquad_cascade_df1_init_f32(&S_test, numStages, (float32_t *)&IIRCoeffs16384_1000LP[0], (float32_t
			*)&IIRStateF32[0]);
			/* IIR滤波 */
			arm_biquad_cascade_df1_f32(&S_test,testOutput, emu_inter_data, config.channel_freq[j]);
			ScaleValue = sacle16384_1000LP;
			arm_max_f32(emu_inter_data, 16384, &EnvelopMax[j],&maxindex);  //换来换去，头都混了
			Parameter.Envelop[j]=EnvelopMax[j]*ScaleValue;
		}	
		
		if(Parameter.EffectiveValue[j] < 0.25f)//Parameter.Arms[j] < 0.25f || 
		{
			Parameter.Vrms[j] = 0.0f;
			Parameter.Drms[j] = 0.0f;
			Parameter.KurtosisIndex[j] = 0.0f;
			Parameter.Envelop[j] = 0.0f;
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

		sendbuf[11+22*iii]=ii+1;
		sendbuf[12+22*iii]=0x05; //5个参数
		

		uint8_t * floatdata=(uint8_t *)&Parameter.EffectiveValue[ii];//[0];
		sendbuf[13+22*iii]=*floatdata;
		sendbuf[14+22*iii]=*(floatdata+1);
		sendbuf[15+22*iii]=*(floatdata+2);
		sendbuf[16+22*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.Vrms[ii];
		sendbuf[17+22*iii]=*floatdata;
		sendbuf[18+22*iii]=*(floatdata+1);
		sendbuf[19+22*iii]=*(floatdata+2);
		sendbuf[20+22*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.Drms[ii];
		sendbuf[21+22*iii]=*floatdata;
		sendbuf[22+22*iii]=*(floatdata+1);
		sendbuf[23+22*iii]=*(floatdata+2);
		sendbuf[24+22*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.KurtosisIndex[ii];
		sendbuf[25+22*iii]=*floatdata;
		sendbuf[26+22*iii]=*(floatdata+1);
		sendbuf[27+22*iii]=*(floatdata+2);
		sendbuf[28+22*iii]=*(floatdata+3);
		floatdata=(uint8_t *)&Parameter.Envelop[ii];
		sendbuf[29+22*iii]=*floatdata;
		sendbuf[30+22*iii]=*(floatdata+1);
		sendbuf[31+22*iii]=*(floatdata+2);
		sendbuf[32+22*iii]=*(floatdata+3);
		iii++;
	} 
	iii--;
  uint32_t length=22*(Acceleration_ADCHS-empty_Acceleration_ADCHS)+7;
  sendbuf[2]=(uint8_t)length;
	sendbuf[3]=(uint8_t)(length>>8);
	sendbuf[33+22*iii]=0;
	for(uint8_t i=1;i<(33+22*iii);i++)
	sendbuf[33+22*iii]+=sendbuf[i];
	sendbuf[34+22*iii]=0x7e;

	WriteDataToTXDBUF(sendbuf,(35+22*iii));
	//发送加速度跟速度，上面两个，下面发送速度
  
	return 1;
}
uint8_t value_statue[20];
void rule_check(void)  //原则上这部分要写活一点，但是报文规范不好，这里不浪费时间了
{
	memset(value_statue,0,20);
	if(Parameter.EffectiveValue[0]>config.Alarm_value_junyue[0])
		value_statue[0]=1;
	if(Parameter.Vrms[0]>config.Alarm_value_junyue[1])
		value_statue[1]=1;
	if(Parameter.Drms[0]>config.Alarm_value_junyue[2])
		value_statue[2]=1;
	if(Parameter.KurtosisIndex[0]>config.Alarm_value_junyue[3])
		value_statue[3]=1;
	if(Parameter.Envelop[0]>config.Alarm_value_junyue[4])
		value_statue[4]=1;
	if(Parameter.EffectiveValue[1]>config.Alarm_value_junyue[5])
		value_statue[5]=1;
	if(Parameter.Vrms[1]>config.Alarm_value_junyue[6])
		value_statue[6]=1;
	if(Parameter.Drms[1]>config.Alarm_value_junyue[7])
		value_statue[7]=1;
	if(Parameter.KurtosisIndex[1]>config.Alarm_value_junyue[8])
		value_statue[8]=1;
	if(Parameter.Envelop[1]>config.Alarm_value_junyue[9])
		value_statue[9]=1;
	if(Parameter.EffectiveValue[2]>config.Alarm_value_junyue[10])
		value_statue[10]=1;
	if(Parameter.Vrms[2]>config.Alarm_value_junyue[11])
		value_statue[11]=1;
	if(Parameter.Drms[2]>config.Alarm_value_junyue[12])
		value_statue[12]=1;
	if(Parameter.KurtosisIndex[2]>config.Alarm_value_junyue[13])
		value_statue[13]=1;
	if(Parameter.Envelop[2]>config.Alarm_value_junyue[14])
		value_statue[14]=1;
	if(Parameter.pdate>config.Alarm_value_junyue[15])
		value_statue[15]=1;
	if(config.battery>config.Alarm_value_junyue[16])
		value_statue[16]=1;
	for(uint8_t i=0;i<16;i++)
	{
		if(value_statue[i]==1)
		{
			SetAlarmStatue();
			break;
		}
		ClrAlarmStatue();
	}
}
uint8_t BoardParameter_withtime_forJUNYUE(void)  //发送特征值,君悦版本
{

	uint32_t empty_Acceleration_ADCHS=0;   //未发送通道
	sendbuf[0]=0x7e;
	sendbuf[1]=COMMAND_BOARD_PARAMETERVALUE_JUNYUE;
	uint32_t 	iii=0;
	uint8_t * floatdata;
	sendbuf[4]=g_tRTC.Year;
	sendbuf[5]=g_tRTC.Year>>8;
	sendbuf[6]=g_tRTC.Mon;
	sendbuf[7]=g_tRTC.Day; //时间用32位表示
	sendbuf[8]=g_tRTC.Hour;
	sendbuf[9]=g_tRTC.Min;
	sendbuf[10]=g_tRTC.Sec; //时间用32位表示
	rule_check();	
	for(uint32_t ii=0;ii<Acceleration_ADCHS;ii++)
	{
		floatdata=(uint8_t *)&Parameter.EffectiveValue[ii];//[0];
		sendbuf[11+25*iii]=*floatdata;
		sendbuf[12+25*iii]=*(floatdata+1);
		sendbuf[13+25*iii]=*(floatdata+2);
		sendbuf[14+25*iii]=*(floatdata+3);
		sendbuf[15+25*iii]=value_statue[0+ii*5]; //5个参数		
		
		floatdata=(uint8_t *)&Parameter.Vrms[ii];
		sendbuf[16+25*iii]=*floatdata;
		sendbuf[17+25*iii]=*(floatdata+1);
		sendbuf[18+25*iii]=*(floatdata+2);
		sendbuf[19+25*iii]=*(floatdata+3);
		sendbuf[20+25*iii]=value_statue[1+ii*5];
		
		
		
		floatdata=(uint8_t *)&Parameter.Drms[ii];
		sendbuf[21+25*iii]=*floatdata;
		sendbuf[22+25*iii]=*(floatdata+1);
		sendbuf[23+25*iii]=*(floatdata+2);
		sendbuf[24+25*iii]=*(floatdata+3);
		sendbuf[25+25*iii]=value_statue[2+ii*5];
		
		
			
		floatdata=(uint8_t *)&Parameter.KurtosisIndex[ii];
		sendbuf[26+25*iii]=*floatdata;
		sendbuf[27+25*iii]=*(floatdata+1);
		sendbuf[28+25*iii]=*(floatdata+2);
		sendbuf[29+25*iii]=*(floatdata+3);
		sendbuf[30+25*iii]=value_statue[3+ii*5];
		
	
			
		floatdata=(uint8_t *)&Parameter.Envelop[ii];
		sendbuf[31+25*iii]=*floatdata;
		sendbuf[32+25*iii]=*(floatdata+1);
		sendbuf[33+25*iii]=*(floatdata+2);
		sendbuf[34+25*iii]=*(floatdata+3);
		sendbuf[35+25*iii]=value_statue[4+ii*5];
		iii++;
	} 
	iii--;
	

	floatdata=(uint8_t *)&Parameter.pdate;
	sendbuf[36+25*2]=*floatdata;
	sendbuf[37+25*2]=*(floatdata+1);
	sendbuf[38+25*2]=*(floatdata+2);
	sendbuf[39+25*2]=*(floatdata+3);
	sendbuf[40+25*2]=value_statue[15];

	floatdata=(uint8_t *)&config.battery;
	sendbuf[41+25*2]=*floatdata;
	sendbuf[42+25*2]=*(floatdata+1);
	sendbuf[43+25*2]=*(floatdata+2);
	sendbuf[44+25*2]=*(floatdata+3);
	sendbuf[45+25*2]=value_statue[16];
	
	uint32_t length=85+7;
	sendbuf[2]=(uint8_t)length;
	sendbuf[3]=(uint8_t)(length>>8);
	sendbuf[46+25*2]=0;
	for(uint8_t i=1;i<(46+25*2);i++)
	sendbuf[46+25*2]+=sendbuf[i];
	sendbuf[47+25*2]=0x7e;

	WriteDataToTXDBUF(sendbuf,(48+25*2));
	//发送加速度跟速度，上面两个，下面发送速度
  
	return 1;
}
uint8_t PeriodWaveToSend[1600];  //这个全局变量开的很草率，以后项目还是用malloc,下次又是什么时候
extern RTC_T send_tRTC;
void BoardAutoPeroidWave1(void)
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
	
	for(uint32_t ii=0;ii<Acceleration_ADCHS;ii++)
	{
		if(((config.DataToSendChannel>>ii)&0x01)==0)  //未使能的通道不发送
		{
			continue;
		}
		// for(uint32_t i=0;i<(config.channel_freq[ii]);i++) //*config.ADtime

		switch(ii)
		{
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
		float inter_factor=config.floatadc[ii]*config.floatscale[ii]*1250.0f/config.floatrange[ii];
		
		for(uint32_t i=0;i<config.channel_freq[ii];i++) //*config.ADtime
		{
			yy = *p;
			p ++;
			sendperioddata = yy * inter_factor;//(int16_t)(yy*Parameter.ReciprocalofRange[ii]);

			PeriodWaveToSend[wpp + 14] = sendperioddata;
			PeriodWaveToSend[wpp + 15] = sendperioddata >>8;
			checksum += (PeriodWaveToSend[wpp + 14] + PeriodWaveToSend[wpp + 15]);			 
			wpp = wpp + 2;			
			if(wpp > (PERIODBOARDPOINTS - 1)) 
			{	
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
	 TEMPBUF[9+4*iii]=config.battery;  //电量
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
extern void TransmitOverInLowPower(void);
volatile uint32_t WaitTransmissionModeCounter=0;
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
			
			
				/*				自动发送波形跟特征值逻辑            */	
				if(config.PeriodTransimissonStatus==TRUE)
				{
					if((Parameter.AutoPeriodTransmissonCounter + 1) >= config.PeriodTransimissonCounter)
					{
						EnablePeroidWaveAutoTransmission();
						Parameter.AutoPeriodTransmissonCounter = 0;
					}else
					{
						Parameter.AutoPeriodTransmissonCounter ++;
					}
					
				}
				/*				自动发送波形跟特征值逻辑            */	
				timetest=RTC_ConvertDatetimeToSeconds(&g_tRTC);		
				bsp_LedToggle(1);
//				
				if(isAblePeroidWaveAutoTransmission()&&(config.DataToBoardMode==PARAMETERMODE))  //自动上传波形
				{		
				
//						bsp_LedStatue(0,1);
					
						EmuData();
					
					 if(Parameter.wakeupsourec==MANUALRESET)
					 {
						 //	EMUTemp();
//						BoardParameter_withtime();
						BoardParameter_withtime_forJUNYUE();
						BoardAutoPeroidWave();  //仅在特征值模式下生效
						DisablePeroidWaveAutoTransmission();
						 DEBUG("SendValue\r\n");
					 }
//						bsp_LedStatue(1,1);
				}
			}
				if(config.Lowpower_Mode==lowpowermode)
				{
					bsp_LedToggle(1);
					 WaritSignalCounter++;
					if(WaritSignalCounter>config.WaitforIEPEtime) //d等待信号滤波稳定开始发送数据
					 {
						 WaitTransmissionModeCounter=0;
						 config.battery=config.battery - 0.02f;
						 if(config.battery <= 0 )
						 {
							 config.battery = 0;
						 }
						 StopSample();
						 saveConfig();
						 while((Parameter.Esp32TransmissionMode==BrainTransmission)&&(WaitTransmissionModeCounter<400)) //确定是透传模式
						 {
							 osDelay(10);
							 WaitTransmissionModeCounter++;
						 }
						
						 osDelay(1); //Wait for the id ... send over
						 if(Esp32_GetSendDeivceInfo_Flag() == 1)
						 {
							 Esp32_ClearSendDeviceInfo_Flag();
							EmuData();
							
							BoardParameter_withtime_forJUNYUE();
							BoardAutoPeroidWave();

							TransmitOverInLowPower(); //后期控制这个，可以决定传多少秒
							 DEBUG("SendValue\r\n");
						 }
						 else
						 {
							 
						 }
						 
						 if(Parameter.wakeupsourec!=VLLS)
						 {
								StopSample();
								osDelay(4000);
								HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);    //外部中断使能
								HAL_NVIC_DisableIRQ(TIM4_IRQn);            //TPM中断使能	//
								osThreadSuspend(Esp32ProcessHandle);   //挂起所有线程
								osThreadSuspend(DateProcessHandle);
								osThreadSuspend(NetProcessHandle);
								osThreadSuspend(RxdBufProcessHandle);
								osThreadSuspend(defaultTaskHandle);
						 }
						 osDelay(100);
						 osThreadSuspend(DateEmuHandle); //挂起自身线程
					 }
					 
				}
				
			 
   	
  
 }
}
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
