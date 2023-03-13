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

#include "stm32f10x.h"  //包含需要的头文件
#include "main.h"       //包含需要的头文件
#include "delay.h"      //包含需要的头文件
#include "usart1.h"     //包含需要的头文件
#include "usart2.h"     //包含需要的头文件
#include "timer1.h"     //包含需要的头文件
#include "timer2.h"     //包含需要的头文件
#include "timer3.h"     //包含需要的头文件
#include "timer4.h"     //包含需要的头文件
#include "wifi.h"	    //包含需要的头文件
#include "led.h"        //包含需要的头文件
#include "mqtt.h"       //包含需要的头文件
#include "key.h"        //包含需要的头文件
#include "dht11.h"      //包含需要的头文件
#include "iic.h"        //包含需要的头文件

char *CMD1 = "APP+DTSW=1C";     //开关1控制命令，状态翻转，开->关 关->开  
char *CMD2 = "APP+DTSW=2C";     //开关2控制命令，状态翻转，开->关 关->开  
char *CMD3 = "APP+DTSW=DC";     //采集状态翻转，如果现在是采集状态就停止，反之停止状态就开始采集
char *CMD4 = "APP+DTSW=??";     //查询2路开关状态 和 采集状态
char  Data_flag = 0;            //是否处于采集状态， 0：停止采集  1：采集中

