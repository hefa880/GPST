#ifndef _MY_FUNCTION_H
#define _MY_FUNCTION_H
#define BIGER 0xaa
#define LITTER 0xa5
#define EQUER  0x55

#define FLASH_TIME  100/*100ms*n*/

#define USE_USART0_DEBUG

//#define ENABLE_DOG
//#define USE_PRINTF
//#define KLX_COIN
#define AA_TO_ZZ
//#define DEBUG_GSM_ANT
//#define DEBUG_GPS_ANT
//#define USE_32768

#ifdef WIN32
#define esPrintChar( ch )    putchar( ch )
#else
    #include "em_usart.h"

    #ifdef USE_USART0_DEBUG
    #define esPrintChar( ch )    USART_Tx( USART0, ch )
    #else
    #define esPrintChar( ch )
    #endif
#endif


extern u8 resetflag;

#define SYSTEM_POWER_ON  0xaa
typedef __packed struct
{
    u8 mydog;/*任务开门狗*/
} STU_POWER;

extern STU_POWER StuPower;




typedef union
{
    struct
    {
        u8    L;
        u8    H;
    } stu;
    u16  total;

} U16STU ;


typedef union
{
    struct
    {
        u8    Low;
        u8 Lower;
        u8 Hight;
        u8    Highter;
    } stu;
    u8 buf[4];
    u32  total;
} U32STU ;
#define UART_FINDING 0xaa
#define UART_FINDED  0x55

u8 * Findbcd(u8 *indata,u16 inlen,u8 *outdata,u16 outlen);
u16 FindString( STRUCT_QUEUE_CTRL *QueueCtrl, u8 *outdata, u16 outlen );

u8 Equre(u8 *buf1,u8* buf2, u16 len);
u8 CalacXORVarity(u8*bufin,u16 len);
u8  DecToString(u16 from,u8*out);
u16 GetArryLen(u8 *buf,u16 maxlen);
void  Mymemcpy(u8*dest,u8 *source ,u16 len);
void Conver_aa_to_55(u8 *indata,u16 inlen,u8 *outbuf,u16*outlen);
u16 strfindzero(u8*buf);
u8 EqureOrNot(u8* dat1,u8* dat2, u16 len);
void WaitToResetSystem(u8 second);
void ResetSystem(void);
u8* Finddate(u8*des,u8 date,u16 maxlen);
void ConverascTohex(u8*asc,u16 asclen,u8*hex);
u8  HexToAscll(u8 In,u8 flag);

void Inithardware(void);
void DateInit(void);
u16   wCptLastDay(u16 Y, u16 M);
void UtcToBeijingDate(u8 *time,u8 hour,u8 minute,u8 inc);
void Memset(u8*data, u8 format ,u16 len );
u8 acstohex(u8 datein);
void ManageMyDog(void);
void FeedTaskDog(void);
void BubbleSort(u16 array[],u16 len);
u8  DecToBcd(u8 in,u8 positive);
void getvarity(u8*varity,u8*date,u16 len);
void myprintf( const char *format, ... );
#endif

