#include"includes.h"

//static u32 packet_total = 0;
static u32 packet_current = 0;

typedef __packed struct
{
    s32 longitude;
    s32 latitude;
    u32 update_time;
    u8  fixed_flag;
} FixPosition_T;

#define PI 3.1415926535898
#define EARTH_RADIUS   6378137 //����뾶(��λm)  
#define rad(d) ( d * PI / 180.0)


/**
    * ����googleMap�е��㷨�õ�����γ��֮��ľ���,���㾫����ȸ��ͼ�ľ��뾫�Ȳ�࣬��Χ��0.2������
    * @param lon1 ��һ��ľ���
    * @param lat1 ��һ���γ��
    * @param lon2 �ڶ���ľ���
    * @param lat3 �ڶ����γ��
    * @return ���صľ��룬��λkm
    *
 */
static u32 GetDistance(double lon1, double lat1, double lon2, double lat2)
{
    s32 distance = 0;
    double radLat1 = rad(lat1);
    double radLat2 = rad(lat2);
    double a = radLat1 - radLat2;
    double b = rad(lon1) - rad(lon2);
    double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
    distance = (int)(s * EARTH_RADIUS);
    //s = Math.round(s * 10000) / 10000;

    return abs(distance);
}

//static u8 ProcessPositionJump(s32 lon, s32 lat, u8 flag, u32 update_time)
u8 ProcessPositionJump(s32 lon, s32 lat, u8 flag, u32 update_time)
{
    static FixPosition_T last_posoition = {0};
    //  double lon1 = 0.0,lat1=0.0,lon2 = 0.0,lat2=0.0;
    u8   ret = 0;
    u32  dis = 0;

    //
    if(
        abs(lon) > 180000000 || abs(lon) < 1000000 ||
        abs(lat) > 90000000  || abs(lat) < 1000000
    )
    {
        return 1;
    }

    if( ( 0 == last_posoition.latitude ) ||
        ( (update_time - last_posoition.update_time ) >
          (GsmSto.moveintervalGPS + GsmSto.moveintervalGPS / 2)
        )
      )
    {
        ret = 0;
        last_posoition.longitude = lon;
        last_posoition.latitude = lat;
        last_posoition.fixed_flag = flag;
        last_posoition.update_time = update_time;
    }
    else
    {
        dis = GetDistance(last_posoition.longitude * 0.000001, \
                          last_posoition.latitude * 0.000001, \
                          lon * 0.000001, lat * 0.000001);

        // 150KM/Hour -> 2.5KM/min
        // 120KM/Hour -> 2KM/min
        // 100KM/Hour -> 1.67KM/min
        // 80KM/Hour ->  1.33KM/min
        if( flag == last_posoition.fixed_flag && last_posoition.fixed_flag == 1 )
        {
            ret = 1;
        }
        else if( flag == 3 && 1 == last_posoition.fixed_flag )
        {
            // GPS->LBS
            ret = 1;
        }
        else if(flag == 1 && 2 == last_posoition.fixed_flag )
        {
            // LBS -> GPS
            ret = 0;
        }

        if(  dis > 1500 * (GsmSto.moveintervalGPS / 60 + ret ) )
        {
            ret = 1;
        }
        else
        {
            ret = 0;
            last_posoition.longitude = lon;
            last_posoition.latitude = lat;
            last_posoition.fixed_flag = flag;
            last_posoition.update_time = update_time;
        }
    }

    return ret;
}



/*

*********************************************************************************************************
*   �� �� ��:void ProcessNetData(void)
*   ����˵��:����һ֡��������
*
*
*   ��    �Σ�
*   �� �� ֵ:
*********************************************************************************************************
*/


void ProcessNetData ( void )
{

    U16STU ID, MessageLen;
    u8 waterid, iTmp;;
    u8 *MessageBody, *pBbff;
    s8 ts8mp;

    if ( GsmRecDataStu.flag != GetAFram )
    {
        return ;
    }

    GsmRecDataStu.flag = 0;
    /*

    //�����õ�һ֡���ݺ�flag��־ΪGetAFram��RecCount�洢���ݵĳ��� RecData��Ϊ:water+lenh+lenL+idh+idL+date......+varity
    u16 RecCount;
    u8 flag;
    u8 RecData[GSMRECMAXLEN];
    */
    waterid = GsmRecDataStu.RecData[0];
    ID.stu.H = GsmRecDataStu.RecData[3];
    ID.stu.L = GsmRecDataStu.RecData[4];
    //���ݲ��ֵĳ���
    MessageLen.stu.H = GsmRecDataStu.RecData[1];
    MessageLen.stu.L = GsmRecDataStu.RecData[2];
    MessageLen.total -= 2;

    //���ݲ��ֵĿ�ʼ��ַ
    MessageBody = &GsmRecDataStu.RecData[5];



    //MessageBody     MessageLen.total     ID.total
    //����������ݷ���ok GsmSta.NetAck=OK;
    GsmSta.BasicKeepAlive = timer.counter;

    myprintf ( "Version = 0x%x%x, The message ID is 0x%x.\r\n", VERSION[0], VERSION[1], ID.total );

    switch ( ID.total )
    {
        case 0x8001:

            /*1�ն�ͨ��Ӧ��  ��ϢID 0x8001
            ��Ϣ��Ϊ����Ϣid+Ӧ�����Ϣid+Ӧ�����Ϣ��ˮ��+ �ɹ���־��1byte 0Ϊ�ɹ� 1Ϊʧ�ܣ�
            */
            if ( MessageLen.total != 4 )
            {
                break;
            }

            if ( ( MessageBody[0] == GsmSta.SendCmd[0] ) && ( MessageBody[1] == GsmSta.SendCmd[1] ) && ( MessageBody[2] == GsmSta.SendId ) )
            {
                GsmSta.NetAck = OK;
            }

#if 0

            if ( ( MessageBody[0] == 0x00 ) && ( MessageBody[1] == 0x0A ) )
            {
                //    GsmStatues.updata=STOP_UPDATE;
                // WaitToResetSystem(3);//ResetSystem();
            }

#endif
            break;

        case 0x8002:/*2�ն˲�������  ��ϢID 0x8002*/
            if ( OK == McuAnswerSetPara ( MessageBody[0], &MessageBody[1] ) )
            {
                McuAnswerCommon ( waterid, ID.total, OK );
            }
            else
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
            }

            break;

        case 0x8003:/*�ն˲�����ѯ ��ϢID 0x8003*/
            McuAnswerAskPara ( MessageBody[0], &MessageBody[1], 1 );
            break;

        case 0x8005:
            if ( MessageLen.total == 0 )
            {
                SendPosition ( TRIG_SEND_POSITION );

            }
            else
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
            }

            break;

#if 0

            if ( MessageLen.total == 0 )
            {

                GsmSta.AskAp = 1;
                McuAnswerCommon ( waterid, ID.total, OK );

            }
            else
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
            }

#endif

        //    McuAnswerCommon(waterid,ID.total,NOT_OK);
        //     break;
        case 0x8006:/*����wifi �ȵ���Ϣ*/
        case 0x8007:/*��ѯwifi mac*/
#if 0
            if ( MessageLen.total == 0 )
            {

                SendMac();


            }
            else
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
            }

