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

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "main.h"       //������Ҫ��ͷ�ļ�
#include "delay.h"      //������Ҫ��ͷ�ļ�
#include "usart1.h"     //������Ҫ��ͷ�ļ�
#include "usart2.h"     //������Ҫ��ͷ�ļ�
#include "timer1.h"     //������Ҫ��ͷ�ļ�
#include "timer3.h"     //������Ҫ��ͷ�ļ�
#include "timer4.h"     //������Ҫ��ͷ�ļ�
#include "wifi.h"	    //������Ҫ��ͷ�ļ�
#include "led.h"        //������Ҫ��ͷ�ļ�
#include "mqtt.h"       //������Ҫ��ͷ�ļ�
#include "key.h"        //������Ҫ��ͷ�ļ�

char *CMD1 = "APP+FourSW1C";  //����1�������״̬��ת����->�� ��->��  
char *CMD2 = "APP+FourSW2C";  //����2�������״̬��ת����->�� ��->��  
char *CMD3 = "APP+FourSW3C";  //����3�������״̬��ת����->�� ��->��  
char *CMD4 = "APP+FourSW4C";  //����4�������״̬��ת����->�� ��->��  
char *CMD5 = "APP+FourASW?";  //��ѯ����ظ���ǰ4·����״̬
char *CMD6 = "APP+FourASW1";  //4·���أ�ȫ����
char *CMD7 = "APP+FourASW0";  //4·���أ�ȫ���ر�

