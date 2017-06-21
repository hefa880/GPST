#ifndef TIMER_CONVER
#define TIMER_CONVER


typedef struct
{

    //������������
    u32 counter;
    u16 w_year;
    u8  w_month;
    u8  w_date;
    u8 hour;
    u8 min;
    u8 sec;
    u8  week;
    u8 time[6];/*BCD���������ʱ����*/
    u8 rtcflag;/*У׼�������250У׼һ��*/
} tm;
extern tm timer;

void CoverTimeToBCD(void);
void RtcInter(void);
void SetRtcTime(u8 *BCDTIME);
void TimerRtcInit(void);
void GetRtcTime(u8* OutBcdTime);
void TimeTask(void);
u8 Is_Leap_Year(u16 year);
u8 TimeCheck(void);

#endif




