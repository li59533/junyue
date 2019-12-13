#include "main.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "app.h"
#include "stdbool.h"
#include "version.h"


extern TIM_HandleTypeDef htim8;
extern uint8_t   RxdBuf[RxdBufLine];

volatile uint16_t RxdBytes=0;
volatile uint16_t TxdBytes=0;

uint8_t  RxdTelBuf[RX_BUFFER_SIZE];
uint8_t RXDCmdBuf[RX_BUFFER_SIZE];

volatile uint8_t ReceivedTel;
volatile uint16_t RxdTelBytes;	

uint16_t TelBufLength=0;

uint8_t  	TxdBuf[TX_BUFFER_SIZE];
uint8_t 	CmdBuf[TX_BUFFER_SIZE];
uint16_t 	CmdBufLength;
extern void software_reset(void);
//extern volatile uint32_t ActualIndex;

//extern volatile uint16_t wave_packege_flag;

#define      sw_version                        "SGQG6666^-^"

uint8_t command_start()
{ 
	{
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
		CmdBufLength=CmdBufLength;
	}
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	if(config.Lowpower_Mode==normalmode)
	{
		StartSample();
	}
	//
	return 1;
}

uint8_t command_stop()
{  
	//DisableIEPEPower();
	 if(config.DataToBoardMode==WAVEMODE)
	 {
//	 TPM_StopSHAOTimer(TPM1);
//	 stop_ad7606_samp();
	 }
	 else	
	 {
//		 LPTMR_StopTimer(LPTMR0);
//		 TPM_StopSHAOTimer(TPM1);
//		 stop_ad7606_samp();
	 }
	 StopSample();
	 osDelay (5); 
	 DisableTempTransmission();
//	 if(HeadIndex==TailIndex)
//		 HeadIndex==TailIndex; //如果停止的时候，发现数据满了，则重新启动下
	 //vTaskDelay (pdMS_TO_TICKS(1)); //延迟一毫秒

	 {
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
		//TxdBytes=0;
	  CmdBufLength=CmdBufLength;

	 }
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 /*********************RCM输出**************/
	 CmdBuf[0]=0X7E;
	 CmdBuf[1]=0XF3;   
	 CmdBuf[2]=8;//8个RCM寄存器
	 CmdBuf[3]=8>>8;   //L
	 CmdBuf[4]=RCC->BDCR;
	 CmdBuf[5]=RCC->CRRCR;
	 CmdBuf[6]=RCC->CSR;
	 CmdBuf[7]=RCC->CCIPR;
	 CmdBuf[8]=RCC->CCIPR2;
	 CmdBuf[9]=RCC->ICSCR;
	 CmdBuf[10]=RCC->CR;
	 CmdBuf[11]=(config.RESETTIME);
	 CmdBuf[12]=0;
	 for(uint8_t i=1;i<12;i++)
	 CmdBuf[12]+=CmdBuf[i];
	 CmdBuf[13]=0x7e;
	 WriteDataToTXDBUF(CmdBuf,14); 
	 osDelay (5); 
	
	  /*********************RCM输出**************/
	 return 1;
}

void test_rtc(void)
{
	CmdBuf[0]=0X7E;
	 CmdBuf[1]=0XF3;   
	 CmdBuf[2]=8;//8个RCM寄存器
	 CmdBuf[3]=8>>8;   //L
	 CmdBuf[4]=RTC->TR;
	 CmdBuf[5]=(RTC->TR)>>8;
	 CmdBuf[6]=RCC->CSR;
	 CmdBuf[7]=RCC->CCIPR;
	 CmdBuf[8]=RCC->CCIPR2;
	 CmdBuf[9]=RCC->ICSCR;
	 CmdBuf[10]=RCC->CR;
	 CmdBuf[11]=(config.RESETTIME);
	 CmdBuf[12]=0;
	 for(uint8_t i=1;i<12;i++)
	 CmdBuf[12]+=CmdBuf[i];
	 CmdBuf[13]=0x7e;
	 WriteDataToTXDBUF(CmdBuf,14);
}
uint8_t command_receive_beacon()
{ 
	 {
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	  CmdBufLength=CmdBufLength;
	 }
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 ReceiveBeaconMessage();
	 return 1;
}