#endif
            McuAnswerCommon ( waterid, ID.total, NOT_OK );
            break;

        case 0x800b:/*Զ������������ip�˿�����*/
            if ( ( GsmSto.updateflag == OK ) || ( resetflag == 0xaa ) )
            {
                return;
            }

            if ( MessageLen.total < 2 )
            {
                break;
            }

            if ( ( MessageBody[0] != 0x01 ) && ( MessageBody[0] != 0x02 ) )
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
                break;
            }

            if ( MessageBody[0] == 0x02 ) /*ʹ�õ�ǰ����������*/
            {

                StartUpdatemcuInit ( 0, 0, 0, 0 );
            }
            else
            {
                StartUpdatemcuInit ( &MessageBody[1 + strfindzero ( &MessageBody[1] ) + 1], strfindzero ( &MessageBody[1 + strfindzero ( &MessageBody[1] ) + 1] ), &MessageBody[1], strfindzero ( &MessageBody[1] ) );
            }

            McuAnswerCommon ( waterid, ID.total, OK );
            break;

        case 0x800c:/*��������800c tuH tuL   Lh LL totall3 totall2 totall1 totall0 varity */
            GsmSta.NetAck = OK;
            UpdateNetCome ( MessageBody );
            break;

        case 0x8012:
            if ( MessageLen.total == 0 )
            {
                WaitToResetSystem ( 10 );
                McuAnswerCommon ( waterid, ID.total, OK );
            }
            else
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
            }

            break;

        case 0x8014:/*����SGEE����*/
            AtNetSgee.Sgeedatecome = OK;
            GsmSta.NetAck = OK;
            SGEENetCome ( MessageBody );
            break;

        case 0x8017:/*���ä������*/
            if ( MessageLen.total == 0 )
            {
                DeleteAllDate();
                McuAnswerCommon ( waterid, ID.total, OK );
            }
            else
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
            }

            break;

        case 0x8019:/*�����ظ�����������*/
            if ( MessageLen.total == 0 )
            {
                DeleteAllDate();
                WriteGsmStoreDateToDefault();
                McuAnswerCommon ( waterid, ID.total, OK );
            }
            else
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
            }

            break;

        case 0x801b:/*�·�ʱ��*/
            if ( ( MessageLen.total != 6 ) && ( MessageLen.total != 13 ) )
            {
                break;
            }

            SetRtcTime ( &MessageBody[0] );
            McuAnswerCommon ( waterid, ID.total, OK );

            if ( MessageLen.total == 13 )
            {
                ts8mp = MessageBody[6];

                if ( ts8mp >= 0 )
                {
                    GsmSto.Timehour = ts8mp;
                }
                else
                {
                    GsmSto.Timehour = -ts8mp;
                    GsmSto.Timehour |= 0x80;
                }

                ts8mp = MessageBody[7];

                if ( ts8mp >= 0 )
                {
                    GsmSto.Timeminute = ts8mp;
                }
                else
                {
                    GsmSto.Timeminute = -ts8mp;
                }

                GsmSto.lcdTemperature[0] = MessageBody[8];
                GsmSto.lcdTemperature[1] = MessageBody[9];

                GsmSto.lcdWeather[0] = MessageBody[10];
                GsmSto.lcdWeather[1] = MessageBody[11];
                GsmSto.lcdWeather[2] = MessageBody[12];
                WriteGsmStoreDate();
            }

            break;

        case 0x801c:
            if ( MessageBody[0] > 5 )
            {
                McuAnswerCommon ( waterid, ID.total, NOT_OK );
                break;
            }

            pBbff = MessageBody + 1;
            GsmSto.sosNumber = MessageBody[0];

            for ( ts8mp = 0;  ts8mp < MessageBody[0]; ts8mp ++ )
            {
                GsmSto.sosPhoneLen[ts8mp] = *pBbff++;

                if ( GsmSto.sosPhoneLen[ts8mp] > 15 )
                {
                    GsmSto.sosPhoneLen[ts8mp] = 15;
                }

                GsmSto.sosPhone[ts8mp][0] = 'A';
                GsmSto.sosPhone[ts8mp][1] = 'T';
                GsmSto.sosPhone[ts8mp][2] = 'D';

                for ( iTmp = 0; iTmp < GsmSto.sosPhoneLen[ts8mp]; iTmp ++ )
                {
                    GsmSto.sosPhone[ts8mp][3 + iTmp] = *pBbff++;
                }

                GsmSto.sosPhone[ts8mp][3 + iTmp] = ';';
                GsmSto.sosPhone[ts8mp][4 + iTmp] = '\r';
                GsmSto.sosPhone[ts8mp][5 + iTmp] = '\0';
                GsmSto.sosPhoneLen[ts8mp] += 5;
            }

            WriteGsmStoreDate();
            McuAnswerCommon ( waterid, ID.total, OK );
            break;

        default :
            break;

    }
}




/*
*********************************************************************************************************
*   �� �� ��:u8  McuAnswerAskPara(u8 TatolParta,u8* Paradata,u8 FromNet)
*   ����˵��:�ն˲�ѯ����
*
*
*   ��    �Σ�TatolParta ��������     Paradata:�����б�
*   �� �� ֵ:
*********************************************************************************************************
*/
/*��Ϣ��Ϊ��0x80 0x03 +����������byte��+����id�б�
Ӧ������Ϊ��Ӧ��0x0003
�����б�=[����id 1��byte��]+
��������  [����id 2��byte��]+
����������������..
*/


/*
Ӧ��
��Ϣ��Ϊ��0x00 0x03 +����������byte��+�����б��������б��
�����б�=[����id��byte��+�������ȣ�byte��+��������]+
������������[����id��byte��+�������ȣ�byte��+��������]+
����������������..

*/
u8  McuAnswerAskPara ( u8 TatolParta, u8 *Paradata, u8 FromNet )
{

    u8 data[500];
    //    u16 outlen;
    u16 len = 0;
    u16 i;

#if 0
    ����id   1byte  ��������1byte   ��������    ����˵��
    0x01���������룩    6��ascll��  0x31 0x32 0x33 0x34 0x35 0x36   ��ʾ��������Ϊ����123456��
    0x02������ip�˿ڣ�  Len ( ascll )   ��liupeng08304.oicp.net: 8899�� Ip�Ͷ˿���ð�Ÿ�������ʾip��liupeng08304.oicp.net, �˿�Ϊ��8899
0x03������apn�� Len ( ascll )   ��cmnet��   ��ʾapnΪ:
    cmnet
    0x04
    ����ֹʱ�㱨λ����Ϣ�����
    2��ʮ�����ƣ���λ�룩   0x00 0x3c   ��ʾ���Ϊ��0x003c, ���Ϊ0�򲻻㱨
    0x05
    ���˶�ʱ�㱨λ����Ϣ�����
    2��ʮ�����ƣ���λ�룩   0x00 0x78   ��ʾ���Ϊ��0x0078s
    0x06�������ͱ�����ֵ��  1   0x0a    ��Χ: 0--100  ��ʾ��������10 % ʱ����
    0x07�������豸id��  10��16���ƣ�    0x12 0x34 0x56 0x78 0x90 0xab 0xcd 0xef 0x12 0x34   ��ʾ�ն�idΪ��0x1234567890abcdef1234
    0x08 ( �����Ȩģʽ )   1   hex 0x00 -- -PAP������ ( Ĭ�� )
    0x01 -- -PAP��������˺�
    0x02-- -CHAP��������˺�

    0x09 ( �����Ȩ�û��� ) len ascII
    0x0a ( �����Ȩ���� )   len ascII
    0x0b ( ���ö���Ϣ���ĺ��� ) len ascII
    0x0c ( ������� )   2��ʮ�����ƣ���λ�룩   0x00 0x78   ��ʾ���Ϊ��0x0078s
#endif

    data[len++] = 0x00;
    data[len++] = 0x05;
    data[len++] = TatolParta;


    for ( i = 0; i < TatolParta; i++ )
    {
        data[len++] = Paradata[i];

        switch ( Paradata[i] )
        {

            case 0x01:/*��������*/
                data[len++] = 6;
                Mymemcpy ( &data[len], GsmSto.mima, 6 );
                len += 6;
                break;

            case 0x02:/*ip�˿�*/
                data[len++] = GsmSto.iplen + GsmSto.portlen + 1;
                Mymemcpy ( &data[len], GsmSto.strip, GsmSto.iplen );
                len += GsmSto.iplen;
                data[len++] = ':';
                Mymemcpy ( &data[len], GsmSto.port, GsmSto.portlen );
                len += GsmSto.portlen;
                break;

            case 0x03:/*apn*/
#if 0
                data[len++] = GsmSto.apnlen;
                Mymemcpy ( &data[len], GsmSto.strapn, GsmSto.apnlen );
                len += GsmSto.apnlen;
#else
                data[len++] = StuApnNet.apnlen;
                Mymemcpy ( &data[len], StuApnNet.strapn, StuApnNet.apnlen );
                len += StuApnNet.apnlen;

#endif


                break;

            case 0x04:/*��ֹʱ�㱨λ����Ϣ���*/
                data[len++] = 2;
                data[len++] = ( u8 ) ( ( GsmSto.staticinterval >> 8 ) & 0x00ff );
                data[len++] = ( u8 ) ( GsmSto.staticinterval & 0x00ff );
                break;

            case 0x05:/*�˶�ʱ�㱨λ����Ϣ���*/
                data[len++] = 4;
                data[len++] = ( u8 ) ( ( GsmSto.moveintervalGPS >> 8 ) & 0x00ff );
                data[len++] = ( u8 ) ( GsmSto.moveintervalGPS & 0x00ff );
                data[len++] = ( u8 ) ( ( GsmSto.moveintervalGSM >> 8 ) & 0x00ff );
                data[len++] = ( u8 ) ( GsmSto.moveintervalGSM & 0x00ff );
                break;

            case 0x06:/*�����ͱ�����ֵ*/
                data[len++] = 1;
                data[len++] = GsmSto.BatteryQuatity;
                break;

            case 0x07:/*�豸id*/
                data[len++] = 10;
                Mymemcpy ( &data[len], GsmSto.ID, 10 );
                len += 10;
                break;

            case 0x08:/*�����Ȩģʽ*/
                data[len++] = 1;

                if ( GsmSto.NetId == PAP_MODE )
                {
                    data[len++] = 1;
                }
                else if ( GsmSto.NetId == CHAP_MODE )
                {
                    data[len++] = 2;
                }
                else
                {
                    data[len++] = 0;
                }

                break;

            case 0x09:/*�����Ȩ�û���*/
                data[len++] = GsmSto.NetIdNameLen;

                if ( GsmSto.NetIdNameLen )
                {
                    Mymemcpy ( & data[len], GsmSto.NetIdName, GsmSto.NetIdNameLen );
                    len += GsmSto.NetIdNameLen;
                }

                break;

            case 0x0a:/*�����Ȩ����*/
                data[len++] = GsmSto.NetIdPsswdLen;

                if ( GsmSto.NetIdPsswdLen )
                {
                    Mymemcpy ( & data[len], GsmSto.NetIdPsswd, GsmSto.NetIdPsswdLen );
                    len += GsmSto.NetIdPsswdLen;
                }

                break;

            case 0x0b:/*���ö���Ϣ���ĺ���*/
                data[len++] = GsmSto.MessageCenterLen;

                if ( GsmSto.NetIdPsswdLen )
                {
                    Mymemcpy ( & data[len], GsmSto.MessageCenter, GsmSto.MessageCenterLen );
                    len += GsmSto.MessageCenterLen;
                }

                break;

            case 0x0c:/*�������*/
                data[len++] = 2;
                data[len++] = ( u8 ) ( ( GsmSto.KeepAliveInter >> 8 ) & 0x00ff );
                data[len++] = ( u8 ) ( GsmSto.KeepAliveInter & 0x00ff );
                break;

            case 0x0d:/*������汾*/
                data[len++] = 2;
                data[len++] = VERSION[0];
                data[len++] = VERSION[1];
                break;

            case 0x0e:/*ʱ��*/
                data[len++] = 1;
                data[len++] = GsmSto.Timehour;
                break;

            case 0x0f:/*12h/24h*/
                data[len++] = 1;
                data[len++] = GsmSto.Is12h;

                break;


            default :
                return NOT_OK;


        }


    }


    //printf("��ѯ�����ܳ�%d\r\n",len);
    if ( FromNet )
    {
        FlashBufWriteLong ( data, len, 0 );
    }
    else
    {
        DebugUartUpdataPackegToUsart ( data, len );
    }

    return OK;


}


