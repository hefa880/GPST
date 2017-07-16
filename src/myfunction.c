#include"includes.h"

u8 debug=DEBUGGSM;


STU_POWER StuPower;
/*
*********************************************************************************************************
*	函 数 名: u8 * Findbcd(u8 *indata,u16 inlen,u8 *outdata,u16 outlen)
*	功能说明: 在indata中找outdata,
*
*
*	形    参：
*	返 回 值:找到返回FINDNUM  否则返回0
*********************************************************************************************************
*/
u8 * Findbcd(u8 *indata,u16 inlen,u8 *outdata,u16 outlen)
{

    u16 i,j,k;
    u8 result=0;
    k=inlen-outlen+1;
    for(i=0; i<k; i++)
    {
        result=0;
        for(j=0; j<outlen; j++)
        {
            if(indata[i+j]!= outdata[j])
                result=1;
        }
        if(result==0)
            return (&indata[i]);
    }
    return 0;
}

u16 FindString( STRUCT_QUEUE_CTRL *QueueCtrl, u8 *outdata, u16 outlen )
{
    u16 i,j,k, nPos, OutIndex = QueueCtrl->OutCnt;
    u16 result = 0xFFFF;
    u8 *indata = (u8 *)STU_AtCommand.rev;

    k=MaxRevLen-outlen+1;
    for(i=0; i<k; i++)
    {
        result=0;
        for ( j=outlen; j>0; j-- )
        {
            if ( OutIndex <= i + outlen - j )
            {
                nPos = MaxRevLen + OutIndex - ( i + outlen - j ) - 1;
            }
            else
            {
                nPos = OutIndex - ( i + outlen - j ) - 1;
            }

            if(indata[nPos]!= outdata[j-1])
            {
                result=1;
                break;
            }
        }
        if(result==0)
            return nPos;
    }
    return 0xFFFF;
}

/*
*********************************************************************************************************
*	函 数 名: u8 Equre(u8 *buf1,u8* buf2, u16 len)
*	功能说明:
*
*
*	形    参：
*	返 回 值:相等返回 ok ，否则not ok
*********************************************************************************************************
*/
u8 Equre(u8 *buf1,u8* buf2, u16 len)
{
    u16 i;
    for(i=0; i<len; i++)
    {
        if(buf1[i]!=buf2[i])
            return NOT_OK;

    }
    return OK;
}
/*
*********************************************************************************************************
*	函 数 名: u16 GetArryLen(u8 *buf,u16 maxlen)
*	功能说明:
*
*
*	形    参：
*	返 回 值:
*********************************************************************************************************
*/
u16 GetArryLen(u8 *buf,u16 maxlen)
{
    u16 i;
    //   u16 j;
    //  j=200;
    for(i=0; i<maxlen; i++)
    {
        if(buf[i]==0)
            return i;

    }
    return i;
}


/*
*********************************************************************************************************
*	函 数 名: u8 CalacXORVarity(u8*bufin,u16 len)
*	功能说明: 计算抑或校验和
*
*
*	形    参：
*	返 回 值:
*********************************************************************************************************
*/
u8 CalacXORVarity(u8*bufin,u16 len)
{
    u8 varity=0;
    getvarity(&varity,bufin,len);
    return varity;
}

//12234---"1234"
//十进制转字符串
u8  DecToString(u16 from,u8*out)
{
    u8 len;
    //65536
    if(from>=10000)
    {
        len=5;
        out[0]=from/10000+0x30;
        out[1]=from%10000/1000+0x30;
        out[2]=from%1000/100+0x30;
        out[3]=from%100/10+0x30;
        out[4]=from%10+0x30;
    }
    else if(from>=1000)
    {
        len=4;
        out[0]=from/1000+0x30;
        out[1]=from%1000/100+0x30;
        out[2]=from%100/10+0x30;
        out[3]=from%10+0x30;
    }
    else if(from>=100)
    {
        out[0]=from/100+0x30;
        out[1]=from%100/10+0x30;
        out[2]=from%10+0x30;
        len=3;
    }
    else if(from>=10)
    {
        out[0]=from/10+0x30;
        out[1]=from%10+0x30;
        len=2;
    }
    else
    {
        out[0]=from+0x30;
        len=1;
    }
    return len;
}

