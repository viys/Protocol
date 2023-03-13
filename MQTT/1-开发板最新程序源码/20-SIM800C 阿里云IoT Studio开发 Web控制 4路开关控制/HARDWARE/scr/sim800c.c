/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            操作SIM800C功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "sim800c.h"	//包含需要的头文件
#include "delay.h"	    //包含需要的头文件
#include "usart1.h"	    //包含需要的头文件
#include "mqtt.h"	    //包含需要的头文件
/*-------------------------------------------------*/
/*函数名：初始化SIM800C的GPIO                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SIM800C_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      //定义一个设置IO端口参数的结构体
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);   //使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                 //准备设置PC0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		  //推免输出方式
	GPIO_Init(GPIOC, &GPIO_InitStructure);            		  //设置PC0
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;                //准备设置PC1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   		  //下拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);            		  //设置PC1
}

/*-------------------------------------------------*/
/*函数名：SIM800C开机                              */
/*参  数：无                                       */
/*返回值：0:开机成功  1:开机失败                   */
/*-------------------------------------------------*/
char SIM800C_Power(void)
{
	if(POWER_STA==0){   							           //如果PC1是低电平，表示目前处于关机状态		
		u1_printf("\r\n目前处于关机状态，准备开机\r\n");       //串口输出信息
		POWER_KEY(1);                                          //先拉高PC0
		Delay_Ms(1500);										   //延时
		POWER_KEY(0);										   //再拉低PC0，开机
	}else{                                                     //反之PC1是高电平，表示目前处于开机状态
		u1_printf("\r\n目前处于开机状态，准备重启\r\n");       //串口输出信息		
		POWER_KEY(1);                                          //先拉高PC0
		Delay_Ms(1500);										   //延时
		POWER_KEY(0);										   //再拉低PC0，关机
		Delay_Ms(1200);                                        //间隔
		POWER_KEY(1);                                          //先拉高PC0
		Delay_Ms(1500);										   //延时
		POWER_KEY(0);										   //再拉低PC0，开机，完成重启
	}	
	Delay_Ms(2000);			                                   //延时，等待状态稳定
	if(POWER_STA){                                             //如果PC1是低电平，表示开机或是重启成功
		u1_printf("开机成功\r\n");                             //串口输出信息
	}else{												       //反之PC1是高电平，表示开始或是重启失败
		u1_printf("开机失败\r\n"); 							   //串口输出信息
		return 1;                                              //返回1，表示失败
	}	
	return 0;                                                  //返回0，表示成功
}
/*-------------------------------------------------*/
/*函数名：发送设置指令                             */
/*参  数：cmd：指令                                */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char SIM800C_SendCmd(char *cmd, int timeout)
{
	SIM800C_RxCounter=0;                           //800C接收数据量变量清零                        
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空800C接收缓冲区 
	SIM800C_printf("%s\r\n",cmd);                  //发送指令
	while(timeout--){                              //等待超时时间到0
		Delay_Ms(100);                             //延时100ms
		if(strstr(SIM800C_RX_BUF,"OK"))            //如果接收到OK表示指令成功
			break;       						   //主动跳出while循环
		u1_printf("%d ",timeout);                  //串口输出现在的超时时间
	}
	u1_printf("\r\n");                             //串口输出信息
	if(timeout<=0)return 1;                        //如果timeout<=0，说明超时时间到了，也没能收到OK，返回1
	else return 0;		         				   //反之，表示正确，说明收到OK，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：CREG指令，查询注册上网络                 */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char SIM800C_CREG(int timeout)
{
	while(timeout--){                                  //等待超时时间到0
		SIM800C_RxCounter=0;                           //800C接收数据量变量清零                        
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空800C接收缓冲区 
		SIM800C_printf("AT+CREG?\r\n");                //发送指令，查看是否注册上
		Delay_Ms(1000);                                //延时1s
		if(strstr(SIM800C_RX_BUF,"+CREG: 0,1"))        //如果接收到 +CREG: 0,1 表示注册上
			break;       						       //主动跳出while循环
		u1_printf("%d ",timeout);                      //串口输出现在的超时时间
	}
	u1_printf("\r\n");                                 //串口输出信息
	if(timeout<=0)return 1;                            //如果timeout<=0，说明超时时间到了，也没能收到+CREG: 0,1，返回1
	else return 0;		         				       //反之，表示正确，说明收到+CREG: 0,1，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：CSQ指令，查询信号                        */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char SIM800C_CSQ(int timeout)
{
	char  temp1[10],temp2[10] ,temp3[10];                    //定义3个临时缓冲区  
	
	SIM800C_RxCounter=0;                          			 //800C接收数据量变量清零                        
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE); 			 //清空800C接收缓冲区 
	SIM800C_printf("AT+CSQ\r\n");            	    		 //发送指令,查询信号质量
	while(timeout--){                                        //等待超时时间到0
		Delay_Ms(100);                            			 //延时100ms
		if(strstr(SIM800C_RX_BUF,"+CSQ:")){		             //接收到 +CSQ: 表示指令成功
			sscanf(SIM800C_RX_BUF,"%[^ ] %[^,],%[^,]",temp1,temp2,temp3);   //格式化搜索，信号质量
			u1_printf("信号强度范围0~31,99表示无信号\r\n");  //串口输出信息
			u1_printf("信号强度：%s\r\n",temp2);             //串口输出信息
			break;       						             //主动跳出while循环
		}
		u1_printf("%d ",timeout);                            //串口输出现在的超时时间
	}
	u1_printf("\r\n");                                       //串口输出信息
	if(timeout<=0)return 1;                                  //如果timeout<=0，说明超时时间到了，也没能收到+CSQ:，返回1
	else return 0;		         				             //反之，表示正确，说明收到+CSQ:，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：CGATT指令，查询是否附着GPRS              */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char SIM800C_CGATT(int timeout)
{
	while(timeout--){                                  //等待超时时间到0
		SIM800C_RxCounter=0;                           //800C接收数据量变量清零                        
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空800C接收缓冲区 
		SIM800C_printf("AT+CGATT?\r\n");               //发送指令，查询是否附着GPRS
		Delay_Ms(1000);                                //延时1s
		if(strstr(SIM800C_RX_BUF,"+CGATT: 1"))		   //接收到 +CGATT: 1 表示指令成功
			break;       						       //主动跳出while循环
		u1_printf("%d ",timeout);                      //串口输出现在的超时时间
	}
	u1_printf("\r\n");                                 //串口输出信息
	if(timeout<=0)return 1;                            //如果timeout<=0，说明超时时间到了，也没能收到+CGATT: 1，返回1
	else return 0;		         				       //反之，表示正确，说明收到+CGATT: 1，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：激活数据网络                             */
/*参  数：无                                       */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char SIM800C_ActivateNetwork(void)
{	                                                              //反之如果time小于timeout，表示正确，通过break主动跳出while
	u1_printf("等待激活GPRS... ...\r\n");                         //串口输出信息
	if(SIM800C_SendCmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",60)){  //发送激活GPRS命令，超时时间6s
		u1_printf("激活GPRS失败\r\n");                            //串口输出信息
		return 1;                                                 //如果if成立，表示超时了，返回1
	}else{                                                        //反之，表示指令正确
		u1_printf("激活GPRS成功\r\n");                            //串口输出信息
		u1_printf("等待设置接入点... ...\r\n");                   //串口输出信息
		if(SIM800C_SendCmd("AT+SAPBR=3,1,\"APN\",\"CMIOT\"",60)){ //发送设置接入点命令，超时时间6s
			u1_printf("设置接入点失败\r\n");                      //串口输出信息
			return 2;                                             //如果if成立，表示超时了，返回2
		}else{													  //反之，表示指令正确
			u1_printf("设置接入点成功\r\n");                      //串口输出信息
			u1_printf("等待打开GPRS网络... ...\r\n");             //串口输出信息
			if(SIM800C_SendCmd("AT+SAPBR=1,1",60)){               //发送打开网络命令，超时时间6s
				u1_printf("打开GPRS网络失败\r\n");                //串口输出信息
				return 3;                                         //如果if成立，表示超时了，返回3
			}else u1_printf("打开GPRS网络成功\r\n");              //串口输出信息	
		}
	}	
	return 0;
}
/*-------------------------------------------------*/
/*函数名：同服务器建立TCP连接                      */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char SIM800C_TCPConnect(int timeout)
{
	SIM800C_RxCounter=0;                           //800C接收数据量变量清零                        
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空800C接收缓冲区 
	SIM800C_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",ServerIP,ServerPort); //发送指令，连接服务器
	while(timeout--){                              //等待超时时间到0
		Delay_Ms(1000);                            //延时1s
		if(strstr(SIM800C_RX_BUF,"CONNECT"))       //如果接收到CONNECT表示连接成功
			break;       						   //主动跳出while循环
		u1_printf("%d ",timeout);                  //串口输出现在的超时时间
	}
	u1_printf("\r\n");                             //串口输出信息
	if(timeout<=0)return 1;                        //如果timeout<=0，说明超时时间到了，也没能收到CONNECT，返回1
	else return 0;		         				   //反之，表示正确，说明收到CONNECT，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：同服务器断开TCP连接                      */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char SIM800C_TCPClose(int timeout)
{
	SIM800C_RxCounter=0;                           //800C接收数据量变量清零                        
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空800C接收缓冲区 
	SIM800C_printf("AT+CIPCLOSE\r\n");             //发送指令，断开连接
	while(timeout--){                              //等待超时时间到0
		Delay_Ms(1000);                            //延时1s
		if(strstr(SIM800C_RX_BUF,"CLOSE OK"))      //如果接收到CLOSE OK表示连接成功
			break;       						   //主动跳出while循环
		u1_printf("%d ",timeout);                  //串口输出现在的超时时间
	}
	u1_printf("\r\n");                             //串口输出信息
	if(timeout<=0)return 1;                        //如果timeout<=0，说明超时时间到了，也没能收到CLOSE OK，返回1
	else return 0;		         				   //反之，表示正确，说明收到CLOSE OK，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：800C连接物联网云服务器                   */
/*参  数：data：数据                               */
/*返回值：无                                       */
/*-------------------------------------------------*/
char SIM800C_Connect_IoTServer(void)
{
	char i;                         //定义一个变量，用于for循环
	
	SIM800C_GPIO_Init();            //控制800C的IO初始化
	if(SIM800C_Power()){            //控制800C开机或重启,如果返回1，说明开机重启失败，准备重启
		return 1;                   //返回1
	}

	u1_printf("请等待注册上网络... ...\r\n");        //串口输出信息
	if(SIM800C_CREG(30)){                            //等待注册上网络，超时单位1s，超时时间30s
		u1_printf("注册网络超时，准备重启\r\n");     //串口输出信息
		return 2;                                    //返回2
	}else u1_printf("注册上网络\r\n");               //串口输出信息
	
	if(SIM800C_CSQ(60)){                             //查询信号强度，超时单位100ms，超时时间6s
		u1_printf("查询信号强度超时，准备重启\r\n"); //串口输出信息
		return 3;                                    //返回3	
	}
	
	u1_printf("请等待附着上GPRS... ...\r\n");        //串口输出信息
	if(SIM800C_CGATT(30)){                           //查询附着GPRS，超时单位1s，超时时间30s
		u1_printf("查询附着GPRS超时，准备重启\r\n"); //串口输出信息
		return 4;                                    //返回4	
	}else u1_printf("附着上GPRS\r\n");               //串口输出信息
	
	u1_printf("请等待设置单链接... ...\r\n");        //串口输出信息
	if(SIM800C_SendCmd("AT+CIPMUX=0",60)){           //设置单链接模式，超时单位100ms，超时时间6s	
		u1_printf("设置单链接失败，准备重启\r\n");   //串口输出信息
		return 5;                                    //返回5	
	}else u1_printf("设置单链接模式成功\r\n");       //串口输出信息

	u1_printf("请等待设置透传... ...\r\n");         //串口输出信息
	if(SIM800C_SendCmd("AT+CIPMODE=1",60)){         //设置透传模式，超时单位100ms，超时时间6s		
		u1_printf("设置透传失败，准备重启\r\n");    //串口输出信息
		return 6;                                   //返回6
	}else u1_printf("设置透传成功\r\n");            //串口输出信息   
	
	if(SIM800C_ActivateNetwork()){                   //准备激活网络
		u1_printf("激活网络失败，准备重启\r\n");     //串口输出信息
		return 7;                                    //返回7
	}
	Delay_Ms(500);                                   //适当延时
	for(i=0;i<3;i++)                                 //重试3次                                   
	{
		u1_printf("请等待连接上服务器... ...\r\n");  //串口输出信息
		if(SIM800C_TCPConnect(10)){                  //同服务器建立TCP连接，超时单位1s，超时时间10s 
			u1_printf("连接失败，准备再次连接\r\n"); //串口输出信息
			Delay_Ms(500);                           //适当延时
			if(SIM800C_TCPClose(10)){                //准备再次连接前要先发送关闭指令，超时单位1s，超时时间10s 
				u1_printf("连接异常，准备重启\r\n"); //串口输出信息
				Delay_Ms(500);                       //适当延时
				return 8;                            //返回7
			}
		}else return 0;                              //正确，返回0
	}
	u1_printf("连接不上服务器，准备重启\r\n");       //串口输出信息
	return 9;                                        //返回9
}