int main(void) 
{	
	Delay_Init();                   //��ʱ���ܳ�ʼ��              
	Usart1_Init(9600);              //����1���ܳ�ʼ����������9600
	Usart2_Init(115200);            //����2���ܳ�ʼ����������115200	
	TIM4_Init(500,8400);            //TIM4��ʼ������ʱʱ�� 500*8400*1000/84000000 = 50ms
	LED_Init();	                    //LED��ʼ��
	KEY_Init();                     //������ʼ��
	WiFi_ResetIO_Init();            //��ʼ��WiFi�ĸ�λIO
	IoT_Parameter_Init();	        //��ʼ����IoTƽ̨MQTT�������Ĳ���	
	
	while(1)                        //��ѭ��
	{		
		/*--------------------------------------------------------------------*/
		/*   Connect_flag=1ͬ����������������,���ǿ��Է������ݺͽ���������    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     �����ͻ���������                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if�����Ļ���˵�����ͻ�������������
				//3������ɽ���if
				//��1�֣�0x10 ���ӱ���
				//��2�֣�0x82 ���ı��ģ���ConnectPack_flag��λ����ʾ���ӱ��ĳɹ�
				//��3�֣�SubcribePack_flag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ�
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("��������:0x%x\r\n",MQTT_TxDataOutPtr[2]);  //������ʾ��Ϣ
					MQTT_TxData(MQTT_TxDataOutPtr);                       //��������
					MQTT_TxDataOutPtr += TBUFF_UNIT;                      //ָ������
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //���ָ�뵽������β����
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //ָ���λ����������ͷ
				} 				
			}//�����ͻ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ������ջ���������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if�����Ļ���˵�����ջ�������������														
				u1_printf("���յ�����:");
				/*-----------------------------------------------------*/
				/*                    ����CONNACK����                  */
				/*-----------------------------------------------------*/				
				//if�жϣ������һ���ֽ���0x20����ʾ�յ�����CONNACK����
				//��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : u1_printf("CONNECT���ĳɹ�\r\n");                            //���������Ϣ	
								    ConnectPack_flag = 1;                                        //CONNECT���ĳɹ������ı��Ŀɷ�
									break;                                                       //������֧case 0x00                                              
						case 0x01 : u1_printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");     //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x01   
						case 0x02 : u1_printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n"); //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x02 
						case 0x03 : u1_printf("�����Ѿܾ�������˲����ã�׼������\r\n");         //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x03
						case 0x04 : u1_printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");   //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x04
						case 0x05 : u1_printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");               //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x05 		
						default   : u1_printf("�����Ѿܾ���δ֪״̬��׼������\r\n");             //���������Ϣ 
									Connect_flag = 0;                                            //Connect_flag���㣬��������					
									break;                                                       //������֧case default 								
					}				
				}			
				//if�жϣ���һ���ֽ���0x90����ʾ�յ�����SUBACK����
				//��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00 :
						case 0x01 : u1_printf("���ĳɹ�\r\n");            //���������Ϣ
							        SubcribePack_flag = 1;                //SubcribePack_flag��1����ʾ���ı��ĳɹ����������Ŀɷ���
									Ping_flag = 0;                        //Ping_flag����
   								    TIM3_ENABLE_30S();                    //����30s��PING��ʱ��
									LED_State();                          //�жϿ���״̬����������������  
									break;                                //������֧                                             
						default   : u1_printf("����ʧ�ܣ�׼������\r\n");  //���������Ϣ 
									Connect_flag = 0;                     //Connect_flag���㣬��������
									break;                                //������֧ 								
					}					
				}
				//if�жϣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					u1_printf("PING���Ļظ�\r\n"); 		  //���������Ϣ 
					if(Ping_flag==1){                     //���Ping_flag=1����ʾ��һ�η���
						 Ping_flag = 0;    				  //Ҫ���Ping_flag��־
					}else if(Ping_flag>1){ 				  //���Ping_flag>1����ʾ�Ƕ�η����ˣ�������2s����Ŀ��ٷ���
						Ping_flag = 0;     				  //Ҫ���Ping_flag��־
						TIM3_ENABLE_30S(); 				  //PING��ʱ���ػ�30s��ʱ��
					}				
				}	
				//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
				//����Ҫ��ȡ��������
				else if((MQTT_RxDataOutPtr[2]==0x30)){ 
					u1_printf("�������ȼ�0����\r\n"); 		   //���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //����ȼ�0��������
				}				
								
				MQTT_RxDataOutPtr +=RBUFF_UNIT;                     //ָ������
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //���ָ�뵽������β����
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //ָ���λ����������ͷ                        
			}//������ջ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ���������������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                             //if�����Ļ���˵�����������������			       
				u1_printf("����:%s\r\n",&MQTT_CMDOutPtr[2]);                 //���������Ϣ
				if(!memcmp(&MQTT_CMDOutPtr[2],CMD1,strlen(CMD1))){           //�ж�ָ������CMD1
					LED1_OUT(!LED1_IN_STA);                                  //����1״̬��ת ���������Ϩ�𣬷�֮���Ϩ��͵���		
					LED_State();	                                         //�ж�4·����״̬����������������					
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD2,strlen(CMD2))){     //�ж�ָ������CMD2
					LED2_OUT(!LED2_IN_STA);                                  //����2״̬��ת ���������Ϩ�𣬷�֮���Ϩ��͵���		
					LED_State();	                                         //�ж�4·����״̬����������������					
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD3,strlen(CMD3))){     //�ж�ָ������CMD3
					LED3_OUT(!LED3_IN_STA);                                  //����3״̬��ת ���������Ϩ�𣬷�֮���Ϩ��͵���		
					LED_State();	                                         //�ж�4·����״̬����������������					
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD4,strlen(CMD4))){     //�ж�ָ������CMD4
					LED4_OUT(!LED4_IN_STA);                                  //����4״̬��ת ���������Ϩ�𣬷�֮���Ϩ��͵���
					LED_State();	                                         //�ж�4·����״̬����������������					
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD5,strlen(CMD5))){     //�ж�ָ������CMD5
					LED_State();	                                         //�ж�4·����״̬����������������                                                         //�������ÿ��أ�������������ᷢ�Ϳ���״̬
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD6,strlen(CMD6))){     //�ж�ָ������CMD6
					LED_AllOn();                                             //ȫ��
					LED_State();	                                         //�ж�4·����״̬����������������
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD7,strlen(CMD7))){     //�ж�ָ������CMD7
					LED_AllOff();                                            //ȫ��
					LED_State();	                                         //�ж�4·����״̬����������������	
				}else u1_printf("δָ֪��\r\n");                             //���������Ϣ		
				
				MQTT_CMDOutPtr += CBUFF_UNIT;                             	 //ָ������
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //���ָ�뵽������β����
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //ָ���λ����������ͷ				
			}//��������������ݵ�else if��֧��β	
		}//Connect_flag=1��if��֧�Ľ�β
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0ͬ�������Ͽ�������,����Ҫ�������ӷ�����         */
		/*--------------------------------------------------------------------*/
		else{ 
			u1_printf("��Ҫ���ӷ�����\r\n");                 //���������Ϣ
			TIM_Cmd(TIM4,DISABLE);                           //�ر�TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //�ر�TIM3  
			WiFi_RxCounter=0;                                //WiFi������������������                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ����� 
			if(WiFi_Connect_IoTServer()==0){   			     //���WiFi�����Ʒ�������������0����ʾ��ȷ������if
				u1_printf("����TCP���ӳɹ�\r\n");            //���������Ϣ
				Connect_flag = 1;                            //Connect_flag��1����ʾ���ӳɹ�	
				WiFi_RxCounter=0;                            //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      //���WiFi���ջ����� 
				MQTT_Buff_Init();                            //��ʼ�����ͻ�����                    
			}				
		}
	}
}
/*-------------------------------------------------*/
/*���������жϿ���״̬����������������             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED_State(void)
{
	char temp[20];                  		//����һ����ʱ������
	
	memset(temp,0,20);             		    //�����ʱ������
	sprintf(temp,"+FourSW:");               //�����ظ�����	
	
	if(LED1_IN_STA) temp[8]  = '0';  		//���LED1�Ǹߵ�ƽ��˵����Ϩ��״̬������1״̬λ��0
	else            temp[8]  = '1';			//��֮��˵���ǵ���״̬������1״̬λ��1	
	if(LED2_IN_STA) temp[9]  = '0';  		//���LED2�Ǹߵ�ƽ��˵����Ϩ��״̬������2״̬λ��0
	else            temp[9]  = '1';			//��֮��˵���ǵ���״̬������2״̬λ��1	
	if(LED3_IN_STA) temp[10] = '0';  		//���LED3�Ǹߵ�ƽ��˵����Ϩ��״̬������3״̬λ��0
	else            temp[10] = '1';			//��֮��˵���ǵ���״̬������3״̬λ��1	
	if(LED4_IN_STA) temp[11] = '0'; 		//���LED4�Ǹߵ�ƽ��˵����Ϩ��״̬������4״̬λ��0
	else            temp[11] = '1';			//��֮��˵���ǵ���״̬������4״̬λ��1	

	MQTT_PublishQs0(P_TOPIC_NAME,temp,12);  //������ݣ�������������	
}