/*
*********************************************************************************************************
*	函 数 名: u8 CalacXORVarity(u8*bufin,u16 len)
*	功能说明: 计算抑或校验和
*
*
*	形    参：
*	返 回 值:
*********************************************************************************************************
*/
void  Mymemcpy(u8*dest,u8 *source ,u16 len)
{

    u16 i;
    for(i=0; i<len; i++)
        dest[i]=source[i];

}

/*
*********************************************************************************************************
*	函 数 名:void Conver_aa_to_55(u8 *indata,u16 inlen,u8 *outbuf,u16*outlen)
*	功能说明: aa--->55 01   55--->55  02
*
*
*	形    参：
*	返 回 值:
*********************************************************************************************************
*/
void Conver_aa_to_55(u8 *indata,u16 inlen,u8 *outbuf,u16*outlen)
{
    u16 i,len=0;
    for(i=0; i<inlen; i++)
    {
        if(indata[i]==0xaa)
        {
            outbuf[len++]=0x55;
            outbuf[len++]=0x01;
        }
        else if(indata[i]==0x55)
        {
            outbuf[len++]=0x55;
            outbuf[len++]=0x02;
        }
        else
            outbuf[len++]=indata[i];
    }
    *outlen=len;

}


/*
*********************************************************************************************************
*	函 数 名:u16 strfindzero(u8*buf)
*	功能说明:
*
*
*	形    参：
*	返 回 值:
*********************************************************************************************************
*/
u16 strfindzero(u8*buf)
{
    u16  i;
    i=0;
    while(0!=buf[i])
    {
        i++;
    }
    return i;

}

/*
*********************************************************************************************************
*	函 数 名: u8 EqureOrNot(u8* dat1,u8* dat2, u16 len)
*	功能说明:比较在制定长度类两个字符串是否相等
*
*
*	形    参：
*	返 回 值:相等返回 OS_TRUE
*********************************************************************************************************
*/
u8 EqureOrNot(u8* dat1,u8* dat2, u16 len)
{
    u16 lent;
    for(lent=0; lent<len; lent++)
    {
        if(dat1[lent]>dat2[lent])
        {
            return BIGER;
        }
        else  if(dat1[lent]<dat2[lent])
        {
            return LITTER;

        }

    }
    return EQUER;
}

/*
*********************************************************************************************************
*	函 数 名: void WaitToResetSystem(u8 second)
*	功能说明:
*
*
*	形    参：
*	返 回 值:相等返回 OS_TRUE
*********************************************************************************************************
*/
u8 resetflag=0;
u16 resetTime=0;
void WaitToResetSystem(u8 second)
{
    resetflag=0xaa;
    resetTime=(u16)second*10;
}


/*
*********************************************************************************************************
*	函 数 名: void ResetSystem(void)
*	功能说明:
*
*
*	形    参：
*	返 回 值:相等返回 OS_TRUE
*********************************************************************************************************
*/
void ResetSystem(void)
{
    if(resetflag!=0xaa)  return;
    if(resetTime--<=1)
    {
        resetflag=0;
        NVIC_SystemReset();
    }

}


/*
*********************************************************************************************************
*	函 数 名: u8* Finddate(u8*des,u8 date,u16 maxlen)
*	功能说明:
*
*
*	形    参：
*	返 回 值:相等返回 OS_TRUE
*********************************************************************************************************
*/
u8* Finddate(u8*des,u8 date,u16 maxlen)
{
    u16 i;
    u8*p;
    p=des;
    for(i=0; i<maxlen; i++)
    {
        if(*p==date)
        {
            return p;
        }
        p++;
    }
    return 0;
}

