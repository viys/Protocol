/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ����Wifi���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "usart2.h"	       //������Ҫ��ͷ�ļ�

extern char UDP_flag;      //0����ʼ��UDPʧ��  1����ʼ��UDP�ɹ�

#define RESET_IO(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)x)  //PA4����WiFi�ĸ�λ

#define WiFi_printf       u2_printf           //����2���� WiFi
#define WiFi_RxCounter    Usart2_RxCounter    //����2���� WiFi
#define WiFi_RX_BUF       Usart2_RxBuff       //����2���� WiFi
#define WiFi_RXBUFF_SIZE  USART2_RXBUFF_SIZE  //����2���� WiFi

#define SSID   "ESP8266-AP"                 //·����SSID����
#define PASS   "123456789"                    //·��������
#define WPA_WPA2_PSK     4                    //���ܷ�ʽ
#define AP_IP  "192.168.10.1"                 //AP�ȵ��ip

void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_GetIP(u16 timeout);
char WiFi_Get_LinkSta(void);
char WiFi_Get_Data(char *data, char *len, char *id);
char WiFi_SendData(char id, char *databuff, int data_len, int timeout);
char WiFi_Connect_UDP(int timeout, int local_port);
char WiFi_SET_AP(char *ssid, char *password, char mode, int timeout);
char WiFi_UDP_AP(void);

#endif


