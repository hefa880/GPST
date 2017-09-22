#include"includes.h"
tm timer;


//月份数据表
u8 const table_week[12]= {0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
//平年的月份日期表

const u8 mon_table[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
//输出:该年份是不是闰年.1,是.0,不是

/*********************************************************************************************************
** 函数名称: u8 Is_Leap_Year(u16 year)
** 功能描述: 判断是否是闰年
** 输　入:
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
u8 Is_Leap_Year(u16 year)

{

    if(year%4==0) //必须能被4整除

    {

        if(year%100==0)

        {

            if(year%400==0)return 1;//如果以00结尾,还要能被400整除

            else return 0;

        }
        else return 1;

    }
    else return 0;

}


/*********************************************************************************************************
** 函数名称: u32  BCDToSecond(u8*time)
** 功能描述: bcd码转化为int 的秒
** 输　入:
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
u32  BCDToSecond(u8*time)
{

    //以1970年1月1日为基准

    //1970~2099年为合法年份

    //返回值:0,成功;其他:错误代码.


    u16 t;

    u32 seccount=0;
    u16 syear=2000;
    s8 smon,sday,hour,min,sec;

    syear+=DecToBcd(time[0],false);//(((time[0]>>4)&0x0f)*10)+(time[0]&0x0f);
    smon=DecToBcd(time[1],false);//(((time[1]>>4)&0x0f)*10)+(time[1]&0x0f);
    sday=DecToBcd(time[2],false);//(((time[2]>>4)&0x0f)*10)+(time[2]&0x0f);
    hour=DecToBcd(time[3],false);//(((time[3]>>4)&0x0f)*10)+(time[3]&0x0f);
    min=DecToBcd(time[4],false);//(((time[4]>>4)&0x0f)*10)+(time[4]&0x0f);
    sec=DecToBcd(time[5],false);//(((time[5]>>4)&0x0f)*10)+(time[5]&0x0f);

    if(syear<2000||syear>2200)return 1;//syear范围1970-2099，此处设置范围为2000-2099

    for(t=1970; t<syear; t++) //把所有年份的秒钟相加

    {

        if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数

        else seccount+=31536000;                    //平年的秒钟数

    }

    smon-=1;

    for(t=0; t<smon; t++)       //把前面月份的秒钟数相加

    {

        seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加

        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数

    }

    seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加

    seccount+=(u32)hour*3600;//小时秒钟数

    seccount+=(u32)min*60;      //分钟秒钟数

    seccount+=sec;//最后的秒钟加上去

    return seccount;

}





/*********************************************************************************************************
** 函数名称: void CoverTimeToBCD(void)
** 功能描述: int 的时间转化为bcd
** 输　入
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
void CoverTimeToBCD(void)
{

    static u32 daycnt=0;

    u32 temp=0;

    u16 temp1=0;
    u8 tmp;

    temp= timer.counter/86400;    //得到天数(秒钟数对应的)
    timer.week = (temp + 4) % 7;  /* 1970年1月1日是星期四，sun为0，依次后推 */

    if(daycnt!=temp)//超过一天了

    {

        daycnt=temp;

        temp1=1970;  //从1970年开始

        while(temp>=365)

        {

            if(Is_Leap_Year(temp1))//是闰年

            {

                if(temp>=366)temp-=366;//闰年的秒钟数

                else
                {
                    temp1++;
                    break;
                }

            }

            else temp-=365;       //平年

            temp1++;

        }

        timer.w_year=temp1;//得到年份

        temp1=0;

        while(temp>=28)//超过了一个月

        {

            if(Is_Leap_Year(timer.w_year)&&temp1==1)//当年是不是闰年/2月份

            {

                if(temp>=29)temp-=29;//闰年的秒钟数

                else break;

            }

            else

            {

                if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年

                else break;

            }

            temp1++;

        }

        timer.w_month=temp1+1;//得到月份

        timer.w_date=temp+1;  //得到日期

    }

    temp= timer.counter%86400;     //得到秒钟数

    timer.hour=temp/3600;     //小时

    timer.min=(temp%3600)/60; //分钟

    timer.sec=(temp%3600)%60; //秒钟

    //转换为bcd码
    temp1=timer.w_year-2000;
    tmp=temp1;
    timer.time[0]=DecToBcd(tmp,true);//(((tmp/10)<<4)&0xf0)|((tmp%10)&0x0f);//year
    tmp=timer.w_month;
    timer.time[1]=DecToBcd(tmp,true);//(((tmp/10)<<4)&0xf0)|((tmp%10)&0x0f);//month
    tmp=timer.w_date;
    timer.time[2]=DecToBcd(tmp,true);//(((tmp/10)<<4)&0xf0)|((tmp%10)&0x0f);//dAY
    tmp=timer.hour;
    timer.time[3]=DecToBcd(tmp,true);//(((tmp/10)<<4)&0xf0)|((tmp%10)&0x0f);//hour
    tmp=timer.min;
    timer.time[4]=DecToBcd(tmp,true);//(((tmp/10)<<4)&0xf0)|((tmp%10)&0x0f);//mniinute
    tmp=timer.sec;
    timer.time[5]=DecToBcd(tmp,true);//(((tmp/10)<<4)&0xf0)|((tmp%10)&0x0f);//senond



}




/*********************************************************************************************************
** 函数名称: void BcdTimeAddSubSecond(s32 second ,u8 addflag ,u8*InTime ,u8 *OutTime)
** 功能描述:
** 输　入
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
void BcdTimeAddSubSecond(s32 second ,u8 addflag ,u8*InTime ,u8 *OutTime)
{

    u32 tmpsecond,secondbefore;
    tmpsecond= BCDToSecond(InTime);
    if(addflag)
    {
        tmpsecond+=second;

    }
    else
    {
        tmpsecond-=second;
    }
    secondbefore=timer.counter;
    timer.counter=tmpsecond;
    CoverTimeToBCD();
    Mymemcpy(OutTime,&timer.time[0],6);
    timer.counter=secondbefore;
    CoverTimeToBCD();

}
/*********************************************************************************************************
** 函数名称: u8 RTC_Get(void)
** 功能描述: RTC时间设置
** 输　入
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
u8 RTC_Get(void)
{

    // timer.counter=RTC_GetCounter();

    CoverTimeToBCD();
    return 0;

}
/*********************************************************************************************************
**获得现在是星期几

**功能描述:输入公历日期得到星期(只允许1901-2099年)

**输入参数：公历年月日

**返回值：星期号
********************************************************************************************************/
u8 RTC_Get_Week(u16 year,u8 month,u8 day)

{

    u16 temp2;

    u8 yearH,yearL;
    yearH=year/100;
    yearL=year%100;

    // 如果为21世纪,年份数加100

    if (yearH>19)yearL+=100;

    // 所过闰年数只算1900年之后的

    temp2=yearL+yearL/4;

    temp2=temp2%7;

    temp2=temp2+day+table_week[month-1];

    if (yearL%4==0&&month<3)temp2--;

    return(temp2%7);

}


/*********************************************************************************************************
** 函数名称: void RtcInter(void)
** 功能描述: RTC秒中断
** 输　入
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
void RtcInter(void)
{

    timer.counter++;
    CoverTimeToBCD();
}



void SetRtcTime(u8 *BCDTIME)
{
    timer.counter=BCDToSecond(BCDTIME);
    CoverTimeToBCD();
}

void GetRtcTime(u8* OutBcdTime)
{
    CoverTimeToBCD();
    Mymemcpy(OutBcdTime, timer.time, 6);
}


void TimerRtcInit(void)
{

    const u8 tim[6]= {0x16,0x04,0x01,0x12,0x50,0x55};
    SetRtcTime((u8*)tim);
}

u32 get_system_time(void)
{
    return  timer.counter;
}


u8 TimeCheck(void)
{
    return timer.time[0];
}

/*********************************************************************************************************
** 函数名称: void TimeTask(void)
** 功能描述: 时间管理
** 输　入
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
void TimeTask(void)
{

    static u8  time=0;
    if(time++>9)/*1second*/
    {
        time=0;
        if(timer.rtcflag<0xfe) timer.rtcflag++;
        if(GpsControlStu.addsecond) GpsControlStu.addsecond--;
        if(GpsControlStu.GpsNoDateTime<0xfd)  GpsControlStu.GpsNoDateTime++;
        if(GpsControlStu.GpsUnfixedTime<0xfffd)  GpsControlStu.GpsUnfixedTime++;
        if( (GpsControlStu.sleepManageTime)&&(GpsControlStu.sleepManageTime!=0xff))  GpsControlStu.sleepManageTime--;
        if(GpsControlStu.FindTime) GpsControlStu.FindTime--;
        if( (unfixedtime<0xfffd)&&(strGpsData.bValidity!='A'))  unfixedtime++;

        //if(unfixedtime<0xfffd)  unfixedtime++;
#if 0
        if(AdxlStu.time)
        {
            AdxlStu.time--;

        }
        else
        {
            AdxlStu.state=ADXL_STATIC;
            if(debug==DEBUGADD)
                printf("--------------------------->静止\r\n");
        }
#endif


    }

}