/*
*********************************************************************************************************
*	函 数 名:u8 acstohex(u8 datein)
*	功能说明:
*
*
*	形    参：31--1
*	返 回 值:相等返回 OS_TRUE
*********************************************************************************************************
*/
u8 acstohex(u8 datein)
{
    if( (datein>='0')&&(datein<='9'))
        return (datein-'0');
    if( (datein>='a')&&(datein<='f'))
        return (datein-'a'+0x0a);
    if( (datein>='A')&&(datein<='F'))
        return (datein-'A'+0x0A);
    return (datein-'0');
}
/*
*********************************************************************************************************
*	函 数 名:u8  HexToAscll(u8 In,u8 flag)
*	功能说明:
*
*
*	形    参：
*	返 回 值:
*********************************************************************************************************
*/
u8  HexToAscll(u8 In,u8 flag)
{

    if(In<=9)
        return (In+'0');

    if(flag)
    {
        if((In>=0x0a)&&(In<=0x0f))
            return (In+'A'-10);
    }
    else
    {

        if((In>=0x0a)&&(In<=0x0f))
            return (In+'a'-10);
    }

    return 0;
}

/*
*********************************************************************************************************
*	函 数 名:void ConverascTohex(u8*asc,u16 asclen,u8*hex)
*	功能说明:
*
*
*	形    参："1234abcdef" ----12 34 ab cd ef
*	返 回 值:相等返回 OS_TRUE
*********************************************************************************************************
*/
void ConverascTohex(u8*asc,u16 asclen,u8*hex)
{

    u16 len;
    asclen=asclen/2;
    for(len=0; len<asclen; len++)
    {

        hex[len]=((acstohex(asc[len*2] )<<4)&0xf0)|(acstohex(asc[len*2+1] )&0x0f);

    }

}
/*
*********************************************************************************************************
*	函 数 名: u16  wCptLastDay(u16 wYear, u16 wMonth);
*	功能说明:
*
*
*	形    参：
*	返 回 值: 本月的天数
*********************************************************************************************************
*/
u16   wCptLastDay(u16 Y, u16 M)
{
    u16 Ed;
    if(M==1 || M==3 || M==5 || M==7 || M==8 || M==10 || M==12) //大月 31天
    {
        Ed=31;
    }
    else if(M!=2)//不是2月份   30天
    {
        Ed=30;
    }
    else  //2月份
    {

        Ed=28;//平年28天
        if(Is_Leap_Year(Y))
            Ed=29;//润年 29天

    }
    return Ed;

}

