/*-----------------------------------------------------*/
/*              超纬电子STM32系列开发板                */
/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*CORE     ：包含STM32的核心程序，官方提供，我们不修改 */
/*STLIB    ：官方提供的库文件，我们不修改              */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"   //包含需要的头文件
#include "main.h"        //包含需要的头文件
#include "delay.h"       //包含需要的头文件
#include "usart1.h"      //包含需要的头文件
#include "iic.h"         //包含需要的头文件
#include "24c02.h" 		 //包含需要的头文件
#include "usart2.h"      //包含需要的头文件
#include "timer1.h"      //包含需要的头文件
#include "timer3.h"      //包含需要的头文件
#include "timer2.h"      //包含需要的头文件
#include "timer4.h"      //包含需要的头文件
#include "wifi.h"	     //包含需要的头文件
#include "led.h"         //包含需要的头文件
#include "key.h"         //包含需要的头文件
#include "dht11.h"  	 //包含需要的头文件
#include "onenet_http.h" //包含需要的头文件

char r_flag = 0;         //8266状态标志 0：需要复位或重新复位  1：正常状态
char s_flag = 0;         //查询开关标志 0：没到查询时间  1：间隔3s的时间到，准备查询    2：报文发送了，等待服务器回复
char d_flag = 0;         //上传数据标志 0：没到上传时间  1：间隔20s的时间到，准备上传   2：报文发送了，等待服务器回复

