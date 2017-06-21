#ifndef GPS__H
#define GPS__H


//#define GPS_USE_UART

#ifndef  GPS_USE_UART
#define GPS_UART LEUART1
#define GPS_RX_PIN   6
#define GPS_RX_PORT gpioPortC

#define GPS_TX_PIN   7
#define GPS_TX_PORT gpioPortC
#else
#define GPS_UART USART1
#define GPS_RX_PIN   0
#define GPS_RX_PORT gpioPortD
#define GPS_TX_PIN   1
#define GPS_TX_PORT gpioPortD
#endif

#define GPS_POWER_PIN   10
#define GPS_POWER_PORT gpioPortC

#define GPS_MON_PIN   9
#define GPS_MON_PORT  gpioPortC

#define GPS_ON_OFF_PIN   8
#define GPS_ON_OFF_PORT  gpioPortC

#define GPS_RF_POWER_PIN   11
#define GPS_RF_POWER_PORT  gpioPortC

#define GpsReadMon()      GPIO_PinInGet(GPS_MON_PORT,GPS_MON_PIN)

#define GpsOnOff_1()  GPIO_PinOutSet(GPS_ON_OFF_PORT,GPS_ON_OFF_PIN)
#define GpsOnOff_0()  GPIO_PinOutClear(GPS_ON_OFF_PORT,GPS_ON_OFF_PIN)


#define GpsPowerOn()  GPIO_PinOutSet(GPS_POWER_PORT,GPS_POWER_PIN)
#define GpsPowerOff()  GPIO_PinOutClear(GPS_POWER_PORT,GPS_POWER_PIN)
#define ReadPower()   GPIO_PinOutGet(GPS_POWER_PORT,GPS_POWER_PIN)



#define GPS_RF_OFF()  GPIO_PinOutSet(GPS_RF_POWER_PORT, GPS_RF_POWER_PIN)
#define GPS_RF_ON()  GPIO_PinOutClear(GPS_RF_POWER_PORT, GPS_RF_POWER_PIN)

#define GPS_OFF  1
#define GPS_ON 0

#define GPS_WAKEUP  0
#define GPS_SLEEP  1
typedef __packed struct
{
    u8 ConTrolStu;/*指令发送状态*/
    u8  addsecond;/*为真自减*/
    u8 result;/*发送指令的时候返回结果*/

    u8 GpsNoDateTime;/*没有gps数据的时长*/
    u16 GpsUnfixedTime;/*gps不定位的时长刚开机的时候认为10min钟没有定位*/
    u8 sleepManageTime;/*休眠时间管理为真按秒子减,ff是静态休眠*/

    u8 Get5Position:1;/*获得5个定位点*/
    u8 Low10Gsv:1;/*信号强度低于10*/
    u8 Low20Gsv:1;/*信号强度低于20*/
    u8 HightGsv:1;/*信号强度正常*/
    u8 stu:2;/*查找信号强度的变量*/
    u8 empty:2;
    u8 FindTime;/*gps查找信号的强度的时间*/
    u8 FindGpsTime;/*寻找合理的点的时间*/

} GPS_CONTROL_STU;
extern GPS_CONTROL_STU GpsControlStu;

#define   RETURN_SUCCESS  OK
#define RETURN_TIMEOUT   NOT_OK
#define  RETURN_WAITING  WAIT


#define SLEEP_TIME_A 30
#define SLEEP_TIME_LOW 40
#define SLEEP_TIME_MIDDLE 25
#define NO_SLEEP      0xff
#define FIND_GSV_TIME  20
#define WAKE_TIME  30
#define SEARCH_TIME  35

#define GPS_EMPTY     		0x00   /*idle*/
#define GPS_1501       		0x01    /*ACK*/
#define GPS_1500       		0x02    /*NCK*/
#define GPS_SHORT      		0x03   /*短路*/
#define GPS_ACK_START  		0x04
#define GPS_NACK_START 	0x05
#define GPS_ACK_FILESIZE	0x06
#define GPS_NACK_FILESIZE    0x07
#define GPS_SEND_PACKEG_ACK 0x08
#define GPS_SEND_PACKEG_NACK 0x09

