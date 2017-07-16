#include"includes.h"

u8 debug=DEBUGGSM;


STU_POWER StuPower;
/*
*********************************************************************************************************
*	�� �� ��: u8 * Findbcd(u8 *indata,u16 inlen,u8 *outdata,u16 outlen)
*	����˵��: ��indata����outdata,
*
*
*	��    �Σ�
*	�� �� ֵ:�ҵ�����FINDNUM  ���򷵻�0
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
*	�� �� ��: u8 Equre(u8 *buf1,u8* buf2, u16 len)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ:��ȷ��� ok ������not ok
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
*	�� �� ��: u16 GetArryLen(u8 *buf,u16 maxlen)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ:
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
*	�� �� ��: u8 CalacXORVarity(u8*bufin,u16 len)
*	����˵��: �����ֻ�У���
*
*
*	��    �Σ�
*	�� �� ֵ:
*********************************************************************************************************
*/
u8 CalacXORVarity(u8*bufin,u16 len)
{
    u8 varity=0;
    getvarity(&varity,bufin,len);
    return varity;
}

//12234---"1234"
//ʮ����ת�ַ���
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
*	�� �� ��: u8 CalacXORVarity(u8*bufin,u16 len)
*	����˵��: �����ֻ�У���
*
*
*	��    �Σ�
*	�� �� ֵ:
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
*	�� �� ��:void Conver_aa_to_55(u8 *indata,u16 inlen,u8 *outbuf,u16*outlen)
*	����˵��: aa--->55 01   55--->55  02
*
*
*	��    �Σ�
*	�� �� ֵ:
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
*	�� �� ��:u16 strfindzero(u8*buf)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ:
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
*	�� �� ��: u8 EqureOrNot(u8* dat1,u8* dat2, u16 len)
*	����˵��:�Ƚ����ƶ������������ַ����Ƿ����
*
*
*	��    �Σ�
*	�� �� ֵ:��ȷ��� OS_TRUE
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
*	�� �� ��: void WaitToResetSystem(u8 second)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ:��ȷ��� OS_TRUE
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
*	�� �� ��: void ResetSystem(void)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ:��ȷ��� OS_TRUE
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
*	�� �� ��: u8* Finddate(u8*des,u8 date,u16 maxlen)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ:��ȷ��� OS_TRUE
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
*	�� �� ��:u8 acstohex(u8 datein)
*	����˵��:
*
*
*	��    �Σ�31--1
*	�� �� ֵ:��ȷ��� OS_TRUE
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
*	�� �� ��:u8  HexToAscll(u8 In,u8 flag)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ:
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
*	�� �� ��:void ConverascTohex(u8*asc,u16 asclen,u8*hex)
*	����˵��:
*
*
*	��    �Σ�"1234abcdef" ----12 34 ab cd ef
*	�� �� ֵ:��ȷ��� OS_TRUE
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
*	�� �� ��: u16  wCptLastDay(u16 wYear, u16 wMonth);
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ: ���µ�����
*********************************************************************************************************
*/
u16   wCptLastDay(u16 Y, u16 M)
{
    u16 Ed;
    if(M==1 || M==3 || M==5 || M==7 || M==8 || M==10 || M==12) //���� 31��
    {
        Ed=31;
    }
    else if(M!=2)//����2�·�   30��
    {
        Ed=30;
    }
    else  //2�·�
    {

        Ed=28;//ƽ��28��
        if(Is_Leap_Year(Y))
            Ed=29;//���� 29��

    }
    return Ed;

}

