/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            onenet_http功能的头文件              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef _ONENET_HTTP_H
#define _ONENET_HTTP_H

#define SWITCH_DID         "530381067"
#define API_KEY            "N8ljkF=YFpbnOIfDqz3nl8Ixl18="

extern char *ServerIP;         //存放服务器IP或是域名
extern int  ServerPort;        //存放服务器的端口号区
extern char RXbuff[2048];      //接收数据缓冲区
extern char TXbuff[2048];      //发送数据缓冲区

void OneNet_GET(char *device_id);
void OneNet_POST(char *device_id);
	
#endif