/*
*********************************************************************************************************
*	函 数 名: void UtcToBeijingDate(u8 *time,u8 hour,u8 minute,u8 inc)
*	功能说明: time为0x12 0x06 0x22 0x12 0x22 0x45表示12年6月22日12点22分45秒
*
*
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
u32  BCDToSecond(u8*time);
void UtcToBeijingDate(u8 *time,u8 hour,u8 minute,u8 inc)
{
    u32 counter;
    u32 counter1;
    counter=BCDToSecond(time);
    counter1=minute*60+3600*hour;
    if(inc)
        counter+=counter1;
    else
        counter-=counter1;
    timer.counter=counter;
    GetRtcTime(time);

#if 0
    //  static u32 PositionOk=0;
    u16 wLastDay,wYear,wMonth,wDay,wHour,wminute;

    //时间
    wminute=(((time[4]>>4)&0x0f)*10)+(time[4]&0x0f);//min
    wHour = (((time[3]>>4)&0x0f)*10)+(time[3]&0x0f);//;//时
    wDay = (((time[2]>>4)&0x0f)*10)+(time[2]&0x0f);//;//日
    wMonth = (((time[1]>>4)&0x0f)*10)+(time[1]&0x0f);//;//月
    wYear = (((time[0]>>4)&0x0f)*10)+(time[0]&0x0f);//年
    if(inc)
        wminute += minute;

    if(inc)
        wHour += hour;
    if(wminute>60)
    {
        wHour+=1;
        wminute-=60;
    }


    wYear += 2000; //加上2000



    if(wHour >= 24)								//时间大于等于24
    {
        wHour -= 24;							//获得真实时间<24
        wDay++;
        wLastDay = wCptLastDay(wYear, wMonth);	//计算出本月最多的天数
        if(wDay>wLastDay)				//天可能需要往前迈进一天，也可能回到第一天
        {

            wDay = 1;							//置第一天
            wMonth++;
            if(wMonth >12)					//不可能有大于12的月份
            {
                wMonth = 1;						//置第一月
                wYear += 1;						//年份跨一年
            }
        }
    }

    wYear -= 2000;	//减去2000
    time[0]=(((wYear/10)<<4)&0xf0)|((wYear%10)&0x0f);
    time[1]=(((wMonth/10)<<4)&0xf0)|((wMonth%10)&0x0f);
    time[2]=(((wDay/10)<<4)&0xf0)|((wDay%10)&0x0f);
    time[3]=(((wHour/10)<<4)&0xf0)|((wHour%10)&0x0f);
    time[4]=(((wminute/10)<<4)&0xf0)|((wminute%10)&0x0f);
#endif

}



/*
*********************************************************************************************************
*	函 数 名:void DateInit(void)
*	功能说明:
*
*
*	形    参：
*	返 回 值
*********************************************************************************************************
*/
void DateInit(void)
{

    GpsControlStu.ConTrolStu=0;
    timer.rtcflag=0xff;
    GpsControlStu.GpsNoDateTime=0;
    GpsControlStu.GpsUnfixedTime=0;
    // StuWifi.cc3000stu=0;
    StuPower.mydog=0;
    GpsStatues.SgeeState=AGPS_IDLE;
    //  GsmSta.askwifi=1;
    GsmSta.askm2m=1;
    // GsmSta.batterylow=0;
    GsmSta.batterylowtrig=0;
    GsmSta.nopower=0;
    // GsmSta.AskAp=0;
    //StuKey.SystemState=SYSTEM_OFF;
    StuKey.SystemState=SYSTEM_ON;
    GpsControlStu.sleepManageTime=0;
    GsmSta.FromUnconnect=0;
    bvkstrGpsData.Latitude=6666666;
    GpsControlStu.Get5Position=NOT_OK;
    GsmSta.SendDate=NOT_OK;
    GpsControlStu.FindGpsTime=0;
    GsmSta.Sending=DATAS_IDLES;
    GsmSta.longitude=0;
    GsmSta.Latitude=bvkstrGpsData.Latitude;
    GsmSta.resettime=0;
    StuKey.SosSendTime=0;
    GsmSta.sos=0;
    GsmSta.ble=0;
    Memset( GsmSta.IMEI,0xff,9);
    GsmSta.LCDState=FLASH_TIME*2;
    GsmSta.opeabuflen=0;
}

/*
*********************************************************************************************************
*	函 数 名:void DateInit(void)
*	功能说明:
*
*
*	形    参：
*	返 回 值
*********************************************************************************************************
*/
void Memset(u8*data, u8 format ,u16 len )
{
    u16 i;
    for(i=0; i<len; i++)
        data[i]=format;


}


/*
*********************************************************************************************************
*	函 数 名:void FeedTaskDog(void)
*	功能说明:任务狗管理
*
*
*	形    参：
*	返 回 值
*********************************************************************************************************
*/
void FeedTaskDog(void)
{
    StuPower.mydog=0;

}

/*
*********************************************************************************************************
*	函 数 名:void ManageMyDog(void)
*	功能说明:任务狗管理
*
*
*	形    参：
*	返 回 值
*********************************************************************************************************
*/
void ManageMyDog(void)
{
    if(StuPower.mydog++>250)
    {
        NVIC_SystemReset();
#ifdef USE_PRINTF
        myprintf("任务看门狗复位\r\n");
#endif

    }

}

/*
*********************************************************************************************************
*	函 数 名:void BubbleSort(u16 array[],u16 len)
*	功能说明:冒泡排序
*
*
*	形    参：
*	返 回 值
*********************************************************************************************************
*/
void BubbleSort(u16 array[],u16 len)
{
    u16 i,j,temp;
    for(j=0; j<len; j++)
        for(i=0; i<len-j; i++)
            if(array[i]>array[i+1])
            {
                temp=array[i];
                array[i]=array[i+1];
                array[i+1]=temp;
            }
}


