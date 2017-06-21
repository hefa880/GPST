#ifndef PROTOCOL_H
#define PROTOCOL_H


typedef __packed struct
{
    u8  command[2];
    u8  time[5];
    u8  battery;
    u8  statues[4];
    u8  longitude[4];//0.000001度经度 东经为正
    u8  Latitude[4];//0.000001度纬度  北纬为正
    u8  high_m[2];
    u8  speed;
    u8   direction;
    u8   SatelCnt;
} STU_GPS_POSITION;


extern u8 VERSION[2];


#if 0
typedef __packed struct
{
    u8  command[2];
    u8  time[5];
    u8  battery;
    u8  statues[4];
    u8  longitude[4];//0.000001度经度 东经为正
    u8  Latitude[4];//0.000001度纬度  北纬为正
    u8  Uncerten[2];
    u8 Ap_count;
    u8 apmac0[6];
    u8 aprssi0;
    u8 ssid0len;
    u8 *ssid0;
    u8 apmac1[6];
    u8 aprssi1;
    u8 apmac2[6];
    u8 aprssi2;
    u8 apmac3[6];
    u8 aprssi3;
    u8 apmac4[6];
    u8 aprssi4;

} STU_WIFI_POSITION;

typedef __packed struct
{
    u8  command[2];
    u8  time[5];
    u8 Ap_count;
    u8 apmac0[6];
    u8 aprssi0;
    u8 apmac1[6];
    u8 aprssi1;
    u8 apmac2[6];
    u8 aprssi2;
    u8 apmac3[6];
    u8 aprssi3;
    u8 apmac4[6];
    u8 aprssi4;
} STU_AP;
#endif
void ProcessNetData(void);
void McuAnswerCommon(u8 waterid,u16 ansid,u8 successflag);
void UpdateNetCome(u8*datein);
void UPdateOk(u8 UpdateResult);
u16  AskAppPacket(void);
u8 CalcUpdateVarity(void);
void StartUpdatemcuInit(u8*ip,u8 iplen,u8*port,u8 portlen);
#define SEND_POSITION_IN_TIME 0xaa
#define TRIG_SEND_POSITION  0x55
void SendPosition(u8 intime);
void FlashBufWriteLong(u8*buf,u16 len,u8 import);
u8  McuAnswerSetPara(u8 TatolParta,u8* Paradata);
u8  McuAnswerAskPara(u8 TatolParta,u8* Paradata,u8 FromNet);
u16  AskSgeePacket(void);
void  SgeeReport(u8 result);
void SGEENetCome(u8*datein);
u8 CalcNetSgeeVarity(void);
void  AskTime(void);
void SendMac(void);
void SendAp(void);
#endif


