/*-----------------------------------------------------*/
/*              ��γ����STM32ϵ�п�����                */
/*-----------------------------------------------------*/
/*                     ����ṹ                        */
/*-----------------------------------------------------*/
/*USER     �����������main��������������������      */
/*HARDWARE ��������������ֹ����������������          */
/*CORE     ������STM32�ĺ��ĳ��򣬹ٷ��ṩ�����ǲ��޸� */
/*STLIB    ���ٷ��ṩ�Ŀ��ļ������ǲ��޸�              */
/*HMAC     �����Ϲ��ڹ�ϣ�����㷨����                  */
/*-----------------------------------------------------*/
/*                                                     */
/*           ����main��������ں���Դ�ļ�              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "main.h"       //������Ҫ��ͷ�ļ�
#include "delay.h"      //������Ҫ��ͷ�ļ�
#include "usart1.h"     //������Ҫ��ͷ�ļ�
#include "usart3.h"     //������Ҫ��ͷ�ļ�
#include "mqtt.h"       //������Ҫ��ͷ�ļ�
#include "timer2.h"     //������Ҫ��ͷ�ļ�
#include "timer4.h"     //������Ҫ��ͷ�ļ�
#include "timer6.h"     //������Ҫ��ͷ�ļ�
#include "sim800c.h"	//������Ҫ��ͷ�ļ�
#include "iic.h"        //������Ҫ��ͷ�ļ�
#include "led.h"        //������Ҫ��ͷ�ļ�
#include "dht11.h"      //������Ҫ��ͷ�ļ�

char *CMD1 = "APP+DTSW=1C";     //����1�������״̬��ת����->�� ��->��  
char *CMD2 = "APP+DTSW=2C";     //����2�������״̬��ת����->�� ��->��  
char *CMD3 = "APP+DTSW=DC";     //�ɼ�״̬��ת����������ǲɼ�״̬��ֹͣ����ֹ֮ͣ״̬�Ϳ�ʼ�ɼ�
char *CMD4 = "APP+DTSW=??";     //��ѯ2·����״̬ �� �ɼ�״̬
char  Data_flag = 0;            //�Ƿ��ڲɼ�״̬�� 0��ֹͣ�ɼ�  1���ɼ���

int main(void) 
{		
	Delay_Init();                   //��ʱ���ܳ�ʼ��              
	Usart1_Init(9600);              //����1���ܳ�ʼ����������9600
	Usart3_Init(115200);            //����2���ܳ�ʼ����������115200	
	TIM4_Init(300,7200);            //TIM4��ʼ������ʱʱ�� 300*7200*1000/72000000 = 30ms	
	LED_Init();	                    //��ʼ��LEDָʾ��	
	IIC_Init();	                    //��ʼ��IIC�ӿ�
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
   								    TIM6_ENABLE_30S();                    //����30s��PING��ʱ��
									LED_DATA_State();                     //�ж�״̬����������������  
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
						TIM6_ENABLE_30S(); 				  //PING��ʱ���ػ�30s��ʱ��
					}				
				}	
				//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
				//����Ҫ��ȡ��������
				else if((MQTT_RxDataOutPtr[2]==0x30)){ 
					u1_printf("�������ȼ�0����\r\n"); 		   //���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //����ȼ�0��������
				}				
				//if�жϣ����һ��������10���ֽڣ���һ���ֽ���0x0D���п����յ��� CLOSED ��ʾ���ӶϿ�
				//���ǽ���else if�������ж�
				else if((MQTT_RxDataOutPtr[1]==10)&&(MQTT_RxDataOutPtr[2]==0x0D)){
					MQTT_RxDataOutPtr[12] = '\0';                   //�����ַ���������
					if(strstr(SIM800C_RX_BUF,"CLOSED")){            //�����������CLOSED����ʾ���ӶϿ���						
						Connect_flag = 0;                           //����״̬��0����ʾ�Ͽ�����Ҫ����������
					}			
				}
				//if�жϣ����һ��������15���ֽڣ���һ���ֽ���0x0D���п����յ��� +PDP: DEACT ��ʾGPRS�Ͽ�
				//���ǽ���else if�������ж�
				else if((MQTT_RxDataOutPtr[1]==15)&&(MQTT_RxDataOutPtr[1]==0x0D)){
					MQTT_RxDataOutPtr[17] = '\0';                   //�����ַ���������
					u1_printf("GPRS�Ͽ�\r\n"); 		                //���������Ϣ
					if(strstr(SIM800C_RX_BUF,"+PDP: DEACT")){       //���������+PDP: DEACT����ʾGPRS�Ͽ���						
						Connect_flag = 0;                           //����״̬��0����ʾ�Ͽ�����Ҫ����������
					}			
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
					LED_DATA_State();	                                     //�ж�2·����״̬�Ͳɼ�״̬����������������
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD2,strlen(CMD2))){     //�ж�ָ������CMD2
					LED2_OUT(!LED2_IN_STA);                                  //����2״̬��ת ���������Ϩ�𣬷�֮���Ϩ��͵���		
					LED_DATA_State();	                                     //�ж�2·����״̬�Ͳɼ�״̬����������������
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD3,strlen(CMD3))){     //�ж�ָ������CMD3
					if(Data_flag==0){                                        //�����Data_flag����0����ʾ��ǰ����ֹͣ�ɼ�״̬�����ǿ�ʼ�ɼ�
						Data_flag = 1;                                       //Data_flag��1����ʾ���ڲɼ�״̬��
					    TIM2_ENABLE_10S();                                   //��ʱ��2��,10s����ɼ���ʪ��
					}else{                                                   //��֮��Data_flag����1����ʾ��ǰ���ڲɼ�״̬������ֹͣ
						Data_flag = 0;                                       //Data_flag��0����ʾ����ֹͣ״̬��
						TIM_Cmd(TIM2,DISABLE);                               //��TIM2 
					}
					LED_DATA_State();	                                     //�ж�2·����״̬�Ͳɼ�״̬����������������
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD4,strlen(CMD4))){     //�ж�ָ������CMD4
					LED_DATA_State();	                                     //�ж�2·����״̬�Ͳɼ�״̬����������������							                                                     //������������ᷢ��״̬
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
			TIM_Cmd(TIM6,DISABLE);                           //�ر�TIM6  
			SIM800C_RxCounter=0;                             //800C������������������                        
			memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);    //���800C���ջ����� 
			if(SIM800C_Connect_IoTServer()==0){   			 //���800C�����Ʒ�������������0����ʾ��ȷ������if
				u1_printf("����TCP���ӳɹ�\r\n");            //���������Ϣ
				Connect_flag = 1;                            //Connect_flag��1����ʾ���ӳɹ�	
				SIM800C_RxCounter=0;                         //800C������������������                        
				memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);//���800C���ջ����� 
				MQTT_Buff_Init();                            //��ʼ��������                    
			}				
		}
	}
}
/*-------------------------------------------------*/
/*���������ж�2·���غͲɼ�״̬����������������    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED_DATA_State(void)
{
	char temp[20];                  		//����һ����ʱ������
	
	memset(temp,0,20);             		    //�����ʱ������
	sprintf(temp,"+DTSW:");          		//�����ظ�����		
	if(LED1_IN_STA)  temp[6] = '0';  		//���LED1�Ǹߵ�ƽ��˵����Ϩ��״̬������1״̬λ��0
	else             temp[6] = '1';			//��֮��˵���ǵ���״̬������1״̬λ��1	
	if(LED2_IN_STA)  temp[7] = '0';  		//���LED2�Ǹߵ�ƽ��˵����Ϩ��״̬������2״̬λ��0
	else             temp[7] = '1';			//��֮��˵���ǵ���״̬������2״̬λ��1
	if(Data_flag==0) temp[8] = '0';         //�����Data_flag����0����ʾ��ǰ����ֹͣ�ɼ�״̬��״̬λ��0
	else             temp[8] = '1';         //��֮��Data_flag����1����ʾ��ǰ���ڲɼ�״̬��״̬λ��1
	MQTT_PublishQs0(P_TOPIC_NAME,temp,9);   //������ݣ�������������	
}