/*
*********************************************************************************************************
*   �� �� ��:void McuAnswerSetPara(u8 TatolParta,u8* Paradata)
*   ����˵��:�ն˲�������
*
*
*   ��    �Σ�TatolParta ��������     Paradata:�����б�
*   �� �� ֵ:
*********************************************************************************************************
*/
/*��Ϣ��Ϊ��0x00 0x03 +����������byte��+�����б��������б��
�����б�= [����id��byte��+�������ȣ�byte��+��������]+
���������[[����id��byte��+�������ȣ�byte��+��������]+
����������������
*/
u8  McuAnswerSetPara ( u8 TatolParta, u8 *Paradata )
{
    u16 ParaPosition;
    u8 *p, *p1;
    u16 i;
    u8 k;
    u8  modebuf[3] = {EMPTY_MODE, PAP_MODE, CHAP_MODE};

    //80 02 01 FF 0A 00 00 00 00 01 31 68 09 47 19 6B
    // [����id��byte��+�������ȣ�byte��+��������]
    ParaPosition = 0;

    for ( i = 0; i < TatolParta; i++ )
    {
        switch ( Paradata[ParaPosition++] )
        {
#if 0
                ����id   1byte  ��������1byte   ��������    ����˵��
                0x01���������룩    6��ascll��  0x31 0x32 0x33 0x34 0x35 0x36   ��ʾ��������Ϊ����123456��
                0x02������ip�˿ڣ�  Len ( ascll )   ��liupeng08304.oicp.net: 8899�� Ip�Ͷ˿���ð�Ÿ�������ʾip��liupeng08304.oicp.net, �˿�Ϊ��8899
0x03������apn�� Len ( ascll )   ��cmnet��   ��ʾapnΪ:
                cmnet
                0x04
                ����ֹʱ�㱨λ����Ϣ�����
                2��ʮ�����ƣ���λ�룩   0x00 0x3c   ��ʾ���Ϊ��0x003c, ���Ϊ0�򲻻㱨
                0x05
                ���˶�ʱ�㱨λ����Ϣ�����
                2��ʮ�����ƣ���λ�룩   0x00 0x78   ��ʾ���Ϊ��0x0078s
                0x06�������ͱ�����ֵ��  1   0x0a    ��Χ: 0--100  ��ʾ��������10 % ʱ����
                0x07�������豸id��  10��16���ƣ�    0x12 0x34 0x56 0x78 0x90 0xab 0xcd 0xef 0x12 0x34   ��ʾ�ն�idΪ��0x1234567890abcdef1234
                0x08 ( �����Ȩģʽ )   1   hex 0x00 -- -PAP������ ( Ĭ�� )
                0x01 -- -PAP��������˺�
                0x02-- -CHAP��������˺�

                0x09 ( �����Ȩ�û��� ) len ascII
                0x0a ( �����Ȩ���� )   len ascII
                0x0b ( ���ö���Ϣ���ĺ��� ) len ascII
                0x0c ( ������� )   2��ʮ�����ƣ���λ�룩   0x00 0x78   ��ʾ���Ϊ��0x0078s
#endif

            case 0x01:/*����Ϣ��������*/
                if ( 6 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {

                    Mymemcpy ( GsmSto.mima, &Paradata[ParaPosition], 6 );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/

                    break;
                }

                return NOT_OK;

            case 0x02:/*����ip�˿�*/
                if ( 7 < Paradata[ParaPosition++] ) /*�жϳ���liupeng08304.oicp.net:8899*/
                {

                    p = &Paradata[ParaPosition];
                    p1 = Finddate ( &p[0], ':', 79 + 1 );

                    if ( p1 == 0 )
                    {
                        return NOT_OK;
                    }

                    GsmSto.iplen = p1 - p;
                    Mymemcpy ( GsmSto.strip, &p[0], GsmSto.iplen );


                    GsmSto.portlen = Paradata[ParaPosition - 1] - 1 - GsmSto.iplen;

                    if ( ( Paradata[ParaPosition - 1] - 1 - GsmSto.iplen ) < 8 )
                    {
                        Mymemcpy ( GsmSto.port, &p1[1], GsmSto.portlen );
                        ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                        break;
                    }

                    WaitToResetSystem ( 30 );
                }

                return NOT_OK;

            case 0x03:/*����apn*/
                ParaPosition++;/*�жϳ���*/

                if ( Paradata[ParaPosition - 1] < 39 )
                {
                    GsmSto.apnlen = Paradata[ParaPosition - 1];
                    Mymemcpy ( GsmSto.strapn, &Paradata[ParaPosition], GsmSto.apnlen );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    WaitToResetSystem ( 30 );
                    break;
                }

                return NOT_OK;

            case 0x04:/*��ֹʱ�㱨λ����Ϣ���*/
                if ( 2 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {


                    GsmSto.staticinterval = ( ( ( u16 ) Paradata[ParaPosition] << 8 ) & 0xff00 ) | ( ( ( u16 ) Paradata[ParaPosition + 1] << 0 ) & 0x00ff );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/

                    break;
                }

                return NOT_OK;

            case 0x05:/*�˶�ʱ�㱨λ����Ϣ���*/
                if ( 4 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {

                    GsmSto.moveintervalGPS = ( ( ( u16 ) Paradata[ParaPosition] << 8 ) & 0xff00 ) | ( ( ( u16 ) Paradata[ParaPosition + 1] << 0 ) & 0x00ff );
                    GsmSto.moveintervalGSM = ( ( ( u16 ) Paradata[ParaPosition + 2] << 8 ) & 0xff00 ) | ( ( ( u16 ) Paradata[ParaPosition + 3] << 0 ) & 0x00ff );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/

                    break;
                }

                return NOT_OK;

            case 0x06:/*�����ͱ�����ֵ*/
                if ( 1 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {
                    if ( Paradata[ParaPosition] < 101 )
                    {
                        GsmSto.BatteryQuatity = Paradata[ParaPosition];
                        ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                        break;
                    }
                }

                return NOT_OK;

            case 0x07:/*�����豸id*/
                if ( 10 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {

                    Mymemcpy ( GsmSto.ID, &Paradata[ParaPosition], 10 );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    WaitToResetSystem ( 30 );
                    break;
                }

                return NOT_OK;

            case 0x08:/*�����Ȩģʽ*/
                if ( 1 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {
                    k = Paradata[ParaPosition];

                    if ( k < 3 )
                    {
                        GsmSto.NetId = modebuf[k];
                        ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    }

                    WaitToResetSystem ( 30 );
                    break;
                }

                return NOT_OK;

            case 0x09:/*�����Ȩ�û���*/
                ParaPosition++;/*�жϳ���*/

                if ( Paradata[ParaPosition - 1] < 79 )
                {
                    GsmSto.NetIdNameLen = Paradata[ParaPosition - 1];
                    Mymemcpy ( GsmSto.NetIdName, &Paradata[ParaPosition], GsmSto.NetIdNameLen );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    WaitToResetSystem ( 30 );
                    break;
                }

                return NOT_OK;

            case 0x0a:/*�����Ȩ����*/
                ParaPosition++;/*�жϳ���*/

                if ( Paradata[ParaPosition - 1] < 79 )
                {
                    GsmSto.NetIdPsswdLen = Paradata[ParaPosition - 1];
                    Mymemcpy ( GsmSto.NetIdPsswd, &Paradata[ParaPosition], GsmSto.NetIdPsswdLen );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    WaitToResetSystem ( 30 );
                    break;
                }

                return NOT_OK;

            case 0x0b:/*���ö���Ϣ���ĺ���*/
                ParaPosition++;/*�жϳ���*/

                if ( Paradata[ParaPosition - 1] < 19 )
                {
                    GsmSto.MessageCenterLen = Paradata[ParaPosition - 1];
                    Mymemcpy ( GsmSto.MessageCenter, &Paradata[ParaPosition], GsmSto.MessageCenterLen );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    break;
                }

                return NOT_OK;

            case 0x0c:/*�������*/
                if ( 2 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {
                    GsmSto.KeepAliveInter = ( ( ( u16 ) Paradata[ParaPosition] << 8 ) & 0xff00 ) | ( ( ( u16 ) Paradata[ParaPosition + 1] << 0 ) & 0x00ff );
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    myprintf ( "GsmSto.KeepAliveInter is setting to %d", GsmSto.KeepAliveInter );
                    break;
                }

                return NOT_OK;

            case 0x0e:/*ʱ��*/
                if ( 1 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {
                    GsmSto.Timehour = Paradata[ParaPosition];
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    break;
                }

                return NOT_OK;

            case 0x0f:/*12 24*/
                if ( 1 == Paradata[ParaPosition++] ) /*�жϳ���*/
                {
                    GsmSto.Is12h = Paradata[ParaPosition];
                    ParaPosition += Paradata[ParaPosition - 1]; /*ƫ�Ƶ���һ������*/
                    break;
                }

                return NOT_OK;

            default :
                return NOT_OK;

        }

    }

    WriteGsmStoreDate();
    return OK;
}

/*
*********************************************************************************************************
*   �� �� ��:  void UPdateOk(void)d)
*   ����˵��: Զ����������:Զ�������ɹ�
*
*
*   ��    �Σ�UpdateResultΪ0��ʾ�����ɹ��������ʾ����ʧ��

*   �� �� ֵ:
*********************************************************************************************************
*/

#if 0
0x00��ʾ�����ɹ���
0x01��ʾ����ʧ�ܣ�дflash����
0x02��ʾ����ʧ��, У�������
0x03��ʾ�������ݰ�������
0x04��ʾ���������ݰ���С������
0x05��ʱʧ��
#endif
void UPdateOk ( u8 UpdateResult )
{
    u8 data[3];
    STRUCT_BLIND_FRAM StuFram;
    data[0] = 0x00;
    data[1] = 0x0a;
    data[2] = UpdateResult;
    StuFram.len = 3;
    Mymemcpy ( &StuFram.buflen[0], ( u8 * ) &data[0], StuFram.len );
    FlashBufWrite ( StuFram, 0 );

}

/*
*********************************************************************************************************
*   �� �� ��:  u8 CalcUpdateVarity(void)
*   ����˵��:������������У����
*
*
*   ��    �Σ�

*   �� �� ֵ:
*********************************************************************************************************
*/
u8 CalcUpdateVarity ( void )
{
    u8 buf[256];
    u16 i, j, k, len;
    u8 varity = 0;
    // u16 m;
    j = GsmSto.Softwarelen / 256;
    k = GsmSto.Softwarelen % 256;
    len = 256;
#ifdef ENABLE_DOG
    WDOG_Feed();
#endif

    for ( i = 0; i <= j; i++ )
    {
        if ( i == j )
        {
            if ( k )
            {
                len = k;
            }
            else
            {
                break;
            }
        }

        ReadApp ( i * 256, buf, len );
        getvarity ( &varity, &buf[0], len );
        //  for(m=0; m<len; m++)
        //    varity^=buf[m];
    }

    return varity;



}
/*
*********************************************************************************************************
*   �� �� ��:  u8 CalcNetSgeeVarity(void)
*   ����˵��:����SGEE����У����
*
*
*   ��    �Σ�

*   �� �� ֵ:
*********************************************************************************************************
*/
u8 CalcNetSgeeVarity ( void )
{
    u8 buf[100];
    u16 i, j, k, len;
    u8 varity = 0;
    // u8 m;



#if 0
    void EraseNetSGEEToFlash ( void )
    void WriteNetSGEEToFlash ( u8 * datain, u16 datelen, u32 offset ) ///
    void ReadNetSGEE ( u8 * outbuf, u32 offset, u16 len )   ///
    u8  ReadSGEEHavedate ( void )
    void WriteSGEEHavedate ( u8 have )
#endif
    j = GpsStatues.offset / 100;
    k = GpsStatues.offset % 100;
    len = 100;

    for ( i = 0; i <= j; i++ )
    {
        if ( i == j )
        {
            if ( k )
            {
                len = k;
            }
            else
            {
                break;
            }
        }

        //   ReadApp(i*100,buf,len);
        ReadNetSGEE ( buf, i * 100, len );

        getvarity ( &varity, &buf[0], len );


        //  for(m=0; m<len; m++)
        //      varity^=buf[m];
    }

    return varity;



}

/*
*********************************************************************************************************
*   �� �� ��:  u16  AskAppPacket(void)
*   ����˵��:���������������������������
*
*
*   ��    �Σ�

*   �� �� ֵ:
*********************************************************************************************************
*/

u16  AskAppPacket ( void )
{
    U16STU packet;
    u8 data[4];
    STRUCT_BLIND_FRAM StuFram;
    data[0] = 0x00;
    data[1] = 0x09;
    packet.total = GsmSto.currentpackeg;
    data[2] = packet.stu.H;
    data[3] = packet.stu.L;
    StuFram.len = 4;
    Mymemcpy ( &StuFram.buflen[0], ( u8 * ) &data[0], StuFram.len );
    FlashBufWrite ( StuFram, 0 );
    return GsmSto.currentpackeg;
}

/*
*********************************************************************************************************
*   �� �� ��:  u16  AskSgeePacket(void)
*   ����˵��:���������������������������
*
*
*   ��    �Σ�

*   �� �� ֵ:
*********************************************************************************************************
*/
u16  AskSgeePacket ( void )
{
    U16STU packet;
    u8 data[4];
    STRUCT_BLIND_FRAM StuFram;
    data[0] = 0x00;
    data[1] = 0x0f;
    packet.total = AtNetSgee.currentpackeg;
    data[2] = packet.stu.H;
    data[3] = packet.stu.L;
    StuFram.len = 4;
    Mymemcpy ( &StuFram.buflen[0], ( u8 * ) &data[0], StuFram.len );
    FlashBufWrite ( StuFram, 0 );
    return AtNetSgee.currentpackeg;
}
/*
*********************************************************************************************************
*   �� �� ��:  void  SgeeReport(u8 result)
*   ����˵��:Sgee����㱨
*
*
*   ��    �Σ�

*   �� �� ֵ:
*********************************************************************************************************
*/
void  SgeeReport ( u8 result )
{
    u8 data[3];
    STRUCT_BLIND_FRAM StuFram;
    data[0] = 0x00;
    data[1] = 0x10;
    data[2] = result;
    StuFram.len = 3;
    Mymemcpy ( &StuFram.buflen[0], ( u8 * ) &data[0], StuFram.len );
    FlashBufWrite ( StuFram, 0 );
}


/*
*********************************************************************************************************
*   �� �� ��:  void  AskTime(void)
*   ����˵��:ʱ������
*
*
*   ��    �Σ�

*   �� �� ֵ:
*********************************************************************************************************
*/
void  AskTime ( void )
{
    u8 data[2];
    STRUCT_BLIND_FRAM StuFram;
    data[0] = 0x00;
    data[1] = 0x14;
    StuFram.len = 2;
    Mymemcpy ( &StuFram.buflen[0], ( u8 * ) &data[0], StuFram.len );
    FlashBufWrite ( StuFram, 0 );
}
#if 0
/*

*********************************************************************************************************
*   �� �� ��:u8  FindKggo(u16 turn,u16 size,u8 *MessageBody)
*   ����˵��:������0x250�ĵ�ַ�Ƿ��
*
*
*   ��    �Σ�
*   �� �� ֵ:
*********************************************************************************************************
*/
const u8  KGGO[4] = {0xe1, 0x12, 0x56, 0x6d};
u8  FindKggo ( u16 turn, u16 size, u8 *MessageBody, u8 netflag )
{
    static u8 stu = 0;

    u32 head, tail, len;
    u8 i;
    head = size * turn;
    tail = head + size - 1;

    switch ( stu )
    {
        case 0:
            if ( ( tail >= 0x250 ) && ( head <= 0x253 ) )
            {
                len = tail + 1 - 0x250;

                if ( len == 1 )
                {
                    if ( MessageBody[size - 1] != KGGO[0] )
                    {


                        if ( netflag )
                        {
                            // GsmStatues.updata=STOP_UPDATE;
                            // UPdateOk(SendNow,3);
                        }
                        else
                        {
                            //  Usart1UpdataSendPackegResult(5);
                        }

                        return NOT_OK;
                    }

                    stu = 3;
                }
                else    if ( len == 2 )
                {
                    if ( ( MessageBody[size - 2] != KGGO[0] ) || ( MessageBody[size - 1] != KGGO[1] ) )
                    {

                        if ( netflag )
                        {
                            //  GsmStatues.updata=STOP_UPDATE;
                            //  UPdateOk(SendNow,3);
                        }
                        else
                        {
                            //   Usart1UpdataSendPackegResult(5);
                        }

                        return NOT_OK;
                    }

                    stu = 2;


                }
                else    if ( len == 3 )
                {
                    if ( ( MessageBody[size - 3] != KGGO[0] ) || ( MessageBody[size - 2] != KGGO[1] ) || ( MessageBody[size - 1] != KGGO[2] ) )
                    {

                        if ( netflag )
                        {
                            //      GsmStatues.updata=STOP_UPDATE;
                            //     UPdateOk(SendNow,3);
                        }
                        else
                        {
                            //    Usart1UpdataSendPackegResult(5);
                        }

                        return NOT_OK;
                    }

                    stu = 1;


                }
                else    if ( len > 3 )
                {
                    if ( ( MessageBody[size - len] != KGGO[0] ) || ( MessageBody[size - len + 1] != KGGO[1] ) || ( MessageBody[size - len + 2] != KGGO[2] ) || ( MessageBody[size - len + 3] != KGGO[3] ) )
                    {

                        if ( netflag )
                        {
                            //  GsmStatues.updata=STOP_UPDATE;
                            //   UPdateOk(SendNow,3);
                        }
                        else
                        {
                            //  Usart1UpdataSendPackegResult(5);
                        }

                        return NOT_OK;
                    }

                    stu = 0;


                }


            }

            break;

        case 3:/*�Ƚ�����3��*/
        case 2:/*�Ƚ�����2��*/
        case 1:/*�Ƚ�����1��*/

            for ( i = 0; i < stu; i++ )
            {
                if ( ( MessageBody[i] != KGGO[i] ) )
                {

                    if ( netflag )
                    {
                        //   GsmStatues.updata=STOP_UPDATE;
                        //   UPdateOk(SendNow,3);
                    }
                    else
                    {
                        // Usart1UpdataSendPackegResult(5);
                    }

                    return NOT_OK;
                }

            }

            stu = 0;
            break;
    }



    return OK;




}
#endif

/*
*********************************************************************************************************
*   �� �� ��:  void UpdateNetCome(u8*datein)
*   ����˵��:
*
*
*   ��    �Σ�

*   �� �� ֵ:
*********************************************************************************************************
*/
void UpdateNetCome ( u8 *datein )
{
    u8 buf[256];
    u8 k, j;
    u16 len, i;
    U16STU turn, stulen;
    U32STU softwarelen;
    turn.stu.H = datein[0];
    turn.stu.L = datein[1];
    stulen.stu.H = datein[2];
    stulen.stu.L = datein[3];
    softwarelen.stu.Highter = datein[4 + stulen.total];
    softwarelen.stu.Hight = datein[5 + stulen.total];
    softwarelen.stu.Lower = datein[6 + stulen.total];
    softwarelen.stu.Low = datein[7 + stulen.total];
    GsmSta.updatetime20sec = 255;
    GsmSta.updatetime20min = 0;

    if ( GsmSto.updateflag == NOT_OK )
    {
        return;
    }

    if ( turn.total == GsmSto.currentpackeg )
    {

        //softwarelen.total
        if ( 0 == turn.total ) /*��0��*/
        {

            GsmSto.packetsize = stulen.total;
            GsmSto.updatevarity = datein[8 + stulen.total];
            GsmSto.totalpackeg = softwarelen.total / stulen.total;

            GsmSto.Softwarelen = softwarelen.total;

            if ( ( softwarelen.total < ( 10 * 1024 ) ) || ( softwarelen.total > ( 64 * 1024 ) ) ) /*���������ݰ���С������*/
            {
                GsmSto.updateflag = NOT_OK;
                UPdateOk ( 4 ); /*����ʧ��*/
                WriteGsmStoreDate();
                return ;
            }


            if ( softwarelen.total % stulen.total )
            {
                GsmSto.totalpackeg++;
            }

            /*��������������*/
            EraseApp ( GsmSto.Softwarelen );
            DeleteAllDate();
        }



        WriteApp ( turn.total * GsmSto.packetsize, &datein[4], stulen.total );
        j = stulen.total / 256;
        k = stulen.total % 256;
        len = 256;

        for ( i = 0; i <= j; i++ )
        {
            if ( i == j )
            {
                if ( k )
                {
                    len = k;
                }
                else
                {
                    break;
                }
            }

            ReadApp ( turn.total * GsmSto.packetsize + i * 256, buf, len );

            if ( EQUER != EqureOrNot ( buf, &datein[4 + i * 256], len ) )
            {
                GsmSto.updateflag = NOT_OK;
                UPdateOk ( 1 ); /*����ʧ��*/
                WriteGsmStoreDate();
                break;
            }



        }



        WriteGsmStoreDate();
        GsmSto.currentpackeg++;


    }




}




/*
*********************************************************************************************************
*   �� �� ��:  void SGEENetCome(u8*datein)
*   ����˵��:
*
*
*   ��    �Σ�

*   �� �� ֵ:
*********************************************************************************************************
*/
void SGEENetCome ( u8 *datein )
{
    static u8 buf[100];
    u8 k, j;
    u16 len, i;
    U16STU turn, stulen;
    U32STU softwarelen;
    turn.stu.H = datein[0];
    turn.stu.L = datein[1];
    stulen.stu.H = datein[2];
    stulen.stu.L = datein[3];
    softwarelen.stu.Highter = datein[4 + stulen.total];
    softwarelen.stu.Hight = datein[5 + stulen.total];
    softwarelen.stu.Lower = datein[6 + stulen.total];
    softwarelen.stu.Low = datein[7 + stulen.total];



#if 0
    void EraseNetSGEEToFlash ( void )
    void WriteNetSGEEToFlash ( u8 * datain, u16 datelen, u32 offset ) ///
    void ReadNetSGEE ( u8 * outbuf, u32 offset, u16 len )   ///
    u8  ReadSGEEHavedate ( void )
    void WriteSGEEHavedate ( u8 have )
#endif

    //softwarelen.total
    if ( 0 == turn.total ) /*��0��*/
    {

        AtNetSgee.packegsize = stulen.total;
        AtNetSgee.varity = datein[8 + stulen.total];
        AtNetSgee.totalpackeg = softwarelen.total / stulen.total;
#if 0

        if ( softwarelen.total == GpsStatues.offset )
        {
            AtNetSgee.overtime = 65532; /*���ݰ���ͬ*/
            SgeeReport ( 4 ); /*����ʧ��*/
            return ;
        }

#endif
        GpsStatues.offset = softwarelen.total;

        if ( ( softwarelen.total < ( 2 * 1024 ) ) || ( softwarelen.total > ( 16 * 1024 ) ) ) /*���������ݰ���С������*/
        {
            AtNetSgee.overtime = 65534; /*���ݰ���С����������ʧ��*/
            SgeeReport ( 3 ); /*����ʧ��*/
            return ;
        }

        if ( softwarelen.total % stulen.total )
        {
            AtNetSgee.totalpackeg++;
        }

        /*��������������*/
        EraseNetSGEEToFlash();

    }

    if ( turn.total == AtNetSgee.currentpackeg )
    {
        //WriteApp(turn.total*GsmSto.packetsize,&datein[4],stulen.total);
        WriteNetSGEEToFlash ( &datein[4], stulen.total, turn.total * AtNetSgee.packegsize );
        j = stulen.total / 100;
        k = stulen.total % 100;
        len = 100;

        for ( i = 0; i <= j; i++ )
        {
            if ( i == j )
            {
                if ( k )
                {
                    len = k;
                }
                else
                {
                    break;
                }
            }

            //  ReadApp(turn.total*GsmSto.packetsize+i*100,buf,len);
            ReadNetSGEE ( buf, turn.total * AtNetSgee.packegsize + i * 100, len );

            if ( EQUER != EqureOrNot ( buf, &datein[4 + i * 100], len ) )
            {
                AtNetSgee.overtime = 65533; /*д�������һ�´���*/
                SgeeReport ( 1 );
                return ;
            }



        }

        AtNetSgee.currentpackeg++;


    }




}

/*
*********************************************************************************************************
*   �� �� ��void FlashBufWriteLong(u8*buf,u16 len,u8 import)
*   ����˵��: �������ݵ����ͻ���
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void FlashBufWriteLong ( u8 *buf, u16 len, u8 import )
{
    u16 len1, len2;
    u8 *p;
    STRUCT_BLIND_FRAM StuFram;
    p = buf;

    if ( len <= A_FRAM_LEN - 1 )
    {
        StuFram.len = len;
        Mymemcpy ( &StuFram.buflen[0], p, StuFram.len );
        FlashBufWrite ( StuFram, import );
    }
    else
    {
        //aa lh ll data0 data1..........

        StuFram.len = 0xaa;
        StuFram.buflen[0] = len >> 8;
        StuFram.buflen[1] = len;
        Mymemcpy ( &StuFram.buflen[2], p, A_FRAM_LEN - 3 );
        p += ( A_FRAM_LEN - 3 );
        FlashBufWrite ( StuFram, Flash_LONG );
        len1 = len - ( A_FRAM_LEN - 3 );

        // len1ʣ��ĳ���
        //len2 Ҫ����ĳ���
        while ( len1 )
        {
            if ( len1 > A_FRAM_LEN )
            {
                len1 -= A_FRAM_LEN;
                len2 = A_FRAM_LEN;
            }
            else
            {
                len2 = len1;
                len1 = 0;
            }

            Mymemcpy ( ( u8 * ) &StuFram, p, len2 );
            FlashBufWrite ( StuFram, Flash_LONG );
            p += len2;


        }


    }

}
/*
*********************************************************************************************************
*   �� �� ��void Tick(STRUCT_BLIND_FRAM StuFram)
*   ����˵��: ����
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void Tick ( void )
{
    STRUCT_BLIND_FRAM StuFram;
    u8 messagebody[3] = {0x00, 0x01};
    static u8 secondbefore;

    if ( GsmSta.voltage < 3700 )
    {
        return;
    }

    if ( ( DEBUGTIM == debug ) && ( secondbefore != ( u8 ) ( timer.counter % 255 ) ) )
    {
        CoverTimeToBCD();
#ifdef USE_PRINTF
        myprintf ( "time:year:%02d month:%02d date :%02d h:%02d min:%02d sec:%02d \r\n", ( u8 ) ( timer.w_year - 2000 ), timer.w_month, timer.w_date, timer.hour, timer.min, timer.sec );
        secondbefore = ( u8 ) ( timer.counter % 255 );
#endif
    }

    if ( !GsmSto.KeepAliveInter )
    {
        return;
    }

    if ( ( ( timer.counter - GsmSta.BasicKeepAlive ) >= ( ( u32 ) GsmSto.KeepAliveInter ) ) &&
         ( NOT_OK == FlashBufRead ( &StuFram ) ) && ( GsmSta.SendingLen == 0 )
       )
    {
        messagebody[2] = GsmSta.Battery;
        CoverTimeToBCD();
        StuFram.len = 3;
        Mymemcpy ( &StuFram.buflen[0], ( u8 * ) &messagebody[0], StuFram.len );
        FlashBufWrite ( StuFram, 0 );
        GsmSta.BasicKeepAlive = timer.counter;

    }

}


#if 0
/*
*********************************************************************************************************
*   �� �� ��void SendAp(void)
*   ����˵��: ����wifi�ȵ���Ϣ
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
//#pragma optimize=none

void SendAp ( void )
{
    u8 *buf;
    u8 len;
    u8 StuAp[40 * 5 + 8];
    u8 i;
    CoverTimeToBCD();
    len = 0;
    StuAp[len++] = 0;
    StuAp[len++] = 6;
    Mymemcpy ( &StuAp[len], &timer.time[1], 5 );
    len += 5;
    StuAp[len++] = StuWifiAp.Ap_count;

    for ( i = 0; i < 5; i++ )
    {
        if ( StuWifiAp.Ap_count > i )
        {
            Mymemcpy ( ( u8 * ) &StuAp[len], ( u8 * ) &StuWifiAp.apmac0[0] + 40 * i, 8 );
            len += 8;
            Mymemcpy ( ( u8 * ) &StuAp[len], ( u8 * ) &StuWifiAp.ssid0[0] + 40 * i, * ( &StuWifiAp.ssid0len + 40 * i ) );
            len += * ( &StuWifiAp.ssid0len + 40 * i );
        }
        else
        {
            break;
        }
    }

    buf = StuAp;


    FlashBufWriteLong ( buf, len, 0 );







}
/*
*********************************************************************************************************
*   �� �� ��void SendMac(void)
*   ����˵��: ����wifi�ȵ���Ϣ
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
//#pragma optimize=none

void SendMac ( void )
{
    u8 buf[8];

    buf[0] = 0;
    buf[1] = 7;
    Mymemcpy ( &buf[2], GsmSta.mac, 6 );
    FlashBufWriteLong ( buf, 8, 0 );

}
#endif
/*
*********************************************************************************************************
*   �� �� ��void SendPosition(void)
*   ����˵��: ����λ����Ϣ
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
//#pragma optimize=none

void SendPosition ( u8 intime )
{

    static u8 stu = 0, fristTimeGetTime = 0 ; //,standp=0;
    static u16 timecounter = 0;
    ///   static u8 time[5];
    u32 ri0 = 1;
    STRUCT_BLIND_FRAM StuFram;
    u32 inter = 0;
    u8 *buf;
    u16 len;
    // double latnow,lonnow;
    static  STU_GPS_POSITION StuGpsPosition = {0};
    //  u8 * StuWifiPosition = (u8*)&StuGpsPosition;
    static   u8  StuWifiPosition[23 + 1] = {0}; //StuWifiPosition[23 + 40 * 1];


    if ( GsmSta.voltage < LOW_VOLTAGE )
    {
        return;
    }


    inter = GsmSto.staticinterval = GsmSto.moveintervalGPS; //=60;

    if(  timer.time[0] < 0x17 )
    {
        if( timer.counter - GsmSta.BasicPositionInter  > 600 )
        {

            if( GsmSta.gsm_p == 0x01 )
            {
                return;
            }

            GsmSta.BasicPositionInter = timer.counter;

            timecounter++;

            if( MASK_POWER_STATUS_SLEEP != ue866_operate_get_sleep() )
            {
                ue866_operate_set_sleep(true);
                GsmSta.gps_p = 0x04; // sleep
            }
            else
            {
                ue866_operate_set_sleep(false);
                GsmSta.gps_p = 0x08;    // wake up
            }

            //  WaitToResetSystem ( 20 );
        }

        if( NOT_OK == FlashBufRead ( &StuFram )  &&  GsmSta.SendingLen == 0  &&  GsmSta.IpOneConnect == OK    )
        {
            AskTime();
        }

        return;
    }


    if ( fristTimeGetTime == 0 && timer.time[0] > 0x16 )
    {
        fristTimeGetTime = 1;
        timecounter = 0;
        GsmSta.BasicPositionInter = timer.counter;// - 60;
        packet_current = 0;

       // return;
    }

    if(/*GsmSta.Latitude == 6666666 && */( ( timer.counter - GsmSta.BasicPositionInter ) >= ( inter - 30) ) || 0 == packet_current  )
    {
        if(bvkstrGpsData.Latitude != 6666666 && bvkstrGpsData.longitude > 0 )
        {
            GsmSta.gps_p = 0x04; // ����

            GsmSta.askm2m = 0;
            // GsmSta.askm2malerag = 0;
            GsmSta.Askmsmback = 0;
            GsmSta.askCSQ = 0;
        }
        else
        {
            if((GsmSta.gps_p & 0x04) == 0x04)
            {
                GsmSta.gps_p  = 0x08; // ����
            }

            if( GPS_START_HOST == GpsControlStu.GpsStartSatus  &&  
                ( ( timer.counter - GsmSta.BasicPositionInter ) >= ( inter - 20) ) || 0 == packet_current  )
            {
                GsmSta.askm2m = 1;
                // GsmSta.askm2malerag = 1;
                GsmSta.Askmsmback = 1;
                GsmSta.askCSQ = 1;

                if(  (GsmSta.gsm_p & MASK_POWER_STATUS_SLEEP) == MASK_POWER_STATUS_SLEEP
                     /* || (GsmSta.gsm_p & MASK_POWER_STATUS_OFF) == MASK_POWER_STATUS_OFF*/ )
                {
                    //GsmSta.gsm_p = MASK_POWER_STATUS_WAKEUP;
                    ue866_operate_set_sleep(false);
                }
            }
        }

    }

    if( 0 == packet_current )
    {
        if( (GsmSta.Latitude != 6666666 && GsmSta.longitude > 0) || 
            (bvkstrGpsData.Latitude != 6666666 && bvkstrGpsData.longitude > 0 ) )
        {
            intime  = TRIG_SEND_POSITION;
        }
    }

  
    switch ( stu )
    {
        case 0:/*ʱ�䵽��ʱ�����ʱ��,Ȼ������γ��*/
            if ( ( ( inter ) &&
                   ( ( timer.counter - GsmSta.BasicPositionInter ) >= inter ) ) ||
                 ( intime != SEND_POSITION_IN_TIME ) /**/ )
            {
                myprintf ( "[%x-%x %x:%x:%x]In time ->Protocol: GPS->Lat %d,Lon:%d;LBS->Lat %d,Lon:%d   SN=%d\r\n",
                           timer.time[1], timer.time[2],
                           timer.time[3], timer.time[4], timer.time[5],
                           bvkstrGpsData.Latitude, bvkstrGpsData.longitude, GsmSta.Latitude, GsmSta.longitude,
                           packet_current
                         );

                if ( /* (unfixedtime<30)&&*/ (bvkstrGpsData.Latitude != 6666666) /*&& bvkstrGpsData.Latitude != bvkstrGpsData.longitude*/ )
                {
                    if( 0 != ProcessPositionJump(bvkstrGpsData.longitude, bvkstrGpsData.Latitude, 1, timer.counter) )
                    {
                        GsmSta.askm2m = 1;
                        GsmSta.askm2malerag = 1;
                        GsmSta.Askmsmback = 1;


                        if(timecounter++ > 100 )
                        {
                            timecounter = 0;
                            // reboot
                            GsmSta.BasicPositionInter = timer.counter;
                            WaitToResetSystem ( 20 );
                        }

                        bvkstrGpsData.Latitude = 6666666 ;
                        return;
                    }

                    GsmSta.gps_p = 0x04;

                    GsmSta.BasicPositionInter = timer.counter;
                    memset(&StuGpsPosition, 0, sizeof(StuGpsPosition));
                    timecounter = 0;
                    StuGpsPosition.longitude[0] = bvkstrGpsData.longitude >> 24;
                    StuGpsPosition.longitude[1] = bvkstrGpsData.longitude >> 16;
                    StuGpsPosition.longitude[2] = bvkstrGpsData.longitude >> 8;
                    StuGpsPosition.longitude[3] = bvkstrGpsData.longitude;
                    StuGpsPosition.Latitude[0] = bvkstrGpsData.Latitude >> 24;
                    StuGpsPosition.Latitude[1] = bvkstrGpsData.Latitude >> 16;
                    StuGpsPosition.Latitude[2] = bvkstrGpsData.Latitude >> 8;
                    StuGpsPosition.Latitude[3] = bvkstrGpsData.Latitude;
                    StuGpsPosition.high_m[0] = bvkstrGpsData.high_m >> 8;
                    StuGpsPosition.high_m[1] = bvkstrGpsData.high_m;
                    StuGpsPosition.speed = bvkstrGpsData.speed;
                    StuGpsPosition.direction = bvkstrGpsData.direction / 2;
                    StuGpsPosition.SatelCnt = bvkstrGpsData.SatelCnt;

                    //  if ( intime != SEND_POSITION_IN_TIME )
                    {
                        goto GPS;
                    }

                    stu = 1;

                    //   standp=0xaa;
                }
                else if ( GsmSta.Latitude != 6666666 /*&& GsmSta.longitude != GsmSta.Latitude*/)
                {
                    if( 0 != ProcessPositionJump(GsmSta.longitude, GsmSta.Latitude, 3, timer.counter))
                    {
                        GsmSta.askm2m = 1;
                        GsmSta.askm2malerag = 1;
                        GsmSta.Askmsmback = 1;
                        timecounter++;

                        if(timecounter > 100 )
                        {
                            timecounter = 0;
                            // reboot
                            GsmSta.BasicPositionInter = timer.counter;
                            WaitToResetSystem ( 20 );
                        }

                        GsmSta.Latitude = 6666666 ;
                        return;
                    }

                    if(   GPS_START_HOST == GpsControlStu.GpsStartSatus )
                    {
                        GsmSta.gps_p = 0x04;    // sleep ����Ѿ���ȡ��GPS���꣬���ڻ�ȡ��LBSʱ����GPS��������
                    }

                    /**/
                    if( timecounter++ > 2 && (0x04 != GsmSta.gps_p) )
                    {
                        //  GsmSta.gps_p = 0x02;
                        timecounter = 0;
                    }

                    GsmSta.BasicPositionInter = timer.counter;
                    memset(StuWifiPosition, 0, sizeof(StuWifiPosition));
                    len = 12;
                    StuWifiPosition[len++] = GsmSta.longitude >> 24;
                    StuWifiPosition[len++] = GsmSta.longitude >> 16;
                    StuWifiPosition[len++] = GsmSta.longitude >> 8;
                    StuWifiPosition[len++] = GsmSta.longitude;
                    StuWifiPosition[len++] = GsmSta.Latitude >> 24;
                    StuWifiPosition[len++] = GsmSta.Latitude >> 16;
                    StuWifiPosition[len++] = GsmSta.Latitude >> 8;
                    StuWifiPosition[len++] = GsmSta.Latitude;
                    StuWifiPosition[len++] = GsmSta.Uncerten >> 8;
                    StuWifiPosition[len++] = GsmSta.Uncerten;
                    StuWifiPosition[len++] =  0; // StuWifiAp.Ap_count;


                    //  if ( intime != SEND_POSITION_IN_TIME )
                    {
                        goto LBS;
                    }

                    stu = 3;
                    //if( (AdxlStu.state==ADXL_STATIC)||( StuKey.SystemState==SYSTEM_OFF))
                    //if(standp==0xaa)
                    /* {
                         //GsmSta.askwifi=1;
                         GsmSta.askm2m=1;
                         GsmSta.askm2malerag=1;
                         GsmSta.Askmsmback=1;
                         timecounter=0;
                         stu=2;
                         //  standp=0;
                     }*/

                }
                else
                {
                    stu = 0;

                    if( timer.counter - GsmSta.BasicPositionInter  >= (60 + inter) )
                    {
                        GsmSta.BasicPositionInter = timer.counter;
                        //  if( GsmSta.CSQ > 10 )
                        timecounter++;
                        GsmSta.askm2malerag = 1;
                        GsmSta.askm2m = 0;
                        GsmSta.gps_p = 0x04;    // sleep

                        // if( MASK_POWER_STATUS_SLEEP  ==  GsmSta.gsm_p )
                        // {
                        //  GsmSta.BasicPositionInter = timer.counter;
                        // }
#if 0

                        if( timecounter % 2 == 0 )
                        {
                            GsmSta.gps_p = 0x08; // wake up
                            //  GsmSta.gsm_p = 0x08;
                        }
                        else  if( timecounter  % 3 ==  0  )
                        {
                            GsmSta.gps_p = 0x04;    // sleep
                            // GsmSta.gsm_p = 0x04;
                        }
                        else  if( timecounter  >=  10  )
                        {
                            //   GsmSta.gps_p = 0x08; // wake up
                            //   GsmSta.gsm_p = 0x08;
                            WaitToResetSystem ( 20 );
                        }

#endif
                    }
                    else
                    {
                        GsmSta.askm2m = 1;
                        GsmSta.Askmsmback = 1;

                    }

                }
            }

            break;

        case 1:/*���͸�gps��γ��*/
GPS:
            //packet_total++;
            packet_current++;
            StuGpsPosition.command[0] = 0;
            StuGpsPosition.command[1] = 3;
            memcpy ( StuGpsPosition.time, &timer.time[1], 5 );
            StuGpsPosition.battery = GsmSta.Battery;

            Memset ( StuGpsPosition.statues, 0, 4 );

            if ( GsmSta.batterylowtrig ) /*Ƿѹ*/
            {
                GsmSta.batterylowtrig = 0;
                StuGpsPosition.statues[3] |= 0x01;
            }

            if ( StuKey.SystemState != SYSTEM_OFF ) /*����*/
            {
                StuGpsPosition.statues[3] |= ( 0x01 << 1 );
            }

            if ( GsmSta.charging == BATTERY_CHARGE ) /*�����*/
            {
                StuGpsPosition.statues[3] |= ( 0x01 << 2 );
            }

            if ( GsmSta.nopower ) /*�������Զ��ػ�*/
            {
                StuGpsPosition.statues[3] |= ( 0x01 << 3 );
                GsmSta.nopower = 0;
            }

            if ( StuKey.SosSendTime )
            {
                StuKey.SosSendTime--;
                StuGpsPosition.statues[3] |= ( 0x01 << 4 );
            }

            if ( 2 == StuBle.State )
            {

                StuGpsPosition.statues[3] |= ( 0x01 << 5 );
            }


            StuGpsPosition.statues[0] = GsmSta.CSQ; // ��ʱ��¼�ź���ʹ�� FatQ
            StuGpsPosition.statues[1] = packet_current;
            StuGpsPosition.statues[2] =  GsmSta.station_count_3g + GsmSta.station_count_gsm;//packet_total;
            FlashProcess ( ri0, 0xaa );
            FlashProcess ( ri0, 0 );
            buf = ( u8 * ) &StuGpsPosition;

            //    myprintf ( "[%x-%x %x:%x:%x]In time LBS ->Protocol: GPS->Lat %d,Lon:%d;LBS->Lat %d,Lon:%d ->last:%x:%x:%x SN=%d\r\n",
            //       myprintf ( "[%x-%x %x:%x:%x]In time LBS ->Protocol: GPS->Lat %d,Lon:%d;LBS->Lat %d,Lon:%d   SN=%d\r\n",
            //                   timer.time[1], timer.time[2],
            //                    timer.time[3], timer.time[4], timer.time[5],
            //                    bvkstrGpsData.Latitude, bvkstrGpsData.longitude, GsmSta.Latitude, GsmSta.longitude,
            //            //      bvkstrGpsData.last_time[2], bvkstrGpsData.last_time[3], bvkstrGpsData.last_time[4],
            //                packet_current
            //            );
            /*
                        if ( bvkstrGpsData.varity != CalacXORVarity ( ( u8 * ) &bvkstrGpsData.Latitude, sizeof ( bvkstrGpsData ) - 1 ) )
                        {
                            //#ifdef USE_PRINTF
                            myprintf ( "GPSУ��%d, now=%d\r\n", bvkstrGpsData.varity, CalacXORVarity ( ( u8 * ) &bvkstrGpsData.Latitude, sizeof ( bvkstrGpsData ) - 1 ) );
                            //#endif
                            stu = 0;
                            break;
                        }
            */
            len = 25; //sizeof(StuGpsPosition);

            //         if ( /*(bvkstrGpsData.longitude==0)||*/ ( bvkstrGpsData.Latitude == 6666666 ) )
            //         {
            //            stu = 0;

            //            return;
            //         }

            GsmSta.Latitude = bvkstrGpsData.Latitude = 6666666;
            GsmSta.BasicPositionInter = timer.counter;
            GsmSta.BasicKeepAlive = timer.counter;
            //GsmSta.CSQ = 0;
            /*
                        if ( ( timer.time[0] < 0x17 ) && ( NOT_OK == FlashBufRead ( &StuFram ) ) && ( GsmSta.SendingLen == 0 ) && ( GsmSta.IpOneConnect == OK ) )
                        {
                            AskTime();
                        }
            */
            FlashBufWriteLong ( buf, len, 0 );
            timecounter = 0;
            stu = 0;
            break;

        case 2:

            if ( /*(unfixedtime<30)&&*/ ( bvkstrGpsData.Latitude != 6666666 ) )
            {
                GsmSta.askm2m = 0;
                GsmSta.askm2malerag = 0;
                GsmSta.Askmsmback = 0;
                // GsmSta.askwifi=0;
                stu = 1;
                break;
            }
            else if ( GsmSta.Latitude != 6666666 )
            {
                GsmSta.askm2m = 0;
                GsmSta.askm2malerag = 0;
                GsmSta.Askmsmback = 0;
                stu = 3;
                break;
            }
            else
            {
                stu = 0;

            }

            break;

        case 3:/*����GSM��γ��*/
LBS:
            // packet_total++;
            packet_current++;
            len = 0;
            StuWifiPosition[len++] = 0;
            StuWifiPosition[len++] = 4;
            //Mymemcpy ( &StuWifiPosition[len], &timer.time[1], 5 );
            memcpy ( &StuWifiPosition[len], &timer.time[1], 5 );

            len += 5;
            StuWifiPosition[len++] = GsmSta.Battery;
            Memset ( &StuWifiPosition[len], 0, 4 );


            StuWifiPosition[len + 0] = GsmSta.CSQ; // ��ʱ��¼�ź���ʹ�� FatQ

            StuWifiPosition[len + 1] = packet_current;
            StuWifiPosition[len + 2] = GsmSta.station_count_3g + GsmSta.station_count_gsm ; //packet_total;

            if ( GsmSta.batterylowtrig ) /*Ƿѹ*/
            {

                GsmSta.batterylowtrig = 0;
                StuWifiPosition[len + 3] |= 0x01;

            }

            if ( StuKey.SystemState != SYSTEM_OFF ) /*����*/
            {
                StuWifiPosition[len + 3] |= ( 0x01 << 1 );
            }

            if ( GsmSta.charging == BATTERY_CHARGE ) /*�����*/
            {
                StuWifiPosition[len + 3] |= ( 0x01 << 2 );
            }

            if ( GsmSta.nopower ) /*�������Զ��ػ�*/
            {
                StuWifiPosition[len + 3] |= ( 0x01 << 3 );
                GsmSta.nopower = 0;
            }

            if ( StuKey.SosSendTime )
            {
                StuKey.SosSendTime--;
                StuWifiPosition[len + 3] |= ( 0x01 << 4 );
            }

            if ( 2 == StuBle.State )
            {

                StuWifiPosition[len + 3] |= ( 0x01 << 5 );
            }

            /*

            if ( ( GsmSta.Latitude == 6666666 ) || ( GsmSta.longitude == 0 )  )
            {
                stu = 0;
                GsmSta.askm2m = 1;
            #ifdef USE_PRINTF
                myprintf ( "GSM��γ��: lon=%d lat=%d\r\n", GsmSta.longitude, GsmSta.Latitude );
            #endif
                return;
            }
            */

            FlashProcess ( ri0, 0xaa );
            len += 4;


            FlashProcess ( ri0, 0 );
            //    myprintf ( "[%x-%x %x:%x:%x]In time LBS ->Protocol: GPS->Lat %d,Lon:%d;LBS->Lat %d,Lon:%d ->last:%x:%x:%x SN=%d\r\n",
            //      myprintf ( "[%x-%x %x:%x:%x]In time LBS ->Protocol: GPS->Lat %d,Lon:%d;LBS->Lat %d,Lon:%d   SN=%d\r\n",
            //                  timer.time[1], timer.time[2],
            //                timer.time[3], timer.time[4], timer.time[5],
            //              bvkstrGpsData.Latitude, bvkstrGpsData.longitude, GsmSta.Latitude, GsmSta.longitude,
            //      bvkstrGpsData.last_time[2], bvkstrGpsData.last_time[3], bvkstrGpsData.last_time[4],
            //            packet_current
            //        );
            /*
                        if ( GsmSta.varitygsmlon != CalacXORVarity ( ( u8 * ) &GsmSta.Latitude, 10 ) )
                        {
                            //#ifdef USE_PRINTF
                            myprintf ( "GSMУ��%d, now=%d\r\n", GsmSta.varitygsmlon, CalacXORVarity ( ( u8 * ) &GsmSta.Latitude, 10 ) );
                            //#endif
                            GsmSta.askm2m = 1;
                            stu = 0;
                            break;
                        }
            */
            buf = StuWifiPosition;


            GsmSta.Latitude = bvkstrGpsData.Latitude = 6666666;
            GsmSta.BasicPositionInter = timer.counter;
            GsmSta.BasicKeepAlive = timer.counter;
            //GsmSta.CSQ = 0;
            /*
                        if ( ( timer.time[0] < 0x17 ) && ( NOT_OK == FlashBufRead ( &StuFram ) ) && ( GsmSta.SendingLen == 0 ) && ( GsmSta.IpOneConnect == OK ) )
                        {
                            AskTime();
                        }
            */
            len = 23;
            FlashBufWriteLong ( buf, len, 0 );
            stu = 0;
            timecounter = 0;
            break;

        default:
            stu = 0;
            break;

    }

}


/*
*********************************************************************************************************
*   �� �� ��:void McuAnswerCommon(u8 waterid,u8 successflag)
*   ����˵��:�ն˷���ͨ��Ӧ��
*
*
*   ��    �Σ�waterid�յ���Ϣ����ˮ��  successflag�ɹ���־��1byte 0Ϊ�ɹ� 1Ϊʧ�ܣ�
*   �� �� ֵ:
*********************************************************************************************************
*/
/*��Ϣ��Ϊ����Ϣid+Ӧ�����Ϣid+Ӧ�����Ϣ��ˮ��+ �ɹ���־��1byte 0Ϊ�ɹ� 1Ϊʧ�ܣ�*/
void McuAnswerCommon ( u8 waterid, u16 ansid, u8 successflag )
{
    STRUCT_BLIND_FRAM StuFram;
    u8 result = 0;
    //Mcu2�ն�ͨ��Ӧ����ϢID   0x0002
    u8 len = 0, data[6];
    //    u16 outlen;
    data[len++] = 0x00;
    data[len++] = 0x02;
    data[len++] = ( ansid >> 8 ) & 0x00ff;
    data[len++] = ( ansid >> 0 ) & 0x00ff;
    data[len++] = waterid;

    if ( successflag == NOT_OK )
    {
        result = 1;
    }

    data[len++] = result;
    StuFram.len = len;
    Mymemcpy ( &StuFram.buflen[0], ( u8 * ) &data[0], StuFram.len );
    FlashBufWrite ( StuFram, 0 );


}



/*
*********************************************************************************************************
*   �� �� ��void StartUpdatemcuInit(u8*ip,u8 iplen,u8*port,u8 portlen)
*   ����˵��: ����������ʼ��
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void StartUpdatemcuInit ( u8 *ip, u8 iplen, u8 *port, u8 portlen )
{

    GsmSto.updateflag = OK;
    GsmSto.currentpackeg = 0;
    GsmSto.totalpackeg = 10;
    GsmSto.updateiplen = iplen;

    if ( iplen )
    {
        Mymemcpy ( GsmSto.updatestrip, ip, iplen );
        GsmSto.portlen = portlen;
        Mymemcpy ( GsmSto.port, port, portlen );
    }

    WriteGsmStoreDate();

}