#define OSP_FRAM_SMOOTH_ACK   0x01
#define OSP_FRAM_ACK   0xaa
#define OSP_FRAM_NCK   0x55
#define OSP_FRAM_OTHER   0xa5
#define OSP_NO_FRAM        0x00
#define OSP_WAIT_ACK       0x5a
#define OSP_WAIT_TO_TRIC      0x5b
#define OSP_WAIT_NMEA       0x03
#define OSP_WAIT_NMEA_OK       0x04
#define OSP_TRICK_OK  0x02
typedef __packed struct
{
    u8 flagack;
    u8 flag;
    u16 checksum;
    u16 len;
    u8 pdata[300];/*消息体*/
} OSP_STU;
extern OSP_STU OspStu;
typedef __packed union
{

    u8 gpgsv[6];/*代表3颗卫星的强度0 1 2存最大值   3 4 5存实时值*/
    struct
    {
        u8 maxsv[3];
        u8 nowsv[3];
    };

} GPS_GPGSV;
extern GPS_GPGSV GpsGsv;
#define AGPS_IDLE 0xaa     /*空闲状态*/
#define AGPS_UPING 0xa5  /*正在从网络更新文件*/
#define AGPS_NEED  0x55  /*需要刷新数据到gps中*/



#define SEND_SGEE_FILE_SIZE   150
typedef __packed struct
{
    u16 U_CurrentPacket;			/*当前包*/
    u16 U_TatolPackeg;                  	/*总包数*/
    u16 LastPackegSize;                    /*最后一包数据的大小*/
    u32  offset;					/*当前地址偏移记录文件总长度*/
    u8    SgeeState ;                            /*agps状态*/


} GPS_STATUES;


typedef __packed struct
{
    u8    	bTime[6];	//当前时间           bcd  yy   16
    u8		bValidity;	//已定位                        bValidity
    s32 Latitude;//0.000001度纬度  北纬为正
    s32 longitude;//0.000001度经度 东经为正
    s16 	       high_m;		//高度                            0---65536
    u8		SatelCnt;	/*星数*/                        //    0-12
    u16		speed;		//速度                          地面速度km/h
    u16		direction;	//						方向 0-360度

} STR_GPS_DATA;
typedef __packed struct
{
    s32 Latitude;//0.000001度纬度  北纬为正
    s32 longitude;//0.000001度经度 东经为正
    s16 	       high_m;		//高度                            0---65536
    u8		SatelCnt;	/*星数*/                        //    0-12
    u16		speed;		//速度                          地面速度km/h
    u16		direction;	//						方向 0-360度
    u8      varity;

} BVKSTR_GPS_DATA;

typedef __packed struct
{

    u8 degragemode;
    u8 position_calc_mode;
    u16 altitude;
    u8 alt_hold_mode;
    u8 alt_hold_source;
    u8 degrade_time_out;
    u8 dr_time_out;
    u8 mersurement_track_smoothing;
} STU_SMOOTH;

extern STR_GPS_DATA		strGpsData;
extern BVKSTR_GPS_DATA  bvkstrGpsData;
extern u16 unfixedtime;

extern GPS_STATUES  GpsStatues;
#define GPS_REC_QUEUE_LENGTH 1024
#define MAX_GPS_FRM_LEN               512
extern u8  B_GpsFrmBuf[MAX_GPS_FRM_LEN];					//用来存放被正确读取的一帧GPS数据包
extern STRUCT_QUEUE_CTRL GpsRxQueueCtrl;					//gps接收队列控制结构体
extern volatile u8 GpsRxQueueBuf[GPS_REC_QUEUE_LENGTH];	//gps接收队列缓存









#define EE_LEN           2047                    /*aa data data data.......... varity*/

typedef __packed struct
{
    u8 EEDATA[EE_LEN+1];
    u8 CGEEDATA[EE_LEN+1];
//    u8 flag;
} EE_STU;
extern EE_STU eeStu;
void GpsDateIn(u8 datein);
u8  WakeUpGps(u8 wakeup);
void GpsTask(void);
void InitGps(void);
void GPSUsartSendStr(u8 *str,u16 len);
u8  ReadSGEEHavedate(void);
void WriteSGEEHavedate(u8 have);
void EraseNetSGEEToFlash(void);
void WriteNetSGEEToFlash(u8 *datain,u16 datelen,u32 offset );
void ReadNetSGEE(u8*outbuf ,u32 offset ,u16 len);
int CalcDistance(double fLati1, double fLong1, double fLati2, double fLong2);
u8 filetsend(double latnow,double lonnow);
u8 ChangeGpsToOSP(void);
u8    SetGpsStatus(void);
void  SendSmoothMode(STU_SMOOTH StuSmooth);
void UsartSend(USART_TypeDef *usart,u8*data,u16 len);
#endif



