/*-----------------------------------------------------*/
/*              ��γ����STM32ϵ�п�����                */
/*-----------------------------------------------------*/
/*                     ����ṹ                        */
/*-----------------------------------------------------*/
/*USER     �����������main��������������������      */
/*HARDWARE ��������������ֹ����������������          */
/*CORE     ������STM32�ĺ��ĳ��򣬹ٷ��ṩ�����ǲ��޸� */
/*STLIB    ���ٷ��ṩ�Ŀ��ļ������ǲ��޸�              */
/*-----------------------------------------------------*/
/*                                                     */
/*           ����main��������ں���Դ�ļ�              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"   //������Ҫ��ͷ�ļ�
#include "main.h"        //������Ҫ��ͷ�ļ�
#include "delay.h"       //������Ҫ��ͷ�ļ�
#include "usart1.h"      //������Ҫ��ͷ�ļ�
#include "iic.h"         //������Ҫ��ͷ�ļ�
#include "24c02.h" 		 //������Ҫ��ͷ�ļ�
#include "usart2.h"      //������Ҫ��ͷ�ļ�
#include "timer1.h"      //������Ҫ��ͷ�ļ�
#include "timer3.h"      //������Ҫ��ͷ�ļ�
#include "timer2.h"      //������Ҫ��ͷ�ļ�
#include "timer4.h"      //������Ҫ��ͷ�ļ�
#include "wifi.h"	     //������Ҫ��ͷ�ļ�
#include "led.h"         //������Ҫ��ͷ�ļ�
#include "key.h"         //������Ҫ��ͷ�ļ�
#include "dht11.h"  	 //������Ҫ��ͷ�ļ�
#include "onenet_http.h" //������Ҫ��ͷ�ļ�

char r_flag = 0;         //8266״̬��־ 0����Ҫ��λ�����¸�λ  1������״̬
char s_flag = 0;         //��ѯ���ر�־ 0��û����ѯʱ��  1�����3s��ʱ�䵽��׼����ѯ    2�����ķ����ˣ��ȴ��������ظ�
char d_flag = 0;         //�ϴ����ݱ�־ 0��û���ϴ�ʱ��  1�����20s��ʱ�䵽��׼���ϴ�   2�����ķ����ˣ��ȴ��������ظ�

int main(void) 
{	
	char *presult;
	
	Delay_Init();                   //��ʱ���ܳ�ʼ��              
	Usart1_Init(9600);              //����1���ܳ�ʼ����������9600
	Usart2_Init(115200);            //����2���ܳ�ʼ����������115200	
	TIM4_Init(300,7200);            //TIM4��ʼ������ʱʱ�� 300*7200*1000/72000000 = 30ms
	LED_Init();	                    //LED��ʼ��
	KEY_Init();                     //������ʼ��
	IIC_Init();                     //��ʼ��IIC�ӿ�
	DHT11_Init();                   //��ʼ��DHT11	
	WiFi_ResetIO_Init();            //��ʼ��WiFi�ĸ�λIO
	
	while(1){                      		
		if(r_flag == 0){                                    //���r_flag��־����0����ʾ��Ҫ��λ����8266ģ��
			if(WiFi_Init()==0){                             //����WiFi_Init����������0��ʾ��λ���óɹ�
				u1_printf("ESP8266״̬��ʼ������\r\n");     //���������Ϣ
				r_flag = 1;                                 //r_flag��־��λ����ʾ8266״̬����
				TIM1_ENABLE_20S();                          //��ʱ��1 20s��ʱ ������ʪ��
				TIM2_ENABLE_3S();	                        //��ʱ��2 3s��ʱ  ��ѯ����״̬
			}
		}
		
		if(Usart2_RxCompleted==1){                                          //�����־Usart2_RxCompleted��λ����ʾ���յ�����
			Usart2_RxCompleted = 0;                                         //���Usart2_RxCompleted��λ��־
			TIM_Cmd(TIM3, DISABLE);                                         //�ر�TIM3��ʱ��
			if(strstr(&RXbuff[2],"200 OK")){                                //����200 OK ��ʾ������ȷ������if	
				if(strstr(&RXbuff[2],"\"errno\":0")){                       //����"errno":0 ��ʾ������������ȷ������if
					if(strstr(&RXbuff[2],"datastreams")){                   //���� datastreams ��ʾ��ȡ������������GET���� ��ѯ����״̬
						presult = strstr(&RXbuff[2],"\"id\":\"switch_1\""); //���� "id":"switch_1" ��ѯ����1״̬
						if(presult!=NULL){                                  //����������ˣ�����if
							if(*(presult-4) == '0') LED1_OFF;               //�����0���ر�LED1
							else LED1_ON;                                   //��֮��1����LED1							
						}
						presult = strstr(&RXbuff[2],"\"id\":\"switch_2\""); //���� "id":"switch_2" ��ѯ����2״̬
						if(presult!=NULL){                                  //����������ˣ�����if
							if(*(presult-4) == '0') LED2_OFF;               //�����0���ر�LED2
							else LED2_ON;                                   //��֮��1����LED2							
						}
						presult = strstr(&RXbuff[2],"\"id\":\"switch_3\""); //���� "id":"switch_3" ��ѯ����1״̬
						if(presult!=NULL){                                  //����������ˣ�����if
							if(*(presult-4) == '0') LED3_OFF;               //�����0���ر�LED3
							else LED3_ON;                                   //��֮��1����LED3							
						}
						presult = strstr(&RXbuff[2],"\"id\":\"switch_4\""); //���� "id":"switch_4" ��ѯ����1״̬
						if(presult!=NULL){                                  //����������ˣ�����if
							if(*(presult-4) == '0') LED4_OFF;               //�����0���ر�LED4
							else LED4_ON;                                   //��֮��1����LED4							
						}
						u1_printf("��ѯ����״̬��ȷ\r\n");                  //������ʾ����
					}else{								                    //��֮û������ datastreams ��ʾ��POST����
						u1_printf("�ϱ���ʪ��������ȷ\r\n");                //������ʾ����
					}
				}
			}else{
				u1_printf("���Ĵ���\r\n");          //������ʾ����
				u1_printf("%s\r\n",&RXbuff[2]);     //������ʾ����
			}
			if(WiFi_Close(50)){                                  //׼���ر����ӣ���ʱ��λ100ms����ʱʱ��5s
				TIM_Cmd(TIM1, DISABLE);                          //�ر�TIM1��ʱ��
				TIM_Cmd(TIM2, DISABLE);                          //�ر�TIM2��ʱ��
				TIM_Cmd(TIM3, DISABLE);                          //�ر�TIM3��ʱ��			
				r_flag = s_flag = d_flag = 0;                    //��־���
                u1_printf("�ر�����ʧ�ܣ�����ESP8266\r\n");      //������ʾ����
			}else{
				u1_printf("�ر����ӳɹ�\r\n");                   //������ʾ����
				if(s_flag==2){                                   //���s_flag����2����ʾ�����ǲ�ѯ���ص�
					s_flag = 0;                                  //�����־λ
					TIM2_ENABLE_3S();	                         //����ʱ��2 3s��ʱ  ��ѯ����״̬
					TIM_Cmd(TIM1, ENABLE);                       //�ؿ�TIM1��ʱ��
				}
				if(d_flag==2){                                   //���d_flag����2����ʾ�����ǲ�ѯ���ص�
					d_flag = 0;                                  //�����־λ
					TIM1_ENABLE_20S();	                         //����ʱ��1 20s��ʱ ������ʪ��
					TIM_Cmd(TIM2, ENABLE);                       //�ؿ�TIM2��ʱ��
				}
			}			
		}
		
		if(s_flag==1){                           //���s_flag��־��λ����ʾ3s���ʱ�䵽������if׼����ѯ����״̬			                   
			s_flag = 2;                          //s_flag����2���ȴ��������ظ�
			u1_printf("׼�����ӷ�����\r\n");     //������ʾ����
			if(WiFi_Connect_Server(50)){         //���ӷ�������100ms��ʱ��λ���ܼ�5s��ʱʱ��							
				TIM_Cmd(TIM1, DISABLE);          //�ر�TIM1��ʱ��
				TIM_Cmd(TIM2, DISABLE);          //�ر�TIM2��ʱ��
				TIM_Cmd(TIM3, DISABLE);          //�ر�TIM3��ʱ��
				Connect_flag = 0;                //���ӳɹ���־���
				r_flag = s_flag = d_flag = 0;    //��־���
				u1_printf("���ӷ�����ʧ��\r\n"); //������ʾ����
			}else{                               //����0������else��֧����ʾ���ӷ������ɹ�
				u1_printf("���ӷ������ɹ�\r\n"); //������ʾ����
				WiFi_RxCounter=0;                           //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //���WiFi���ջ����� 
				Connect_flag = 1;                //���ӳɹ���־��λ
				OneNet_GET(SWITCH_DID);          //������ѯ����״̬�ı���
				WiFi_printf(TXbuff);             //�ѹ����õı��ķ���������
				TIM3_ENABLE_5S();                //����ʱ��3 5s�Ķ�ʱ ���5s�ڷ����� û����������Ҫ���¸�λ8266			
			}
		} 
		
		if(d_flag==1){                           //���d_flag��־��λ����ʾ20s���ʱ�䵽������if׼���ϱ���ʪ������		                   
			d_flag = 2;                          //d_flag����2���ȴ��������ظ�
			u1_printf("׼�����ӷ�����\r\n");     //������ʾ����
			if(WiFi_Connect_Server(50)){         //���ӷ�������100ms��ʱ��λ���ܼ�5s��ʱʱ��
				TIM_Cmd(TIM1, DISABLE);          //�ر�TIM1��ʱ��
				TIM_Cmd(TIM2, DISABLE);          //�ر�TIM2��ʱ��
				TIM_Cmd(TIM3, DISABLE);          //�ر�TIM3��ʱ��
				Connect_flag = 0;                //���ӳɹ���־���
				r_flag = s_flag = d_flag = 0;    //��־���
				u1_printf("���ӷ�����ʧ��\r\n"); //������ʾ����
			}else{                               //����0������else��֧����ʾ���ӷ������ɹ�
				u1_printf("���ӷ������ɹ�\r\n"); //������ʾ����
				WiFi_RxCounter=0;                           //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //���WiFi���ջ����� 
				Connect_flag = 1;                //���ӳɹ���־��λ
				OneNet_POST(TEMPHUMI_DID);       //�������ϱ���ʪ�����ݵı���
				WiFi_printf(TXbuff);             //�ѹ����õı��ķ���������
				TIM3_ENABLE_5S();                //����ʱ��3 5s�Ķ�ʱ ���5s�ڷ����� û����������Ҫ���¸�λ8266				
			}
		} 
	}
}