/*
*********************************************************************************************************
*	�� �� ��: void UtcToBeijingDate(u8 *time,u8 hour,u8 minute,u8 inc)
*	����˵��: timeΪ0x12 0x06 0x22 0x12 0x22 0x45��ʾ12��6��22��12��22��45��
*
*
*	��    �Σ�
*	�� �� ֵ: ��
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

    //ʱ��
    wminute=(((time[4]>>4)&0x0f)*10)+(time[4]&0x0f);//min
    wHour = (((time[3]>>4)&0x0f)*10)+(time[3]&0x0f);//;//ʱ
    wDay = (((time[2]>>4)&0x0f)*10)+(time[2]&0x0f);//;//��
    wMonth = (((time[1]>>4)&0x0f)*10)+(time[1]&0x0f);//;//��
    wYear = (((time[0]>>4)&0x0f)*10)+(time[0]&0x0f);//��
    if(inc)
        wminute += minute;

    if(inc)
        wHour += hour;
    if(wminute>60)
    {
        wHour+=1;
        wminute-=60;
    }


    wYear += 2000; //����2000



    if(wHour >= 24)								//ʱ����ڵ���24
    {
        wHour -= 24;							//�����ʵʱ��<24
        wDay++;
        wLastDay = wCptLastDay(wYear, wMonth);	//�����������������
        if(wDay>wLastDay)				//�������Ҫ��ǰ����һ�죬Ҳ���ܻص���һ��
        {

            wDay = 1;							//�õ�һ��
            wMonth++;
            if(wMonth >12)					//�������д���12���·�
            {
                wMonth = 1;						//�õ�һ��
                wYear += 1;						//��ݿ�һ��
            }
        }
    }

    wYear -= 2000;	//��ȥ2000
    time[0]=(((wYear/10)<<4)&0xf0)|((wYear%10)&0x0f);
    time[1]=(((wMonth/10)<<4)&0xf0)|((wMonth%10)&0x0f);
    time[2]=(((wDay/10)<<4)&0xf0)|((wDay%10)&0x0f);
    time[3]=(((wHour/10)<<4)&0xf0)|((wHour%10)&0x0f);
    time[4]=(((wminute/10)<<4)&0xf0)|((wminute%10)&0x0f);
#endif

}



/*
*********************************************************************************************************
*	�� �� ��:void DateInit(void)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ
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
*	�� �� ��:void DateInit(void)
*	����˵��:
*
*
*	��    �Σ�
*	�� �� ֵ
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
*	�� �� ��:void FeedTaskDog(void)
*	����˵��:���񹷹���
*
*
*	��    �Σ�
*	�� �� ֵ
*********************************************************************************************************
*/
void FeedTaskDog(void)
{
    StuPower.mydog=0;

}

/*
*********************************************************************************************************
*	�� �� ��:void ManageMyDog(void)
*	����˵��:���񹷹���
*
*
*	��    �Σ�
*	�� �� ֵ
*********************************************************************************************************
*/
void ManageMyDog(void)
{
    if(StuPower.mydog++>250)
    {
        NVIC_SystemReset();
#ifdef USE_PRINTF
        myprintf("�����Ź���λ\r\n");
#endif

    }

}

/*
*********************************************************************************************************
*	�� �� ��:void BubbleSort(u16 array[],u16 len)
*	����˵��:ð������
*
*
*	��    �Σ�
*	�� �� ֵ
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

���ڳ����н��õ�������Щ��:

void va_start( va_list arg_ptr, prev_param );

type va_arg( va_list arg_ptr, type );

void va_end( va_list arg_ptr );

va��������variable-argument(�ɱ����)����˼.

��Щ�궨����stdarg.h��,�����õ��ɱ�����ĳ���Ӧ�ð������ͷ�ļ�.

�ƺ��������ȶ���һ��va_list�͵ı���,������arg_ptr,���������ָ�������ַ��ָ��.��Ϊ�õ������ĵ�ַ֮���ٽ�ϲ��������ͣ����ܵõ�������ֵ��

��Ȼ����va_start���ʼ�����ж���ı���arg_ptr,�����ĵڶ��������ǿɱ�����б��ǰһ������,Ҳ�������һ���̶�������

��Ȼ��������va_arg��ʹarg_ptr���ؿɱ�����ĵ�ַ,�õ������ַ֮�󣬽�ϲ��������ͣ��Ϳ��Եõ�������ֵ��Ȼ����������

���趨������������������������жϲ���ֵ�Ƿ�Ϊ-1��ע�ⱻ���ĺ����ڵ���ʱ�ǲ�֪���ɱ��������ȷ��Ŀ�ģ�����Ա�����Լ��ڴ�����ָ������������



����myprintf����
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