uint16_t command_id(void)
{ 
	
	 {
		CmdBuf[0]=0x7e;
		CmdBuf[1]=0x02;
		CmdBuf[2]=0x08;
		CmdBuf[3]=0x00;
		CmdBuf[4]=config.SNnumber[0];
		CmdBuf[5]=config.SNnumber[1];
		CmdBuf[6]=config.SNnumber[2];
		CmdBuf[7]=config.SNnumber[3];
		CmdBuf[8]=config.SNnumber[4];
		CmdBuf[9]=config.SNnumber[5];
		CmdBuf[10]=config.SNnumber[6];
		CmdBuf[11]=config.SNnumber[7];
		CmdBuf[12]=0;
		for(uint16_t i=1;i<12;i++)
		CmdBuf[12]+=CmdBuf[i];
		CmdBuf[13]=0x7e;
	  CmdBufLength=14;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	 return 1;
}

uint16_t command_channelkind(void)
{

	{
		CmdBuf[0]=0x7e;
		CmdBuf[1]=0x03;
		CmdBuf[2]=0x06;
		CmdBuf[3]=0x00;
		CmdBuf[4]=Acceleration_ADCHS+Temp_ADCHS; //两个，一个温度一个加速度
		CmdBuf[5]=0x02;  //这一块，每种传感器手工添加
		CmdBuf[6]=0x01;
		CmdBuf[7]=Acceleration_ADCHS;
		CmdBuf[8]=0x02;
		CmdBuf[9]=Temp_ADCHS;
		CmdBuf[10]=0;
		for(uint16_t i=1;i<10;i++)
		CmdBuf[10]+=CmdBuf[i];

		CmdBuf[11]=0x7e;
		CmdBufLength=12;
		WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	}
	return 1;
}
uint8_t command_setip()  //设置IP地址  //设置DHCP
{
	{
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
		CmdBufLength=CmdBufLength;
	 }
#ifdef ProgramESP32ByMyself
	 CmdBuf[1]=0xe4; //
	 for(uint32_t i=1;i<(CmdBufLength-2);i++)
	 CmdBuf[CmdBufLength-2]+=CmdBuf[i];
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
#endif
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 for(uint8_t i=0;i<CmdBuf[5];i++)
	 config.LocalIP[i]=CmdBuf[6+i];//把AP字符串赋值给config
	 config.LocalIP[CmdBuf[5]]=0;
	 for(uint8_t i=0;i<CmdBuf[7+CmdBuf[5]];i++)
	 config.LocalMASK[i]=CmdBuf[8+CmdBuf[5]+i];//把AP字符串赋值给config
	 config.LocalMASK[CmdBuf[7+CmdBuf[5]]]=0;
	 for(uint8_t i=0;i<CmdBuf[9+CmdBuf[5]+CmdBuf[7+CmdBuf[5]]];i++)
	 config.LocalGATEWAY[i]=CmdBuf[10+CmdBuf[5]+CmdBuf[7+CmdBuf[5]]+i];//把AP字符串赋值给config
	 config.LocalGATEWAY[CmdBuf[9+CmdBuf[5]+CmdBuf[7+CmdBuf[5]]]]=0;
	 //for(uint8_t i=0;i<CmdBuf[11+CmdBuf[5]+CmdBuf[7+CmdBuf[5]]];i++)
	 config.DHCP=CmdBuf[12+CmdBuf[5]+CmdBuf[7+CmdBuf[5]]+CmdBuf[9+CmdBuf[5]+CmdBuf[7+CmdBuf[5]]]];//把AP字符串赋值给config
	 return 1;
}
uint8_t command_replyip()  //设置IP地址
{
	 uint8_t localIPlength=strlen(config.LocalIP);
	 uint8_t LocalMASKlength=strlen(config.LocalMASK);
	 uint8_t LocalGATEWAYlength=strlen(config.LocalGATEWAY);
	 CmdBufLength=15+localIPlength+LocalMASKlength+LocalGATEWAYlength;
	 CmdBuf[0]=0X7E;
	 CmdBuf[1]=0X04;   //T
	 CmdBuf[2]=CmdBufLength-6;
	 CmdBuf[3]=(CmdBufLength-6)>>8;   //L
	 CmdBuf[4]=0X00;   //V_IP
	 CmdBuf[5]=localIPlength;
	{
		for(uint8_t i=0;i<localIPlength;i++)
		CmdBuf[6+i]=config.LocalIP[i];
	 
	 }
   CmdBuf[6+localIPlength]=0X01;   //V_MASK
	 CmdBuf[7+localIPlength]=LocalMASKlength;
	{
		for(uint8_t i=0;i<LocalMASKlength;i++)
		CmdBuf[8+localIPlength+i]=config.LocalMASK[i];
	 
	 }
	 CmdBuf[8+localIPlength+LocalMASKlength]=0X02;   //V_GATEWAY
	 CmdBuf[9+localIPlength+LocalMASKlength]=LocalGATEWAYlength;
	{
		for(uint8_t i=0;i<LocalGATEWAYlength;i++)
		CmdBuf[10+localIPlength+LocalMASKlength+i]=config.LocalGATEWAY[i];
	 
	 }
	 CmdBuf[10+localIPlength+LocalMASKlength+LocalGATEWAYlength]=0X03;
	 CmdBuf[11+localIPlength+LocalMASKlength+LocalGATEWAYlength]=0X01;
	 CmdBuf[12+localIPlength+LocalMASKlength+LocalGATEWAYlength]=config.DHCP;
	 CmdBuf[13+localIPlength+LocalMASKlength+LocalGATEWAYlength]=0;
	 for(uint8_t i=1;i<(13+localIPlength+LocalMASKlength+LocalGATEWAYlength);i++)
	 CmdBuf[13+localIPlength+LocalMASKlength+LocalGATEWAYlength]+=CmdBuf[i];//把AP字符串赋值给config
	 CmdBuf[14+localIPlength+LocalMASKlength+LocalGATEWAYlength]=0x7e;
	 CmdBufLength=15+localIPlength+LocalMASKlength+LocalGATEWAYlength;
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 return 1;
}

uint8_t command_replyap()  //返回IP地址
{  
	//这边要判断是否工厂设置
	 uint8_t APSSIDlength=strlen(config.APssid);
	 uint8_t APPASSWORDlength=strlen(config.APpassword);
	 CmdBufLength=10+APSSIDlength+APPASSWORDlength;
	 CmdBuf[0]=0X7E;
	 CmdBuf[1]=0X53;   //T
	 CmdBuf[2]=CmdBufLength-6;
	 CmdBuf[3]=(CmdBufLength-6)>>8;   //L
	 CmdBuf[4]=0X00;   //V_IP
	 CmdBuf[5]=APSSIDlength;
	{
		for(uint8_t i=0;i<APSSIDlength;i++)
		CmdBuf[6+i]=config.APssid[i];
	 
	 }
   CmdBuf[6+APSSIDlength]=0X01;   //V_MASK
	 CmdBuf[7+APSSIDlength]=APPASSWORDlength;
	{
		for(uint8_t i=0;i<APPASSWORDlength;i++)
		CmdBuf[8+APSSIDlength+i]=config.APpassword[i];
	 
	 }
	 CmdBuf[8+APSSIDlength+APPASSWORDlength]=0X00;   //校验和
	 
	 for(uint8_t i=1;i<(8+APSSIDlength+APPASSWORDlength);i++)
	 CmdBuf[8+APSSIDlength+APPASSWORDlength]+=CmdBuf[i];//把AP字符串赋值给config
	 CmdBuf[9+APSSIDlength+APPASSWORDlength]=0x7e;
	 CmdBufLength=10+APSSIDlength+APPASSWORDlength;
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 return 1;
}

uint8_t command_replytcpserve()  //返回IP地址
{
	 uint8_t TcpServer_IPlength=strlen(config.TcpServer_IP);
	 uint8_t TcpServer_Portlength=strlen(config.TcpServer_Port);
	 CmdBufLength=10+TcpServer_Portlength+TcpServer_IPlength;
	 CmdBuf[0]=0X7E;
	 CmdBuf[1]=0X54;   //T
	 CmdBuf[2]=CmdBufLength-6;
	 CmdBuf[3]=(CmdBufLength-6)>>8;   //L
	 CmdBuf[4]=0X00;   //V_IP
	 CmdBuf[5]=TcpServer_IPlength;
	{
		for(uint8_t i=0;i<TcpServer_IPlength;i++)
		CmdBuf[6+i]=config.TcpServer_IP[i];
	 
	 }
   CmdBuf[6+TcpServer_IPlength]=0X01;   //V_MASK
	 CmdBuf[7+TcpServer_IPlength]=TcpServer_Portlength;
	{
		for(uint8_t i=0;i<TcpServer_Portlength;i++)
		CmdBuf[8+TcpServer_IPlength+i]=config.TcpServer_Port[i];
	 
	 }
	 CmdBuf[8+TcpServer_IPlength+TcpServer_Portlength]=0X00;   //校验和
	 
	 for(uint8_t i=1;i<(8+TcpServer_IPlength+TcpServer_Portlength);i++)
	 CmdBuf[8+TcpServer_IPlength+TcpServer_Portlength]+=CmdBuf[i];//把AP字符串赋值给config
	 CmdBuf[9+TcpServer_IPlength+TcpServer_Portlength]=0x7e;
	 CmdBufLength=10+TcpServer_IPlength+TcpServer_Portlength;
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 return 1;
}


float interscale=0;

extern void emu_sprase_index();

uint8_t command_reply_scale()
{  
	unsigned char *floatCdata;
	uint16_t channelnum=Acceleration_ADCHS;  //这边限制通道个数
	CmdBuf[0]=0X7E;
	CmdBuf[1]=0X08;   
	CmdBuf[2]=channelnum*17;
	CmdBuf[3]=(channelnum*17)>>8;   //L
	for(uint8_t i=0;i<channelnum;i++)
	{
	CmdBuf[4+17*i]=i;   //V_IP

	CmdBuf[5+17*i]=config.channel_freq[i];
	CmdBuf[6+17*i]=config.channel_freq[i]>>8; 
	CmdBuf[7+17*i]=config.channel_freq[i]>>16; 
	CmdBuf[8+17*i]=config.channel_freq[i]>>24; 		 
	interscale=1.0f/config.floatscale[i];  

	floatCdata=(unsigned char *)&interscale;
	CmdBuf[9+17*i]=*floatCdata;
	CmdBuf[10+17*i]=*(floatCdata+1);;
	CmdBuf[11+17*i]=*(floatCdata+2);
	CmdBuf[12+17*i]=*(floatCdata+3);
	floatCdata=(unsigned char *)&config.alarmgate[i];
	CmdBuf[13+17*i]=*floatCdata;
	CmdBuf[14+17*i]=*(floatCdata+1);;
	CmdBuf[15+17*i]=*(floatCdata+2);
	CmdBuf[16+17*i]=*(floatCdata+3);
	floatCdata=(unsigned char *)&config.floatrange[i];
	CmdBuf[17+17*i]=*floatCdata;
	CmdBuf[18+17*i]=*(floatCdata+1);;
	CmdBuf[19+17*i]=*(floatCdata+2);
	CmdBuf[20+17*i]=*(floatCdata+3);

	}

	CmdBuf[channelnum*17+4]=0;
	for(uint16_t j=1;j<(channelnum*17+4);j++)
	{
	CmdBuf[channelnum*17+4]+=CmdBuf[j];
	}
	CmdBuf[channelnum*17+5]=0x7e;
	CmdBufLength=channelnum*17+6;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);

	emu_sprase_index();//更新采样的稀疏参数

   return 1;
}

