/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_ESP32_H
#define __BSP_ESP32_H
#include <string.h>  
#include <stdbool.h>
#include <stdarg.h>
/* ���ⲿ���õĺ������� */

void bsp_Esp32_Init(void);

#define EnableEsp32Power()  {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_SET);}
#define DisableEsp32Power() {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);}

#define ResetEsp32()  {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11,GPIO_PIN_SET);}
#define RunEsp32() {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11,GPIO_PIN_RESET);}

bool Esp32_SetIP_1(char *localIP,char *LocalMASK,char *LocalGATEWAY,uint8_t dhcp);  //����IP��ַ
bool Esp32_SetAP_1(char *APssid,char *APpassword );  //����IP��ַ
bool Esp32_applynetset(void);  //����IP��ַ
bool Esp32_SetTCPSERVER_1(char *TcpServer_IP,char *TcpServer_Port);
uint8_t WriteDataToTXDBUF(uint8_t * source,uint16_t length);
#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
