#ifndef GSM_OPERATION_h
#define GSM_OPERATION_h



//#define GSM_USE_UART

#define RST_PIN   15
#define RST_PORT  gpioPortA

#define DTR_PIN   13
#define DTR_PORT  gpioPortE

#define RTS_PIN   8
#define RTS_PORT  gpioPortE

#define CTS_PIN   9
#define CTS_PORT  gpioPortE


#define GSM_TX_PIN   14
#define GSM_TX_PORT  gpioPortE
#define GSM_RX_PIN   15
#define GSM_RX_PORT  gpioPortE


//#define CFUN_FIVE

#define   GSM_DEFAULT_URL          "www.gopinpoint.com"
#define   GSM_DEFAULT_PORT         "5678"
#define   GSM_DEFAULT_APN          "unim2m.gzm2mapn" 





#define DTR_ENABLE()    GPIO_PinOutClear(DTR_PORT,DTR_PIN) // alway enable when 3G model was power on
#define DTR_DISABLE()   GPIO_PinOutSet(DTR_PORT,DTR_PIN)

#define GSM_RESET_ON()  GPIO_PinOutSet(RST_PORT,RST_PIN)
#define GSM_RESET_OFF()  GPIO_PinOutClear(RST_PORT,RST_PIN)

#define RTS_READY_0()       GPIO_PinOutClear(RTS_PORT,RTS_PIN)
#define RTS_UNREADY_1()       GPIO_PinOutSet(RTS_PORT,RTS_PIN)

#define PowerWait  0xaa
#define PowerOK  0


#define  GSM_SLEEP  0x00
#define  GSM_WAKE  0x01

#define OK  1
#define NOT_OK 0
#define WAIT  0xa5

#define MESSAGE_NUMBERS 5
#define PDU_HEX    0xa5  /*字节70汉字*/
#define PDU    0xaa           /*字节35汉字*/
#define TXT     0x55

#define NOT_READ  0xfe
#define NOT_MATCH  0xff
#define BATTERY_CHARGE  1
#define BATTERY_NOT_CHARGE 0
#define SENDING_BUF_LEN 1224


#define DATAS_SENDING  0xaa
#define DATAS_IDLES   0


#define OPEAR_MAX_LEN    5
typedef  __packed  struct
{
    volatile u8 PowerResetStu;/*gsm 开机复位状态切换专用*/
    u8  IpOneConnect;/*主网络在连接状态*/
    u8 CREG;
    u8 CSQ;
    u8 CGREG;
    u8 QSS;
    u8 IMEI[9];
    u16 UnRevLen;/*来自ssi*/
    u16 UnSendLen;
    u8 SocketState;
    u8 NetAck;
    u16 SendingLen;

    u8 SendingBuf[SENDING_BUF_LEN];
    u8 MNC;
    u16 MCC;
    u16 RevLen;/*来及sring*/
    volatile u16 asklen;   //处理收到的数据 SRECV:1,23 
    volatile u8 DateCome;
    u8 PhoneCome;
    u8 PnoneNumber[20];
    u8 PnoneNumberLen;
    u8 MessageCome;
    s32 Latitude;//0.000001度纬度
    s32 longitude;//0.000001度经度
    u16 Uncerten;//范围
    u8 varitygsmlon;
    u8 LaLoBuf[50];
    u8 LaloBufLen;

    u32 BasicKeepAlive;
    u32 BasicPositionInter;
    u8 SendCmd[2];
    u8 SendId;


    u16 updatetime20sec;/*20s重发*/
    u16 updatetime20min;/*20min 超时*/

    u8 msgs;/*短信个数短信编号*/
    u16 ReadMsgTime;/*读短信的时长*/





    u8 messagelen[MESSAGE_NUMBERS];
    u8 messagetype[MESSAGE_NUMBERS];
    u8 messagebuf[MESSAGE_NUMBERS][140];
    u8 msgnumberlen[MESSAGE_NUMBERS];
    u8 msgnumber[MESSAGE_NUMBERS][19];

    u16 voltage;/*电压*/
    u8 Battery;/*电池电量0--100*/




    u8 AskNetAtLen;



    u8 operturn;/*oxff表示没有找到匹配的，0xfe表示没有读到*/

    u8 empty   : 1;
    u8 askm2m     : 1;

    u8 batterylowtrig: 1; /*从不欠压到欠压*/
    u8 charging : 1;
    u8 ful: 1;

    u8 nopower: 1;
    u8 empty2: 1; /*请求ap*/

    u8 FromUnconnect: 1;

    u8 SendDate: 1;
    u8 askm2malerag: 1;
    u8 offtime: 5; /*没有接外部电源的次数*/
    u8 Askmsmback: 1;


    u8 LcdStatecharge: 1;
    u8 LcdStateReserve;
    u8 LcdStateBattery;/*电池电量0--100*/


    u8 Sending;/*DATAS_SENDING*/

    u8 mac[6];
    u8 resettime;/*连续复位次数*/

    u8 sos;
    u8 ble;
    u16 LCDState;/*1---on   0---off*/

    u8 opeabuf[OPEAR_MAX_LEN];
    u8 opeabuflen;
    u8 operator_flag;
    u8 station_count_gsm;
    u8 station_count_3g;
    u8 charge_shutdown;
    u8 gps_p;
    u8 gsm_p;
    u8 at_result;

    u8 askCSQ: 1;
} GsmStatuesstu;

extern GsmStatuesstu GsmSta;



