/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*          实现各种中断服务函数的源文件           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //包含需要的头文件
#include "stm32f10x_it.h"  //包含需要的头文件
#include "main.h"          //包含需要的头文件
#include "delay.h"         //包含需要的头文件
#include "usart1.h"        //包含需要的头文件
#include "usart2.h"        //包含需要的头文件
#include "timer4.h"        //包含需要的头文件
#include "wifi.h"	       //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：串口2接收中断函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)   
{                      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //如果USART_IT_RXNE标志置位，表示有数据到了，进入if分支
		if(UDP_flag==0){                                    //如果UDP_flag等于0，当前初始化UDP还未成功，处于指令配置状态
			if(USART2->DR){                                 //处于指令配置状态时，非零值才保存到缓冲区	
				Usart2_RxBuff[Usart2_RxCounter]=USART2->DR; //保存到缓冲区	
				Usart2_RxCounter ++;                        //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
			}		
		}else{		                                        //反之UDP_flag等于1，初始化UDP成功
			Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;   //把接收到的数据保存到Usart2_RxBuff中				
			if(Usart2_RxCounter == 0){    					//如果Usart2_RxCounter等于0，表示是接收的第1个数据，进入if分支				
				TIM_Cmd(TIM4,ENABLE); 
			}else{                        					//else分支，表示果Usart2_RxCounter不等于0，不是接收的第一个数据
				TIM_SetCounter(TIM4,0);  
			}	
			Usart2_RxCounter ++;         				    //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
		}		
	}
}
/*-------------------------------------------------*/
/*函数名：定时器4中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){                //如果TIM_IT_Update置位，表示TIM4溢出中断，进入if	
		Usart2_RxCompleted = 1;                                       //串口2接收完成标志位置位
		memcpy(&Data_buff[2],Usart2_RxBuff,Usart2_RxCounter);         //拷贝数据
		Data_buff[0] = WiFi_RxCounter/256;                            //记录接收的数据量		
		Data_buff[1] = WiFi_RxCounter%256;                            //记录接收的数据量
		Data_buff[WiFi_RxCounter+2] = '\0';                           //加入结束符
		WiFi_RxCounter=0;                                             //清零计数值
		TIM_Cmd(TIM4, DISABLE);                        				  //关闭TIM4定时器
		TIM_SetCounter(TIM4, 0);                        			  //清零定时器4计数器
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     			  //清除TIM4溢出中断标志 	
	}
}
/*-------------------------------------------------*/
/*函数名：不可屏蔽中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：硬件出错后进入的中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：内存管理中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：预取指失败，存储器访问失败中断处理函数   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：未定义的指令或非法状态处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：软中断，SWI 指令调用的处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*函数名：调试监控器处理函数                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*函数名：可挂起的系统服务处理函数                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
}

/*-------------------------------------------------*/
/*函数名：SysTic系统嘀嗒定时器处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SysTick_Handler(void)
{
 
}
