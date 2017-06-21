#ifndef _GSM_H
#define _GSM_H

#define AT_WAIT 0xaa
#define AT_ERROR 0x55
#define AT_OK    0xa5

#define AT_STATE_IDLE  0xaa
#define AT_STATE_BUSY   0x55


#define MaxRevLen 1124

#define FIND_STR_MAX_LEN 47


#define CHAP_MODE 0xaa
#define PAP_MODE 0x55
#define EMPTY_MODE 0

typedef struct
{
     u16 waittime;
    u8 times;
    u8 AtState;
     volatile u8 rev[MaxRevLen];
    u8 AtStu;/*初始化为0*/

    u8 findstrstu;
    u8 findstrlen;
    u8 findstr[FIND_STR_MAX_LEN];
} STRUCT_ATCOMMAND;

extern volatile STRUCT_ATCOMMAND  STU_AtCommand;

u8  ProcessGsmQueue(void(*revnetfunction)(u8 indate));/*主函数处理*/
void RevGsmQueue(u8 indate);/*中断接收*/
void InitGsmQueue(void);/*队列初始化*/
u8  SendAtCommand(u8 *AtCommend ,u16 len,u8* atflag,u8 times,u8 waittime,void(*GsmSendDate)(u8*sendstr,u16 len));/*发送AT指令*/

#endif