uint8_t command_reply_alarm_value()
{  
	 unsigned char *floatCdata;
	 uint16_t channelnum=17;  //这边限制通道个数
	 CmdBuf[0]=0X7E;
	 CmdBuf[1]=COMMAND_REPLY_ALARMVALUE;   
	 CmdBuf[2]=channelnum*4;
	 CmdBuf[3]=(channelnum*4)>>8;   //L
	 for(uint8_t i=0;i<channelnum;i++)
	 {
		 floatCdata=(unsigned char *)&config.Alarm_value_junyue[i];
	   CmdBuf[4+4*i]=*floatCdata;
	   CmdBuf[5+4*i]=*(floatCdata+1);
	   CmdBuf[6+4*i]=*(floatCdata+2);
	   CmdBuf[7+4*i]=*(floatCdata+3);	 
	 }

	 CmdBuf[channelnum*4+4]=0;
	 for(uint16_t j=1;j<(channelnum*4+4);j++)
	 {
		CmdBuf[channelnum*4+4]+=CmdBuf[j];
	 }
	 CmdBuf[channelnum*4+5]=0x7e;
	 CmdBufLength=channelnum*4+6;
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 
   return 1;
}


float temp0[Temp_ADCHS];
uint8_t command_adjust_temp()
{ 
	 {for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
		CmdBufLength=CmdBufLength;

	 }
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);


	 saveConfig();
   
	 
	 return 1;
}

volatile uint16_t esp32_ready=0;
volatile uint16_t WifiConnectedFlag=0;
volatile uint16_t WifiDisconnectedFlag=0;

uint8_t command_esp32_statue(void)
{ 
	switch(RXDCmdBuf[4]){
		case 0x01:
			esp32_ready=1;
		break;
		case 0x02:
			WifiConnectedFlag=1;
		break;
		case 0x03:
			WifiDisconnectedFlag=1;
		break;
		default:
			break;

	}
	 return 1;
}

uint8_t command_set_battery(void)
{
	{
	for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	CmdBufLength=CmdBufLength;

	}
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);

	float battery = 0.0f;
	
	battery = *(float *) &CmdBuf[4];

	if(battery >= 0.1f && battery <= 100.0f)
	{
		config.battery = battery;
		saveConfig();
	}
	
	   
   return 1;
}

uint8_t command_adjust_adc()
{ 
	 {
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
		CmdBufLength=CmdBufLength;
	 }
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);

  //用的PT100
	 if(CmdBuf[4]==1){
	 config.floatscale[0]=1;
   config.floatscale[1]=1;
	 config.floatscale[2]=1;
	 config.floatadc[0]=1;
   config.floatadc[1]=1;
	 config.floatadc[2]=1;
	 return 1;
	 }
	 if(CmdBuf[5]==1){
	 config.floatadc[0]=100.0f/Parameter.EffectiveValue[0];	 		 
	 return 1;
	 }
	 if(CmdBuf[6]==1){
	 config.floatadc[1]=100.0f/Parameter.EffectiveValue[1];	 		 
	 return 1;
	 }
	 if(CmdBuf[7]==1){
	 config.floatadc[2]=100.0f/Parameter.EffectiveValue[2];	 		 

	 saveConfig();
   return 1;

	 }
 
	return 1;

}
	 
uint8_t command_reply_SampleParameter(void)   //设置采样参数
{


	{
		CmdBuf[0]=0x7e;
		CmdBuf[1]=COMMAND_REPLY_RATE;
		CmdBuf[2]=0x0B;
		CmdBuf[3]=0x00;
		CmdBuf[4]=config.ADfrequence; //两个，一个温度一个加速度
		
		CmdBuf[5]=config.ADfrequence>>8;
		CmdBuf[6]=config.ADfrequence>>16; //两个，一个温度一个加速度
		
		CmdBuf[7]=config.ADfrequence>>24;
		CmdBuf[8]=config.ADtime;
		CmdBuf[9]=config.PeriodTransimissonCounter;
		CmdBuf[10]=config.PeriodTransimissonCounter>>8;
		CmdBuf[11]=(uint8_t)config.PeriodTransimissonStatus;
//		CmdBuf[12]=(config.PeriodTransimissonCounter*config.workcycleseconds);
//		CmdBuf[13]=(config.PeriodTransimissonCounter*config.workcycleseconds)>>8;
		CmdBuf[12]=config.workcycleseconds;
		CmdBuf[13]=config.workcycleseconds>>8;
		CmdBuf[14]=(uint8_t)config.ParameterTransimissonStatus;
		CmdBuf[15]=0;
		for(uint16_t i=1;i<15;i++)
		CmdBuf[15]+=CmdBuf[i];
		 
    CmdBuf[16]=0x7e;
	  CmdBufLength=17;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }

	 return 1;
}

typedef union  
{  
    float fdata;  
    unsigned long ldata;  
}FloatLongType; 


