
#ifndef FLASH_H
#define FLASH_H


#define S_4__K  4096
#define S_2__K  2048
#define S_64__K   65536
#define PAGE_SIZE  S_2__K
#define S_16__K  0x4000
#define S_26__K  26624
//FLASH规划


#define BootLoadForApp          0                            /*0--4k    4k*/
#define AppStart                ( BootLoadForApp+S_4__K )    /*4k-68k   64k*/
#define GsmPara                 ( AppStart+S_64__K )         /*68k-70k  2k*/
#define GsmParaBvk              ( GsmPara+S_2__K )           /*70k-72k  2k*/
#define BLIND_STU               ( GsmParaBvk+S_2__K )        /*72k-74k  2k重启前的盲点缓冲数据保存*/
#define UpdatePara              ( BLIND_STU+S_2__K )         /*74k-76k  2k升级完成标志*/
#define Rever                   ( UpdatePara )               /* 无 */
#define AppbvkStart             ( Rever+S_2__K )             /*76k--128k 52k 升级缓冲盲点，数据保存，agps数据保存*/


#define AppSEC                     29                        /*升级数据的长度*/



#define NET_SGEE_ADDR           AppbvkStart                           /*AGPS下载时*/
#define NET_SGEE_SEC              8
#define NET_SGEE_MAX_LEN    S_16__K                                        /*16k*/

#define SGEE_ADDR           (AppbvkStart+S_16__K)                            /*gps平时的操作用*/
#define SGEE_SEC              8
#define SGEE_MAX_LEN    S_16__K                                                     /*16k*/








#define DateBufStart                ( SGEE_ADDR+ S_16__K)                                 /*网络换冲数据保存*/
#define DATE_BUF_LEN             S_26__K                                                         /*盲点缓冲数据的长度*/
#define DATE_BUF_LEN_SEC     13                                                                   /*扇区数*/


//说明:buf 缓冲和app备份使用同一个区域
//app bvk
//PROGRAMOK!  共十个字节表示需要重写flsah
//BUFFON!!!!     表示缓冲就绪








typedef union
{
    u8 date[4];
    u32 total;
} UNION_U32_STU;



#define BLIND_BUF_LEN 2040
#define A_FRAM_LEN   40
#define BLIND_MAXTIMES   51   //(BLIND_BUF_LEN/A_FRAM_LEN)
typedef __packed struct
{
    u16 unit;
    u8  buflen[BLIND_BUF_LEN];
    u8 var;
    u8  left[4];
    u8 varity;
} STRUCT_BLIND_BUF;

typedef struct
{
    u8 len;
    u8  buflen[A_FRAM_LEN-1];
} STRUCT_BLIND_FRAM;

#define Flash_IMPORT   0xaa
#define Flash_LONG   0x55

extern STRUCT_BLIND_BUF BlindStu;
extern u8 WriteFlag;


void FLASH_writeWord(uint32_t address, uint32_t data);
void FLASH_eraseOneBlock(u32 blockStart);
void FLASH_WriteDate(u32 adr,u32 count, u8  *buffer);
void FLASH_ReadDate(u32 adr,u32 count, u8  *buffer);

u8  FlashBufRead(STRUCT_BLIND_FRAM *StuFram);
u8  FlashBufDelete(void);
u8  FlashBufWrite(STRUCT_BLIND_FRAM StuFram,u8 Important);
void ReadBlind(void);


//void FLASH_CalcPageSize(void);
void EnableIrq(u32 ri0);
void DisableIrq(u32 ri0);
void TestFlash(void);
void EraseApp(u32 applen);
void ReadApp(u32 addr,u8*writebuf,u16 len);
void WriteApp(u32 addr,u8*readbuf,u16 len);
void DeleteAllDate(void);
//当判断缓冲中没有数据的时候保存
void WriteWhenNoBlindDate(void);
void FlashProcess(u32 ri0,u8 Enable_Disable);

#endif
