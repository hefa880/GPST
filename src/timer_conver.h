#ifndef TIMER_CONVER
#define TIMER_CONVER


typedef struct
{

    //公历日月年周
    u32 counter;
    u16 w_year;
    u8  w_month;
    u8  w_date;
    u8 hour;
    u8 min;
    u8 sec;
    u8  week;
    u8 time[6];/*BCD码的年月日时分秒*/
    u8 rtcflag;/*校准间隔大于250校准一次*/
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




