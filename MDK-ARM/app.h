#ifndef _APP_H_
#define _APP_H_
/* FreeRTOS kernel includes. */
#include "main.h"
#include "cmsis_os.h"
#include "bsp.h"



/*********************一体式的老版的打开一个Integrated_Sensor宏就可以，新版本的要加上New_Integrated_Sensor*/
/**************************分体的打开分体式的****************/
#define ProgramESP32ByMyself

//#define ADCHS 0x02  //这个通道是内部adc的通道树，写的太乱了，有时间要推翻
//#define Acceleration_ADCHS 0x03  //一个加速度

//#define Temp_ADCHS 0x02  //两个温度
//#define PT1000

#define SAMPLE_ADCH 8
#define AD7682_ADCHS 0x04  //一个加速度
#define Acceleration_ADCHS 0x03  //一个加速度
#define Temp_ADCHS 1
#define SAMPLEPOINTS 128
#define BOARDPOINTS SAMPLEPOINTS*AD7682_ADCHS*sizeof(int16_t)
#define PERIODBOARDPOINTS SAMPLEPOINTS*sizeof(int16_t)*2 //(4*ADCHS*SAMPLEPOINTS)
//#define AD7606_ADCHS 4
//#define AD7606SAMPLEPOINTS 128
//#define AD7606_SAMPLE_COUNT AD7606_ADCHS*2*AD7606SAMPLEPOINTS

 //(4*ADCHS*SAMPLEPOINTS)

//#define ADC16_ByDMA_SAMPLE_COUNT (2*ADCHS*SAMPLEPOINTS) /* The ADC16 sample count. */
#define IdleRxdBufSize 1024  //串口空闲中断用来接受数据缓存区长度

#define RX_BUFFER_SIZE 1600
#define TX_BUFFER_SIZE 1600

extern volatile uint8_t HeadIndex;
extern volatile uint8_t TailIndex;
#define TxdBufLine  64
#define Increase(x)   {x=(x+1)%TxdBufLine;}
#define isTxdBufFull() ((HeadIndex+1)%TxdBufLine==TailIndex)
#define isTxdBufEmpty() (HeadIndex==TailIndex)

extern SPI_HandleTypeDef SpiHandle;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim7;
//#define MaxStoreSeconds 2
//#define LiteWaveBufline  16384*Acceleration_ADCHS*MaxStoreSeconds
//#define IncreaseLiteWave(x)   {x=(x+1)%LiteWaveBufline;}
//#define isLiteWaveBufFull() ((LiteWaveHeadIndex+1)%LiteWaveBufline==LiteWaveTailIndex)
//#define isLiteWaveBufEmpty() (LiteWaveHeadIndex==LiteWaveTailIndex)

//extern float LiteWave[16384*Acceleration_ADCHS*MaxStoreSeconds];
//extern volatile uint32_t LiteWaveHeadIndex;
//extern volatile uint32_t LiteWaveTailIndex;
//extern volatile uint32_t LiteWaveNum;

//extern uint8_t TXDBUF[TxdBufLine][BOARDPOINTS+20];
//extern volatile uint8_t HeadIndex;
//extern volatile uint8_t TailIndex;
//extern uint16_t TXDBUFLength[TxdBufLine];
//extern void DelayMS ( uint16_t t);
//extern volatile uint16_t RxdBufHeadIndex;
//extern volatile uint16_t RxdBufTailIndex;


//extern uint8_t CmdBuf[TX_BUFFER_SIZE];
//extern uint16_t CmdBufLength;


#define EMUPONITS  16390*2  //最大取2S的数据

extern volatile uint16_t RxdBufHeadIndex;
extern volatile uint16_t RxdBufTailIndex;
#define RxdBufLine 1024
#define IncreaseRxdBufNum(x)   {x=(x+1)%RxdBufLine;}
#define isRxdBufFull() ((RxdBufHeadIndex+1)%RxdBufLine==RxdBufTailIndex)
#define isRxdBufEmpty() (RxdBufHeadIndex==RxdBufTailIndex)