#define AT_AT 0
#define AT_ATE0 1
#define AT_CGMR 2
#define AT_CGMM 3
#define AT_CACHEDNS 4
#define AT_IMEI 5
#define AT_FLO     6
#define AT_QSS    7
#define AT_CLIP    8
#define AT_CMGF    9

#define AT_SGACTAUTH    10

#define AT_MONI7   6

#define MASK_POWER_STATUS_NOMAL          0x00
#define MASK_POWER_STATUS_REST           0x01
#define MASK_POWER_STATUS_WAKEUP         0x02
#define MASK_POWER_STATUS_SLEEP          0x04
#define MASK_POWER_STATUS_OFF            0xF0







typedef __packed struct
{

    u8 *oper;/*mcc MNC*/
    u8 *strapn;
    u8 NetId;/*网络鉴权模式*/
    u8 *NetIdName;/*网络鉴权用户名*/
    u8 *NetIdPsswd;/*网络密码161*/
} STU_APN;
#define MIN_GPS_INTER  29
#define MAX_APN    8
extern const STU_APN StuApn[MAX_APN];

typedef __packed struct
{
    u8 apnlen;
    u8 *strapn;
    u8 NetId;/*网络鉴权模式*/
    u8 NetIdNameLen;
    u8 *NetIdName;/*网络鉴权用户名*/
    u8 NetIdPsswdLen;
    u8 *NetIdPsswd;/*网络密码161*/
} STU_APN_NET;
extern STU_APN_NET StuApnNet;

typedef __packed  struct
{
    u8 iplen;
    u8 strip[79];
    u8 apnlen;
    u8 strapn[39];
    u8 portlen;
    u8 port[7];//128


    u16 moveintervalGPS;
    u16 staticinterval;
    u8 BatteryQuatity;/*电量0--100报警阈值*/
    u8 first;
    u8 ID[10];
    u8 mima[6];
    u16 KeepAliveInter;//24

    u8 NetId;/*网络鉴权模式*/
    u8 NetIdNameLen;
    u8 NetIdName[79];/*网络鉴权用户名*/
    u8 NetIdPsswdLen;
    u8 NetIdPsswd[79];/*网络密码161*/



    u8 updateflag;
    u8 updatevarity;
    u32 Softwarelen;
    u16 totalpackeg;
    u16  packetsize;
    u16 currentpackeg;//12

    u8 updateiplen;
    u8 updatestrip[79];
    u8 updateportlen;
    u8 updateport[7];//88


    u8 MessageCenter[19];
    u8 MessageCenterLen;/*短消息中心号码长度不为0时候设置，设置后置0   20*/


    u16 moveintervalGSM;

    u8 Is12h;

    u8 Timehour;/*-12   timehour=12, timehoure|=0x80*/
    u8 Timeminute;

    u8 sosNumber;
    u8 sosPhone[5][20];
    u8 sosPhoneLen[5];   /* 106 bytes */

    u8 lcdTemperature[2];
    u8 lcdWeather[3];

    u8 left[1024 - 434 - 2 - 2 - 1 - 106 - 5];
    u8 varity;/*数据最后的校验码*/
} GsmStorestu;

extern GsmStorestu GsmSto;

#define GSMRECMAXLEN  4196
#define IDOK 0xaa
#define  GetAFram 0x55
typedef struct
{

    //解析好的一帧数据后，flag标志为GetAFram，RecCount存储数据的长度 RecData中为:water+lenh+lenL+idh+idL+date......+varity
    u16 RecCount;
    u8 flag;
    u8 RecData[GSMRECMAXLEN];
} GSM_REC_DATA_STU;
extern GSM_REC_DATA_STU GsmRecDataStu;

typedef struct
{
    u8 atturn;
    u8 *AtCommend;
    u16 len;
    u8 *flag;
    u8 times;
    u16 waittimeSecond;
} AT_STRUCT;

void GsmInit(void);
void GsmTask(void);
extern u8 PbulicBuf[1024];/*读flash用*/
void WriteGsmStoreDate(void);
void ReadGsmStoreDate(void);
void WriteGsmStoreDateToDefault(void);

typedef __packed struct
{
    u8 varity;
    u8 Sgeedatecome;
    u16 time;/*时间标准*/
    u16 overtime;
    u16 packegsize;
    u16 currentpackeg;
    u16 totalpackeg;
} AT_NET_SGEE;

extern AT_NET_SGEE AtNetSgee;

u8 ActiveGprs(u8 *buf, u8 flag);
u8 SetApn(u8 *buf);
u8 AT_SD(u8 *buf);
void GsmGetStatues(u8 datain, void(*revnetfunction)(u8 indate));
void PrintfNetDate(u8 *buf, u16 len);
u8  AskNetDate(u8 *buf);
void Tick(void);
u8  ATReadMsg(u8 *buf);
u8  ATDeleteMsg(u8 *buf);
void ReadMsg(void);
u8 GsmM2MAsk(void);
void InitGsmdate(void);


void DeleteMsgBuf(void);
u8 ReadMsgBuf(u8 *TYPE);
void WriteMsgBuf(u8 *data, u8 len, u8 *number, u8 numberlen, u8 TYPE);
u8 ATCMGS_DATE(u8 *buf);
u8 ATCMGS(u8 *buf);
u8 ATCSMP(u8 *buf);
u8 ATCSCS(u8 *buf);
void ProcessUpdateSGEE(void);
void InTime(void);
u8 READ_CTS(void);
u8  Ue910ShutDown(void);
void GiveStoreApn(void);
u8 ProcessPhone(void);
#endif

