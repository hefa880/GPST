#ifndef usart_H
#define usart_H

#include "includes.h"
extern u8 debug;
#define DEBUGGSM 0xaa
#define DEBUGGPS DEBUGGSM+1
#define DEBUGADD DEBUGGPS+1
#define DEBUGWIF DEBUGADD+1
#define DEBUGADC DEBUGWIF+1
#define DEBUGTIM DEBUGADC+1
#define DEBUGNET DEBUGTIM+1
#define DEBUGAGP DEBUGNET+1
#define DEBUGANT DEBUGAGP+1/*调试gps天线*/
#define POWER_ON 1
#define POWER_OFF  0
#define DebugUart_REC_QUEUE_LENGTH  1124
#include "queue.h"
extern STRUCT_QUEUE_CTRL             DebugUartRxQueueCtrl;					//gps接收队列控制结构体
extern volatile u8 DebugUartRxQueueBuf[DebugUart_REC_QUEUE_LENGTH];	//gps接收队列缓存
#define usart1_buf_len     1124
typedef __packed struct
{
    u8 updateflag;
    u8 Overtime;/*22s超时退出*/
    u8   usart1_receive_buf[usart1_buf_len];
} usart1_stu;
extern usart1_stu Usart1Stu;




void DebugUartSend(u8 *in,u16 len);

void initLeuart(LEUART_TypeDef *leuart,u32 baudrate,u8 powerflag);
void UartSendStr(LEUART_TypeDef *leuart,u8 * str,u16 len);
int putchar(int ch);
void DebugProcess(u8 datein);
void uartSetup(USART_TypeDef *usart, uint32_t board,u8 enable);
void DebugUartTask(void);
void ana_message(u8 *rxdate,u16 message_len);
void DebugUartUpdataSendPackegTurn(void);
void DebugUartUpdataSendAck(u8 *idbody,u8 result);
void DebugUartUpdataPackegToUsart(u8  *Mydate,u16 datelen);
void PrintUsart(u8 ch);
void TaskUsbSend(void);
void UsbSendTobuf(u8*datain,u16 len);

#endif


