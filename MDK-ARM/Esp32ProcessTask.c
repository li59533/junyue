#include "main.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "app.h"
#include "Esp32ProcessTask.h"
#define      macUser_Esp32_LocalID                        "192.168.100.12"                //连不上去时，备用的就是这个
#define      macUser_Esp32_LocalGATAWAY                   "192.168.100.1"           
#define      macUser_Esp32_LocalMASK                      "255.255.255.0"           

#define      macUser_Esp32_ApSsid                         "yec-test"// "Tenda_4F7AC0"//"yec-test"                //要连接的热点的名称
#define      macUser_Esp32_ApPwd                          ""           //要连接的热点的密钥

#define      macUser_Esp32_TcpServer_IP                   "192.168.100.234"//"192.168.0.112"// //     //要连接的服务器的 IP
#define      macUser_Esp32_TcpServer_Port                 "8712"  //"8712"//             //要连接的服务器的端口


//#define FirstLinkFactoryLink 1

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup      version_Private_Variables 
 * @brief         
 * @{  
 */
static volatile uint8_t  esp_senddeviceinfo_flag = 0;



/**
 * @}
 */



extern volatile uint16_t esp32_ready;
extern volatile uint16_t WifiConnectedFlag;
extern volatile uint16_t WifiDisconnectedFlag;

extern uint16_t command_channelkind(void);
extern uint16_t command_id(void);
extern uint16_t command_reply_SampleParameter(void);
extern uint16_t command_reply_scale(void);
extern uint8_t command_reply_alarm_value(void);
extern uint8_t command_reply_sw_version(void);
extern uint8_t command_reply_channel_condition(void);   //返回通道字
extern uint8_t command_reply_alarm_value();
extern uint8_t command_replyap();  //返回ap info
extern uint8_t command_reply_runmode(void);
extern uint8_t command_reply_lowpower(void);
extern uint8_t command_reply_lowpower_value(void);
extern uint8_t command_replytcpserve(void);  //返回IP地址
extern uint8_t command_replyip(void);
extern void test_rtc(void);
void Esp32ProcessFunction ( void *pvParameters )
{ 
	uint32_t JoinAPcount=0;
	osDelay (3200);
	EnableEsp32Power();
	HAL_Delay(100);
	RunEsp32();
	while ( 1 )                                                                        
	{		
		if(esp32_ready)
		{
			esp32_ready=0;
			
#ifdef  FirstLinkFactoryLink
			Esp32_SetIP_1(macUser_Esp32_LocalID,macUser_Esp32_LocalGATAWAY,macUser_Esp32_LocalMASK,1); //设置IP地址
			osDelay (3);	
			Esp32_SetAP_1(macUser_Esp32_ApSsid, macUser_Esp32_ApPwd);  //返回IP地址
			osDelay (3);
			Esp32_SetTCPSERVER_1(macUser_Esp32_TcpServer_IP, macUser_Esp32_TcpServer_Port);  //返回IP地址
#else
			Esp32_SetIP_1(config.LocalIP,config.LocalMASK,config.LocalGATEWAY,config.DHCP);  //设置IP地址
			osDelay (3);
			test_rtc();
			Esp32_SetAP_1(config.APssid, config.APpassword);  //返回IP地址
			osDelay (3);
			Esp32_SetTCPSERVER_1(config.TcpServer_IP, config.TcpServer_Port);  //返回IP地址
#endif
			osDelay (3);
			Esp32_applynetset();
			while((!WifiConnectedFlag)&&(JoinAPcount<1200))
			{
				bsp_LedToggle(1);
				osDelay (10);  //10ms轮询一次，如果想连接的更快，降低轮询时间
				JoinAPcount++;
			}
			
			if(JoinAPcount>1198)
			{
				ResetEsp32();
				osDelay(100);
				RunEsp32();
				osDelay(400);                //当第一次连接连不上的时候，会重启ESP32连一个默认热点，其实不需要重启
				esp32_ready=0;  //直接重置
#ifdef  FirstLinkFactoryLink
				Esp32_SetIP_1(config.LocalIP,config.LocalMASK,config.LocalGATEWAY,config.DHCP);  //设置IP地址
				osDelay (3);
				Esp32_SetAP_1(config.APssid, config.APpassword);  //返回IP地址
				osDelay (3);
				Esp32_SetTCPSERVER_1(config.TcpServer_IP, config.TcpServer_Port);  //返回IP地址
#else
				Esp32_SetIP_1(macUser_Esp32_LocalID,macUser_Esp32_LocalGATAWAY,macUser_Esp32_LocalMASK,1); //设置IP地址
				osDelay (3);
				Esp32_SetAP_1(macUser_Esp32_ApSsid, macUser_Esp32_ApPwd);  //返回IP地址
				osDelay (3);
				Esp32_SetTCPSERVER_1(macUser_Esp32_TcpServer_IP, macUser_Esp32_TcpServer_Port);  //返回IP地址
#endif				
				osDelay (3);
				Esp32_applynetset();
			while((!WifiConnectedFlag))
				{
					bsp_LedToggle(1);
				osDelay (20);			
				}
				
				esp32_ready=0;
			}
			
			JoinAPcount=0;
			Parameter.Esp32TransmissionMode=NoBrainTransmission;
			clrFactoryLink();
			/*
			// ---- old software com-----
			command_channelkind();
			command_id();
			command_reply_SampleParameter();
			command_reply_scale();
			command_reply_alarm_value();			
			*/
			// ---------------------------
			
			//  ---- new software com----
			command_channelkind();
			command_id();
			command_reply_sw_version();
			command_reply_channel_condition();   //返回通道字
			
			command_reply_SampleParameter();
			command_replyap();
			command_replytcpserve();  //返回IP地址
			command_reply_runmode();
			command_replyip();
			command_reply_lowpower();
			command_reply_scale();
			command_reply_alarm_value();
			command_reply_lowpower_value();
			
			
			
			/**/
			Esp32_SetSendDeviceInfo_Flag();
			
		}
		if(WifiDisconnectedFlag)
		{
			Parameter.Esp32TransmissionMode=BrainTransmission;
			WifiConnectedFlag=0;
			WifiDisconnectedFlag=0;//取消wifidisconconect报警
			if(config.DataToBoardMode==WAVEMODE)
			{
				StopSample();
			}
			else	
			{	
				StopSample();
			}
			DisableTempTransmission();
		}

		osDelay (2); 
		

	
		
}
}


void Esp32_SetSendDeviceInfo_Flag(void)
{
	esp_senddeviceinfo_flag = 1;
}
void Esp32_ClearSendDeviceInfo_Flag(void)
{
	esp_senddeviceinfo_flag = 0;
}

uint8_t Esp32_GetSendDeivceInfo_Flag(void)
{
	return esp_senddeviceinfo_flag;
}