//#define IncreaseRxdBufNum(x)   {x=(x+1)%RxdBufLine;}
//#define isRxdBufFull() ((RxdBufHeadIndex+1)%RxdBufLine==RxdBufTailIndex)
//#define isRxdBufEmpty() (RxdBufHeadIndex==RxdBufTailIndex)
//命令行
#define COMMAND_STOP 0x00
#define COMMAND_START 0x01
#define COMMAND_ID 0x02
#define COMMAND_CHANNELKIND 0x03
#define COMMAND_REPLYIP 0x04
#define COMMAND_SETIP 0x05  //设置IP地址
#define COMMAND_REPLY_RATE 0x06   //设置采样参数
#define COMMAND_SAMPLE_RATE 0x07   //设置采样参数
#define COMMAND_REPLY_SCALE 0x08
#define COMMAND_SET_SCALE 0x09
#define COMMAND_ADJUST_TEMP 0x69
#define COMMAND_ADJUST_ADC 0x68
#define COMMAND_SET_SNNUMBER 0x88


#define COMMAND_RECEIVE_BEACON 0x11
#define COMMAND_RECEIVE_ACTIVE_BEACON 0x12
#define COMMAND_SET_FACTORY_PARAMETER 0x13

#define COMMAND_BoardAlarm_Statue 0x14
#define COMMAND_SET_ALARM_SOURCE 0x15
#define COMMAND_REPLY_ALARM_SOURCE 0x16
#define COMMAND_SET_LOW_POWER 0x17
#define COMMAND_TRANSMINT_OVER 0x18
#define COMMAND_REPLY_LOW_POWER 0x19
#define COMMAND_RECEIVE_TRANSMINT_OVER 0x23

#define COMMAND_REQUIRE_PERIODWAVE 0x30 //请求一次单次波形
#define COMMAND_SET_CHANNEL_CONDITION 0x0a  //设置通道字
#define COMMAND_SET_RUNMODE 0x0B  //工作模式，特征值模式，还是波形模式，或者报警模式啥的
#define COMMAND_SET_TIME 0x50  //设置时间
#define COMMAND_SET_AP 0x51  //设置AP地址
#define COMMAND_SET_TCPSERVE 0x52  //设置server端的地址
#define COMMAND_REPLYAP 0x53
#define COMMAND_REPLYTCPSERVE 0x54
#define COMMAND_APPLYNETSET 0x55
#define COMMAND_REPLY_RUNMODE 0x56  //工作模式，特征值模式，还是波形模式，或者报警模式啥的
#define COMMAND_REPLY_CHANNEL_CONDITION 0x57  //设置通道字

#define COMMAND_SET_ALARMVALUE 0x72  //设置报警值 君悦用
#define COMMAND_REPLY_ALARMVALUE 0x73  //设置报警值 君悦用
#define COMMAND_BOARD_PARAMETERVALUE_JUNYUE 0x74  //返回特征值 君悦用
#define COMMAND_SET_LOWPOWER_VALUE 0x75  //设置低功耗的设置值 君悦用
#define COMMAND_REPLY_LOWPOWER_VALUE 0x76  //返回低功耗的设置 君悦用

#define COMMAND_REPLY_SW_VERSION 0x80  //版本号
#define COMMAND_SET_SW_VERSION 0x81  //版本号
#define COMMAND_IAP_DATA 0x82  //IAP数据包
#define COMMAND_IAP_STATUE 0x83  //IAP状态包
#define COMMAND_RESET_SYSTEM 0x84  //重启软件

#define COMMAND_ESP32_STATUE 0xEA  //重启软件


#define IAP_ADDRESS 0x00100000


extern osThreadId_t defaultTaskHandle;
extern osThreadId_t DateProcessHandle;
extern osThreadId_t DateEmuHandle;
extern osThreadId_t RxdBufProcessHandle;
extern osThreadId_t Esp32ProcessHandle;

typedef enum Equality_relation{Greater=0,Equal=1,Less=2} Equality_relation;
typedef enum Logical_relation{AND=0,OR} Logical_relation;
typedef enum Parameter_name{daois=0,sdaOR} Parameter_name;
typedef struct  RULE
{
 // uint8_t Alarm_source_channel;//该报警源所属通道号
	bool 	End_statue;  //是否为该通道的最后一条报警源
	uint8_t channel_number;
	Parameter_name parameter_NAME;
 	Logical_relation logical; //报警源逻辑关系
	Equality_relation relationship; //报警源与阈值等式关系
	float gatedata;
}RULE;

