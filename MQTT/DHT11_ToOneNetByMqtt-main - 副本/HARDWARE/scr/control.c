/*-------------------------------------------------*/
/*                                                 */
/*                 ���ƶ˿�           		   	   */
/*                                                 */
/*-------------------------------------------------*/

//LED�������ݷ��͸�������

#include "stm32f10x.h"
#include "control.h"
#include "usart.h"
#include "mqtt.h"

extern char *ledFlag;               //�����״̬                   

/*-------------------------------------------------*/
/*�����������ݳ��� length ����                      */
/*��  ������                                       */
/*����ֵ������                 				       */
/*-------------------------------------------------*/
int length(int a)
{
	int i = 1;
	while(a/10 != 0)
	{
		i++;
		a = a/10;
	}
	return i;
}

/*-------------------------------------------------*/
/*�����������Ϳ����豸����                          */
/*��  ������                                       */
/*����ֵ��							               */
/*-------------------------------------------------*/
void Send_Data(void)
{		
	char  head1[3];
	char  temp[50];          	//����һ����ʱ������1,��������ͷ
	char  tempAll[100];       	//����һ����ʱ������2��������������

	int   dataLen;     	  	    //���ĳ���	
	
	memset(temp,       0, 50);  //��ջ�����1
	memset(tempAll,    0, 100); //��ջ�����2
	memset(head1,      0, 3);   //���MQTTͷ                           						 

	sprintf(temp, "{\"ledFlag\":\"%s\"}", ledFlag);//��������
	
	head1[0] = 0x03; //�̶���ͷ
	head1[1] = 0x00; //�̶���ͷ
	head1[2] = strlen(temp); //ʣ�೤��	
	sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);

	dataLen = strlen(temp) + 3;
	printf("%s\r\n", tempAll+3);
	//printf("�ܳ��ȣ�%d\r\n",Init_len);
	MQTT_PublishQs0(Data_TOPIC_NAME, tempAll, dataLen); //������ݣ�������������		
}


