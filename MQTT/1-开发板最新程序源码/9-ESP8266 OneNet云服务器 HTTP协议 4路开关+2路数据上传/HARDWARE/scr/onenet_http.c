/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            onenet_http���ܵ�Դ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"    //������Ҫ��ͷ�ļ�
#include "onenet_http.h"  //������Ҫ��ͷ�ļ�
#include "stdio.h"        //������Ҫ��ͷ�ļ�
#include "stdarg.h"		  //������Ҫ��ͷ�ļ� 
#include "string.h"       //������Ҫ��ͷ�ļ�
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "dht11.h"  	  //������Ҫ��ͷ�ļ�

char *ServerIP = "183.230.40.33";          //OneNet������ IP��ַ
int   ServerPort = 80;                     //OneNet������ �˿ں�
char RXbuff[2048];                         //�������ݻ�����
char TXbuff[2048];                         //�������ݻ�����

/*-------------------------------------------------*/
/*��������OneNet������ HTTP GET����                */
/*��  ����device_id�� �豸ID                       */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
void OneNet_GET(char *device_id)
{
	char temp[128];
	
	memset(TXbuff,0,2048);   //��ջ�����
    memset(temp,0,128);      //��ջ�����                                             
	sprintf(TXbuff,"GET /devices/%s/datapoints HTTP/1.1\r\n",device_id);//��������
	sprintf(temp,"api-key:%s\r\n",API_KEY);                             //��������
	strcat(TXbuff,temp);                                                //׷�ӱ���
	strcat(TXbuff,"Host:api.heclouds.com\r\n\r\n");                     //׷�ӱ���
}
/*-------------------------------------------------*/
/*��������OneNet������ HTTP POST����               */
/*��  ����device_id�� �豸ID                       */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
void OneNet_POST(char *device_id)
{
	char temp[128]; 
	char databuff[128]; 
	char humidity;           //����һ������������ʪ��ֵ
	char temperature;        //����һ�������������¶�ֵ
	
	memset(TXbuff,0,2048);   //��ջ�����
    memset(temp,0,128);      //��ջ�����
    memset(databuff,0,128);  //��ջ�����
	sprintf(TXbuff,"POST /devices/%s/datapoints?type=3 HTTP/1.1\r\n",device_id);//��������
	sprintf(temp,"api-key:%s\r\n",API_KEY);          //��������
	strcat(TXbuff,temp);                             //׷�ӱ���
	strcat(TXbuff,"Host:api.heclouds.com\r\n");      //׷�ӱ���	

    DHT11_Read_Data(&temperature,&humidity);	    //��ȡ��ʪ��ֵ
	sprintf(databuff,"{\"temp_data\":%d,\"humi_data\":%d}",temperature,humidity);   //�����ϱ�����

	sprintf(temp,"Content-Length:%d\r\n\r\n",strlen(databuff));  //��������
	strcat(TXbuff,temp);                                         //׷�ӱ���
	sprintf(temp,"%s\r\n\r\n",databuff);                         //��������
	strcat(TXbuff,temp);                                         //׷�ӱ���
	u1_printf("%s\r\n",databuff);
}