typedef struct  CONFIG				 // 配置信息
{
	uint16_t vaildsign;

	uint8_t baundrate;    /* =0:600    =1:1200 		=2:2400 		=3:4800 		=4:9600 */
	uint8_t addr; 
	uint64_t SNnumber; 
	uint8_t parity;		// =0 : n,8,1   =1: o,8,1  =2: e,8,1	 数据格式
	float floatscale[12];
	//float floatrange[12];
	uint8_t DisplayMode;  // 显示模式　=0　固定　=1 循环
	uint8_t interface_type[12]; // 输入类型
	uint8_t unit[12];  // 单位
	float floatrange[12]; // 转换系数
	float floatadjust[12]; // 修正值
	//uint16_t interface_addr[12]; // modbus 地址 上传
	float alarmgate[6]; // 绝对值
	float floatadc[6]; // 绝对值
	uint8_t means	;// 均值类型
	uint16_t means_times; // 均值积算周期
	uint16_t freq;  // 采样频率 Hz
	uint16_t avr_count;
	uint8_t reflash; // 刷新时间 
	uint16_t din_addr;  //  开关量输入寄存器地址
	uint16_t dout_addr; //  开关量输出寄存器地址
	uint32_t force_gate,force_backlash;  // 应变启动阈值， 回差。
	uint16_t max_addr0,max_addr1;        //
	uint16_t vlpsseconds;            //  
	uint16_t vlprseconds;           //
	uint16_t runseconds;           //
	uint16_t pga;                 //
	uint16_t workcycleseconds;   //  工作周期
	uint16_t fangda;            //  放大倍数
	uint16_t boardset;         // 发射功率设置
	uint16_t ADtime;          //AD单次采样时间
	uint16_t ADfrequence;    //AD采样频率
	  
	uint64_t alarmminutetime;  //开始报警时间
	uint32_t FLASH_WRADDR;    //flash起始地址
	uint8_t  DataToBoardMode; 
	uint16_t  DataToSendChannel;
	uint8_t  DHCP;
	char APssid[20];
	char APpassword[20];
	char TcpServer_IP[20];
	char TcpServer_Port[10];
	char LocalIP[20];
	char LocalGATEWAY[20];
	char LocalMASK[20];
	uint32_t PeriodTransimissonCounter;
	uint8_t PeriodTransimissonStatus;
	uint8_t ParameterTransimissonStatus;
	uint8_t RequirePeriodChannel; //请求哪些单次波形的通道
	uint8_t RESETTIME;
	uint8_t Enable_active_beacon;
	uint8_t Iap_flag;
	uint32_t Iap_datalength;
	uint32_t channel_freq[12];
	uint32_t BeaconInterval;
	char server_address[20];
	char DNS_SERVERIP[20];
	
	uint8_t Lowpower_Mode;
	uint8_t scan_channel;
	uint8_t Waitforsleeptime;
	uint8_t WaitforIEPEtime;
	float  filterMean[6];
	uint32_t WaitToPeriodTransmissonCounter;
	float Alarm_value_junyue[20];//给君悦的20个报警值
	float battery;//给君悦的20个报警值
	uint16_t workcycleseconds_inAlarmStatue;
	int16_t tempCompensation; 
	uint8_t Alarm_source_number[10];//10个规则，每个规则的报警源数目，为0则不报警。
	RULE Alarm_source[10][20]; //二维数组，最多10个规则，每个规则最多20个报警源

}CONFIG;

extern  struct CONFIG  config;  //配置信息


//extern float AlarmData[EMUPONITS];

typedef enum {No_OS_Smapling = 0x000, OS_2_Smapling, OS_4_Smapling, OS_8_Smapling, OS_16_Smapling, OS_32_Smapling, OS_64_Smapling, }  Average_level;


