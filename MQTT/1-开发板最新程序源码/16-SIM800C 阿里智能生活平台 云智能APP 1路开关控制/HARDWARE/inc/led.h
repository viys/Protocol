/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��LED���ܵ�ͷ�ļ�                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LED_H
#define __LED_H

#define LED1_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_1, (BitAction)x)  //PB1����LED1������PB1�ĵ�ƽ�����Ե���Ϩ��LED1
#define LED2_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_0, (BitAction)x)  //PB0����LED2������PB0�ĵ�ƽ�����Ե���Ϩ��LED2

#define LED1_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1) //PB1����LED1,��ȡ��ƽ״̬�������ж�LED1�ǵ�������Ϩ��
#define LED2_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0) //PB0����LED2,��ȡ��ƽ״̬�������ж�LED2�ǵ�������Ϩ��
  
#define LED1_ON       GPIO_ResetBits(GPIOB, GPIO_Pin_1)         //PB1����LED1��������������PB1��ƽ������LED1
#define LED1_OFF      GPIO_SetBits(GPIOB, GPIO_Pin_1)           //PB1����LED1��������������PB1��ƽ��Ϩ��LED1
 
#define LED2_ON       GPIO_ResetBits(GPIOB, GPIO_Pin_0)         //PB0����LED2��������������PB0��ƽ������LED2
#define LED2_OFF      GPIO_SetBits(GPIOB, GPIO_Pin_0)           //PB0����LED2��������������PB0��ƽ��Ϩ��LED2

void LED_Init(void);               //��ʼ��	
void LED_AllOn(void);              //��������LED
void LED_AllOff(void);             //Ϩ������LED

#endif
