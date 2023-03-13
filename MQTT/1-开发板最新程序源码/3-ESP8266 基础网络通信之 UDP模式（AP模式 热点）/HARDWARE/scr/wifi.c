/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ����602Wifi���ܵ�Դ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "main.h"       //������Ҫ��ͷ�ļ�
#include "wifi.h"	    //������Ҫ��ͷ�ļ�
#include "delay.h"	    //������Ҫ��ͷ�ļ�
#include "usart1.h"	    //������Ҫ��ͷ�ļ�
#include "key.h"        //������Ҫ��ͷ�ļ�

char wifi_mode = 1;     //����ģʽ 0��SSID������д�ڳ�����   1��Smartconfig��ʽ��APP����
char UDP_flag = 0;      //0����ʼ��UDPʧ��  1����ʼ��UDP�ɹ�

/*-------------------------------------------------*/
/*����������ʼ��WiFi�ĸ�λIO                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void WiFi_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      //����һ������IO�˿ڲ����Ľṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);   //ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //׼������PA4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		  //���������ʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		  //����PA4
	RESET_IO(1);                                              //��λIO���ߵ�ƽ
}
/*-------------------------------------------------*/
/*��������WiFi��������ָ��                         */
/*��  ����cmd��ָ��                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_SendCmd(char *cmd, int timeout)
{
	WiFi_RxCounter=0;                           //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //���WiFi���ջ����� 
	WiFi_printf("%s\r\n",cmd);                  //����ָ��
	while(timeout--){                           //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                          //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"OK"))            //������յ�OK��ʾָ��ɹ�
			break;       						//��������whileѭ��
		u1_printf("%d ",timeout);               //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                          //���������Ϣ
	if(timeout<=0)return 1;                     //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         				//��֮����ʾ��ȷ��˵���յ�OK��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi��λ                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Reset(int timeout)
{
	RESET_IO(0);                                    //��λIO���͵�ƽ
	Delay_Ms(500);                                  //��ʱ500ms
	RESET_IO(1);                                    //��λIO���ߵ�ƽ	
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                              //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"ready"))             //������յ�ready��ʾ��λ�ɹ�
			break;       						    //��������whileѭ��
		u1_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�ready������1
	else return 0;		         				    //��֮����ʾ��ȷ��˵���յ�ready��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi����·����ָ��                       */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter=0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ����� 
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS); //����ָ��	
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		Delay_Ms(1000);                             //��ʱ1s
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP\r\n\r\nOK")) //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						    //��������whileѭ��
		u1_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       //��ȷ������0
}
/*-------------------------------------------------*/
/*��������WiFi_Smartconfig                         */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Smartconfig(int timeout)
{	
	WiFi_RxCounter=0;                           //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //���WiFi���ջ�����     
	while(timeout--){                           //�ȴ���ʱʱ�䵽0
		Delay_Ms(1000);                         //��ʱ1s
		if(strstr(WiFi_RX_BUF,"connected"))     //������ڽ��ܵ�connected��ʾ�ɹ�
			break;                              //����whileѭ��  
		u1_printf("%d ",timeout);               //����������ڵĳ�ʱʱ��  
	}	
	u1_printf("\r\n");                          //���������Ϣ
	if(timeout<=0)return 1;                     //��ʱ���󣬷���1
	return 0;                                   //��ȷ����0
}
/*-------------------------------------------------*/
/*���������ȴ�����·����                           */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		Delay_Ms(1000);                             //��ʱ1s
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP"))       //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						    //��������whileѭ��
		u1_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       //��ȷ������0
}
/*-------------------------------------------------*/
/*���������ȴ�����wifi����ȡIP��ַ                 */
/*��  ����ip������IP������                         */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_GetIP(u16 timeout)
{
	char *presult1,*presult2;
	char ip[50];
	
	WiFi_RxCounter=0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ����� 
	WiFi_printf("AT+CIFSR\r\n");                    //����ָ��	
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                              //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"OK"))                //������յ�OK��ʾ�ɹ�
			break;       						    //��������whileѭ��
		u1_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else{
		presult1 = strstr(WiFi_RX_BUF,"\"");
		if( presult1 != NULL ){
			presult2 = strstr(presult1+1,"\"");
			if( presult2 != NULL ){
				memcpy(ip,presult1+1,presult2-presult1-1);
				u1_printf("ESP8266��IP��ַ��%s\r\n",ip);     //������ʾIP��ַ
				return 0;    //��ȷ����0
			}else return 2;  //δ�յ�Ԥ������
		}else return 3;      //δ�յ�Ԥ������	
	}
}
/*-------------------------------------------------*/
/*����������ȡ����״̬                             */
/*��  ������                                       */
/*����ֵ������״̬                                 */
/*        0����״̬                                */
/*        1���пͻ��˽���                          */
/*        2���пͻ��˶Ͽ�                          */
/*-------------------------------------------------*/
char WiFi_Get_LinkSta(void)
{
	char id_temp[10]={0};    //�����������ID
	char sta_temp[10]={0};   //�����������״̬
	
	if(strstr(WiFi_RX_BUF,"CONNECT")){                 //������ܵ�CONNECT��ʾ�пͻ�������	
		sscanf(WiFi_RX_BUF,"%[^,],%[^,]",id_temp,sta_temp);
		u1_printf("�пͻ��˽��룬ID=%s\r\n",id_temp);  //������ʾ��Ϣ
		WiFi_RxCounter=0;                              //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);        //���WiFi���ջ�����     
		return 1;                                      //�пͻ��˽���
	}else if(strstr(WiFi_RX_BUF,"CLOSED")){            //������ܵ�CLOSED��ʾ�����ӶϿ�	
		sscanf(WiFi_RX_BUF,"%[^,],%[^,]",id_temp,sta_temp);
		u1_printf("�пͻ��˶Ͽ���ID=%s\r\n",id_temp);        //������ʾ��Ϣ
		WiFi_RxCounter=0;                                    //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);              //���WiFi���ջ�����     
		return 2;                                            //�пͻ��˶Ͽ�
	}else return 0;                                          //��״̬�ı�	
}
/*-------------------------------------------------*/
/*����������ȡ�ͻ�������                           */
/*        ����س����з�\r\n\r\n��Ϊ���ݵĽ�����   */
/*��  ����data�����ݻ�����                         */
/*��  ����len�� ������                             */
/*��  ����id��  �������ݵĿͻ��˵�����ID           */
/*����ֵ������״̬                                 */
/*        0��������                                */
/*        1��������                                */
/*-------------------------------------------------*/
char WiFi_Get_Data(char *data, char *len, char *id)
{
	char temp[10]={0};      //������
	char *presult;

	if(strstr(WiFi_RX_BUF,"\r\n\r\n")){                     //�������ŵĻس�������Ϊ���ݵĽ�����
		sscanf(WiFi_RX_BUF,"%[^,],%[^,],%[^:]",temp,id,len);//��ȡ�������ݣ���Ҫ��id�����ݳ���	
		presult = strstr(WiFi_RX_BUF,":");                  //����ð�š�ð�ź��������
		if( presult != NULL )                               //�ҵ�ð��
			sprintf((char *)data,"%s",(presult+1));         //ð�ź�����ݣ����Ƶ�data
		WiFi_RxCounter=0;                                   //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);             //���WiFi���ջ�����    
		return 1;                                           //�����ݵ���
	} else return 0;                                        //�����ݵ���
}
/*-------------------------------------------------*/
/*����������������������                           */
/*��  ����databuff�����ݻ�����<2048                */
/*��  ����data_len�����ݳ���                       */
/*��  ����id��      �ͻ��˵�����ID                 */
/*��  ����timeout�� ��ʱʱ�䣨10ms�ı�����         */
/*����ֵ������ֵ                                   */
/*        0���޴���                                */
/*        1���ȴ��������ݳ�ʱ                      */
/*        2�����ӶϿ���                            */
/*        3���������ݳ�ʱ                          */
/*-------------------------------------------------*/
char WiFi_SendData(char id, char *databuff, int data_len, int timeout)
{    
	int i;
	
	WiFi_RxCounter=0;                                 //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);           //���WiFi���ջ����� 
	WiFi_printf("AT+CIPSEND=%d,%d\r\n",id,data_len);  //����ָ��	
    while(timeout--){                                 //�ȴ���ʱ���	
		Delay_Ms(10);                                 //��ʱ10ms
		if(strstr(WiFi_RX_BUF,">"))                   //������յ�>��ʾ�ɹ�
			break;       						      //��������whileѭ��
		u1_printf("%d ",timeout);                     //����������ڵĳ�ʱʱ��
	}
	if(timeout<=0)return 1;                                   //��ʱ���󣬷���1
	else{                                                     //û��ʱ����ȷ       	
		WiFi_RxCounter=0;                                     //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);               //���WiFi���ջ����� 	
		for(i=0;i<data_len;i++)WiFi_printf("%c",databuff[i]); //��������	
		while(timeout--){                                     //�ȴ���ʱ���	
			Delay_Ms(10);                                     //��ʱ10ms
			if(strstr(WiFi_RX_BUF,"SEND OK")){                //�������SEND OK����ʾ���ͳɹ�			 
			WiFi_RxCounter=0;                                 //WiFi������������������                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);           //���WiFi���ջ����� 			
				break;                                        //����whileѭ��
			} 
			if(strstr(WiFi_RX_BUF,"link is not valid")){      //�������link is not valid����ʾ���ӶϿ�			
				WiFi_RxCounter=0;                             //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);       //���WiFi���ջ����� 			
				return 2;                                     //����2
			}
	    }
		if(timeout<=0)return 3;      //��ʱ���󣬷���3
		else return 0;	            //��ȷ������0
	}	
}
/*-------------------------------------------------*/
/*����������ʼ��UDPͨ�ţ�������͸��ģʽ            */
/*��  ����timeout�� ��ʱʱ�䣨100ms�ı�����        */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
char WiFi_Connect_UDP(int timeout, int local_port)
{	
	WiFi_RxCounter=0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ�����   
	WiFi_printf("AT+CIPSTART=\"UDP\",\"%s\",%d,%d,0\r\n",ServerIP,ServerPort,local_port);//����ָ��
	while(timeout--){                               //�ȴ���ʱ���
		Delay_Ms(100);                              //��ʱ100ms	
		if(strstr(WiFi_RX_BUF ,"OK"))               //������ܵ�OK��ʾ�ɹ�
			break;                                  //����whileѭ��
		u1_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��  
	}
	u1_printf("\r\n");                        //���������Ϣ
	if(timeout<=0)return 1;                   //��ʱ���󣬷���3
	else                                      //���ӳɹ���׼������͸��
	{
		u1_printf("׼������͸��\r\n");                  //������ʾ��Ϣ
		WiFi_RxCounter=0;                               //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ�����     
		WiFi_printf("AT+CIPSEND\r\n");                  //���ͽ���͸��ָ��
		while(timeout--){                               //�ȴ���ʱ���
			Delay_Ms(100);                              //��ʱ100ms	
			if(strstr(WiFi_RX_BUF,"\r\nOK\r\n\r\n>"))   //���������ʾ����͸���ɹ�
				break;                          //����whileѭ��
			u1_printf("%d ",timeout);           //����������ڵĳ�ʱʱ��  
		}
		if(timeout<=0)return 2;                 //͸����ʱ���󣬷���2	
	}
	return 0;	                                //�ɹ�����0	
}
/*-------------------------------------------------*/
/*������������AP                                   */         
/*��  ����ssid��ssid                               */
/*��  ����password������                           */
/*��  ����mode:���ܷ�ʽ                            */
/*��  ����timeout ����ʱʱ��                       */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
char WiFi_SET_AP(char *ssid, char *password, char mode, int timeout)
{
	WiFi_RxCounter=0;                                                   //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);                             //���WiFi���ջ����� 
	WiFi_printf("AT+CWSAP=\"%s\",\"%s\",4,%d\r\n",ssid,password,mode);  //��������APָ��
	while(timeout--){                           //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                          //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"OK"))            //������յ�OK��ʾָ��ɹ�
			break;       						//��������whileѭ��
		u1_printf("%d ",timeout);               //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                          //���������Ϣ
	if(timeout<=0)return 1;                     //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         				//��֮����ʾ��ȷ��˵���յ�OK��ͨ��break��������while
}
/*-------------------------------------------------*/
/*�����������ӷ�����                               */
/*��  ������                                       */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_UDP_AP(void)
{	
	char res;
	char temp[50];
	
	u1_printf("׼����λģ��\r\n");                     //������ʾ����
	if(WiFi_Reset(50)){                                //��λ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u1_printf("��λʧ�ܣ�׼������\r\n");           //���ط�0ֵ������if��������ʾ����
		return 1;                                      //����1
	}else u1_printf("��λ�ɹ�\r\n");                   //������ʾ����
	
	u1_printf("׼������APģʽ\r\n");                   //������ʾ����
	if(WiFi_SendCmd("AT+CWMODE=2",50)){                //����APģʽ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u1_printf("����APģʽʧ�ܣ�׼������\r\n");     //���ط�0ֵ������if��������ʾ����
		return 2;                                      //����2
	}else u1_printf("����APģʽ�ɹ�\r\n");             //������ʾ����
	
	u1_printf("׼������AP����\r\n");                   //������ʾ����
	u1_printf("SSID��%s\r\n",SSID);                    //������ʾ����
	u1_printf("���룺%s\r\n",PASS);                    //������ʾ����
	u1_printf("AP�ȵ�IP��%s\r\n",AP_IP);               //������ʾ����
	u1_printf("���ܷ�ʽ��WPA_WPA2_PSK\r\n");           //������ʾ����

	u1_printf("׼������AP�ȵ�IP\r\n");                 //������ʾ����
	memset(temp,0,50);                                 //�����ʱ������
	sprintf(temp,"AT+CIPAP=\"%s\"",AP_IP);             //��������
	if(WiFi_SendCmd(temp,50)){                         //����AP�ȵ�IP��100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u1_printf("����AP�ȵ�IPʧ�ܣ�׼������\r\n");   //���ط�0ֵ������if��������ʾ����
		return 3;                                      //����3
	}else u1_printf("����AP�ȵ�IP�ɹ�\r\n");           //������ʾ����
	
	u1_printf("׼������AP����\r\n");                   //������ʾ����
	if(WiFi_SET_AP(SSID,PASS,WPA_WPA2_PSK,50)){        //����AP������100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u1_printf("����AP����ʧ�ܣ�׼������\r\n");     //���ط�0ֵ������if��������ʾ����
		return 4;                                      //����4
	}else u1_printf("����AP�����ɹ�\r\n");             //������ʾ����
	
	u1_printf("׼������͸��\r\n");                     //������ʾ����
	if(WiFi_SendCmd("AT+CIPMODE=1",50)){               //����͸����100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u1_printf("����͸��ʧ�ܣ�׼������\r\n");       //���ط�0ֵ������if��������ʾ����
		return 11;                                     //����11
	}else u1_printf("�ر�͸���ɹ�\r\n");               //������ʾ����
	
	u1_printf("׼���رն�·����\r\n");                 //������ʾ����
	if(WiFi_SendCmd("AT+CIPMUX=0",50)){                //�رն�·���ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u1_printf("�رն�·����ʧ�ܣ�׼������\r\n");   //���ط�0ֵ������if��������ʾ����
		return 12;                                     //����12
	}else u1_printf("�رն�·���ӳɹ�\r\n");           //������ʾ����
	
	u1_printf("׼����ʼ��UDP\r\n");                    //������ʾ����
	res = WiFi_Connect_UDP(100,5050);                  //��ʼ��UDP��100ms��ʱ��λ���ܼ�10s��ʱʱ��
	if(res==1){                						   //����1������if
		u1_printf("��ʼ��UDP��ʱ��׼������\r\n");      //������ʾ����
		return 13;                                     //����13
	}else if(res==2){								   //����4������if��
		u1_printf("����͸��ʧ��\r\n");                 //������ʾ����
		return 14;                                     //����14
	}	
	u1_printf("��ʼ��UDP�ɹ�\r\n");                    //������ʾ����
	return 0;                                          //��ȷ����0	
}