typedef enum {BrainTransmission=0,NoBrainTransmission=1,} Esp32TransmissionMode;
typedef enum {EnableDHCP=1,DisableDHCP=0,} DHCPmode;
typedef enum {PARAMETERMODE=1,WAVEMODE=2,FFTWAVEMODE=3,FFTPARAMETERMODE=4,IDLEMODE=5,LITEWAVEMODE=6} DataToBoard_TYPE;
/* TYPE_HBR 全桥 */
typedef enum {TYPE_NONE=0,TYPE_MA=1,TYPE_V=2, TYPE_IEPE=3,TYPE_PT=4,TYPE_HBR=5,TYPE_AC_V=6,} INTERFACE_TYPE;
typedef enum {UNIT_NONE=0,UNIT_V=1,UNIT_A=2,UNIT_KV=3,UNIT_TEMP=4,UNIT_M=5,UNIT_M_S=6,UNIT_M_S2=7,UNIT_G=8,UNIT_MA=9,UNIT_DB=10,UNIT_MM_S=11,} INTERFACE_UNIT;
typedef enum {MEANS_NONE=0,
							MEANS_MEANS, // 均值
							MEANS_RMS, // 均方根值
							MEANS_P,  // 峰值
							MEANS_PP, // 峰峰值
} MEANS_TYPE;  
typedef enum {BAUND2400=0,BAUND4800,BAUND9600,BAUND19200,BAUND38400,} BAUND_RATE;
//typedef enum {BAM4E33=0,BAM4E31=1, BAM4I33=2,BAM4I31=3,BAM4U33=4,BAM4U31=5,BAM4H33=6,BAM4H31=7,BAM4P33=8,BAM4P31=9,BAM4Q33=10,BAM4Q31=11}MODEL ;

typedef  struct PARAMETER				 // 所有参数
{
	uint16_t vaildsign;
	int32_t int_v[12];
	int32_t int_av[12];
	int64_t s[12]; // 累积
	int64_t as[12]; // 累积
	uint32_t sparse_index[12];
	float v[12];
	float fv[12];
	float vs[12];  //速度累加和，浮点型
	float av[12];
	float adate;                //加速度
	float vdate;                 //速度
	float xdate;                  //位移
	float pdate;                   //温度
	float scale_v[12];
	int32_t gate[12];
	int32_t backlash[12];
	uint8_t alarm[12]; // 报警标志
	uint16_t din;  // 低8位对应 8个通道输入
	uint16_t dout; // 低4位对应 4个输出通道 
	uint32_t status;
	int32_t ch1_int_max,ch2_int_max;
	int32_t ch1_int_max1,ch2_int_max2;
	float ch1_max,ch2_max;
	int32_t vibrate_gate1,vibrate_gate2,vibrate_backlash1,vibrate_backlash2;
	int32_t force_gate,force_backlash;
	int16_t selfpgaallow;
	uint16_t daytime;
	uint16_t minutetime;
	uint16_t selffangda;
  uint16_t alarmminutetime;
	uint8_t Esp32TransmissionMode;
	uint32_t sleeptime;
	uint8_t wakeupsourec;
	float Abs_average[12];
	float ReciprocalofADfrequence[16];
	float Inter_Skew[12]; //歪度
	float InterIIMarginIndex[12];//裕度指标中间变量
	float InterMAX[12];
	float InterMIN[12];
	float S_sum[12];
	float SS_sum[12];
	float SSS_sum[12];
	float SSSS_sum[12];
	float Abs_S_average[12];
	float PeakValue[12];  //峰峰值
	float EffectiveValue[12]; //有效值
	float Skew[12]; //歪度
	float MaxValue[12]; //峰值
	float Kurtosis[12]; //峭度
	float Mean[12]; //均值
	float WaveformIndex[12]; //波形指标
	float PeakIndex[12];//峰值指标
	float PulseIndex[12];//脉冲指标
	float MarginIndex[12];//裕度指标
	float KurtosisIndex[12];//峭度指标
	float Inter_MarginIndex[12]; //峭度
	float S_average[12]; //平均值总值
	float average[12]; //平均值
	float ReciprocalofEMUnumber[16]; //采样频率的倒数，采样周期
	float ReciprocalofRange[16];//量程的倒数
	float Arms[12];
	float Vrms[12];
	float Drms[12];
	float Envelop[12];
	//
	float F_sum[12];     //s(k)之和
	float FS_sum[12];   //f(k)s(k)的和
	float FFS_sum[12];  //F(K)F(K)S(K)
	float FFFFS_sum[12];  //F(K)F(K)S(K)
	float F2_sum[12];     //feature2的中间和
	float F3_sum[12];     //feature3的中间和
	float F4_sum[12];     //feature4的中间和
	float F6_sum[12];     //feature6的中间和
	float F11_sum[12];     //feature11的中间和
	float F12_sum[12];     //feature12的中间和
	float F13_sum[12];     //feature12的中间和
	float F_feature1[12];
	float F_feature2[12];
	float F_feature3[12];
	float F_feature4[12];
	float F_feature5[12];
	float F_feature6[12];
	float F_feature7[12];
	float F_feature8[12];
	float F_feature9[12];
	float F_feature10[12];
	float F_feature11[12];
	float F_feature12[12];
	float F_feature13[12];
	uint32_t IapDataLength;
	uint32_t PeroidWaveTransmissionCounter;
	uint32_t AutoPeriodTransmissonCounter;
	uint64_t DeviceKey;
}PARAMETER;