//positive=true:12---->0x12
//positive=false:0x12---->12
u8  DecToBcd(u8 in,u8 positive)
{
    u8 i;
    if(positive==true)
        i=(((in/10)<<4)&0xf0)|((in%10)&0x0f);
    else
        i=(((in>>4)&0x0f)*10)+(in&0x0f);
    return i;
}

void getvarity(u8*varity,u8*date,u16 len)
{
    u16 i;
    for(i=0; i<len; i++)
        varity[0]^=date[i];

}









/*

⑴在程序中将用到以下这些宏:

void va_start( va_list arg_ptr, prev_param );

type va_arg( va_list arg_ptr, type );

void va_end( va_list arg_ptr );

va在这里是variable-argument(可变参数)的意思.

这些宏定义在stdarg.h中,所以用到可变参数的程序应该包含这个头文件.

⑵函数里首先定义一个va_list型的变量,这里是arg_ptr,这个变量是指向参数地址的指针.因为得到参数的地址之后，再结合参数的类型，才能得到参数的值。

⑶然后用va_start宏初始化⑵中定义的变量arg_ptr,这个宏的第二个参数是可变参数列表的前一个参数,也就是最后一个固定参数。

⑷然后依次用va_arg宏使arg_ptr返回可变参数的地址,得到这个地址之后，结合参数的类型，就可以得到参数的值。然后进行输出。

⑸设定结束条件，这里的条件就是判断参数值是否为-1。注意被调的函数在调用时是不知道可变参数的正确数目的，程序员必须自己在代码中指明结束条件。



简易myprintf函数
 */

// va_arg.c

#include "stdio.h"

#include "stdarg.h"


static void esPrintStr( char *str )
{
    while ( *str != '\0' ) {
        esPrintChar( *str ++ );
    }
}

static void esPrintInt( int num, int radix )
{
    unsigned int  number = num;
    int  count = 0, unit;
    char buf[10];

    if ( ( radix == 10 ) && ( number >> 31 ) ) {
        esPrintChar( '-' );
        number = ( ~number + 1 ) & 0x7FFFFFFF;
    }

    do {
        unit   = number % radix;
        number = number / radix;

        if ( unit < 10 ) {
            buf[count++] = unit + '0';
        }
        else {
            buf[count++] = unit + 55;
        }
    } while ( number );

    if ( radix == 16 ) {
        buf[count] = '0';
        count = ( count + 1 ) & 0xFFFFFFFE;
    }
    while ( count > 0 ) {
        esPrintChar( buf[--count] );
    }
}

void myprintf( const char *format, ... )
{
    va_list arg_ptr;
    char    *str = ( char * )format;
    char    *s, y;
    int     x;

    va_start( arg_ptr, format );

    while ( *str != '\0' ) {
        if( *str != '%' ) {
            esPrintChar( *str ++ );
        }
        else {
            str ++;
            switch ( *str ++ ) {
            case 'd' :
                x = va_arg( arg_ptr, int );
                esPrintInt( x, 10 );
                break;

            case 'x' :
                x = va_arg( arg_ptr, int );
                esPrintInt( x, 16 );
                break;

            case 'c' :
                y = va_arg( arg_ptr, char );
                esPrintChar( y );
                break;

            case 's' :
                s = va_arg( arg_ptr, char* );
                if ( s != NULL ) {
                    esPrintStr( s );
                }
                break;

            default :
                esPrintStr( "\nError : Invalid format char '" );
                esPrintChar( *( --str ) );
                esPrintStr( "'.\n" );
                va_end( arg_ptr );
                return;
            }
        }
    }

    va_end( arg_ptr );
}


/*
int main(int argc, char* argv[])

{

myprintf("positive int = %d\n", 11);

myprintf("negtive int = %d\n", -11);

myprintf("char = %c\n", 'c');

myprintf("string = %s\n", "hello");

myprintf("float = %f\n", 3.14f);

myprintf("positive int = %d; negtive int = %d; char = %c; string = %s; float = %f\n", 13, -13, 'c', "hello!", 3.14f);

return 1;

}


*/
//myprintf("positive int = %d; negtive int = %d; char = %c; string = %s; float = %f\n", 13, -13, 'c', "hello!", 3.14f);


