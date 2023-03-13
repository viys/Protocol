/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ����SIM800C���ܵ�Դ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "sim800c.h"	//������Ҫ��ͷ�ļ�
#include "delay.h"	    //������Ҫ��ͷ�ļ�
#include "usart1.h"	    //������Ҫ��ͷ�ļ�
#include "mqtt.h"	    //������Ҫ��ͷ�ļ�
/*-------------------------------------------------*/
/*����������ʼ��SIM800C��GPIO                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SIM800C_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      //����һ������IO�˿ڲ����Ľṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);   //ʹ��PC�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                 //׼������PC0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		  //���������ʽ
	GPIO_Init(GPIOC, &GPIO_InitStructure);            		  //����PC0
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;                //׼������PC1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   		  //��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);            		  //����PC1
}

/*-------------------------------------------------*/
/*��������SIM800C����                              */
/*��  ������                                       */
/*����ֵ��0:�����ɹ�  1:����ʧ��                   */
/*-------------------------------------------------*/
char SIM800C_Power(void)
{
	if(POWER_STA==0){   							           //���PC1�ǵ͵�ƽ����ʾĿǰ���ڹػ�״̬		
		u1_printf("\r\nĿǰ���ڹػ�״̬��׼������\r\n");       //���������Ϣ
		POWER_KEY(1);                                          //������PC0
		Delay_Ms(1500);										   //��ʱ
		POWER_KEY(0);										   //������PC0������
	}else{                                                     //��֮PC1�Ǹߵ�ƽ����ʾĿǰ���ڿ���״̬
		u1_printf("\r\nĿǰ���ڿ���״̬��׼������\r\n");       //���������Ϣ		
		POWER_KEY(1);                                          //������PC0
		Delay_Ms(1500);										   //��ʱ
		POWER_KEY(0);										   //������PC0���ػ�
		Delay_Ms(1200);                                        //���
		POWER_KEY(1);                                          //������PC0
		Delay_Ms(1500);										   //��ʱ
		POWER_KEY(0);										   //������PC0���������������
	}	
	Delay_Ms(2000);			                                   //��ʱ���ȴ�״̬�ȶ�
	if(POWER_STA){                                             //���PC1�ǵ͵�ƽ����ʾ�������������ɹ�
		u1_printf("�����ɹ�\r\n");                             //���������Ϣ
	}else{												       //��֮PC1�Ǹߵ�ƽ����ʾ��ʼ��������ʧ��
		u1_printf("����ʧ��\r\n"); 							   //���������Ϣ
		return 1;                                              //����1����ʾʧ��
	}	
	return 0;                                                  //����0����ʾ�ɹ�
}
/*-------------------------------------------------*/
/*����������������ָ��                             */
/*��  ����cmd��ָ��                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char SIM800C_SendCmd(char *cmd, int timeout)
{
	SIM800C_RxCounter=0;                           //800C������������������                        
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���800C���ջ����� 
	SIM800C_printf("%s\r\n",cmd);                  //����ָ��
	while(timeout--){                              //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                             //��ʱ100ms
		if(strstr(SIM800C_RX_BUF,"OK"))            //������յ�OK��ʾָ��ɹ�
			break;       						   //��������whileѭ��
		u1_printf("%d ",timeout);                  //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                             //���������Ϣ
	if(timeout<=0)return 1;                        //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         				   //��֮����ʾ��ȷ��˵���յ�OK��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������CREGָ���ѯע��������                 */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char SIM800C_CREG(int timeout)
{
	while(timeout--){                                  //�ȴ���ʱʱ�䵽0
		SIM800C_RxCounter=0;                           //800C������������������                        
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���800C���ջ����� 
		SIM800C_printf("AT+CREG?\r\n");                //����ָ��鿴�Ƿ�ע����
		Delay_Ms(1000);                                //��ʱ1s
		if(strstr(SIM800C_RX_BUF,"+CREG: 0,1"))        //������յ� +CREG: 0,1 ��ʾע����
			break;       						       //��������whileѭ��
		u1_printf("%d ",timeout);                      //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                                 //���������Ϣ
	if(timeout<=0)return 1;                            //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�+CREG: 0,1������1
	else return 0;		         				       //��֮����ʾ��ȷ��˵���յ�+CREG: 0,1��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������CSQָ���ѯ�ź�                        */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char SIM800C_CSQ(int timeout)
{
	char  temp1[10],temp2[10] ,temp3[10];                    //����3����ʱ������  
	
	SIM800C_RxCounter=0;                          			 //800C������������������                        
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE); 			 //���800C���ջ����� 
	SIM800C_printf("AT+CSQ\r\n");            	    		 //����ָ��,��ѯ�ź�����
	while(timeout--){                                        //�ȴ���ʱʱ�䵽0
		Delay_Ms(100);                            			 //��ʱ100ms
		if(strstr(SIM800C_RX_BUF,"+CSQ:")){		             //���յ� +CSQ: ��ʾָ��ɹ�
			sscanf(SIM800C_RX_BUF,"%[^ ] %[^,],%[^,]",temp1,temp2,temp3);   //��ʽ���������ź�����
			u1_printf("�ź�ǿ�ȷ�Χ0~31,99��ʾ���ź�\r\n");  //���������Ϣ
			u1_printf("�ź�ǿ�ȣ�%s\r\n",temp2);             //���������Ϣ
			break;       						             //��������whileѭ��
		}
		u1_printf("%d ",timeout);                            //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                                       //���������Ϣ
	if(timeout<=0)return 1;                                  //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�+CSQ:������1
	else return 0;		         				             //��֮����ʾ��ȷ��˵���յ�+CSQ:��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������CGATTָ���ѯ�Ƿ���GPRS              */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char SIM800C_CGATT(int timeout)
{
	while(timeout--){                                  //�ȴ���ʱʱ�䵽0
		SIM800C_RxCounter=0;                           //800C������������������                        
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���800C���ջ����� 
		SIM800C_printf("AT+CGATT?\r\n");               //����ָ���ѯ�Ƿ���GPRS
		Delay_Ms(1000);                                //��ʱ1s
		if(strstr(SIM800C_RX_BUF,"+CGATT: 1"))		   //���յ� +CGATT: 1 ��ʾָ��ɹ�
			break;       						       //��������whileѭ��
		u1_printf("%d ",timeout);                      //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                                 //���������Ϣ
	if(timeout<=0)return 1;                            //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�+CGATT: 1������1
	else return 0;		         				       //��֮����ʾ��ȷ��˵���յ�+CGATT: 1��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������������������                             */
/*��  ������                                       */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char SIM800C_ActivateNetwork(void)
{	                                                              //��֮���timeС��timeout����ʾ��ȷ��ͨ��break��������while
	u1_printf("�ȴ�����GPRS... ...\r\n");                         //���������Ϣ
	if(SIM800C_SendCmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",60)){  //���ͼ���GPRS�����ʱʱ��6s
		u1_printf("����GPRSʧ��\r\n");                            //���������Ϣ
		return 1;                                                 //���if��������ʾ��ʱ�ˣ�����1
	}else{                                                        //��֮����ʾָ����ȷ
		u1_printf("����GPRS�ɹ�\r\n");                            //���������Ϣ
		u1_printf("�ȴ����ý����... ...\r\n");                   //���������Ϣ
		if(SIM800C_SendCmd("AT+SAPBR=3,1,\"APN\",\"CMIOT\"",60)){ //�������ý���������ʱʱ��6s
			u1_printf("���ý����ʧ��\r\n");                      //���������Ϣ
			return 2;                                             //���if��������ʾ��ʱ�ˣ�����2
		}else{													  //��֮����ʾָ����ȷ
			u1_printf("���ý����ɹ�\r\n");                      //���������Ϣ
			u1_printf("�ȴ���GPRS����... ...\r\n");             //���������Ϣ
			if(SIM800C_SendCmd("AT+SAPBR=1,1",60)){               //���ʹ����������ʱʱ��6s
				u1_printf("��GPRS����ʧ��\r\n");                //���������Ϣ
				return 3;                                         //���if��������ʾ��ʱ�ˣ�����3
			}else u1_printf("��GPRS����ɹ�\r\n");              //���������Ϣ	
		}
	}	
	return 0;
}
/*-------------------------------------------------*/
/*��������ͬ����������TCP����                      */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char SIM800C_TCPConnect(int timeout)
{
	SIM800C_RxCounter=0;                           //800C������������������                        
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���800C���ջ����� 
	SIM800C_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",ServerIP,ServerPort); //����ָ����ӷ�����
	while(timeout--){                              //�ȴ���ʱʱ�䵽0
		Delay_Ms(1000);                            //��ʱ1s
		if(strstr(SIM800C_RX_BUF,"CONNECT"))       //������յ�CONNECT��ʾ���ӳɹ�
			break;       						   //��������whileѭ��
		u1_printf("%d ",timeout);                  //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                             //���������Ϣ
	if(timeout<=0)return 1;                        //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�CONNECT������1
	else return 0;		         				   //��֮����ʾ��ȷ��˵���յ�CONNECT��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������ͬ�������Ͽ�TCP����                      */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char SIM800C_TCPClose(int timeout)
{
	SIM800C_RxCounter=0;                           //800C������������������                        
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���800C���ջ����� 
	SIM800C_printf("AT+CIPCLOSE\r\n");             //����ָ��Ͽ�����
	while(timeout--){                              //�ȴ���ʱʱ�䵽0
		Delay_Ms(1000);                            //��ʱ1s
		if(strstr(SIM800C_RX_BUF,"CLOSE OK"))      //������յ�CLOSE OK��ʾ���ӳɹ�
			break;       						   //��������whileѭ��
		u1_printf("%d ",timeout);                  //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                             //���������Ϣ
	if(timeout<=0)return 1;                        //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�CLOSE OK������1
	else return 0;		         				   //��֮����ʾ��ȷ��˵���յ�CLOSE OK��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������800C�����������Ʒ�����                   */
/*��  ����data������                               */
/*����ֵ����                                       */
/*-------------------------------------------------*/
char SIM800C_Connect_IoTServer(void)
{
	char i;                         //����һ������������forѭ��
	
	SIM800C_GPIO_Init();            //����800C��IO��ʼ��
	if(SIM800C_Power()){            //����800C����������,�������1��˵����������ʧ�ܣ�׼������
		return 1;                   //����1
	}

	u1_printf("��ȴ�ע��������... ...\r\n");        //���������Ϣ
	if(SIM800C_CREG(30)){                            //�ȴ�ע�������磬��ʱ��λ1s����ʱʱ��30s
		u1_printf("ע�����糬ʱ��׼������\r\n");     //���������Ϣ
		return 2;                                    //����2
	}else u1_printf("ע��������\r\n");               //���������Ϣ
	
	if(SIM800C_CSQ(60)){                             //��ѯ�ź�ǿ�ȣ���ʱ��λ100ms����ʱʱ��6s
		u1_printf("��ѯ�ź�ǿ�ȳ�ʱ��׼������\r\n"); //���������Ϣ
		return 3;                                    //����3	
	}
	
	u1_printf("��ȴ�������GPRS... ...\r\n");        //���������Ϣ
	if(SIM800C_CGATT(30)){                           //��ѯ����GPRS����ʱ��λ1s����ʱʱ��30s
		u1_printf("��ѯ����GPRS��ʱ��׼������\r\n"); //���������Ϣ
		return 4;                                    //����4	
	}else u1_printf("������GPRS\r\n");               //���������Ϣ
	
	u1_printf("��ȴ����õ�����... ...\r\n");        //���������Ϣ
	if(SIM800C_SendCmd("AT+CIPMUX=0",60)){           //���õ�����ģʽ����ʱ��λ100ms����ʱʱ��6s	
		u1_printf("���õ�����ʧ�ܣ�׼������\r\n");   //���������Ϣ
		return 5;                                    //����5	
	}else u1_printf("���õ�����ģʽ�ɹ�\r\n");       //���������Ϣ

	u1_printf("��ȴ�����͸��... ...\r\n");         //���������Ϣ
	if(SIM800C_SendCmd("AT+CIPMODE=1",60)){         //����͸��ģʽ����ʱ��λ100ms����ʱʱ��6s		
		u1_printf("����͸��ʧ�ܣ�׼������\r\n");    //���������Ϣ
		return 6;                                   //����6
	}else u1_printf("����͸���ɹ�\r\n");            //���������Ϣ   
	
	if(SIM800C_ActivateNetwork()){                   //׼����������
		u1_printf("��������ʧ�ܣ�׼������\r\n");     //���������Ϣ
		return 7;                                    //����7
	}
	Delay_Ms(500);                                   //�ʵ���ʱ
	for(i=0;i<3;i++)                                 //����3��                                   
	{
		u1_printf("��ȴ������Ϸ�����... ...\r\n");  //���������Ϣ
		if(SIM800C_TCPConnect(10)){                  //ͬ����������TCP���ӣ���ʱ��λ1s����ʱʱ��10s 
			u1_printf("����ʧ�ܣ�׼���ٴ�����\r\n"); //���������Ϣ
			Delay_Ms(500);                           //�ʵ���ʱ
			if(SIM800C_TCPClose(10)){                //׼���ٴ�����ǰҪ�ȷ��͹ر�ָ���ʱ��λ1s����ʱʱ��10s 
				u1_printf("�����쳣��׼������\r\n"); //���������Ϣ
				Delay_Ms(500);                       //�ʵ���ʱ
				return 8;                            //����7
			}
		}else return 0;                              //��ȷ������0
	}
	u1_printf("���Ӳ��Ϸ�������׼������\r\n");       //���������Ϣ
	return 9;                                        //����9
}
