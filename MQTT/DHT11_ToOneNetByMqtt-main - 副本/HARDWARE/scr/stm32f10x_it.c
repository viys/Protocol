/*-------------------------------------------------*/
/*                                                 */
/*            	   �жϷ�����          	   	   */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //������Ҫ��ͷ�ļ�
#include "stm32f10x_it.h"  //������Ҫ��ͷ�ļ�
#include "usart.h"        //������Ҫ��ͷ�ļ�
#include "timer3.h"        //������Ҫ��ͷ�ļ�
#include "mqtt.h"          //������Ҫ��ͷ�ļ�
#include "dht11.h"         //������Ҫ��ͷ�ļ�                  

/*-------------------------------------------------*/
/*������������3�����жϺ�����������ȼ�������������ݣ�*/
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
uint8_t ucTemp;
void USART3_IRQHandler(void)   
{                      
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)    //���USART_IT_RXNE��־��λ����ʾ�����ݵ��ˣ�����if��֧
	{  
		if(connectFlag == 0) 							     //���connectFlag����0����ǰ��û�����ӷ�����������ָ������״̬
		{                               
			if(USART3->DR)
			{                                     			 //����ָ������״̬ʱ������ֵ�ű��浽������	
				Usart3_RxBuff[Usart3_RxCounter] = USART3->DR;//���浽������
				Usart3_RxCounter++; 						 //ÿ����1���ֽڵ����ݣ�Usart3_RxCounter��1����ʾ���յ���������+1 
			}					
		}
		else
		{		                                           	 //��֮connectFlag����1�������Ϸ�������	
			Usart3_RxBuff[Usart3_RxCounter] = USART3->DR;    //�ѽ��յ������ݱ��浽Usart3_RxBuff��				
			if(Usart3_RxCounter == 0)
			{    									    	 //���Usart3_RxCounter����0����ʾ�ǽ��յĵ�1�����ݣ�����if��֧				
				TIM_Cmd(TIM4, ENABLE); 
			}
			else										     //else��֧����ʾ��Usart3_RxCounter������0�����ǽ��յĵ�һ������
			{                        									    
				TIM_SetCounter(TIM4, 0);  
			}	
			Usart3_RxCounter++;         				     //ÿ����1���ֽڵ����ݣ�Usart3_RxCounter��1����ʾ���յ���������+1 
		}
		ucTemp = USART_ReceiveData(USART3);
		USART_SendData(USART1,ucTemp);  
	}
} 
/*-------------------------------------------------*/
/*����������ʱ��4�жϷ�����������MQTT����          */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//���TIM_IT_Update��λ����ʾTIM4����жϣ�����if	
	{                	
		memcpy(&MQTT_RxDataInPtr[2], Usart3_RxBuff, Usart3_RxCounter);  //�������ݵ����ջ�����
		MQTT_RxDataInPtr[0] = Usart3_RxCounter/256;                  	//��¼���ݳ��ȸ��ֽ�
		MQTT_RxDataInPtr[1] = Usart3_RxCounter%256;					 	//��¼���ݳ��ȵ��ֽ�
		MQTT_RxDataInPtr += RBUFF_UNIT;                                	//ָ������
		if(MQTT_RxDataInPtr == MQTT_RxDataEndPtr)                     	//���ָ�뵽������β����
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                    	//ָ���λ����������ͷ
		Usart3_RxCounter = 0;                                        	//����2������������������
		TIM_SetCounter(TIM3, 0);                                     	//���㶨ʱ��3�����������¼�ʱping������ʱ��
		TIM_Cmd(TIM4, DISABLE);                        				 	//�ر�TIM4��ʱ��
		TIM_SetCounter(TIM4, 0);                        			 	//���㶨ʱ��4������
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     			 	//���TIM4����жϱ�־ 	
	}
}
/*-------------------------------------------------*/
/*����������ʱ��3�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//���TIM_IT_Update��λ����ʾTIM3����жϣ�����if	
		{  
		switch(pingFlag) 					//�ж�pingFlag��״̬
		{                               
			case 0:							//���pingFlag����0����ʾ����״̬������Ping����  
					MQTT_PingREQ(); 		//���Ping���ĵ����ͻ�����  
					break;
			case 1:							//���pingFlag����1��˵����һ�η��͵���ping���ģ�û���յ��������ظ�������1û�б����Ϊ0�������������쳣������Ҫ��������pingģʽ
					TIM3_ENABLE_2S(); 	    //���ǽ���ʱ��6����Ϊ2s��ʱ,���ٷ���Ping����
					MQTT_PingREQ();			//���Ping���ĵ����ͻ�����  
					break;
			case 2:							//���pingFlag����2��˵����û���յ��������ظ�
			case 3:				            //���pingFlag����3��˵����û���յ��������ظ�
			case 4:				            //���pingFlag����4��˵����û���յ��������ظ�	
					MQTT_PingREQ();  		//���Ping���ĵ����ͻ����� 
					break;
			case 5:							//���pingFlag����5��˵�����Ƿ����˶��ping�����޻ظ���Ӧ�������������⣬������������
					connectFlag = 0;        //����״̬��0����ʾ�Ͽ���û���Ϸ�����
					TIM_Cmd(TIM3, DISABLE); //��TIM3 				
					break;			
		}
		pingFlag++;           		   		//pingFlag����1����ʾ�ַ�����һ��ping���ڴ��������Ļظ�
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //���TIM3����жϱ�־ 	
	}
}

/*-------------------------------------------------*/
/*����������ʱ��2�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM2_IRQHandler(void)
{
	char humidity;				//����һ������������ʪ��ֵ
	char temperature;			//����һ�������������¶�ֵ	
				
	char head1[3];
	char temp[50];				//����һ����ʱ������1,��������ͷ
	char tempAll[100];			//����һ����ʱ������2��������������
	
	int	dataLen = 0;			//���ĳ���
	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)	
	{ 
		DHT11_Read_Data(&temperature,&humidity);//��ȡ��ʪ��ֵ

		memset(temp,    0, 50);				    //��ջ�����1
		memset(tempAll, 0, 100);				//��ջ�����2
		memset(head1,   0, 3);					//���MQTTͷ

		sprintf(temp,"{\"temperature\":\"%d%d\",\"humidity\":\"%d%d\"}",
					  temperature/10, temperature%10, humidity/10, humidity%10);//��������

		head1[0] = 0x03; 						//�̶���ͷ
		head1[1] = 0x00; 						//�̶���ͷ
		head1[2] = strlen(temp);  				//ʣ�೤��	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		
		printf("\r\n"); //������ʾ�������
		printf("%s\r\n", tempAll + 3);
		
		dataLen = strlen(temp) + 3;
		MQTT_PublishQs0(Data_TOPIC_NAME,tempAll, dataLen);//������ݣ�������������
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);   	
	}
}

/*-------------------------------------------------*/
/*�����������������жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ӳ������������жϴ�����             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*���������ڴ�����жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ԥȡָʧ�ܣ��洢������ʧ���жϴ�����   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������δ�����ָ���Ƿ�״̬������           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*�����������жϣ�SWI ָ����õĴ�����           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*�����������Լ����������                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*���������ɹ����ϵͳ��������                 */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
}