uint8_t command_set_scale()
{ 
	{
	for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	CmdBufLength=CmdBufLength;

	}
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	uint8_t channelnum=(((uint32_t)CmdBuf[3]<<8)+CmdBuf[2])/17;
	for(uint16_t j=0;j<Acceleration_ADCHS;j++)//for(uint16_t j=0;j<channelnum;j++) //只对加速度进行设置
	{
		FloatLongType fl;  
		fl.ldata=0;  
		config.channel_freq[j]=(((uint32_t)CmdBuf[8+17*j]<<24)+((uint32_t)CmdBuf[7+17*j]<<16)
							+((uint32_t)CmdBuf[6+17*j]<<8)+CmdBuf[5+17*j]); 

		fl.ldata=0;  
		fl.ldata=CmdBuf[12+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[11+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[10+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[9+17*j];  
		config.floatscale[j]=1.0f/fl.fdata;  

		fl.ldata=0;  
		fl.ldata=CmdBuf[16+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[15+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[14+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[13+17*j];  

		config.alarmgate[j]=fl.fdata; 
		fl.ldata=0; 
		fl.ldata=CmdBuf[20+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[19+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[18+17*j];  
		fl.ldata=(fl.ldata<<8)|CmdBuf[17+17*j];  
		config.floatrange[j]=fl.fdata; 

		Parameter.ReciprocalofRange[j]=32768/config.floatrange[j];
		
		if(config.channel_freq[j] != 0x4000 && config.channel_freq[j] != 0x2000)
		{
			loadConfig();
			return 0;
		}

	}
	emu_sprase_index();//更新采样的稀疏参数
	saveConfig();
	   
   return 1;
}

uint8_t command_set_alarm_value()
{ 
	 {
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
		CmdBufLength=CmdBufLength;

	 }
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 uint8_t channelnum=(((uint32_t)CmdBuf[3]<<8)+CmdBuf[2])/4;
	 for(uint16_t j=0;j<channelnum;j++)//for(uint16_t j=0;j<channelnum;j++) //只对加速度进行设置
	 {
		FloatLongType fl;  
		fl.ldata=0;  
    fl.ldata=CmdBuf[7+4*j];  
    fl.ldata=(fl.ldata<<8)|CmdBuf[6+4*j];  
    fl.ldata=(fl.ldata<<8)|CmdBuf[5+4*j];  
    fl.ldata=(fl.ldata<<8)|CmdBuf[4+4*j];  
		config.Alarm_value_junyue[j]=fl.fdata;  
	 }
	 saveConfig();
   
   return 1;
}


uint8_t command_set_ap(void)   //设置TCP_SERVER的目标上位机地址
{
	{
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	  CmdBufLength=CmdBufLength;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
#ifdef ProgramESP32ByMyself
	 CmdBuf[1]=0xe5; //
	 for(uint32_t i=1;i<(CmdBufLength-2);i++)
	 CmdBuf[CmdBufLength-2]+=CmdBuf[i];
	 WriteDataToTXDBUF(CmdBuf,CmdBufLength);
#endif
	 for(uint8_t i=0;i<CmdBuf[5];i++)
	 config.APssid[i]=CmdBuf[6+i];//把AP字符串赋值给config
	 config.APssid[CmdBuf[5]]=0;
	 for(uint8_t i=0;i<CmdBuf[7+CmdBuf[5]];i++)
	 config.APpassword[i]=CmdBuf[8+CmdBuf[5]+i];//把AP字符串赋值给config
	 config.APpassword[CmdBuf[7+CmdBuf[5]]]=0;
	 return (1);
}

uint8_t command_reply_runmode(void)   //查询工作模式
{
	{
		CmdBuf[0]=0x7e;
		CmdBuf[1]=0x56;
		CmdBuf[2]=0x01;
		CmdBuf[3]=0x00;
		CmdBuf[4]=config.DataToBoardMode; //两个，一个温度一个加速度

		CmdBuf[5]=0;
		for(uint16_t i=1;i<5;i++)
		CmdBuf[5]+=CmdBuf[i];		 
		CmdBuf[6]=0x7e;
		CmdBufLength=7;
		WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	 return 1;
}

uint16_t command_receive_active_beacon(void)
{
	ReceiveActiveBeaconMessage();
	return 1;
}

uint8_t command_set_lowpower(void)
{
	{
	for(uint8_t i=0;i<CmdBufLength;i++)
	CmdBuf[i]=RXDCmdBuf[i];
	CmdBufLength=CmdBufLength;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	}
	
	config.scan_channel=CmdBuf[5];
	config.Waitforsleeptime=CmdBuf[6];
	config.WaitforIEPEtime=CmdBuf[7];
	//  config.workcycleseconds_inAlarmStatue=CmdBuf[8]+(uint16_t)(CmdBuf[9]<<8);
			
	if(CmdBuf[4] == config.Lowpower_Mode)
	{
		saveConfig();
	}
	else
	{
		config.Lowpower_Mode = CmdBuf[4];
		saveConfig();
		
		if(config.Lowpower_Mode == normalmode)
		{
		    software_reset();
		}
	}
	return 1;
}

uint8_t command_set_lowpower_value(void)
{
	{
	for(uint8_t i=0;i<CmdBufLength;i++)
	CmdBuf[i]=RXDCmdBuf[i];
	CmdBufLength=CmdBufLength;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
 }
  config.workcycleseconds_inAlarmStatue=CmdBuf[4]+(uint16_t)(CmdBuf[5]<<8);
  short tem_compensation=(short)CmdBuf[6]+(((short)CmdBuf[7])<<8);
  config.tempCompensation=(int16_t )tem_compensation;
	saveConfig();	
 return 1;
}

uint8_t command_reply_lowpower_value(void)
{
	{
		CmdBuf[0]=0x7e;
		CmdBuf[1]=COMMAND_REPLY_LOWPOWER_VALUE;
		CmdBuf[2]=0x08;  //8个字节长度
		CmdBuf[3]=0x00;
		CmdBuf[4]=config.workcycleseconds_inAlarmStatue;
		CmdBuf[5]=config.workcycleseconds_inAlarmStatue>>8;
		CmdBuf[6]=(short)config.tempCompensation;//config.Waitforsleeptime;
		CmdBuf[7]=(short)config.tempCompensation>>8;//config.WaitforIEPEtime;
		CmdBuf[8]=0;//config.workcycleseconds_inAlarmStatue;
		CmdBuf[9]=0;//config.workcycleseconds_inAlarmStatue>>8;
		CmdBuf[10]=0;
		CmdBuf[11]=0;
		CmdBuf[12]=0;
		for(uint16_t i=1;i<12;i++)
		CmdBuf[12]+=CmdBuf[i];
		 
    CmdBuf[13]=0x7e;
	  CmdBufLength=14;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	 return 1;
}

uint8_t command_reply_lowpower(void)
{

	{
		CmdBuf[0]=0x7e;
		CmdBuf[1]=COMMAND_REPLY_LOW_POWER;
		CmdBuf[2]=0x08;  //8个字节长度
		CmdBuf[3]=0x00;
		CmdBuf[4]=config.Lowpower_Mode; //两个，一个温度一个加速度	
		CmdBuf[5]=config.scan_channel;
		CmdBuf[6]=config.Waitforsleeptime;
		CmdBuf[7]=config.WaitforIEPEtime;
		CmdBuf[8]=0;//config.workcycleseconds_inAlarmStatue;
		CmdBuf[9]=0;//config.workcycleseconds_inAlarmStatue>>8;
		CmdBuf[10]=0;
		CmdBuf[11]=0;
		CmdBuf[12]=0;
		for(uint16_t i=1;i<12;i++)
		CmdBuf[12]+=CmdBuf[i];
		 
    CmdBuf[13]=0x7e;
	  CmdBufLength=14;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	 return 1;
}

uint8_t command_setrunmode(void)   //设置工作模式
{
	{
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	  CmdBufLength=CmdBufLength;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	 switch(CmdBuf[4]){
		 case 1:
			
		   //vTaskDelay (pdMS_TO_TICKS(10));
		   
			 config.DataToBoardMode=PARAMETERMODE;
		   EnableTempTransmission();
		  // TPM_StartSHAOTimer(TPM1, kTPM_SystemClock); //去低通效果，后期不能这么采，要直接减去一个值
		 break;
		 case 2:
		
			 config.DataToBoardMode=WAVEMODE;
		   EnableTempTransmission();
		   
		 break;
		 case 3:
			
			 config.DataToBoardMode=FFTWAVEMODE;
		   EnableTempTransmission();
		 break;
		 case 4:
			 
			 config.DataToBoardMode=FFTPARAMETERMODE;
		   EnableTempTransmission();
		 break;
		 case 5:
			 config.DataToBoardMode=IDLEMODE;
		   DisableTempTransmission();
		   
		 break;
		 case 6:
			 config.DataToBoardMode=LITEWAVEMODE;
		   DisableTempTransmission();
		  
		 break;
		 default:
		 break;
	 } 

 return 1;	
}  
uint8_t command_set_snnumber(void)   //设置TCP_SERVER的目标上位机地址
{
	{
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	  CmdBufLength=CmdBufLength;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }

	 config.SNnumber[0] = CmdBuf[4];
	 config.SNnumber[1] = CmdBuf[5];
	 config.SNnumber[2] = CmdBuf[6];
	 config.SNnumber[3] = CmdBuf[7];
	 config.SNnumber[4] = CmdBuf[8];
	 config.SNnumber[5] = CmdBuf[9];
	 config.SNnumber[6] = CmdBuf[10];
	 config.SNnumber[7] = CmdBuf[11];
	
	 saveConfig();
	 return (1);
}
uint8_t command_set_channel_condition(void)   //设置TCP_SERVER的目标上位机地址
{
	{
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	  CmdBufLength=CmdBufLength;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }

	 config.DataToSendChannel=RXDCmdBuf[4];//把AP字符串赋值给config
	 //saveConfig();
	 return (1);
}

extern void EarseFlash(uint32_t address,uint32_t length);

volatile uint32_t receive_iap_data_length=0;
volatile uint8_t receive_iap_pack_index=0;
uint8_t reply_shut_iap(void)
{
	CmdBuf[0]=0x7e;	
	CmdBuf[1]=COMMAND_IAP_STATUE;	
	CmdBuf[2]=0x05;	
	CmdBuf[3]=0x00;	
	CmdBuf[4]=0x04;	
	CmdBuf[5]=0x00;	
	CmdBuf[6]=0x00;	
	CmdBuf[7]=0x00;	
	CmdBuf[8]=0x00;	
	CmdBuf[9]=0x00;
	CmdBuf[9]=COMMAND_IAP_STATUE+0X09;	
	CmdBuf[10]=0x7e;
	CmdBufLength=11;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	return 1;
}

uint8_t reply_complete_iap(void)
{
	CmdBuf[0]=0x7e;	
	CmdBuf[1]=COMMAND_IAP_STATUE;	
	CmdBuf[2]=0x05;	
	CmdBuf[3]=0x00;	
	CmdBuf[4]=0x03;	
	CmdBuf[5]=0x00;	
	CmdBuf[6]=0x00;	
	CmdBuf[7]=0x00;	
	CmdBuf[8]=0x00;	
	CmdBuf[9]=0x00;	
	CmdBuf[9]=COMMAND_IAP_STATUE+0X08;	
	CmdBuf[10]=0x7e;
	CmdBufLength=11;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	return 1;
}

uint8_t reply_requie_contioue_iap(void)
{
	CmdBuf[0]=0x7e;	
	CmdBuf[1]=COMMAND_IAP_STATUE;	
	CmdBuf[2]=0x05;	
	CmdBuf[3]=0x00;	
	CmdBuf[4]=0x02;	
	CmdBuf[5]=0x00;	
	CmdBuf[6]=0x00;	
	CmdBuf[7]=0x00;	
	CmdBuf[8]=0x00;	
	CmdBuf[9]=0x00;	
	CmdBuf[9]=COMMAND_IAP_STATUE+0X07;	
	CmdBuf[10]=0x7e;
	CmdBufLength=11;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	return 1;
}

uint8_t reply_requie_contioue_iap_data(void)
{
	CmdBuf[0]=0x7e;	
	CmdBuf[1]=COMMAND_IAP_DATA;	
	CmdBuf[2]=0x01;	
	CmdBuf[3]=0x00;	
	CmdBuf[4]=0x01;	
	CmdBuf[5]=0x00;	
	CmdBuf[5]=COMMAND_IAP_DATA+0X02;	
	CmdBuf[6]=0x7e;
	CmdBufLength=7;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	return 1;
}

uint8_t reply_stop_iap_data(void)
{
	CmdBuf[0]=0x7e;	
	CmdBuf[1]=COMMAND_IAP_DATA;	
	CmdBuf[2]=0x01;	
	CmdBuf[3]=0x00;	
	CmdBuf[4]=0x00;	
	CmdBuf[5]=0x00;	
	CmdBuf[5]=COMMAND_IAP_DATA+0X01;	
	CmdBuf[6]=0x7e;
	CmdBufLength=7;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	return 1;
}

uint8_t command_reply_sw_version(void)
{
	char * version_ptr = 0;
	version_ptr = Version_Get_Str();
	CmdBuf[0]=0x7e;	
	CmdBuf[1]=COMMAND_REPLY_SW_VERSION;	
	CmdBuf[2]=0x10;
	CmdBuf[3]=0x00;
	for(uint32_t i=0;i<16;i++) //strcpy
	{
		//CmdBuf[4+i]=sw_version[i];
		CmdBuf[4+i]=version_ptr[i];
	}
	CmdBuf[20]=0;
	for(uint32_t i=1;i<20;i++) //strcpy
	{
		CmdBuf[20]+=CmdBuf[i];
	}
	CmdBuf[21]=0x7e;
	CmdBufLength=22;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	return 1;
}

volatile uint32_t erase_data_length=0;
uint8_t command_iap_statue(void)   //设置TCP_SERVER的目标上位机地址
{
	if(RXDCmdBuf[4]==0x01)
	{
	 
//	 stop_ad7606_samp();
		Parameter.IapDataLength=(uint32_t)RXDCmdBuf[5]+((uint32_t)RXDCmdBuf[6]<<8)+((uint32_t)RXDCmdBuf[7]<<16)+((uint32_t)RXDCmdBuf[8]<<24);
		if((Parameter.IapDataLength%64)!=0)
			erase_data_length=(Parameter.IapDataLength/64+1)*64;
		else 
		erase_data_length=Parameter.IapDataLength;
	EarseFlash(IAP_ADDRESS,erase_data_length);
	receive_iap_data_length=0;	
	receive_iap_pack_index=0;
	reply_requie_contioue_iap();
	}else if(RXDCmdBuf[4]==0x03)
	{
	receive_iap_data_length=0;	
	receive_iap_pack_index=0;
	
	reply_shut_iap();
	}
	
	 return (1);
}
extern void WRBYTES(uint32_t address,uint32_t num, uint8_t *Data);
void WRBYTES_iap_data(uint32_t address,uint32_t num, uint8_t *Data)
{
	WRBYTES(address, num, Data);
}
uint8_t rece_iap_data[0x1000];
uint8_t error_iap;
uint8_t command_iap_data(void)   //设置TCP_SERVER的目标上位机地址
{
	uint8_t write_32_Byte[64]={0};
	static uint32_t iap_ptr = 0;
	static uint16_t flash_page = 0;
	if(receive_iap_pack_index==RXDCmdBuf[7])
	{
		receive_iap_pack_index++;
		uint32_t current_iap_pack_length = RXDCmdBuf[2]+((uint16_t)RXDCmdBuf[3]<<8) - 4;
		if(Parameter.IapDataLength>=(receive_iap_data_length+current_iap_pack_length))
		{
		
			if(Parameter.IapDataLength==(receive_iap_data_length+current_iap_pack_length))
			{
				memcpy(&rece_iap_data[iap_ptr],(uint8_t *)RXDCmdBuf+8,current_iap_pack_length);
				iap_ptr += current_iap_pack_length;
				

				WRBYTES_iap_data(IAP_ADDRESS + flash_page * 0x1000 ,0x1000,(uint8_t *)rece_iap_data);
				flash_page ++;
				iap_ptr = 0;
				memset(rece_iap_data , 0 ,0x1000 );

			}
			else
			{

				memcpy(&rece_iap_data[iap_ptr],(uint8_t *)RXDCmdBuf+8,current_iap_pack_length);
				iap_ptr += current_iap_pack_length;
				
				if(iap_ptr >= 0x1000)
				{
					WRBYTES_iap_data(IAP_ADDRESS + flash_page * 0x1000 ,0x1000,(uint8_t *)rece_iap_data);
					flash_page ++;
					iap_ptr = 0;
					memset(rece_iap_data , 0 ,0x1000 );
				}
				
			}
			receive_iap_data_length+=current_iap_pack_length;
				if(Parameter.IapDataLength==receive_iap_data_length)
				{
					  flash_page = 0;
						reply_complete_iap();
					  config.Iap_datalength=erase_data_length;
					  config.Iap_flag=0x01;
					  saveConfig();
					  software_reset();
				}
				else 
					reply_requie_contioue_iap_data();
		}
		else
		{
			receive_iap_data_length=0;	
			receive_iap_pack_index=0;	
			reply_stop_iap_data();
		}
	}else
	{
	reply_stop_iap_data();	
	}
//	 saveConfig();
	 return (1);
}


uint16_t command_set_factory_parameter(void)
{
	return 1;
}

extern void BoardPeroidWave(void); 

uint8_t command_require_periodwave(void)
{ 
	config.RequirePeriodChannel=RXDCmdBuf[4];
	EnablePeroidWaveTransmission();
//	if(isAblePeroidWaveTransmission()&&(config.DataToBoardMode==PARAMETERMODE))
//		 {
//			 BoardPeroidWave();  //仅在特征值模式下生效
//		 }
	return 1;
}
uint8_t command_reply_channel_condition(void)   //返回通道字
{
	{
		CmdBuf[0]=0x7e;
		CmdBuf[1]=0x57;
		CmdBuf[2]=0x02;
		CmdBuf[3]=0x00;
		CmdBuf[4]=config.DataToSendChannel; //两个，一个温度一个加速度
		
		CmdBuf[5]=0x00;//config.DataToSendChannel>>8;
		CmdBuf[6]=0;
		for(uint16_t i=1;i<6;i++)
		CmdBuf[6]+=CmdBuf[i];
		 
    CmdBuf[7]=0x7e;
	  CmdBufLength=8;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	 return 1;
}





uint8_t command_set_SampleParameter(void)   //设置采样参数
{
	{
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	  CmdBufLength=CmdBufLength;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
 
	 
	 config.ADfrequence= (uint32_t)CmdBuf[4]+((uint32_t)CmdBuf[5]<<8)+((uint32_t)CmdBuf[6]<<16)+((uint32_t)CmdBuf[7]<<24);//设置采样速率
	 config.ADtime= 1;//CmdBuf[8]; //AD采样时间
	 //if(config.ADtime>2) config.ADtime=2;   //限制AD采样时间为2S
	 config.PeriodTransimissonCounter =(uint16_t)CmdBuf[9]+((uint16_t)CmdBuf[10]<<8);

	 config.PeriodTransimissonStatus=(bool)CmdBuf[11]; //是否使能自动发送
	 config.workcycleseconds=(uint16_t)CmdBuf[12]+((uint16_t)CmdBuf[13]<<8);	 
//	 uint32_t counter=(uint16_t)CmdBuf[12]+((uint16_t)CmdBuf[13]<<8);	  //是否使能自动发送
//	 config.PeriodTransimissonCounter=(uint32_t)(counter/config.workcycleseconds); //自动发送周期
	 config.ParameterTransimissonStatus=(bool)CmdBuf[14];
	 StopSample();
	 osDelay(10);
	 
	 
	// TPM_SetTimerPeriod(TPM1, ((CLOCK_GetFreq(kCLOCK_PllFllSelClk)/1)/(config.ADfrequence*ADCHS)-1));
//	 Parameter.ReciprocalofADfrequence=1.0f/((float)config.ADfrequence); //采样频率倒数;
//	 Parameter.ReciprocalofEMUnumber=1.0f/((float)config.ADfrequence*config.ADtime); //跟采样时间有关	 
	 saveConfig();
	 return (1);
}
uint8_t command_set_alarm_source(void)
{
	FloatLongType fff;
	uint8_t persource_byte_length=9;//1个源编号，一个通道所属编号，一个特征参数编号，一个逻辑关系编号，一个等式关系编号，一个float的阈值
	uint8_t source_number=(RXDCmdBuf[2]+((uint16_t)RXDCmdBuf[3]<<8))/9;
	uint8_t rule_index=RXDCmdBuf[4];
	if(RXDCmdBuf[5]==0) config.Alarm_source_number[rule_index]=0; //如果报警未使能，则该通道报警规则的报警源长度为0
		else 
			config.Alarm_source_number[rule_index]=source_number;
	for(uint8_t i=0;i<source_number;i++)
		{
			RXDCmdBuf[6+persource_byte_length*i]=RXDCmdBuf[6+persource_byte_length*i];
			config.Alarm_source[rule_index][i].channel_number=RXDCmdBuf[7+persource_byte_length*i];
			config.Alarm_source[rule_index][i].parameter_NAME=RXDCmdBuf[8+persource_byte_length*i];
			config.Alarm_source[rule_index][i].logical=RXDCmdBuf[9+persource_byte_length*i];
			config.Alarm_source[rule_index][i].relationship=RXDCmdBuf[10+persource_byte_length*i];
			fff.ldata=(uint64_t)RXDCmdBuf[11+persource_byte_length*i]|(uint64_t)(RXDCmdBuf[12+persource_byte_length*i]<<8)|(uint64_t)(RXDCmdBuf[13+persource_byte_length*i]<<16)|(uint64_t)(RXDCmdBuf[14+persource_byte_length*i]<<24);
			config.Alarm_source[rule_index][i].gatedata=fff.fdata;
		}
		saveConfig();
		return 1;
	    
}

void reply_rule_enablesoure(uint8_t rule_index)
{

	{
		FloatLongType ffl; 
		CmdBuf[0]=0x7e;
		CmdBuf[1]=COMMAND_REPLY_ALARM_SOURCE;
		uint16_t persource_byte_length=9;//
		uint16_t source_number=config.Alarm_source_number[rule_index];
		CmdBuf[2]=source_number*9+2;  //8个字节长度
		CmdBuf[3]=(source_number*9+2)>>8;
		CmdBuf[4]=rule_index; //规则号
		CmdBuf[5]=1;//该规则号使能
		for(uint16_t i=0;i<source_number;i++)
		{
		CmdBuf[6+persource_byte_length*i]=i;
		CmdBuf[7+persource_byte_length*i]=config.Alarm_source[rule_index][i].channel_number;
		CmdBuf[8+persource_byte_length*i]=config.Alarm_source[rule_index][i].parameter_NAME;
		CmdBuf[9+persource_byte_length*i]=config.Alarm_source[rule_index][i].logical;
		CmdBuf[10+persource_byte_length*i]=config.Alarm_source[rule_index][i].relationship;
		ffl.fdata=config.Alarm_source[rule_index][i].gatedata;	
		CmdBuf[11+persource_byte_length*i]=ffl.ldata;
		CmdBuf[12+persource_byte_length*i]=ffl.ldata>>8;
		CmdBuf[13+persource_byte_length*i]=ffl.ldata>>16;
		CmdBuf[14+persource_byte_length*i]=ffl.ldata>>24;
		}
		CmdBuf[6+persource_byte_length*source_number]=0;//校验和至0
		for(uint16_t i=1;i<(6+source_number*persource_byte_length);i++)
			CmdBuf[6+persource_byte_length*source_number]+=CmdBuf[i];
		 
    CmdBuf[7+source_number*persource_byte_length]=0x7e;
	  CmdBufLength=8+source_number*persource_byte_length;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	//return 1;
}

void reply_rule_disablesoure(uint8_t rule_index)
{

	{
		CmdBuf[0]=0x7e;
		CmdBuf[1]=COMMAND_REPLY_ALARM_SOURCE;
		CmdBuf[2]=0x02;  //8个字节长度
		CmdBuf[3]=0x00;
		CmdBuf[4]=rule_index; //两个，一个温度一个加速度	
		CmdBuf[5]=0;
		CmdBuf[6]=0;
		for(uint16_t i=1;i<6;i++)
		CmdBuf[6]+=CmdBuf[i];
		CmdBuf[7]=0x7e;
		CmdBufLength=8;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	//return 1;
}

void command_reply_alarm_source(void)
{
//	uint32_t sourece_statue;
//	command_reply_lowpower();
//	command_reply_lowpower();
	for(uint8_t i=0;i<10;i++)
	{
		 //command_reply_lowpower();
		if(config.Alarm_source_number[i]==0)
			 reply_rule_disablesoure(i);
	//	command_reply_lowpower();//reply_rule_disablesoure(i);
		else reply_rule_enablesoure(i);
		//command_reply_lowpower();
	}	    
}


uint8_t command_set_time(void)   //设置时间
{ 
	{
		for(uint8_t i=0;i<CmdBufLength;i++)
		CmdBuf[i]=RXDCmdBuf[i];
	  CmdBufLength=CmdBufLength;
    WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	 }
	RTC_WriteClock(((uint16_t)CmdBuf[5]<<8)+CmdBuf[4], CmdBuf[6], CmdBuf[7], CmdBuf[8], CmdBuf[9], CmdBuf[10]); 

	 
	 return (1);
}


uint8_t command_set_tcpserve(void)   //设置TCP_SERVER的目标上位机地址
{
	
	for(uint8_t i=0;i<CmdBufLength;i++)
	{
		CmdBuf[i]=RXDCmdBuf[i];
	}
		
	CmdBufLength=CmdBufLength;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	
	#ifdef ProgramESP32ByMyself
	CmdBuf[1]=0xe7; //
	for(uint32_t i=1;i<(CmdBufLength-2);i++)
	{
		CmdBuf[CmdBufLength-2]+=CmdBuf[i];
	}
	
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
	#endif 
	for(uint8_t i=0;i<CmdBuf[5];i++)
	{
		config.TcpServer_IP[i]=CmdBuf[6+i];//把AP字符串赋值给config
	}
	
	config.TcpServer_IP[CmdBuf[5]]=0;
	for(uint8_t i=0;i<CmdBuf[7+CmdBuf[5]];i++)
	{
		config.TcpServer_Port[i]=CmdBuf[8+CmdBuf[5]+i];//把AP字符串赋值给config
	}
	
	config.TcpServer_Port[CmdBuf[7+CmdBuf[5]]]=0;
	return (1);
}

void TransmitOverInLowPower(void)
{ 

	CmdBuf[0]=0x7e;
	CmdBuf[1]=COMMAND_TRANSMINT_OVER;
	CmdBuf[2]=0x00;
	CmdBuf[3]=0x00;
	CmdBuf[4]=COMMAND_TRANSMINT_OVER;
	CmdBuf[5]=0x7e;
	WriteDataToTXDBUF(CmdBuf,6);
	 

}
uint8_t command_applynetset()
{ 
	{
	for(uint8_t i=0;i<CmdBufLength;i++)
	CmdBuf[i]=RXDCmdBuf[i];
	CmdBufLength=CmdBufLength;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
 }
#ifdef ProgramESP32ByMyself
	CmdBuf[0]=0x7e;
	CmdBuf[1]=0xff;
	CmdBuf[2]=0x00;
	CmdBuf[3]=0x00;
	CmdBuf[4]=0xff;
	CmdBuf[5]=0x7e;
  CmdBufLength=6;
	WriteDataToTXDBUF(CmdBuf,CmdBufLength);
  saveConfig();
#else
 {
//	uint8_t JoinAPcount=0;
//  saveConfig(); //这边save 跟擦除有什么关系
//	Esp32_ExitUnvarnishSend(); //首先退出透传模式，对ESP32进行操作
//  
//	while(!Esp32_DisconnAP());
//  
//  Esp32_Net_Mode_Choose ( STA );
//  if(config.DHCP==EnableDHCP)
//	Esp32_setDHCP(ENABLE);
//	else
// 	Esp32_SetIP ( config.LocalIP,config.LocalGATEWAY,config.LocalMASK ); //设置IP
//  
//	//  //while ( (! Esp32_JoinAP ( macUser_Esp32_ApSsid, macUser_Esp32_ApPwd )) )
//  while ( (! Esp32_JoinAP ( config.APssid, config.APpassword ))&&(JoinAPcount<5) )
//	{JoinAPcount++;}	    //连接10次接不到就选择另一个
//	
//	if(JoinAPcount>4)
//	{
//		JoinAPcount=0;
//		vTaskDelay (pdMS_TO_TICKS(100));
//	  macEsp32_RST_HIGH_LEVEL();
//    vTaskDelay (pdMS_TO_TICKS(2500)); //rest后，保持2s的时间
//		MK27_USART_Config(115200L);
//		vTaskDelay (pdMS_TO_TICKS(100));
//		Esp32_SETUARTBaudrate (1000000L);
//		vTaskDelay (pdMS_TO_TICKS(100)); 
//		MK27_USART_Config(1000000L);
//		Esp32_Net_Mode_Choose ( STA );
//		if(config.DHCP==EnableDHCP)
//		Esp32_setDHCP(ENABLE);
//	  else
//		Esp32_SetIP (macUser_Esp32_LocalID,macUser_Esp32_LocalGATAWAY,macUser_Esp32_LocalMASK ); 
//		while ( (! Esp32_JoinAP ( macUser_Esp32_ApSsid, macUser_Esp32_ApPwd )) ){};
//	  Esp32_Enable_MultipleId ( DISABLE );

//	  while ( !	Esp32_Link_Server ( enumTCP, macUser_Esp32_TcpServer_IP, macUser_Esp32_TcpServer_Port , Single_ID_0 ) ){};
//    Set_Esp32_UnvarnishSend(); //设置进入透传模式
//		setFactoryLink();
//		return 1;
//	}
//	Esp32_Enable_MultipleId ( DISABLE );
//	
//	while ( !	Esp32_Link_Server ( enumTCP, config.TcpServer_IP, config.TcpServer_Port, Single_ID_0 ) ){};
//  Set_Esp32_UnvarnishSend(); //设置进入透传模式
//	clrFactoryLink(); //不是工厂连接	
	}
#endif
	return 1;
	
}

uint8_t command_reset_system(void)
{
	 software_reset();
	return 1;
}
int32_t lastcounter=0;
int32_t errorcounter=0;
int32_t rightcounter=0;
uint8_t command_counter(void)
{ 
	 rightcounter++;
	if((lastcounter+1)%128==RXDCmdBuf[4]){}
	else {
		errorcounter++;
	}
  lastcounter=RXDCmdBuf[4];
 return 1;	
}

uint8_t command_server_accept_data(void)
{


//	CmdBuf[0]=0x7e;
//	CmdBuf[1]=COMMAND_RECEIVE_TRANSMINT_OVER;
//	CmdBuf[2]=0x00;
//	CmdBuf[3]=0x00;
//	CmdBuf[4]=COMMAND_RECEIVE_TRANSMINT_OVER;
//	CmdBuf[5]=0x7e;
//	WriteDataToTXDBUF(CmdBuf,6);
	 

	
  if(Parameter.wakeupsourec==VLLS)
	{
		StopSample();
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);    //外部中断使能
		HAL_NVIC_DisableIRQ(TIM4_IRQn);            //TPM中断使能	//
		
		osThreadSuspend(defaultTaskHandle);
		osThreadSuspend(Esp32ProcessHandle);   //挂起所有线程
		osThreadSuspend(DateProcessHandle);
		osThreadSuspend(DateEmuHandle);
		osThreadSuspend(RxdBufProcessHandle);
	}
	return 1;
}
uint8_t AnalyCmd(uint16_t length)
{ 
//	uint8_t sum=0;
//	if(RxdBuf[0]!=0x7e)
//		return 0;              //起始位不对，pass
//  if((((uint32_t)RxdBuf[3]<<8)+RxdBuf[2])!=length-6)
//		return 0;              //长度位不对，pass
//	for(uint16_t i=1;i<length-2;i++)
//	  sum+=RxdBuf[i];
//	if(sum!=RxdBuf[length-2])
//		return 0;              //求和位不对，pass
	switch( RXDCmdBuf[1] ){     //看命令行
		case COMMAND_ID:     //0x02
			   command_id();
			break;
		case COMMAND_CHANNELKIND:     //0x03 通道类型设置
			   command_channelkind();
			break;
		case COMMAND_REPLYIP:     //0x03 通道类型设置
			   command_replyip();
			break;
		case COMMAND_STOP:
			   command_stop();
			break;
		case COMMAND_RECEIVE_BEACON:
				command_receive_beacon();
			break;
		case COMMAND_SETIP:   //设置IP地址
			   command_setip();
			break;
		case COMMAND_SET_RUNMODE:   //设置IP地址
			   command_setrunmode();
			break;
		case COMMAND_START:
			   command_start();
			break;
		case COMMAND_REQUIRE_PERIODWAVE:
			   command_require_periodwave();
			break;
		case COMMAND_SET_AP:
			   command_set_ap();
			break;
		case COMMAND_RECEIVE_ACTIVE_BEACON:
					command_receive_active_beacon();
			break;
		case COMMAND_SET_FACTORY_PARAMETER:
					command_set_factory_parameter();
			break;
		case COMMAND_SET_CHANNEL_CONDITION:
			   command_set_channel_condition();
			break;
		case COMMAND_REPLY_CHANNEL_CONDITION:
			   command_reply_channel_condition();
			break;
		case COMMAND_REPLY_RATE:    //设置采样参数
			   command_reply_SampleParameter();
		  break;
		case COMMAND_SAMPLE_RATE:    //设置采样参数
			   command_set_SampleParameter();
			break;
		case COMMAND_SET_TCPSERVE:   //设置tcp server的目标地址类似
			   command_set_tcpserve();
		  break;
		case COMMAND_REPLYTCPSERVE:   //返回tcp server的目标地址类似
			   command_replytcpserve();
			break; 
		case COMMAND_SET_TIME:           //返回ap值
			   command_set_time();
      break;		
		case COMMAND_REPLYAP:
			   command_replyap();  //返回ap值
			break;
    case COMMAND_APPLYNETSET:
			   command_applynetset();  //应用网络设置
			break;
		 case 0x40:
			   command_counter();  //应用网络设置
			break;
		 case COMMAND_SET_ALARM_SOURCE:
			   command_set_alarm_source();
			break;
		case COMMAND_REPLY_ALARM_SOURCE:
//			   command_reply_alarm_source();
			break;
		case COMMAND_SET_LOW_POWER:
			   command_set_lowpower();
			break;
		case COMMAND_REPLY_LOW_POWER:
		     command_reply_lowpower();
			break;
		case COMMAND_SET_LOWPOWER_VALUE:
			   command_set_lowpower_value();
			break;
		case COMMAND_REPLY_LOWPOWER_VALUE:
		     command_reply_lowpower_value();
			break;
		case COMMAND_REPLY_RUNMODE:
			   command_reply_runmode();
		  break;
		case COMMAND_ADJUST_TEMP:
			   command_adjust_temp();
		  break;
		case COMMAND_ADJUST_ADC:
			   command_adjust_adc();
		  break;
		case COMMAND_SET_SCALE:
			   command_set_scale();
			break;
		case COMMAND_REPLY_SCALE:
			   command_reply_scale();
			break;
    case COMMAND_SET_SNNUMBER:
			   command_set_snnumber();
			break;
		case COMMAND_IAP_STATUE:
			   command_iap_statue();
			break;
		case COMMAND_REPLY_SW_VERSION:
			   command_reply_sw_version();
			break;
		case COMMAND_IAP_DATA:
			   command_iap_data();
			break;
		case COMMAND_RESET_SYSTEM:
			   command_reset_system();
			break;
		case COMMAND_TRANSMINT_OVER:
			   command_server_accept_data();
			break;
		case COMMAND_SET_ALARMVALUE:
			   command_set_alarm_value();
			break;
		case COMMAND_REPLY_ALARMVALUE:
			   command_reply_alarm_value();
			break;
		case COMMAND_ESP32_STATUE:
			   command_esp32_statue();
			break;
		case COMMAND_SET_BATTERY:
				command_set_battery();
			break;
		default:
			return 1;
	}
 return 1;
	
}






uint16_t getTelLength(void)  //求的是TLV中V的长度
{
					
	return(((uint16_t)RxdTelBuf[3]<<8)+RxdTelBuf[2]);
  
}
uint8_t  isVaildTel(void)
{
	if(RxdBytes>=1)if(RxdTelBuf[0]!=0x7e)return(0);
//	if(RxdBytes>=2)if((RxdTelBuf[1]!=COMMAND_START)&&(RxdTelBuf[1]!=COMMAND_STOP)&&(RxdTelBuf[1]!=COMMAND_ID)
//		                  &&(RxdTelBuf[1]!=COMMAND_CHANNELKIND)&&(RxdTelBuf[1]!=COMMAND_REPLYIP)&&(RxdTelBuf[1]!=COMMAND_SETIP)
//						  &&(RxdTelBuf[1]!=COMMAND_REPLY_RATE)&&(RxdTelBuf[1]!=COMMAND_SAMPLE_RATE)&&(RxdTelBuf[1]!=COMMAND_ADJUST_TEMP)
//						  &&(RxdTelBuf[1]!=COMMAND_REPLY_SCALE)&&(RxdTelBuf[1]!=COMMAND_SET_SCALE)&&(RxdTelBuf[1]!=COMMAND_ADJUST_ADC)
//						  &&(RxdTelBuf[1]!=COMMAND_SET_SNNUMBER)&&(RxdTelBuf[1]!=COMMAND_REQUIRE_PERIODWAVE)&&(RxdTelBuf[1]!=COMMAND_SET_CHANNEL_CONDITION)
//							&&(RxdTelBuf[1]!=COMMAND_SET_RUNMODE)&&(RxdTelBuf[1]!=COMMAND_SET_TIME)&&(RxdTelBuf[1]!=COMMAND_SET_AP)
//						  &&(RxdTelBuf[1]!=COMMAND_RECEIVE_BEACON)&&(RxdTelBuf[1]!=COMMAND_SET_TCPSERVE)&&(RxdTelBuf[1]!=COMMAND_REPLYAP)
//						  &&(RxdTelBuf[1]!=COMMAND_REPLYTCPSERVE)&&(RxdTelBuf[1]!=COMMAND_APPLYNETSET)&&(RxdTelBuf[1]!=COMMAND_REPLY_RUNMODE)
//							&&(RxdTelBuf[1]!=COMMAND_REPLY_CHANNEL_CONDITION)&&(RxdTelBuf[1]!=COMMAND_RECEIVE_ACTIVE_BEACON)&&(RxdTelBuf[1]!=COMMAND_SET_FACTORY_PARAMETER)
//							&&(RxdTelBuf[1]!=COMMAND_IAP_DATA)&&(RxdTelBuf[1]!=COMMAND_IAP_STATUE)&&(RxdTelBuf[1]!=COMMAND_REPLY_SW_VERSION)&&(RxdTelBuf[1]!=COMMAND_RESET_SYSTEM))
//				return(0);  //命令行最大ID 后期版本不做判定
	
  if(RxdBytes>=4) { 
		uint16_t length=getTelLength();
		if((length>1000)) return(0);  //限制最大的为1000
	}
	return(1);				 // 合法的
}
uint8_t sumofRxdBuf(uint16_t l)  //求和的报文，不包含起始标识,	l	的长度包括起始标识
{ 
	uint8_t sum=0;
	if(l<2) return (0);
	for(uint16_t i=1;i<l-2;i++)
	 sum=sum+RxdTelBuf[i];
	return (sum);
}
uint8_t isTelComplete(void)	   // =0 不完整  =1 sum Error =2 正确
{
	uint32_t  temp8;
	uint32_t   dat_len;

	if(RxdBytes<4)return(0);
  ////////////////
	dat_len=getTelLength()+6;	//
	if(dat_len==0)return(0);
	if(RxdBytes<(dat_len))return(0);

	temp8=sumofRxdBuf(dat_len);
 
  if (RxdTelBuf[dat_len-1]==0x7e)
		return(2); 
	if (RxdTelBuf[dat_len-2]==temp8)
		return(2); 
	else{
		return(1);
	}	
}						 

uint8_t leftRxdTel(void)		//数组左移一位
{
	uint32_t i;
	if(RxdBytes<1)return(0);     // 无法左移
	for	(i=1;i<RxdBytes;i++)
	{
		RxdTelBuf[i-1]=RxdTelBuf[i];		
	}
	RxdBytes--;
	return(1);					 // 丢弃一个字节成功

}

 void RxdByte(uint8_t c)
{	
	uint32_t 	i;
	RxdTelBuf[RxdBytes]=c;
	RxdBytes++;

	switch(RxdBytes)
	{
		case 0:	break;
		case 3:	break;
		case 1:
		case 2:
		case 4:while(!isVaildTel())	//如果不合法			 
				{
					if(!leftRxdTel())break;	  // 丢弃首字节
				}
				break;
			
		
		default:		
				i=isTelComplete();
				if(i==2)
				{
					//do some thing
					for(uint16_t j=0;j<RxdBytes;j++)
					RXDCmdBuf[j]=RxdTelBuf[j];
					CmdBufLength=RxdBytes;
					AnalyCmd(CmdBufLength);
					
					RxdBytes=0;
					
				}
				else if(i==1)	 // CRC error
				{
					leftRxdTel();
					while(!isVaildTel())	//如果不合法			 
					{
						if(!leftRxdTel())break;
					}	
				}
				else if(i==0) //没收完继续收
				{
				
				}
				else
				{
				}
				break;
			
		}
	
}

uint8_t ReadRxdBuf(void)
{ 
	uint8_t  c;
	c=RxdBuf[RxdBufTailIndex];
	IncreaseRxdBufNum(RxdBufTailIndex);
	return (c);
}

void RxdBufProcessFunction(void *argument)
{ 
	
	while(1)
	{		
		if(!isRxdBufEmpty()) //如果接受到报文，且在透传模式
		{
			RxdByte(ReadRxdBuf());		
		}
		else 
			osDelay(1);		
		}
}