typedef enum {PRE_TX=0,IN_TX=1,} TXcondition;
typedef enum {MANUALRESET = 0, VLLS = !DISABLE} WakeupSourec;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
typedef enum {normalmode = 0,lowpowermode = 1 } RUNMODE;
				
struct ADJUSTDATA					 // 校准数据
{	uint16_t vaildsign;
	uint32_t  UaSub,UbSub,UcSub;
	uint32_t IaSub,IbSub,IcSub;
	int32_t PhaseA,PhaseB,PhaseC;
	uint32_t  CVSub,OVSub;
};

extern  struct PARAMETER Parameter;



#define isReceiveActiveBeaconMessage() ((Parameter.status&0x01)!=0)
#define ReceiveActiveBeaconMessage() {Parameter.status|=0x01;}
#define DeleteActiveBeaconMessage() {Parameter.status&=~0x01;}

#define isFactoryLink() ((Parameter.status&0x02)!=0)
#define setFactoryLink() {Parameter.status|=0x02;}
#define clrFactoryLink() {Parameter.status&=~0x02;}

#define isAbleTempTransmission() ((Parameter.status&0x04)!=0)
#define EnableTempTransmission() {Parameter.status|=0x04;}
#define DisableTempTransmission() {Parameter.status&=~0x04;}

#define isAblePeroidWaveTransmission() ((Parameter.status&0x08)!=0)
#define EnablePeroidWaveTransmission() {Parameter.status|=0x08;}
#define DisablePeroidWaveTransmission() {Parameter.status&=~0x08;}


#define isAbleLiteWaveTransmission() ((Parameter.status&0x10)!=0)
#define EnableLiteWaveTransmission() {Parameter.status|=0x10;}
#define DisableLiteWaveTransmission() {Parameter.status&=~0x10;}

#define isAblePeroidWaveAutoTransmission() ((Parameter.status&0x20)!=0)
#define EnablePeroidWaveAutoTransmission() {Parameter.status|=0x20;}
#define DisablePeroidWaveAutoTransmission() {Parameter.status&=~0x20;}

#define isCollectingOverInParameterMode() ((Parameter.status&0x40)!=0)  //正在采集，不上传数据
#define overCollectingInParameterMode() {Parameter.status|=0x40;}
#define BeginCollectingInParameterMode() {Parameter.status&=~0x40;}

#define isReceiveBeaconMessage() ((Parameter.status&0x80)!=0)  //正在采集，不上传数据
#define ReceiveBeaconMessage() {Parameter.status|=0x80;}
#define DeleteBeaconMessage() {Parameter.status&=~0x80;}

#define IsableProcessInParameterMode() ((Parameter.status&0x0100)!=0)  //使能特征值下的数据处理
#define EnableProcessInParameterMode() {Parameter.status|=0x0100;}
#define DisableProcessInParameterMode() {Parameter.status&=~0x0100;}

#define IsNeedRestartCollect() ((Parameter.status&0x0200)!=0)  //使能特征值下的数据处理
#define NeedRestartCollect() {Parameter.status|=0x0200;}
#define NotNeedRestartCollect() {Parameter.status&=~0x0200;}

#define isLMT01Running() ((Parameter.status&0x0400)!=0)
#define setLMT01Running() {Parameter.status|=0x0400;}
#define clrLMT01Running() {Parameter.status&=~0x0400;}

#define isLMT01Running() ((Parameter.status&0x0400)!=0)
#define setLMT01Running() {Parameter.status|=0x0400;}
#define clrLMT01Running() {Parameter.status&=~0x0400;}

#define isInAlarmStatue() ((Parameter.status&0x0400)!=0)
#define SetAlarmStatue() {Parameter.status|=0x0400;}
#define ClrAlarmStatue() {Parameter.status&=~0x0400;}

#endif