int main(void) 
{	
	char *presult;
	
	Delay_Init();                   //延时功能初始化              
	Usart1_Init(9600);              //串口1功能初始化，波特率9600
	Usart2_Init(115200);            //串口2功能初始化，波特率115200	
	TIM4_Init(300,7200);            //TIM4初始化，定时时间 300*7200*1000/72000000 = 30ms
	LED_Init();	                    //LED初始化
	KEY_Init();                     //按键初始化
	IIC_Init();                     //初始化IIC接口
	DHT11_Init();                   //初始化DHT11	
	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
	
	while(1){                      		
		if(r_flag == 0){                                    //如果r_flag标志等于0，表示需要复位重置8266模块
			if(WiFi_Init()==0){                             //调用WiFi_Init函数，返回0表示复位重置成功
				u1_printf("ESP8266状态初始化正常\r\n");     //串口输出信息
				r_flag = 1;                                 //r_flag标志置位，表示8266状态正常
				TIM1_ENABLE_20S();                          //定时器1 20s定时 发送温湿度
				TIM2_ENABLE_3S();	                        //定时器2 3s定时  查询开关状态
			}
		}
		
		if(Usart2_RxCompleted==1){                                          //如果标志Usart2_RxCompleted置位，表示接收到数据
			Usart2_RxCompleted = 0;                                         //清除Usart2_RxCompleted置位标志
			TIM_Cmd(TIM3, DISABLE);                                         //关闭TIM3定时器
			if(strstr(&RXbuff[2],"200 OK")){                                //搜索200 OK 表示报文正确，进入if	
				if(strstr(&RXbuff[2],"\"errno\":0")){                       //搜索"errno":0 表示数据流操作正确，进入if
					if(strstr(&RXbuff[2],"datastreams")){                   //搜索 datastreams 表示获取的数据流，是GET报文 查询开关状态
						presult = strstr(&RXbuff[2],"\"id\":\"switch_1\""); //搜索 "id":"switch_1" 查询开关1状态
						if(presult!=NULL){                                  //如果搜索到了，进入if
							if(*(presult-4) == '0') LED1_OFF;               //如果是0，关闭LED1
							else LED1_ON;                                   //反之是1，打开LED1							
						}
						presult = strstr(&RXbuff[2],"\"id\":\"switch_2\""); //搜索 "id":"switch_2" 查询开关2状态
						if(presult!=NULL){                                  //如果搜索到了，进入if
							if(*(presult-4) == '0') LED2_OFF;               //如果是0，关闭LED2
							else LED2_ON;                                   //反之是1，打开LED2							
						}
						presult = strstr(&RXbuff[2],"\"id\":\"switch_3\""); //搜索 "id":"switch_3" 查询开关1状态
						if(presult!=NULL){                                  //如果搜索到了，进入if
							if(*(presult-4) == '0') LED3_OFF;               //如果是0，关闭LED3
							else LED3_ON;                                   //反之是1，打开LED3							
						}
						presult = strstr(&RXbuff[2],"\"id\":\"switch_4\""); //搜索 "id":"switch_4" 查询开关1状态
						if(presult!=NULL){                                  //如果搜索到了，进入if
							if(*(presult-4) == '0') LED4_OFF;               //如果是0，关闭LED4
							else LED4_ON;                                   //反之是1，打开LED4							
						}
						u1_printf("查询开关状态正确\r\n");                  //串口提示数据
					}else{								                    //反之没搜索到 datastreams 表示是POST报文
						u1_printf("上报温湿度数据正确\r\n");                //串口提示数据
					}
				}
			}else{
				u1_printf("报文错误\r\n");          //串口提示数据
				u1_printf("%s\r\n",&RXbuff[2]);     //串口提示数据
			}
			if(WiFi_Close(50)){                                  //准备关闭连接，超时单位100ms，超时时间5s
				TIM_Cmd(TIM1, DISABLE);                          //关闭TIM1定时器
				TIM_Cmd(TIM2, DISABLE);                          //关闭TIM2定时器
				TIM_Cmd(TIM3, DISABLE);                          //关闭TIM3定时器			
				r_flag = s_flag = d_flag = 0;                    //标志清除
                u1_printf("关闭连接失败，重置ESP8266\r\n");      //串口提示数据
			}else{
				u1_printf("关闭连接成功\r\n");                   //串口提示数据
				if(s_flag==2){                                   //如果s_flag等于2，表示数据是查询开关的
					s_flag = 0;                                  //清除标志位
					TIM2_ENABLE_3S();	                         //开定时器2 3s定时  查询开关状态
					TIM_Cmd(TIM1, ENABLE);                       //重开TIM1定时器
				}
				if(d_flag==2){                                   //如果d_flag等于2，表示数据是查询开关的
					d_flag = 0;                                  //清除标志位
					TIM1_ENABLE_20S();	                         //开定时器1 20s定时 发送温湿度
					TIM_Cmd(TIM2, ENABLE);                       //重开TIM2定时器
				}
			}			
		}
		
		if(s_flag==1){                           //如果s_flag标志置位，表示3s间隔时间到，进入if准备查询开关状态			                   
			s_flag = 2;                          //s_flag等于2，等待服务器回复
			u1_printf("准备连接服务器\r\n");     //串口提示数据
			if(WiFi_Connect_Server(50)){         //连接服务器，100ms超时单位，总计5s超时时间							
				TIM_Cmd(TIM1, DISABLE);          //关闭TIM1定时器
				TIM_Cmd(TIM2, DISABLE);          //关闭TIM2定时器
				TIM_Cmd(TIM3, DISABLE);          //关闭TIM3定时器
				Connect_flag = 0;                //连接成功标志清除
				r_flag = s_flag = d_flag = 0;    //标志清除
				u1_printf("连接服务器失败\r\n"); //串口提示数据
			}else{                               //返回0，进入else分支，表示连接服务器成功
				u1_printf("连接服务器成功\r\n"); //串口提示数据
				WiFi_RxCounter=0;                           //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //清空WiFi接收缓冲区 
				Connect_flag = 1;                //连接成功标志置位
				OneNet_GET(SWITCH_DID);          //构建查询开关状态的报文
				WiFi_printf(TXbuff);             //把构建好的报文发给服务器
				TIM3_ENABLE_5S();                //开定时器3 5s的定时 如果5s内服务器 没有数据来，要重新复位8266			
			}
		} 
		
		if(d_flag==1){                           //如果d_flag标志置位，表示20s间隔时间到，进入if准备上报温湿度数据		                   
			d_flag = 2;                          //d_flag等于2，等待服务器回复
			u1_printf("准备连接服务器\r\n");     //串口提示数据
			if(WiFi_Connect_Server(50)){         //连接服务器，100ms超时单位，总计5s超时时间
				TIM_Cmd(TIM1, DISABLE);          //关闭TIM1定时器
				TIM_Cmd(TIM2, DISABLE);          //关闭TIM2定时器
				TIM_Cmd(TIM3, DISABLE);          //关闭TIM3定时器
				Connect_flag = 0;                //连接成功标志清除
				r_flag = s_flag = d_flag = 0;    //标志清除
				u1_printf("连接服务器失败\r\n"); //串口提示数据
			}else{                               //返回0，进入else分支，表示连接服务器成功
				u1_printf("连接服务器成功\r\n"); //串口提示数据
				WiFi_RxCounter=0;                           //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //清空WiFi接收缓冲区 
				Connect_flag = 1;                //连接成功标志置位
				OneNet_POST(TEMPHUMI_DID);       //构建查上报温湿度数据的报文
				WiFi_printf(TXbuff);             //把构建好的报文发给服务器
				TIM3_ENABLE_5S();                //开定时器3 5s的定时 如果5s内服务器 没有数据来，要重新复位8266				
			}
		} 
	}
}
