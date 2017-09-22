#include"includes.h"
tm timer;


//�·����ݱ�
u8 const table_week[12]= {0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
//ƽ����·����ڱ�

const u8 mon_table[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
//���:������ǲ�������.1,��.0,����

/*********************************************************************************************************
** ��������: u8 Is_Leap_Year(u16 year)
** ��������: �ж��Ƿ�������
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
********************************************************************************************************/
u8 Is_Leap_Year(u16 year)

{

    if(year%4==0) //�����ܱ�4����

    {

        if(year%100==0)

        {

            if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400����

            else return 0;

        }
        else return 1;

    }
    else return 0;

}


/*********************************************************************************************************
** ��������: u32  BCDToSecond(u8*time)
** ��������: bcd��ת��Ϊint ����
** �䡡��:
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
********************************************************************************************************/
u32  BCDToSecond(u8*time)
{

    //��1970��1��1��Ϊ��׼

    //1970~2099��Ϊ�Ϸ����

    //����ֵ:0,�ɹ�;����:�������.


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

    if(syear<2000||syear>2200)return 1;//syear��Χ1970-2099���˴����÷�ΧΪ2000-2099

    for(t=1970; t<syear; t++) //��������ݵ��������

    {

        if(Is_Leap_Year(t))seccount+=31622400;//�����������

        else seccount+=31536000;                    //ƽ���������

    }

    smon-=1;

    for(t=0; t<smon; t++)       //��ǰ���·ݵ����������

    {

        seccount+=(u32)mon_table[t]*86400;//�·����������

        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������

    }

    seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ����������

    seccount+=(u32)hour*3600;//Сʱ������

    seccount+=(u32)min*60;      //����������

    seccount+=sec;//�������Ӽ���ȥ

    return seccount;

}





/*********************************************************************************************************
** ��������: void CoverTimeToBCD(void)
** ��������: int ��ʱ��ת��Ϊbcd
** �䡡��
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
********************************************************************************************************/
void CoverTimeToBCD(void)
{

    static u32 daycnt=0;

    u32 temp=0;

    u16 temp1=0;
    u8 tmp;

    temp= timer.counter/86400;    //�õ�����(��������Ӧ��)
    timer.week = (temp + 4) % 7;  /* 1970��1��1���������ģ�sunΪ0�����κ��� */

    if(daycnt!=temp)//����һ����

    {

        daycnt=temp;

        temp1=1970;  //��1970�꿪ʼ

        while(temp>=365)

        {

            if(Is_Leap_Year(temp1))//������

            {

                if(temp>=366)temp-=366;//�����������

                else
                {
                    temp1++;
                    break;
                }

            }

            else temp-=365;       //ƽ��

            temp1++;

        }

        timer.w_year=temp1;//�õ����

        temp1=0;

        while(temp>=28)//������һ����

        {

            if(Is_Leap_Year(timer.w_year)&&temp1==1)//�����ǲ�������/2�·�

            {

                if(temp>=29)temp-=29;//�����������

                else break;

            }

            else

            {

                if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��

                else break;

            }

            temp1++;

        }

        timer.w_month=temp1+1;//�õ��·�

        timer.w_date=temp+1;  //�õ�����

    }

    temp= timer.counter%86400;     //�õ�������

    timer.hour=temp/3600;     //Сʱ

    timer.min=(temp%3600)/60; //����

    timer.sec=(temp%3600)%60; //����

    //ת��Ϊbcd��
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
** ��������: void BcdTimeAddSubSecond(s32 second ,u8 addflag ,u8*InTime ,u8 *OutTime)
** ��������:
** �䡡��
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
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
** ��������: u8 RTC_Get(void)
** ��������: RTCʱ������
** �䡡��
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
********************************************************************************************************/
u8 RTC_Get(void)
{

    // timer.counter=RTC_GetCounter();

    CoverTimeToBCD();
    return 0;

}
/*********************************************************************************************************
**������������ڼ�

**��������:���빫�����ڵõ�����(ֻ����1901-2099��)

**�������������������

**����ֵ�����ں�
********************************************************************************************************/
u8 RTC_Get_Week(u16 year,u8 month,u8 day)

{

    u16 temp2;

    u8 yearH,yearL;
    yearH=year/100;
    yearL=year%100;

    // ���Ϊ21����,�������100

    if (yearH>19)yearL+=100;

    // ����������ֻ��1900��֮���

    temp2=yearL+yearL/4;

    temp2=temp2%7;

    temp2=temp2+day+table_week[month-1];

    if (yearL%4==0&&month<3)temp2--;

    return(temp2%7);

}


/*********************************************************************************************************
** ��������: void RtcInter(void)
** ��������: RTC���ж�
** �䡡��
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
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
** ��������: void TimeTask(void)
** ��������: ʱ�����
** �䡡��
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
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
                printf("--------------------------->��ֹ\r\n");
        }
#endif


    }

}


