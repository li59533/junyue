#include "main.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "app.h"
#include "Esp32ProcessTask.h"
#define      macUser_Esp32_LocalID                        "192.168.100.12"                //������ȥʱ�����õľ������
#define      macUser_Esp32_LocalGATAWAY                   "192.168.100.1"           
#define      macUser_Esp32_LocalMASK                      "255.255.255.0"           

#define      macUser_Esp32_ApSsid                         "yec-test"// "Tenda_4F7AC0"//"yec-test"                //Ҫ���ӵ��ȵ������
#define      macUser_Esp32_ApPwd                          ""           //Ҫ���ӵ��ȵ����Կ

#define      macUser_Esp32_TcpServer_IP                   "192.168.100.234"//"192.168.0.112"// //     //Ҫ���ӵķ������� IP
#define      macUser_Esp32_TcpServer_Port                 "8712"  //"8712"//             //Ҫ���ӵķ������Ķ˿�


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
extern uint8_t command_reply_channel_condition(void);   //����ͨ����
extern uint8_t command_reply_alarm_value();
extern uint8_t command_replyap();  //����ap info
extern uint8_t command_reply_runmode(void);
extern uint8_t command_reply_lowpower(void);
extern uint8_t command_reply_lowpower_value(void);
extern uint8_t command_replytcpserve(void);  //����IP��ַ
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
			Esp32_SetIP_1(macUser_Esp32_LocalID,macUser_Esp32_LocalGATAWAY,macUser_Esp32_LocalMASK,1); //����IP��ַ
			osDelay (3);	
			Esp32_SetAP_1(macUser_Esp32_ApSsid, macUser_Esp32_ApPwd);  //����IP��ַ
			osDelay (3);
			Esp32_SetTCPSERVER_1(macUser_Esp32_TcpServer_IP, macUser_Esp32_TcpServer_Port);  //����IP��ַ
#else
			Esp32_SetIP_1(config.LocalIP,config.LocalMASK,config.LocalGATEWAY,config.DHCP);  //����IP��ַ
			osDelay (3);
			test_rtc();
			Esp32_SetAP_1(config.APssid, config.APpassword);  //����IP��ַ
			osDelay (3);
			Esp32_SetTCPSERVER_1(config.TcpServer_IP, config.TcpServer_Port);  //����IP��ַ
#endif
			osDelay (3);
			Esp32_applynetset();
			while((!WifiConnectedFlag)&&(JoinAPcount<1200))
			{
				bsp_LedToggle(1);
				osDelay (10);  //10ms��ѯһ�Σ���������ӵĸ��죬������ѯʱ��
				JoinAPcount++;
			}
			
			if(JoinAPcount>1198)
			{
				ResetEsp32();
				osDelay(100);
				RunEsp32();
				osDelay(400);                //����һ�����������ϵ�ʱ�򣬻�����ESP32��һ��Ĭ���ȵ㣬��ʵ����Ҫ����
				esp32_ready=0;  //ֱ������
#ifdef  FirstLinkFactoryLink
				Esp32_SetIP_1(config.LocalIP,config.LocalMASK,config.LocalGATEWAY,config.DHCP);  //����IP��ַ
				osDelay (3);
				Esp32_SetAP_1(config.APssid, config.APpassword);  //����IP��ַ
				osDelay (3);
				Esp32_SetTCPSERVER_1(config.TcpServer_IP, config.TcpServer_Port);  //����IP��ַ
#else
				Esp32_SetIP_1(macUser_Esp32_LocalID,macUser_Esp32_LocalGATAWAY,macUser_Esp32_LocalMASK,1); //����IP��ַ
				osDelay (3);
				Esp32_SetAP_1(macUser_Esp32_ApSsid, macUser_Esp32_ApPwd);  //����IP��ַ
				osDelay (3);
				Esp32_SetTCPSERVER_1(macUser_Esp32_TcpServer_IP, macUser_Esp32_TcpServer_Port);  //����IP��ַ
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
			command_reply_channel_condition();   //����ͨ����
			
			command_reply_SampleParameter();
			command_replyap();
			command_replytcpserve();  //����IP��ַ
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
			WifiDisconnectedFlag=0;//ȡ��wifidisconconect����
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