int main(void) 
{	
	Delay_Init();                   //延时功能初始化              
	Usart1_Init(9600);              //串口1功能初始化，波特率9600
	Usart2_Init(115200);            //串口2功能初始化，波特率115200	
	TIM4_Init(500,7200);            //TIM4初始化，定时时间 500*7200*1000/72000000 = 50ms
	LED_Init();	                    //LED初始化
	KEY_Init();                     //按键初始化
	IIC_Init();                     //初始化IIC接口
	DHT11_Init();                   //初始化DHT11	
	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
	IoT_Parameter_Init();	        //初始化云IoT平台MQTT服务器的参数	
	
	while(1)                        //主循环
	{		
		/*--------------------------------------------------------------------*/
		/*   Connect_flag=1同服务器建立了连接,我们可以发布数据和接收推送了    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     处理发送缓冲区数据                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if成立的话，说明发送缓冲区有数据了
				//3种情况可进入if
				//第1种：0x10 连接报文
				//第2种：0x82 订阅报文，且ConnectPack_flag置位，表示连接报文成功
				//第3种：SubcribePack_flag置位，说明连接和订阅均成功，其他报文可发
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("发送数据:0x%x\r\n",MQTT_TxDataOutPtr[2]);  //串口提示信息
					MQTT_TxData(MQTT_TxDataOutPtr);                       //发送数据
					MQTT_TxDataOutPtr += TBUFF_UNIT;                      //指针下移
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //如果指针到缓冲区尾部了
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //指针归位到缓冲区开头
				} 				
			}//处理发送缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理接收缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if成立的话，说明接收缓冲区有数据了														
				u1_printf("接收到数据:");
				/*-----------------------------------------------------*/
				/*                    处理CONNACK报文                  */
				/*-----------------------------------------------------*/				
				//if判断，如果第一个字节是0x20，表示收到的是CONNACK报文
				//接着我们要判断第4个字节，看看CONNECT报文是否成功
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : u1_printf("CONNECT报文成功\r\n");                            //串口输出信息	
								    ConnectPack_flag = 1;                                        //CONNECT报文成功，订阅报文可发
									break;                                                       //跳出分支case 0x00                                              
						case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");     //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x01   
						case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n"); //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x02 
						case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");         //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x03
						case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");   //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x04
						case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");               //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x05 		
						default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");             //串口输出信息 
									Connect_flag = 0;                                            //Connect_flag置零，重启连接					
									break;                                                       //跳出分支case default 								
					}				
				}			
				//if判断，第一个字节是0x90，表示收到的是SUBACK报文
				//接着我们要判断订阅回复，看看是不是成功
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00 :
						case 0x01 : u1_printf("订阅成功\r\n");            //串口输出信息
							        SubcribePack_flag = 1;                //SubcribePack_flag置1，表示订阅报文成功，其他报文可发送
									Ping_flag = 0;                        //Ping_flag清零
   								    TIM3_ENABLE_30S();                    //启动30s的PING定时器	
									LED_DATA_State(); 						
									break;                                //跳出分支                                             
						default   : u1_printf("订阅失败，准备重启\r\n");  //串口输出信息 
									Connect_flag = 0;                     //Connect_flag置零，重启连接
									break;                                //跳出分支 								
					}					
				}
				//if判断，第一个字节是0xD0，表示收到的是PINGRESP报文
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					u1_printf("PING报文回复\r\n"); 		  //串口输出信息 
					if(Ping_flag==1){                     //如果Ping_flag=1，表示第一次发送
						 Ping_flag = 0;    				  //要清除Ping_flag标志
					}else if(Ping_flag>1){ 				  //如果Ping_flag>1，表示是多次发送了，而且是2s间隔的快速发送
						Ping_flag = 0;     				  //要清除Ping_flag标志
						TIM3_ENABLE_30S(); 				  //PING定时器重回30s的时间
					}				
				}	
				//if判断，如果第一个字节是0x30，表示收到的是服务器发来的推送数据
				//我们要提取控制命令
				else if((MQTT_RxDataOutPtr[2]==0x30)){ 
					u1_printf("服务器等级0推送\r\n"); 		   //串口输出信息 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //处理等级0推送数据
				}				
								
				MQTT_RxDataOutPtr +=RBUFF_UNIT;                     //指针下移
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //如果指针到缓冲区尾部了
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //指针归位到缓冲区开头                        
			}//处理接收缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理命令缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                             //if成立的话，说明命令缓冲区有数据了			       
				u1_printf("命令:%s\r\n",&MQTT_CMDOutPtr[2]);                 //串口输出信息
				if(!memcmp(&MQTT_CMDOutPtr[2],CMD1,strlen(CMD1))){           //判断指令，如果是CMD1
					LED1_OUT(!LED1_IN_STA);                                  //开关1状态翻转 如果点亮就熄灭，反之如果熄灭就点亮		
					LED_DATA_State();	                                     //判断2路开关状态和采集状态，并发布给服务器
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD2,strlen(CMD2))){     //判断指令，如果是CMD2
					LED2_OUT(!LED2_IN_STA);                                  //开关2状态翻转 如果点亮就熄灭，反之如果熄灭就点亮		
					LED_DATA_State();	                                     //判断2路开关状态和采集状态，并发布给服务器
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD3,strlen(CMD3))){     //判断指令，如果是CMD3
					if(Data_flag==0){                                        //如果，Data_flag等于0，表示当前处于停止采集状态，我们开始采集
						Data_flag = 1;                                       //Data_flag置1，表示处于采集状态了
					    TIM2_ENABLE_10S();                                   //定时器2，,10s间隔采集温湿度
					}else{                                                   //反之，Data_flag等于1，表示当前处于采集状态，我们停止
						Data_flag = 0;                                       //Data_flag置0，表示处于停止状态了
						TIM_Cmd(TIM2,DISABLE);                               //关TIM2 
					}
					LED_DATA_State();	                                     //判断2路开关状态和采集状态，并发布给服务器
				}else if(!memcmp(&MQTT_CMDOutPtr[2],CMD4,strlen(CMD4))){     //判断指令，如果是CMD4
					LED_DATA_State();	                                     //判断2路开关状态和采集状态，并发布给服务器							                                                     //不做处理，后面会发送状态
				}else u1_printf("未知指令\r\n");                             //串口输出信息	
				
				MQTT_CMDOutPtr += CBUFF_UNIT;                             	 //指针下移
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //如果指针到缓冲区尾部了
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //指针归位到缓冲区开头				
			}//处理命令缓冲区数据的else if分支结尾	
		}//Connect_flag=1的if分支的结尾
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0同服务器断开了连接,我们要重启连接服务器         */
		/*--------------------------------------------------------------------*/
		else{ 
			u1_printf("需要连接服务器\r\n");                 //串口输出信息
			TIM_Cmd(TIM4,DISABLE);                           //关闭TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //关闭TIM3  
			WiFi_RxCounter=0;                                //WiFi接收数据量变量清零                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //清空WiFi接收缓冲区 
			if(WiFi_Connect_IoTServer()==0){   			     //如果WiFi连接云服务器函数返回0，表示正确，进入if
				u1_printf("建立TCP连接成功\r\n");            //串口输出信息
				Connect_flag = 1;                            //Connect_flag置1，表示连接成功	
				WiFi_RxCounter=0;                            //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      //清空WiFi接收缓冲区 
				MQTT_Buff_Init();                            //初始化发送缓冲区                    
			}				
		}
	}
}
/*-------------------------------------------------*/
/*函数名：判断2路开关和采集状态，并发布给服务器    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_DATA_State(void)
{
	char temp[20];                  		//定义一个临时缓冲区
	
	memset(temp,0,20);             		    //清空临时缓冲区
	sprintf(temp,"+DTSW:");          		//构建回复数据		
	if(LED1_IN_STA)  temp[6] = '0';  		//如果LED1是高电平，说明是熄灭状态，开关1状态位置0
	else             temp[6] = '1';			//反之，说明是点亮状态，开关1状态位置1	
	if(LED2_IN_STA)  temp[7] = '0';  		//如果LED2是高电平，说明是熄灭状态，开关2状态位置0
	else             temp[7] = '1';			//反之，说明是点亮状态，开关2状态位置1
	if(Data_flag==0) temp[8] = '0';         //如果，Data_flag等于0，表示当前处于停止采集状态，状态位置0
	else             temp[8] = '1';         //反之，Data_flag等于1，表示当前处于采集状态，状态位置1
	MQTT_PublishQs0(P_TOPIC_NAME,temp,9);   //添加数据，发布给服务器	
}
