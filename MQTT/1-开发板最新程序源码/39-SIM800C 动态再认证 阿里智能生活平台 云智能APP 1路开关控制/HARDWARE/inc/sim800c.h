/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*             ����SIM800C���ܵ�ͷ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __SIM800C_H
#define __SIM800C_H

#include "usart3.h"	    //������Ҫ��ͷ�ļ�

#define POWER_STA       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)        //PC1�ĵ�ƽ�������ж�800C�ǿ������ǹػ�
#define POWER_KEY(x)    GPIO_WriteBit(GPIOC, GPIO_Pin_0, (BitAction)x)  //PC0����800C�Ŀ��ػ�

#define SIM800C_printf       u3_printf           //����2���� 800C
#define SIM800C_RxCounter    Usart3_RxCounter    //����2���� 800C
#define SIM800C_RX_BUF       Usart3_RxBuff       //����2���� 800C
#define SIM800C_RXBUFF_SIZE  USART3_RXBUFF_SIZE  //����2���� 800C
 
void SIM800C_GPIO_Init(void);
char SIM800C_Power(void);
char SIM800C_SendCmd(char *, int);
char SIM800C_CREG(int);
char SIM800C_CSQ(int timeout);
char SIM800C_CGATT(int timeout);
char SIM800C_ActivateNetwork(void);
char SIM800C_TCPConnect(int);
void SIM800C_TxData(unsigned char *);
char SIM800C_Connect_IoTServer(void);
char SIM800C_HTTPData(int, char *, int);
char SIM800C_HTTPRead(char *, char *, int);

#endif


