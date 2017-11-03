#include"includes.h"

GsmStatuesstu GsmSta;
GsmStorestu GsmSto;
GSM_REC_DATA_STU GsmRecDataStu;
u8 ATSend ( u8 AtTurn ); /*ֻ����һ���ļ�ʹ��*/
void ProcessRevNetDate ( u8 datein ); /*ֻ����һ���ļ�ʹ��*/
void ProcessProtectGsmNet ( void ); /*ֻ����һ���ļ�ʹ��*/
u8 ProcessSendNetDate ( u8 *DateIn, u16 DateLen, u8 rightnow ); /*ֻ����һ���ļ�ʹ��*/
u8 ProcessSmsPhone ( void ); /*ֻ����һ���ļ�ʹ��*/
void ProcessUpdateMcu ( void ); /*ֻ����һ���ļ�ʹ��*/
u8  GsmSendAtCommand ( u8 *AtCommend, u16 len, u8 *flag, u8 times, u16 waittimeSecond );
u8  GsmReset ( void );
//u8  PowerOn(void);
u8  GetGsmBufData ( void );
void PackDateToBuf ( u8 *indata, u16 inlen, u8 *outbuf, u16 *outlen );
u8 PbulicBuf[1024];/*��flash��*/
void InTime ( void );
void WriteChars ( char *datas, u8 len, u8 PageAddr, u8 LineAddr );

static u8 sosIndex = 0, sosStaus = 0xAA;

extern STRUCT_QUEUE_CTRL GsmRxQueueCtrl;
extern u8 gGsmInit;

u8 gGsmPowerDown = 0;

#define UE866_OPERATE

/*
*********************************************************************************************************
*   �� �� ��:void GsmTask(void)
*   ����˵��:GSM task   500ms����һ��
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void GsmTask ( void )
{

    ProcessGsmQueue ( ProcessRevNetDate );
#ifdef UE866_OPERATE
    ue866_operate_manage_at();
#else
    ProcessProtectGsmNet();
#endif
    //



}
/*
*********************************************************************************************************
*   �� �� ��void GsmUartSendStr(u8 *str,u16 len)
*   ����˵��: GSM�����ַ���
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void GsmUartSendStr ( u8 *str, u16 len )
{
#ifndef GSM_USE_UART
    UartSendStr ( LEUART0, str, len );
#else
    u16 i;


    for ( i = 0; i < len; i++ )
    {
        USART_Tx ( USART2, str[i] );
    }

    if ( debug == DEBUGGSM )
    {
        DebugUartSend ( str, len );
    }

#endif
}

/*
*********************************************************************************************************
*   �� �� ��void ProcessRevNetDate(u8 datein)
*   ����˵��: GSM������������
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ProcessRevNetDate ( u8 data )
{

    static u8 stu = 0;
    U16STU t16mp;
    vu16 i;
    static u8 databefore;
    static u8 varity = 0;

    switch ( stu )
    {
        case 0:
            if ( data == 0xaa )
            {
                stu++;
                GsmRecDataStu.flag = 0;
                GsmRecDataStu.RecCount = 0;
            }

            break;

        case 1:
            if ( data == 0x55 ) /*��Ҫת��*/
            {
                stu = 2;

            }
            else if ( data == 0xaa ) /*�յ�������*/
            {
                t16mp.stu.H = GsmRecDataStu.RecData[1];
                t16mp.stu.L = GsmRecDataStu.RecData[2];

                if ( t16mp.total != ( GsmRecDataStu.RecCount - 4 ) ) /*���ݳ��Ȳ���*/
                {
                    if ( databefore == 0xaa )
                    {
                        GsmRecDataStu.flag = 0;
                        GsmRecDataStu.RecCount = 0;

                    }
                    else
                    {
                        GsmRecDataStu.flag = 0;
                        GsmRecDataStu.RecCount = 0;
                        stu = 0;
                    }

                    break;

                }




                //����У����
                //=============================================

                getvarity ( &varity, &GsmRecDataStu.RecData[0], t16mp.total + 3 );

                //=================================================
                if ( varity == GsmRecDataStu.RecData[GsmRecDataStu.RecCount - 1] )
                {
                    GsmRecDataStu.RecData[GsmRecDataStu.RecCount++] = data;
                    GsmRecDataStu.flag = GetAFram;
                    stu = 0;
                }
                else
                {

                    GsmRecDataStu.flag = 0;
                    GsmRecDataStu.RecCount = 0;
                    stu = 0;
                    break;

                }
            }
            else/*һ���ַ�ֱ�ӽ��뻺��*/
            {

                GsmRecDataStu.RecData[GsmRecDataStu.RecCount++] = data;

                if ( ( GsmRecDataStu.RecCount >= 10 ) && ( GsmRecDataStu.flag == 0 ) ) /*���ID�Ƿ���ȷ*/
                {

                    if ( EQUER == EqureOrNot ( ( u8 * ) & GsmRecDataStu.RecData[0], GsmSto.ID, 10 ) )
                    {
                        varity = 0;
                        getvarity ( &varity, &GsmRecDataStu.RecData[0], 10 );
                        GsmRecDataStu.flag = IDOK;
                        GsmRecDataStu.RecCount -= 10;

                        if ( GsmRecDataStu.RecCount )
                        {
                            Mymemcpy ( ( u8 * ) &GsmRecDataStu.RecData[0], ( u8 * ) &GsmRecDataStu.RecData[10], GsmRecDataStu.RecCount );
                        }


                    }
                    else
                    {
                        GsmRecDataStu.flag = 0;
                        GsmRecDataStu.RecCount = 0;
                        stu = 0;
                        break;
                    }

                }

            }

            break;

        case 2:
            if ( data == 0x01 )
            {
                stu = 1;
                GsmRecDataStu.RecData[GsmRecDataStu.RecCount++] = 0xaa;
            }
            else if ( data == 0x02 )
            {
                stu = 1;
                GsmRecDataStu.RecData[GsmRecDataStu.RecCount++] = 0x55;
            }
            else
            {
                GsmRecDataStu.flag = 0;
                GsmRecDataStu.RecCount = 0;
                stu = 0;
            }

            break;

        default:
            stu = 0;
            break;


    }

    databefore = data;



}
/*
*********************************************************************************************************
*   �� �� ��void InTime(void)
*   ����˵��: GSM��ʱ������������
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
//#pragma optimize=none
void InTime ( void )
{
    
    if ( GsmSta.voltage < LOW_VOLTAGE  || MASK_POWER_STATUS_OFF == GsmSta.gsm_p )
    {
        return;
    }

    if ( ( GsmSto.updateflag != OK ) && ( StuKey.SystemState != SYSTEM_OFF ) )
    {
        Tick();
        // myprintf("InTime:\r\n");
        SendPosition ( SEND_POSITION_IN_TIME );
    }
}
/*
*********************************************************************************************************
*   �� �� ��    void GiveStoreApn(void)
*   ����˵��:
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void GiveStoreApn ( void )
{
    StuApnNet.apnlen = GsmSto.apnlen;
    StuApnNet.strapn = GsmSto.strapn;
    StuApnNet.NetId = GsmSto.NetId;
    StuApnNet.NetIdNameLen = GsmSto.NetIdNameLen;
    StuApnNet.NetIdName = GsmSto.NetIdName;
    StuApnNet.NetIdPsswdLen = GsmSto.NetIdPsswdLen;
    StuApnNet.NetIdPsswd = GsmSto.NetIdPsswd;
}
/*
*********************************************************************************************************
*   �� �� ��    void GiveApnValue(void)
*   ����˵��: ��APN��ֵ
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void GiveApnValue ( void )
{
#if 0
    volatile u8 i;
    i = GsmSta.operturn;

    if ( GsmSta.operturn == NOT_MATCH )
    {

        GiveStoreApn();
    }
    else
    {

        StuApnNet.apnlen = GetArryLen ( StuApn[i].strapn, 100 );
        StuApnNet.strapn = StuApn[i].strapn;
        StuApnNet.NetId = StuApn[i].NetId;
        StuApnNet.NetIdNameLen = GetArryLen ( StuApn[i].NetIdName, 100 );
        StuApnNet.NetIdName = StuApn[i].NetIdName;
        StuApnNet.NetIdPsswdLen = GetArryLen ( StuApn[i].NetIdPsswd, 100 );
        StuApnNet.NetIdPsswd = StuApn[i].NetIdPsswd;

    }

#else

    GiveStoreApn();
#endif
}

/*
*********************************************************************************************************
*   �� �� ��u8 ReadyRevNetDate(void)
*   ����˵��: GSM�鿴�Ƿ�����������
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/

u8 ReadyRevNetDate ( void )
{
    if ( ( ( GsmSta.UnRevLen ) || ( GsmSta.DateCome == OK ) ) && ( STU_AtCommand.AtState == AT_STATE_IDLE ) )
    {
        //InitGsmQueue();
        if ( ( GsmSta.UnRevLen ) || ( GsmSta.DateCome != OK ) )
        {
            GsmSta.DateCome = OK;
            GsmSta.RevLen = GsmSta.UnRevLen;
            GsmSta.UnRevLen = 0;
        }

        return OK;
    }

    return NOT_OK;
}
/*
*********************************************************************************************************
*   �� �� ��void InitGsmDate(void)
*   ����˵��:
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/

void InitGsmDate ( void )
{

    GsmSta.PowerResetStu = 0;
    GsmSta.QSS = NOT_OK;
    GsmSta.CGREG = NOT_OK;
    GsmSta.CSQ = NOT_OK;
    GsmSta.CREG = NOT_OK;
    GsmSta.SendDate = NOT_OK;
    GsmSta.Sending = DATAS_IDLES;
}
/*
*********************************************************************************************************
*   �� �� ��void ProcessProtectGsmNet(void)
*   ����˵��: GSM�����ػ�
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/

const AT_STRUCT AtCommands[] =
{
    {0, "AT\r\n", 4, "OK", 3, 5}, //0
    {1, "ATE0\r", 5, "OK", 1, 15}, // 1
    {2, "AT+CGMR\r", 8, "OK", 1, 5}, /*����汾2*/
#ifndef CFUN_FIVE
    {3, "AT+CGMM\r", 8, "OK", 1, 5}, /*ģ����3*/
#else
    {3, "AT+CFUN=5\r", 10, "OK", 3, 5}, /*��������ר��*/
#endif
    {4, "AT#CACHEDNS=1\r", 14, "OK", 1, 5}, /*��������ip4*/
    {5, "AT#CGSN\r", 8, "OK", 1, 5}, /*IMEI5*/


    //  {6,"AT+FLO=0\r",9,"OK",1,20},// 6
    {6, "AT#CFLO=0\r", 10, "OK", 1, 20}, // /* Command Flow Control*/ 6


    {7, "AT#QSS?\r", 8, "OK", 1, 5}, /*��Ҫ���⴦��7*/
    {8, "AT+CLIP=1\r", 10, "OK", 1, 30}, // 8
    {9, "AT+CMGF=1\r", 10, "OK", 1, 15}, // 9
    {10, "AT#MONI=0\r\n", 10, "OK", 1, 30}, // 9
    {11, "AT#SGACTAUTH=2\r", 15, "OK", 1, 15}, /*CHAP10*/
    {12, "AT#SGACTAUTH=1\r", 15, "OK", 1, 15}, /*other 10*/

    {13, "AT#MSCLASS=8,1\r", 15, "OK", 2, 30}, /*ʡ��ģʽ����һ�μ���12*/
    // {13,"AT#HTTPCFG?\r",12,"OK",1,15}, //
    //  {13,"ATE0\r",5,"OK",1,15}, // 1

    {14, "AT#SCFG=1,1,900,0,600,10\r", 25, "OK", 1, 10}, /*����һ�μ���13*/
    {15, "AT#SCFG=2,1,900,0,600,10\r", 25, "OK", 1, 10}, /*����һ�μ���14*/
    //  {16,"AT+CGQMIN=1,0,0,0,0,0\r",22,"OK",1,15},/*����һ�μ���15*/
    {16, "ATE0\r", 5, "OK", 1, 15}, // 1
    {17, "ATE0\r", 5, "OK", 1, 15}, // 1
    //{17,"AT+CGQREQ=1,0,0,3,0,0\r",22,"OK",1,15},/*����һ�μ���16*/



    {18, "AT#SH=1\r", 8, "OK", 1, 5}, /*��������ר��  Socket Shutdown  */


    /*�ر�gprs*/
    {19, "ATE0\r", 5, "OK", 1, 15}, // 1
    {20, "AT+CSQ\r", 7, "OK", 1, 5}, /*��������ר�� 18*/
    {21, "AT+CREG?\r", 9, "OK", 3, 5}, /*��������ר��19*/
    {22, "AT+CGREG?\r", 10, "OK", 3, 5}, /*��������ר��*/
    {23, "AT+CGATT=1\r", 11, "OK", 1, 100}, /*��������ר��*/



    {24, "AT#SERVINFO\r", 12, "OK", 1, 20}, /*��������ר��*/


    {25, "AT+CFUN=5\r", 10, "OK", 3, 5}, /*��������ר��*/

    {26, "AT+CNMI=3,1\r", 12, "OK", 1, 100}, /*��������ר��  New Message Indications To Terminal Equipment  */
    //{27,"AT#SCFGEXT=1,1,0,0,0,0\r",23,"OK",1,15},/*��������ר��*/
    {27, "AT#SCFGEXT=1,1,0,0,0,0\r", 23, "OK", 1, 15}, /*��������ר��*/


    { 28, "AT+CGDCONT?\r", 12, "OK", 2, 5}, /*��ѯapn 11*/

    { 29, "AT+CGATT=1\r", 11, "OK", 1, 100}, /*��վ��Ϣ*/

    { 30, "AT#SI=1\r", 8, "OK", 1, 5}, /**/
    { 31, "AT#SS=1\r", 8, "OK", 3, 3}, /**/
    { 32, "AT+CMGL=\"ALL\"\r", 14, "OK", 1, 5}, /*  List Messages  */
    { 33, "AT+CMGD=1,4\r", 12, "OK", 1, 5}, /* Delete Message  */
    { 34, "AT#AGPSSND=0,0,0,3\r\n", 20, "OK", 1, 200}, /*Get AGPS*/
    { 35, "AT#SHDN\r", 8, "OK", 1, 20}, /* Software Shutdown */
    {36, "AT&K0\r", 6, "OK", 1, 20}, // 6
    {37, "AT#MONI\r\n", 8, "OK", 1, 30}, // 9
    //  {38,"AT+WS46=22\r",11,"OK",1,30},// 9
    {38, "ATA\r", 4, "OK", 1, 30}, //  /* used to answer to an incoming call*/ 9
    {39, "ATH\r", 4, "OK", 1, 30}, // /*  close the current conversation (voice or data). */ 9
    {40, "ATE1\r", 5, "OK", 1, 15}, // /* Command Echo   0 - disables command echo */ 1
    { 41, "AT#SYSHALT=0,1\r", 15, "OK", 1, 50}, /*System turn-off */  //  AT#FASTSYSHALT

    { 42, "ATD;\r",   5, "OK", 1, 30 }, /* Call a phone */
    { 43, "AT+CLCC\r", 8, "OK", 1, 30 }, /* Get the Phone Activity Status */
    { 44, "ATH\r",    4, "OK", 1, 30 }, /* Get the Phone Activity Status */
    { 45, "AT#SHDN\r", 8, "OK", 1, 30 },
    /* { 45,"ATS0=3\r", 7,"OK",1,30 }, */
    //  { 44,"AT+CLCC\r",8,"OK",2,20 }   /* List Current Calls */
    {46, "AT+COPS=0\r", 10, "OK", 1, 30}, /*operator_flag*/  //
    //   {46, "AT+COPS=1,2,46001,2\r", 20, "OK", 1, 30}, /*operator_flag*/  // ǿ��ʹ��3G
    {47, "AT+COPS=1,2,45403,2\r", 20, "OK", 1, 30},
    {48, "AT+COPS=?\r", 10, "OK", 1, 1000},

    //  {49, "AT#NTP=\"118.103.146.184\",123,1,5,0\r", 12, "OK", 2, 10000},  /*request mode clock time */

    //  {50, "AT+CCLK?\r", 9, "OK", 2, 70},  /*request mode clock time */
    //  {51, "AT#CCLK=\"02/09/07,22:30:00+04,1\"\r", 32, "OK", 2, 70},  /*request mode clock time */


    //
    //AT+CSCA=+8613800755500,145
    //

};
STU_APN_NET StuApnNet;
#define HARDY_DEBUG 0
#ifdef HARDY_DEBUG
typedef struct
{
    u8  state;
    u8  cmd;
} at_debug_data;

at_debug_data at_debug[200];
u8 g_index = 0;
#endif
u8 creg_called = 0;
u8 at1_called = 0;
u8 cops_called = 0;
u8 cops_set = 0;
u16 poweron_counter = 0;
u16 poweroff_counter = 0;
u16 powerreset_counter = 0;

void ProcessProtectGsmNet ( void )
{
#define PHONE_COME 22
    static u8 stu = 0, atstu = 0, errtimes = 0, connecttimes = 0, nocardtime = 0,/**/ waitcreg = 0;
    static u16 timer = 0, sleepinter = 0, connecttimer = 0;
    static u8 askcsqtime = 0, requestStationTime = 0;
    static u16 nDelay = 0;
    u8 atenable = true;
#ifndef DEBUG_GSM_ANT
    STRUCT_BLIND_FRAM StuFram;
#endif
    static u8 buf[300];
    u8 apn[50];
    u8 tmp;
    u8 msgtype;




    // if ( READ_GSMPOWER_STATUS() )
    {
        if ( gGsmInit == 0xAA )
        {
            gGsmInit = 0;

            GsmSta.PowerResetStu = 0;
            /* GSM Reset */
            stu = 101;
        }
    }
    // else
    {
        //      return;
    }

    if ( timer )
    {
        timer--;
    }

    if ( connecttimer )
    {
        connecttimer--;
    }

    /*
          if( GsmSta.gsm_p == 0x08)
            {
               GsmSta.gsm_p = 0;
                sleepinter = 0;
                stu = 4;
                askcsqtime = 200;
            }


        if(  sleepinter == 249 )
        {
            sleepinter = 249;
            return;

        }
    */

    if ( sleepinter )
    {
        sleepinter--;
    }

    if ( waitcreg )
    {
        waitcreg--;
    }

#if 0

    if ( ( GsmSta.sos ) && ( STU_AtCommand.AtState == AT_STATE_IDLE ) && ( sosAtStu == 42 ) )
    {
        sosStu   = stu;
        sosAtStu = atstu;
        stu      = 0;
        atstu    = 42;

        GPIO_PinOutClear ( RLED_GPIO_PORT, RLED_GPIO_PIN );
    }

#endif

    if ( gGsmPowerDown == 0xAA )
    {
        stu      = 0;
        atstu    = 45;
    }



    if ( ( GsmSta.gsm_p & 0x02 ) == 0x02 ||   GsmSta.askm2malerag  == 1 )
    {
        GsmSta.gsm_p = 0;
        GsmSta.askm2malerag  = 0;
        stu = 100; /*enter reset mode*/
    }
    else if ( ( GsmSta.gsm_p & 0x04 ) == 0x04 )
    {
        //     GsmSta.gsm_p = 0;
        //   sleepinter = 249;
        //    stu = 9;
        //   atstu = 25;

    }



    //  myprintf ( "stu is %d, atstu is %d \r\n", stu, atstu );

    switch ( stu )
    {
        case 0:/*һ���ʼ��at�·�*/
            tmp = ATSend ( atstu );
            GsmSta.at_result = tmp;

            if ( AT_ERROR == tmp )
            {
                if ( atstu == AT_AT )
                {
                    GsmSta.PowerResetStu = 0;
                    stu = 101; /*power on*/
                }
                else if ( atstu == 45 )
                {
                    GSM_POWER_OFF();
                }
                else
                {
                    if ( errtimes > 1 )
                    {
                        stu = 100; /*reatart*/
                        GsmSta.PowerResetStu = 0;
                    }
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                if ( atstu == 45 )
                {
                    GSM_POWER_OFF();
                }

#if 0
                else if ( atstu == 43 )
                {

                    if ( sosStaus == 0 )
                    {
                        GPIO_PinOutSet ( RLED_GPIO_PORT, RLED_GPIO_PIN );
                        GsmMuteOn();

                        atstu      = sosAtStu;
                        stu        = sosStu;
                        sosAtStu   = 42;
                        GsmSta.sos = 0;

                        sosIndex   = 0;
                    }
                    else
                    {
                        if ( sosStaus == 3 )
                        {
                            nDelay = 30;
                            stu    = 120;
                        }
                        else
                        {
                            sosIndex ++;
                            atstu = 42;

                            for ( ; sosIndex < 10; sosIndex ++ )
                            {
                                if ( GsmSto.sosPhoneLen[sosIndex >> 1] > 5 )
                                {
                                    break;
                                }
                            }

                            if ( sosIndex >= 10 )
                            {
                                GPIO_PinOutSet ( RLED_GPIO_PORT, RLED_GPIO_PIN );
                                GsmMuteOn();

                                atstu      = sosAtStu;
                                stu        = sosStu;
                                sosAtStu   = 42;
                                GsmSta.sos = 0;

                                sosIndex   = 0;
                            }
                        }
                    }

                    sosStaus = 0xAA;

                }

#endif
                else if ( atstu == 48 )
                {
                    stu = 103;
                }

                else
                {

                    atstu++;
                }

#if 0

                if ( atstu == 2 )
                {
                    atstu = 38;

                }

                if ( atstu == 39 )
                {
                    atstu = 2;

                }

#endif
#if 1

                if ( atstu == 2 )
                {
                    stu = 103;
                    atstu = 48;
                }

#endif

                if ( atstu == 10 )
                {
                    atstu = 21;
                    stu = 18;
                    GsmSta.CREG = NOT_OK;
                    timer = 3000;
                    waitcreg = 50;
                }

                if ( atstu == 8 )
                {
                    if ( GsmSta.QSS != OK )
                    {
#if 0
                        stu = 100; /*reatart*/
                        GsmSta.PowerResetStu = 0;

                        if ( nocardtime++ > 1 ) /*û�м�⵽sim��ϵͳ�ػ�*/
                        {

                            StuKey.SystemState = SYSTEM_OFF;
                            PowerHoldOff();
                            //  LedFlash(200,10);
                            stu = 16;
                            atstu = 35;
                        }

#endif
                    }
                    else
                    {
                        nocardtime = 0;
                    }
                }


            }
            else
            {
                errtimes++;
            }

            break;

        case 1:/*����ģʽ����*/
            tmp = ATSend ( atstu );

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                atstu = 28;
                errtimes = 0;
                InitGsmQueue();
                stu++;
            }

            break;

        case 2:/*ȷ��һ���Բ�����û������*/
            tmp = ATSend ( atstu );

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                errtimes = 0;

                Mymemcpy ( apn, StuApnNet.strapn, StuApnNet.apnlen );
                apn[StuApnNet.apnlen] = '"';

                if ( 0 != Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, apn, StuApnNet.apnlen + 1 ) )
                {
                    stu += 2;
                    atstu = 18;
                }
                else
                {
                    atstu = 13;
                    stu++;
                }

            }

            break;

        case 3:/*gsmһ���Բ�������*/
            if ( atstu == 18 )
            {
                tmp = SetApn ( buf );
            }
            else
            {
                tmp = ATSend ( atstu );
            }

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                atstu++;
                errtimes = 0;

                if ( atstu == 19 )
                {
                    stu++;
                    atstu = 18;
                }
            }

            break;

        case 4:/*�ж���������,��������*/

            if ( GsmSta.CREG == OK )
            {
                if ( GsmSta.IpOneConnect == OK )
                {
                    ProcessUpdateMcu();

                    atenable = ProcessSendNetDate ( 0, 0, NOT_OK );

                    if ( GsmSta.Sending != DATAS_SENDING )
                    {
#if 1

                        if ( ( atenable == true ) && ( OK == ReadyRevNetDate() ) )
                        {

                            GsmSta.AskNetAtLen = 0;
                            stu = 10; /*��ѯ�Ƿ�����������*/
                            break;
                        }

#endif


#if 1

                        // ProcessUpdateSGEE();  //  ��ʱ��ȡ FatQ 20170722

                        if ( ( atenable == true ) && ( OK == GsmM2MAsk() ) /**/)
                        {
                            stu = 12; /*����m2m��γ��*/
                            atstu = 34;
                            break;
                        }

#endif

                    }
                }

#if 0

                if ( ( atenable == true ) && ( OK == ProcessPhone() ) )
                {
                    atstu = 38; /*���յ绰*/
                    stu = PHONE_COME;
                    break;
                }


                if ( ( atenable == true ) && ( OK == ProcessSmsPhone() ) )
                {
                    atstu = 9; /*���ն���*/
                    stu = 11;
                    GsmSta.ReadMsgTime = 0;
                    GsmSta.msgs = 0;
                    break;
                }

#endif
            }

            if ( askcsqtime < 200 )
            {
                askcsqtime++;
            }

            //     if ( ( atenable == true ) &&   ( askcsqtime > 100 )/*  &&   1 == GsmSta.askCSQ &&  GsmSta.CSQ < 5 */ ) //&&(GsmSta.LCDState)))/*��ʱ org 100->10s��ѯCSQ*/
            if ( ( atenable == true ) &&   ( askcsqtime > 100 ) && GsmSta.askCSQ == 1)
            {
                atstu = 20;
                stu = 20;
                askcsqtime = 0;
                GsmSta.askCSQ  = 0;
                break;
            }

#if 0

            if( requestStationTime < 500 )
            {
                requestStationTime ++;
            }

            if ( ( atenable == true ) && ( ( requestStationTime > 400 ) ) ) //&&(GsmSta.LCDState)))/*��ʱ org 100->10s��ѯCSQ*/
            {
                atstu = 48;
                stu = 103;
                requestStationTime = 0;
                break;
            }

            if ( ( atenable == true ) && ( OK == ReadMsgBuf ( &msgtype ) ) )
            {
                stu = 13; /*���Ͷ���*/
                break;

            }

#endif

#ifndef DEBUG_GSM_ANT

            if ( ( atenable == true ) && ( ( NOT_OK == FlashBufRead ( &StuFram ) ) && ( GsmSta.SendingLen == 0 ) &&
                                           ( !sleepinter ) && ( GsmSto.updateflag != OK ) ) )
            {

                /*�鿴ϵͳ�Ƿ�ػ�*/
                if ( StuKey.SystemState == SYSTEM_OFF )
                {
                    stu = 5;
                    atstu = 18;
                    break;
                }

#ifndef CFUN_FIVE

                //   if ( ( atenable == true ) && ( GSM_WAKE == READ_CTS() )  && atstu != 25 )
                if ( ( atenable == true ) && atstu != 25 )
                {
                    sleepinter = 50;
                    stu = 9;
                    atstu = 25;
                    //     GsmSta.gsm_p =0x4;
                    break;
                }

#endif

            }



            if ( ( atenable == true ) && ( ( GsmSta.IpOneConnect != OK ) && ( ( OK == FlashBufRead ( &StuFram ) ) ||
                                                                              ( GsmSto.updateflag == OK ) || ( GsmSta.SendingLen > 0 ) ) && ( connecttimer == 0 ) ) )
            {
                atstu = 18;
                GsmSta.FromUnconnect = 1;
                stu++;
            }

#endif
            break;

        case 5:/*sh=1,  sgact*/
            if ( atstu == 19 )
            {
                tmp = ActiveGprs ( buf, 0 );
            }
            else
            {
                tmp = ATSend ( atstu );
            }

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                atstu++;
                errtimes = 0;

                if ( atstu == 20 )
                {
                    atstu = 19;
                    timer = 18000; /*30min*/
                    stu++;

                    if ( StuKey.SystemState == SYSTEM_OFF )
                    {
                        stu = 16;
                        atstu = 41;
                        break;
                    }
                }
            }

            break;

        case 6:/*wait state   wait 30min ����*/
            tmp = ATSend ( atstu );

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                atstu++;
                errtimes = 0;

                if ( atstu == 23 )
                {

                    atstu = 19;

                    if ( GsmSta.CREG == NOT_OK )
                    {

                        atstu = 7;
                    }

#if 1

                    if ( ( GsmSta.CSQ > 5 ) && ( GsmSta.CREG == OK ) && ( GsmSta.CGREG == OK ) )
#else
                    if ( ( GsmSta.CSQ > 5 ) && ( GsmSta.CREG == OK ) )
#endif
                    {
                        stu++;
                        atstu = 26;
                    }

                }
                else if ( atstu == 8 )
                {
                    if ( GsmSta.QSS == NOT_OK )
                    {
                        {
                            stu = 100; /*reatart*/
                            GsmSta.PowerResetStu = 0;
                        }
                    }
                    else
                    {
                        atstu = 19;
                    }


                }

                if ( !timer )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }
            }

            break;

        case 7:/*cnmi sgact ext AT#SERVINFO*/
            if ( atstu == 28 )
            {
                tmp = ActiveGprs ( buf, 1 );
            }
            else
            {
                tmp = ATSend ( atstu );
            }

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 2 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                atstu++;

                if ( atstu == 29 )
                {
                    atstu = 24;
                }
                else if ( atstu == 25 )
                {
                    stu++;
                    timer = 0;
                }

                errtimes = 0;
            }

            break;

        case 8:/*connect gprs  ������������ �������Ӳ��Ϻ�ȴ�5min*/
            tmp = AT_SD ( buf );

            if ( AT_ERROR == tmp )
            {

                if ( connecttimes < 2 )
                {
                    connecttimer = 60;    //600; /*��Ϣ60s == 600 */
                }
                else
                {
                    connecttimer = 600 ;    //6000; /*��Ϣ600s == 6000*/
                }

                if ( connecttimes < 0xfd )
                {
                    connecttimes++;
                }

                stu = 4;
            }
            else if ( AT_OK == tmp )
            {
                connecttimes = 0;
                errtimes = 0;
                GsmSta.updatetime20sec = 180;
                GsmSta.IpOneConnect = OK;
                myprintf ( "server connection is%d\r\n", GsmSta.IpOneConnect );
                //stu=21;
                //atstu=38;
                stu = 4;
            }

            break;
#ifndef CFUN_FIVE

        case 9:/*����sleep*/

            tmp = ATSend ( atstu );

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                DTR_ENABLE();
                // DTR_DISABLE();
                RTS_READY_0();
                GSM_RESET_OFF();
                // GPIO_PinModeSet(gpioPortD, 4, gpioModeInput, 0);
                //GPIO_PinModeSet(gpioPortD, 4, gpioModePushPull, 1);
                errtimes = 0;
                stu = 4;
            }

            break;
#endif

        case 11:/*������*/
            GsmSta.ReadMsgTime++;

            if ( atstu == 200 )
            {
                tmp = ATReadMsg ( buf );
            }
            else         if ( atstu == 201 )
            {
                tmp = ATDeleteMsg ( buf );
            }
            else
            {
                tmp = ATSend ( atstu );
            }

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                if ( atstu == 33 )
                {
                    stu = 4;
                    GsmSta.MessageCome = NOT_OK;
                }

                if ( GsmSta.ReadMsgTime > 3000 ) /*5min��ʱɾ��ȫ������*/
                {
                    atstu = 33;
                    break;
                }

                if ( atstu == 9 )
                {
                    atstu = 32;
                }
                else if ( atstu == 32 )
                {
                    if ( GsmSta.msgs )
                    {
                        InitGsmQueue();
                        atstu = 200; /*����Ӧ�Ķ���*/
                        errtimes = 0;
                        break;
                    }
                    else
                    {
                        stu = 4;
                        GsmSta.MessageCome = NOT_OK;
                        errtimes = 0;
                        break;
                    }
                }
                else if ( atstu == 200 )
                {

                    ReadMsg();
                    atstu = 201;
                    errtimes = 0;
                    break;
                }
                else      if ( atstu == 201 )
                {
                    GsmSta.msgs = 0;
                    atstu = 32;
                    errtimes = 0;
                    break;
                }

                errtimes = 0;
            }

            break;

        case 12:/*����m2m*/
            tmp = ATSend ( atstu );

            if ( AT_WAIT != tmp )
            {
#if 0
                atstu = 37;
                stu = 20;
#else
                stu = 4;
#endif
                GsmSta.Askmsmback = 0;
                errtimes = 0;
            }

            break;

        case 13:/*���Ͷ���Ϣ*/

            // void DeleteMsgBuf(void);
            //ReadMsgBuf(msgdate,&msglen,&msgtype)
            //void WriteMsgBuf(u8* data,u8 len,u8 TYPE);
            if ( GsmSta.IpOneConnect == OK )
            {
                //ֱ�ӷ���
                atstu = 0;
                stu = 15;
            }
            else
            {
                stu++;/*reg cnmi*/
                timer = 1800; /*3min*/
                atstu = 21;
            }

            break;

        case 14:
            tmp = ATSend ( atstu );

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                if ( atstu == 21 )
                {
                    if ( GsmSta.CREG == OK )
                    {
                        atstu = 26;
                        errtimes = 0;
                        break;
                    }

                }

                if ( atstu == 26 )
                {
                    errtimes = 0;
                    atstu = 9;
                    break;
                }

                if ( atstu == 9 )
                {
                    stu++;
                    atstu = 0;
                }

                if ( !timer )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes = 0;

            }

            break;

        case 15:
            if ( atstu == 0 )
            {
                tmp = ATCSCS ( buf );
            }
            else  if ( atstu == 1 )
            {
                tmp = ATCSMP ( buf );
            }
            else  if ( atstu == 2 )
            {
                tmp = ATCMGS ( buf );
            }
            else  if ( atstu == 3 )
            {
                tmp = ATCMGS_DATE ( buf );
            }

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {

                if ( atstu == 3 )
                {
                    stu = 4;
                }

                atstu++;
                errtimes = 0;
            }

            break;

        case 16:/*GSM����ػ�*/
            tmp = ATSend ( atstu );

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {

                //DTR_DISABLE();
                DTR_ENABLE();
                RTS_READY_0();
                //RTS_UNREADY_1();
                GSM_RESET_OFF();

                initLeuart ( LEUART0, MODEM_IPR, POWER_OFF );

                GsmSta.charge_shutdown = 0xFE;
                errtimes = 0;
                stu++;
            }

            break;

        case 17:/*�ж�ϵͳ�Ƿ񿪻�*/
            if ( StuKey.SystemState != SYSTEM_OFF )
            {
                initLeuart ( LEUART0, MODEM_IPR, POWER_ON );
                errtimes = 0;
                stu = 101;
                GsmSta.IpOneConnect = NOT_OK;

            }

            break;

        case 18:/*������Ӫ��*/
            if ( timer )
            {
                if ( waitcreg )
                {
                    break;
                }

                waitcreg = 50;
                tmp = ATSend ( atstu );

                if ( AT_ERROR == tmp )
                {
                    if ( errtimes > 1 )
                    {
                        stu = 100; /*reatart*/
                        GsmSta.PowerResetStu = 0;
                    }

                    errtimes++;
                }
                else if ( AT_OK == tmp )
                {
                    errtimes = 0;

                    if ( GsmSta.CREG == OK )
                    {
                        timer = 3000;
                        stu++;
                        GsmSta.operturn = NOT_READ;
                        atstu = 24;
                    }
                }


            }
            else
            {
                stu = 100; /*reatart*/

            }

            break;

        case 19:
            if ( timer )
            {
                tmp = ATSend ( atstu );

                if ( AT_ERROR == tmp )
                {
                    if ( errtimes > 1 )
                    {
                        stu = 100; /*reatart*/
                        GsmSta.PowerResetStu = 0;
                    }

                    errtimes++;
                }
                else if ( AT_OK == tmp )
                {
                    errtimes = 0;

                    if ( GsmSta.operturn != NOT_READ )
                    {
                        GiveApnValue();
                        atstu = 11;

                        if ( StuApnNet.NetId == CHAP_MODE )
                        {
                            atstu = 12;
                        }

                        stu = 1;
                    }
                }
            }
            else
            {
                stu = 100; /*reatart*/
                GsmSta.PowerResetStu = 0;
            }

            break;

        case 20:
            tmp = ATSend ( atstu );

            if ( AT_WAIT != tmp )
            {
                stu = 4;
                errtimes = 0;
            }

            break;

        case 21:
            tmp = ATSend ( atstu );

            if ( AT_ERROR == tmp )
            {
                if ( errtimes > 1 )
                {
                    stu = 100; /*reatart*/
                    GsmSta.PowerResetStu = 0;
                }

                errtimes++;
            }
            else if ( AT_OK == tmp )
            {
                errtimes = 0;
                stu = 4;
            }

            break;

        case PHONE_COME:
#ifdef DEBUG_GPS_ANT

            stu = 21;
            GsmSta.PhoneCome = NOT_OK;
            break;
#else
            stu = 21;
            atstu = 39;
            GsmSta.PhoneCome = NOT_OK;
            break;
#endif
#if 0

        case 10:/*����netdate*/

            tmp = ATSend ( 40 );

            if ( AT_WAIT != tmp )
            {

                stu = 23;
                GsmSta.DateCome = NOT_OK;
                errtimes = 0;
            }

            break;

        case 23:
            tmp = AskNetDate ( buf );

            if ( AT_WAIT != tmp )
            {

                stu = 24;
                GsmSta.DateCome = NOT_OK;
                errtimes = 0;
            }

            break;

        case 24:
            tmp = ATSend ( 1 );

            if ( AT_WAIT != tmp )
            {

                stu = 4;
                GsmSta.DateCome = NOT_OK;
                errtimes = 0;
            }

            break;
#else

        case 10:
            tmp = AskNetDate ( buf );

            if ( AT_WAIT != tmp )
            {

                stu = 4;
                GsmSta.DateCome = NOT_OK;
                errtimes = 0;
            }

            break;

#endif

        case 100:
        case 101:

            //   if( (Ue910Mode)||(PowerOK==GsmReset()))
            if ( PowerOK == GsmReset() )
            {
                errtimes = 0;
                GsmSta.sos = 0;
                InitGsmDate();
#if 0
                stu = 0;
                atstu = 0;
#else
                stu = 120;
                nDelay = 50;
                atstu = 0;
#endif
            }

            gGsmPowerDown = 0;
            break;

        case 102:
            tmp = ATSend ( atstu );

            if ( tmp == AT_OK )
            {
                atstu = 2;
                stu = 0;
            }

            cops_set = 1;
            break;

        case 103:
            tmp = ATSend ( atstu );

            if ( tmp == AT_OK )
            {
                if ( GsmSta.operator_flag & 0x03 )
                {
                    atstu = 46;
                    stu = 102;
#ifdef USE_PRINTF
                    myprintf ( "cops=0 will be setting\r\n" );
#endif
                }
                else if ( GsmSta.operator_flag & 0x1c )
                {
                    atstu = 47;
                    stu = 102;
#ifdef USE_PRINTF
                    myprintf ( "operator 3 will be setting\r\n" );
#endif
                }
                else
                {
                    nDelay = 30;
                    stu    = 120;
                    atstu  = 48;
                }

            }

            break;

        case 120:
            if ( nDelay == 0 )
            {
                stu = 0;
            }
            else
            {
                nDelay --;
            }

            break;

        default:
            stu = 0;
            atstu = 0;
            errtimes = 0;
            break;


    }


}

/*
*********************************************************************************************************
*   �� �� ��u16 ActiveGprs(u8*buf,u8 flag)
*   ����˵��: flagΪ�漤��gprs ,����ر�gprs
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 ActiveGprs ( u8 *buf, u8 flag )
{
    u8 len;
    len = 0;

    if ( flag )
    {
        Mymemcpy ( buf, "AT#SGACT=1,1", sizeof ( "AT#SGACT=1,0" ) - 1 );
    }
    else
    {
        Mymemcpy ( buf, "AT#SGACT=1,0", sizeof ( "AT#SGACT=1,0" ) - 1 );
    }

    len = sizeof ( "AT#SGACT=1,0" ) - 1;

    if ( ( StuApnNet.NetId == CHAP_MODE ) || ( StuApnNet.NetId == PAP_MODE ) )
    {
        buf[len++] = ',';
        Mymemcpy ( &buf[len], StuApnNet.NetIdName, StuApnNet.NetIdNameLen );
        len += StuApnNet.NetIdNameLen;
        buf[len++] = ',';
        Mymemcpy ( &buf[len], StuApnNet.NetIdPsswd, StuApnNet.NetIdPsswdLen );
        len += StuApnNet.NetIdPsswdLen;
    }

    buf[len++] = 0x0d;
    buf[len] = 0;
    len = GsmSendAtCommand ( buf, len, "OK", 1, 150 );
    return len;
}

/*
*********************************************************************************************************
*   �� �� ��u8 ATCSCS(u8*buf)
*   ����˵��:u8 ATCSCS(u8*buf)
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 ATCSCS ( u8 *buf )
{
    u8 len;
    len = 0;

    if ( ( GsmSto.MessageCenterLen == 0 ) || ( GsmSto.MessageCenterLen > 19 ) )
    {
        return AT_OK;
    }

    Mymemcpy ( buf, "AT+CSCA=", sizeof ( "AT+CSCA=" ) - 1 );
    len = sizeof ( "AT+CSCA=" ) - 1;
    Mymemcpy ( &buf[len], GsmSto.MessageCenter, GsmSto.MessageCenterLen );
    len += GsmSto.MessageCenterLen;
    buf[len++] = ',';
    Mymemcpy ( &buf[len], "145", 3 );
    len += 3;
    buf[len++] = 0x0d;
    buf[len] = 0;
    len = GsmSendAtCommand ( buf, len, "OK", 1, 5 );

    if ( AT_OK == len )
    {
        GsmSto.MessageCenterLen = 0;
        WriteGsmStoreDate();

    }

    return len;
}

/*
*********************************************************************************************************
*   �� �� ��u8 ATCSCS(u8*buf)
*   ����˵��:u8 ATCSCS(u8*buf)
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 ATCSMP ( u8 *buf )
{

    u8 TYPE;
    u8 len = 0;
    ReadMsgBuf ( &TYPE );

    if ( TYPE == TXT )
    {
        Mymemcpy ( buf, "AT+CSMP=17,167,0,0", sizeof ( "AT+CSMP=17,167,0,1" ) - 1 );
        len = sizeof ( "AT+CSMP=17,167,0,1" ) - 1;
    }
    else
    {
        Mymemcpy ( buf, "AT+CSMP=17,167,0,8", sizeof ( "AT+CSMP=17,167,0,8" ) - 1 );
        len = sizeof ( "AT+CSMP=17,167,0,8" ) - 1;
    }


    buf[len++] = 0x0d;
    buf[len] = 0;
    len = GsmSendAtCommand ( buf, len, "OK", 1, 10 );

    if ( AT_OK == len )
    {
        GsmSto.MessageCenterLen = 0;
        WriteGsmStoreDate();

    }

    return len;
}

/*
*********************************************************************************************************
*   �� �� ��u8 ATCMGS(u8*buf)
*   ����˵��:u8 ATCSCS(u8*buf)
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 ATCMGS ( u8 *buf )
{

    u8 len;
    u8 i;
    len = 0;

    for ( i = 0; i < MESSAGE_NUMBERS; i++ )
    {
        if ( GsmSta.messagelen[i] != 0 )
        {

            Mymemcpy ( buf, "AT+CMGS=", sizeof ( "AT+CMGS=" ) - 1 );
            len = sizeof ( "AT+CMGS=" ) - 1;
            Mymemcpy ( &buf[len], &GsmSta.msgnumber[i][0], GsmSta.msgnumberlen[i] );
            len += GsmSta.msgnumberlen[i];
            buf[len++] = 0x0d;
            buf[len] = 0;
            len = GsmSendAtCommand ( buf, len, "> ", 1, 100 );

            return len;
        }
    }

    return len;
}

/*
*********************************************************************************************************
*   �� �� ��u8 ATCMGS_DATE(u8*buf)
*   ����˵��:
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 ATCMGS_DATE ( u8 *buf )
{
    u16 len, len1;
    u16 i;
    u8 *data;
    len = 0;

    for ( i = 0; i < MESSAGE_NUMBERS; i++ )
    {
        if ( GsmSta.messagelen[i] != 0 )
        {

            if ( GsmSta.messagetype[i] != PDU_HEX )
            {
                Mymemcpy ( &buf[len], &GsmSta.messagebuf[i][0], GsmSta.messagelen[i] );
                len = GsmSta.messagelen[i];
            }
            else
            {
                len1 = GsmSta.messagelen[i] - 1;
                len = 0;
                data = &GsmSta.messagebuf[i][0];

                for ( i = 0; i < len1; i++ )
                {
                    buf[len++] =  HexToAscll ( ( ( data[i] ) >> 4 ) & 0x0f, 1 );
                    buf[len++] =  HexToAscll ( ( data[i] ) & 0x0f, 1 );
                }

                buf[len++] = 0x1a;
            }

            len = GsmSendAtCommand ( buf, len, "OK", 1, 150 );

            if ( AT_OK == len )
            {
                DeleteMsgBuf();


            }

            return len;
        }
    }

    return len;
}

/*
*********************************************************************************************************
*   �� �� ��u8  AskNetDate(u8*buf)
*   ����˵��: ������������
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8  AskNetDate ( u8 *buf )
{

    if ( 0 == GsmSta.AskNetAtLen )
    {

        Mymemcpy ( buf, "AT#SRECV=1,", 11 );
        GsmSta.AskNetAtLen = 11;

        if ( GsmSta.RevLen > 1000 )
        {
            GsmSta.asklen = 1000;
            GsmSta.UnRevLen = GsmSta.RevLen - 1000;
        }
        else
        {
            GsmSta.asklen = GsmSta.RevLen;
            GsmSta.UnRevLen = 0;
        }

        GsmSta.AskNetAtLen += DecToString ( GsmSta.asklen, &buf[GsmSta.AskNetAtLen] );
        //  buf[GsmSta.AskNetAtLen++]=',';
        //   buf[GsmSta.AskNetAtLen++]='1';

        buf[GsmSta.AskNetAtLen++] = 0x0d;
        //   buf[GsmSta.AskNetAtLen++]=0x0d;
        //  buf[GsmSta.AskNetAtLen++]=0x0a;


        buf[GsmSta.AskNetAtLen] = 0;
    }

    return GsmSendAtCommand ( buf, GsmSta.AskNetAtLen, "OK", 1, 8 );


}
/*
*********************************************************************************************************
*   �� �� ��u8  ATReadMsg(u8*buf)
*   ����˵��: ������Ϣ
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8  ATReadMsg ( u8 *buf )
{

    u8 len;
    len = 0;
    Mymemcpy ( buf, "AT+CMGR=000\r", 12 );
    len = 12;

    if ( GsmSta.msgs > 100 )
    {
        buf[8] = ( GsmSta.msgs / 100 ) + 0x30;

        buf[9] = ( GsmSta.msgs % 100 / 10 ) + 0x30;
        buf[10] = ( GsmSta.msgs % 10 ) + 0x30;
    }
    else
    {
        buf[8] = ( GsmSta.msgs % 100 / 10 ) + 0x30;
        buf[9] = ( GsmSta.msgs % 10 ) + 0x30;
        buf[10] = '\r';
        len = 11;
    }

    len = GsmSendAtCommand ( buf, len, "OK", 1, 8 );
    return len;


}
/*
*********************************************************************************************************
*   �� �� ��u8  ATDeleteMsg(u8*buf)
*   ����˵��: ������Ϣ
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8  ATDeleteMsg ( u8 *buf )
{

    u8 len;
    len = 0;
    Mymemcpy ( buf, "AT+CMGD=000\r", 12 );
    len = 12;

    if ( GsmSta.msgs > 100 )
    {
        buf[8] = ( GsmSta.msgs / 100 ) + 0x30;
        buf[9] = ( GsmSta.msgs % 100 / 10 ) + 0x30;
        buf[10] = ( GsmSta.msgs % 10 ) + 0x30;
    }
    else
    {
        buf[8] = ( GsmSta.msgs % 100 / 10 ) + 0x30;
        buf[9] = ( GsmSta.msgs % 10 ) + 0x30;
        buf[10] = '\r';
        len = 11;
    }

    len = GsmSendAtCommand ( buf, len, "OK", 1, 8 );
    return len;


}

/*
*********************************************************************************************************
*   �� �� ��    void FindMsgNumber(u8 *datein,u16 len,u8*number,u8*numberlen)
*   ����˵��: ������Ϣ���ͷ��ĺ���
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void FindMsgNumber ( u8 *datein, u16 len, u8 *number, u8 *numberlen )
{
    //+CMGR: "REC READ","+8613168094719","","13/08/01,14:58:45+32"
    u8 *p, *p1;
    numberlen[0] = 0;

    if ( 0 == ( p = Findbcd ( ( u8 * ) datein, len, "+CMGR:", 6 ) ) )
    {
        return;
    }

    if ( 0 == ( p1 = Findbcd ( &p[6], len - 6, ",\"", 2 ) ) )
    {
        return;
    }

    if ( 0 == ( p = Findbcd ( &p1[2], len - 20, "\",", 2 ) ) )
    {
        return;
    }

    numberlen[0] = p - p1 - 2;

    Mymemcpy ( number, &p1[2], numberlen[0] );

}
/*
*********************************************************************************************************
*   �� �� �� void ReadMsg(void)
*   ����˵��: ������Ϣ
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ReadMsg ( void )
{
    u8 *p, *p1;
    u8 store;
    u8 mima = NOT_OK;
    u8 msg[200];
    u8 msglen;
    u8 messagenumber[19];
    u8 messagelen;

    if ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "CJSET", 5 ) ) )
    {

        mima = OK;
    }
    else    if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "pwd:", 4 ) ) ) || //PWD:123456
                 ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "PWD:", 4 ) ) )
               )
    {

        if ( EQUER == EqureOrNot ( &p[4], GsmSto.mima, 6 ) )
        {
            mima = OK;
        }

    }

    FindMsgNumber ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, messagenumber, &messagelen );


    msglen = 0;
    store = 0;

    if ( mima == OK )
    {

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "ip:(", 4 ) ) ) || //ip:(liupeng08304.oicp.net:8899)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "IP:(", 4 ) ) )
           )
        {

            p1 = Finddate ( &p[4], ':', 79 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            GsmSto.iplen = p1 - p - 4;
            Mymemcpy ( GsmSto.strip, &p[4], GsmSto.iplen );

            p = Finddate ( &p1[1], ')', 7 + 1 );

            if ( p == 0 )
            {
                return;
            }

            GsmSto.portlen = p - p1 - 1;
            Mymemcpy ( GsmSto.port, &p1[1], GsmSto.portlen );
            store = 1;
            WaitToResetSystem ( 30 );
            Mymemcpy ( &msg[msglen], "ip port,", 8 );
            msglen += 8;
        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "SETPWD:(", 8 ) ) ) || //SETPWD:(123456)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "setpwd:(", 8 ) ) )
           )
        {
            p1 = Finddate ( &p[8], ')', 6 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            Mymemcpy ( GsmSto.mima, &p[8], 6 );
            store = 1;

            Mymemcpy ( &msg[msglen], "PWD,", 4 );
            msglen += 4;

        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "APN:(", 5 ) ) ) || //apn:(cmnet)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "apn:(", 5 ) ) )
           )
        {
            p1 = Finddate ( &p[5], ')', 39 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            GsmSto.apnlen = p1 - p - 5;
            Mymemcpy ( GsmSto.strapn, &p[5], GsmSto.apnlen );
            store = 1;
            WaitToResetSystem ( 30 );
            Mymemcpy ( &msg[msglen], "APN,", 4 );
            msglen += 4;
        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "moving:(", 8 ) ) ) || //moving:(60,120)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "MOVING:(", 8 ) ) )
           )
        {
            p1 = Finddate ( &p[8], ')', 11 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            GsmSto.moveintervalGPS = strtol ( ( char * ) &p[8], NULL, 10 );
            p1 = Finddate ( &p[8], ',', 6 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            GsmSto.moveintervalGSM = strtol ( ( char * ) &p1[1], NULL, 10 );
            Mymemcpy ( &msg[msglen], "MOVING,", 7 );
            msglen += 7;
            store = 1;
        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "unmove:(", 8 ) ) ) || //unmove:(180)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "UNMOVE:(", 8 ) ) )
           )
        {
            p1 = Finddate ( &p[8], ')', 5 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            GsmSto.staticinterval = strtol ( ( char * ) &p[8], NULL, 10 );
            store = 1;

            Mymemcpy ( &msg[msglen], "UNMOVE,", 7 );
            msglen += 7;
        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "alive:(", 7 ) ) ) || //alive:(180)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "ALIVE:(", 7 ) ) )
           )
        {
            p1 = Finddate ( &p[7], ')', 5 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            GsmSto.KeepAliveInter = strtol ( ( char * ) &p[7], NULL, 10 );
            store = 1;

            Mymemcpy ( &msg[msglen], "alive,", 6 );
            msglen += 6;
        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "BTRAM:(", 7 ) ) ) || //BTRAM:(10)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "btram:(", 7 ) ) )
           )
        {
            p1 = Finddate ( &p[7], ')', 3 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            GsmSto.BatteryQuatity = strtol ( ( char * ) &p[7], NULL, 10 );
            store = 1;
        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "id:(", 4 ) ) ) || //id:(01234567890123456789)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "ID:(", 4 ) ) )
           )
        {
            p1 = Finddate ( &p[4], ')', 20 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            if ( 20 == ( p1 - p - 4 ) )
            {
                ConverascTohex ( &p[4], 20, GsmSto.ID );
                store = 1;
            }

            WaitToResetSystem ( 30 );
            Mymemcpy ( &msg[msglen], "ID,", 3 );
            msglen += 3;

        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "NETMODE:", 8 ) ) ) || //NETMODE:EMPTY     NETMODE:PAPNAME(liupeng)PAPPWD(123456)  NETMODE:CHAPNAME(liupeng)CHAPPWD(123456)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "netmode:", 8 ) ) )
           )
        {
            //NETMODE:EMPTY     NETMODE:PAPNAME(liupeng)PAPPWD(123456)  NETMODE:CHAPNAME(liupeng)CHAPPWD(123456)
            if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "NETMODE:EMPTY", 13 ) ) ) || //NETMODE:EMPTY     NETMODE:PAPNAME(liupeng)PAPPWD(123456)  NETMODE:CHAPNAME(liupeng)CHAPPWD(123456)
                 ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "netmode:empty", 13 ) ) )
               )
            {
                GsmSto.NetId = EMPTY_MODE;
                store = 1;
                WaitToResetSystem ( 30 );
                Mymemcpy ( &msg[msglen], "EMPTY MODE,", 11 );
                msglen += 11;
            }

            if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "NETMODE:PAPNAME", 15 ) ) ) || // NETMODE:PAPNAME(liupeng)PAPPWD(123456)  NETMODE:CHAPNAME(liupeng)CHAPPWD(123456)
                 ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "netmode:papname", 15 ) ) )
               )
            {
                p1 = Finddate ( &p[16], ')', 79 + 1 );

                if ( p1 == 0 )
                {
                    return;
                }

                store = 1;
                GsmSto.NetId = PAP_MODE;
                GsmSto.NetIdNameLen = p1 - ( p + 16 );
                Mymemcpy ( GsmSto.NetIdName, &p[16], GsmSto.NetIdNameLen );

                p = Finddate ( &p1[8], ')', 79 + 1 );

                if ( p == 0 )
                {
                    return;
                }

                GsmSto.NetIdPsswdLen = p - ( p1 + 8 );
                Mymemcpy ( GsmSto.NetIdPsswd, &p1[8], GsmSto.NetIdPsswdLen );
                WaitToResetSystem ( 30 );
                Mymemcpy ( &msg[msglen], "NET MODE PAP,", 13 );
                msglen += 13;
            }

            if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "NETMODE:CHAPNAME", 16 ) ) ) || //NETMODE:CHAPNAME(liupeng)CHAPPWD(123456)
                 ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "netmode:chapname", 16 ) ) )
               )
            {
                p1 = Finddate ( &p[17], ')', 79 + 1 );

                if ( p1 == 0 )
                {
                    return;
                }

                store = 1;
                GsmSto.NetId = CHAP_MODE;
                GsmSto.NetIdNameLen = p1 - ( p + 17 );
                Mymemcpy ( GsmSto.NetIdName, &p[17], GsmSto.NetIdNameLen );

                p = Finddate ( &p1[9], ')', 79 + 1 );

                if ( p == 0 )
                {
                    return;
                }

                GsmSto.NetIdPsswdLen = p - ( p1 + 9 );
                Mymemcpy ( GsmSto.NetIdPsswd, &p1[9], GsmSto.NetIdPsswdLen );
                WaitToResetSystem ( 30 );
                Mymemcpy ( &msg[msglen], "NET MODE CHAP,", 14 );
                msglen += 14;
            }

        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "up:(", 4 ) ) ) || //up:(liupeng08304.oicp.net:8899)  up:()
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "UP:(", 4 ) ) )
           )
        {

            if ( p[4] == ')' )
            {
                StartUpdatemcuInit ( 0, 0, 0, 0 );

            }
            else
            {
                p1 = Finddate ( &p[4], ':', 79 + 1 );

                if ( p1 == 0 )
                {
                    return;
                }

                GsmSto.updateiplen = p1 - p - 4;
                Mymemcpy ( GsmSto.updatestrip, &p[4], GsmSto.updateiplen );

                p = Finddate ( &p1[1], ')', 7 + 1 );

                if ( p == 0 )
                {
                    return;
                }

                GsmSto.updateportlen = p - p1 - 1;
                Mymemcpy ( GsmSto.updateport, &p1[1], GsmSto.updateportlen );

                GsmSto.updateflag = OK;
                GsmSto.currentpackeg = 0;
                GsmSto.totalpackeg = 10;


                store = 1;
                GsmSta.IpOneConnect = NOT_OK;

                Mymemcpy ( &msg[msglen], "UP,", 3 );
                msglen += 3;
            }
        }

        if ( ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "cm:(", 4 ) ) ) || //CM:(008613168094719)
             ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "CM:(", 4 ) ) )
           )
        {
            p1 = Finddate ( &p[4], ')', 19 + 1 );

            if ( p1 == 0 )
            {
                return;
            }

            GsmSto.MessageCenterLen = p1 - p - 4;
            Mymemcpy ( GsmSto.MessageCenter, &p[4], GsmSto.MessageCenterLen );
            store = 1;

            Mymemcpy ( &msg[msglen], "CM,", 3 );
            msglen += 3;
        }

    }

    if ( store != 0 )
    {
        WriteGsmStoreDate();

    }

#if 0

    if ( ( msglen > 0 ) && ( messagelen > 5 ) )
    {

        Mymemcpy ( &msg[msglen], "SET OK", 6 );
        msglen += 6;
        WriteMsgBuf ( msg, msglen, messagenumber, messagelen, TXT );
    }

#endif
    //up:(liupeng08304.oicp.net:8899)  up:()  NETMODE:EMPTY     NETMODE:PAPNAME(liupeng)PAPPWD(123456)  NETMODE:CHAPNAME(liupeng)CHAPPWD(123456) id:(01234567890123456789)
    //ip:(liupeng08304.oicp.net:8899) SETPWD:123456 apn:(cmnet) moving:(60) unmove:(180) alive:(180) BTRAM:(10)
    //ip:(liupeng08304.oicp.net:8899) SETPWD:123456 apn:(cmnet) moving:(60) unmove:(180) alive:(180) BTRAM:(10)
    //pwd:123456up:(liupeng08304.oicp.net:8899)NETMODE:PAPNAME(liupeng)PAPPWD(123456)SETPWD:654321 apn:(cmnet) moving:(60) unmove:(180) alive:(180) BTRAM:(10)

}





/*
*********************************************************************************************************
*   �� �� ��u8 SetApn(u8*buf)
*   ����˵��:����apn
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 SetApn ( u8 *buf )
{
    u8 tmp;
    Mymemcpy ( buf, "AT+CGDCONT=1,ip,", sizeof ( "AT+CGDCONT=1,ip," ) - 1 );
    tmp = 16;
    Mymemcpy ( &buf[tmp], StuApnNet.strapn, StuApnNet.apnlen );
    tmp += StuApnNet.apnlen;
    buf[tmp++] = 0x0d;
    buf[tmp] = 0;
    tmp = GsmSendAtCommand ( buf, tmp, "OK", 1, 15 );
    return tmp;
}
/*
*********************************************************************************************************
*   �� �� ��u8 AT_SD(u8*buf)
*   ����˵��:���磻����
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 AT_SD ( u8 *buf )
{
    // Open socket 1 in command mode
    // AT#SD=1,0,80,��www.google.com��,0,0,1
    u8 tmp;
    Mymemcpy ( buf, "AT#SD=1,0,", sizeof ( "AT#SD=1,0," ) - 1 );
    tmp = sizeof ( "AT#SD=1,0," ) - 1;


    if ( ( GsmSto.updateflag == OK ) && ( GsmSto.updateiplen ) )
    {
        Mymemcpy ( &buf[tmp], GsmSto.updateport, GsmSto.updateportlen );
        tmp += GsmSto.updateportlen;
    }
    else
    {
        Mymemcpy ( &buf[tmp], GsmSto.port, GsmSto.portlen );
        tmp += GsmSto.portlen;
    }


    buf[tmp++] = ',';
    buf[tmp++] = '\"';

    if ( ( GsmSto.updateflag == OK ) && ( GsmSto.updateiplen ) )
    {
        Mymemcpy ( &buf[tmp], GsmSto.updatestrip, GsmSto.updateiplen );
        tmp += GsmSto.updateiplen;
    }
    else
    {
        Mymemcpy ( &buf[tmp], GsmSto.strip, GsmSto.iplen );
        tmp += GsmSto.iplen;
    }

    buf[tmp++] = '\"';
    buf[tmp++] = ',';
    Mymemcpy ( &buf[tmp], "0,0,1", 5 );
    tmp += 5;
    buf[tmp++] = 0x0d;
    buf[tmp] = 0;
    tmp = GsmSendAtCommand ( buf, tmp, "OK", 1, 120 );
    return tmp;
}
/*
*********************************************************************************************************
*   �� �� ��u8 ProcessSendNetDate(u8*DateIn,u16 DateLen,u8 rightnow)
*   ����˵��: ���ͻ����е���������
*                                rightnowΪok���Ϸ��ͣ������ͻ���
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 ProcessSendNetDate ( u8 *DateIn, u16 DateLen, u8 rightnow )
{

    static u8 tmp[30];
    static u8 stu = 0;
    static u8 len;
    u8 result;
    u16 i;
    result = true;

    switch ( stu )
    {
        case 0:
            if ( rightnow != OK )
            {
                if ( ( GsmSta.FromUnconnect ) && ( GsmSta.SendingLen ) )
                {
                    GsmSta.FromUnconnect = 0;
                    goto L1;
                }
                else  if ( OK != GetGsmBufData() )
                {
                    return result;
                }
            }
            else
            {
                PackDateToBuf ( DateIn, DateLen, GsmSta.SendingBuf, &i );
                GsmSta.SendingLen = i;
            }

L1:
            Mymemcpy ( tmp, "AT#SSENDEXT=1,", 14 );
            tmp[12] = '1';
            len = 14;

            if ( GsmSta.SendingLen > 1024 )
            {
                GsmSta.SendingLen = 0;
                return result;
            }

            len += DecToString ( GsmSta.SendingLen, &tmp[len] );
            tmp[len++] = 0x0d;
            stu++;
            GsmSta.SendDate = OK;
            GPIO_PinModeSet ( GSM_TX_PORT, GSM_TX_PIN, gpioModePushPull, 0 );
            PrintfNetDate ( GsmSta.SendingBuf, GsmSta.SendingLen );
            GsmSta.Sending = DATAS_SENDING;

            break;

        case 1:
        case 2:
        case 3:

            i = GsmSendAtCommand ( tmp, ( u16 ) len, "> ", 1, 20 );

            if ( i == AT_ERROR )
            {
                stu++;

            }

            if ( i == AT_OK )
            {
                stu = 5;
            }

            result = false;
            break;

        case 4:
            stu = 0;
            GsmSta.Sending = DATAS_IDLES;
            GsmSta.IpOneConnect = NOT_OK;
            myprintf ( "server connection is %d\r\n", GsmSta.IpOneConnect );
            result = false;
            break;

        case 5:
            i = GsmSendAtCommand ( GsmSta.SendingBuf, GsmSta.SendingLen, "OK", 1, 20 );

            if ( i != AT_WAIT )
            {

                if ( GSM_TX_PORT == gpioPortE )
                    while ( ! ( LEUART0->STATUS & LEUART_STATUS_TXBL ) );
                else
                    while ( ! ( USART2->STATUS & USART_STATUS_TXBL ) );

                GPIO_PinModeSet ( GSM_TX_PORT, GSM_TX_PIN, gpioModeInput, 0 );
                GsmSta.SendDate = NOT_OK;

                if ( i == AT_ERROR )
                {

                    stu = 4;
                }

                if ( i == AT_OK )
                {
                    GsmSta.Sending = DATAS_IDLES;
                    stu = 0;
                }
            }

            result = false;
            break;


    }


    return result;


}

/*
*********************************************************************************************************
*   �� �� ��    void PrintfNetDate(u8* buf,u16 len)
*   ����˵��: ��ӡgsm������Ϣ
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void PrintfNetDate ( u8 *buf, u16 len )
{
#ifdef USE_PRINTF
    u16 i;

    if ( debug == DEBUGNET )
    {

        //  RTC_Get();
        //  printf("time:%02d:%02d:%02d",timer.hour,timer.min,timer.sec);

        myprintf ( "SED:" );

        for ( i = 0; i < len; i++ )
        {

            if ( ( i > 1 ) && ( buf[i - 1] == 0xaa ) && ( buf[i] == 0xaa ) )
            {
                // PrintUsart(0x0d);
                //  PrintUsart(0x0a);
                myprintf ( "\r\n    " );
            }

            myprintf ( "%02x ", buf[i] );
            //PrintUsart(HexToAscll((buf[i]>>4)&0x0f, 0));
            // PrintUsart(HexToAscll(buf[i]&0x0f, 0));
        }

        //    PrintUsart(0x0d);
        //    PrintUsart(0x0a);
        myprintf ( "\r\n" );

    }

#endif


}
/*
*********************************************************************************************************
*   �� �� ��u8 ProcessSmsPhone(void)
*   ����˵��: GSM������ź͵绰
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 ProcessSmsPhone ( void )
{

    if ( GsmSta.MessageCome == OK )
    {
        return OK;
    }
    else
    {
        return NOT_OK;
    }



}
/*
*********************************************************************************************************
*   �� �� ��u8 ProcessPhone(void)
*   ����˵��: GSM����绰
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 ProcessPhone ( void )
{

    if ( GsmSta.PhoneCome == OK )
    {
        return OK;
    }
    else
    {
        return NOT_OK;
    }



}
/*
*********************************************************************************************************
*   �� �� ��void ProcessUpdateMcu(void)
*   ����˵��: GSM�����������
*                              ���������������ʱ��:��ջ�����Ҫ���͵����ݣ�ֹͣ���ͳ������������������ݣ��������ٿռ�
�洢������֪������ʧ�ܻ�ɹ���ֻ����һ·�������ӣ�������������������������ӵ�����������
��������С�̶�Ϊ512��һ��һ������
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ProcessUpdateMcu ( void )
{
    static u8 stu = 0;
    u8 PROGRAM[12] = "PROGRAM";
    STRUCT_BLIND_FRAM StuFram;
    GsmSta.updatetime20sec++;
    GsmSta.updatetime20min++;

    if ( ( GsmSto.updateflag != OK ) || ( resetflag == 0xaa ) )
    {
        stu = 0;
        return;
    }

    switch ( stu )
    {
        case 0:

            if ( GsmSto.updateflag == OK )
            {
                GsmSta.updatetime20sec = 201;
                GsmSta.updatetime20min = 0;

                if ( GsmSto.updateiplen )
                {
                    GsmSta.IpOneConnect = NOT_OK;
                }

                stu++;
            }

            break;

        case 1:
            if ( GsmSta.IpOneConnect == OK )
            {
                stu++;
            }

            break;

        case 2:
            if ( GsmSto.currentpackeg == GsmSto.totalpackeg ) /*���½���*/
            {

                if ( GsmSto.updatevarity == CalcUpdateVarity() )
                {
                    FLASH_eraseOneBlock ( UpdatePara );
                    PROGRAM[7] = GsmSto.updatevarity;
                    PROGRAM[8] = ( u8 ) ( ( GsmSto.Softwarelen >> 0 ) & 0x000000ff );
                    PROGRAM[9] = ( u8 ) ( ( GsmSto.Softwarelen >> 8 ) & 0x000000ff );
                    PROGRAM[10] = ( u8 ) ( ( GsmSto.Softwarelen >> 16 ) & 0x000000ff );
                    PROGRAM[11] = ( u8 ) ( ( GsmSto.Softwarelen >> 24 ) & 0x000000ff );
                    FLASH_WriteDate ( UpdatePara, 12, PROGRAM );
                    GsmSto.updateflag = NOT_OK;
                    WriteGsmStoreDate();
#ifdef USE_PRINTF
                    myprintf ( "Version = 0x%x%x, Download data successful! Total packeg is %d.\r\n", VERSION[0], VERSION[1], GsmSto.totalpackeg );
#endif
                    /*wait to ack to reset system*/
                    WaitToResetSystem ( 20 );
                    stu = 0;
                    UPdateOk ( 0 );
                }
                else
                {

                    GsmSto.updateflag = NOT_OK;
                    WriteGsmStoreDate();
                    UPdateOk ( 2 );
                    stu = 0;
                }

            }
            else  if ( GsmSta.updatetime20min > 12000 ) /*20min��Ӧ��ʱ�����½���*/
            {
#ifdef USE_PRINTF
                myprintf ( "Version = 0x%x%x, Time is over.\r\n", VERSION[0], VERSION[1] );
#endif
                GsmSto.updateflag = NOT_OK;
                WriteGsmStoreDate();
                UPdateOk ( 5 );
                stu = 0;
            }
            else  if ( ( GsmSta.updatetime20sec > 200 ) && ( NOT_OK == FlashBufRead ( &StuFram ) ) && ( GsmSto.updateflag == OK ) ) /*������һ��*/
            {
#ifdef USE_PRINTF
                myprintf ( "Version = 0x%x%x, The total packet is %d, The current packet is %d.\r\n", VERSION[0], VERSION[1], GsmSto.totalpackeg, GsmSto.currentpackeg );
#endif
                AskAppPacket();
                GsmSta.updatetime20sec = 0;
            }

            break;

    }
}
/*
*********************************************************************************************************
*   �� �� ��void ProcessUpdateSGEE(void)
*   ����˵��: SGEE���ݹ���
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
AT_NET_SGEE AtNetSgee;
void ProcessUpdateSGEE ( void )
{
    STRUCT_BLIND_FRAM StuFram;
    static u8 stu = 2;


    if ( ( GsmSto.updateflag == OK ) || ( resetflag == 0xaa ) )
    {
        return;
    }


    if ( AtNetSgee.time < 0xfd )
    {
        AtNetSgee.time++;
    }

    if ( AtNetSgee.overtime < 0xfff0 )
    {
        AtNetSgee.overtime++;
    }

    if ( GsmSta.charging != BATTERY_CHARGE )
    {
        if ( stu > 2 )
        {
            stu = 2;
            GpsStatues.SgeeState = AGPS_IDLE;
        }

        return;
    }


    /*��Ӧ���ʱ��AtNetSgee.overtime=0  */
    switch ( stu )
    {
        case 0:
            if ( timer.hour < 6 )
            {
                stu++;
            }

            if ( debug == DEBUGAGP )
            {
                debug = DEBUGGPS;
                stu = 2;
            }

            break;

        case 1:
            if ( timer.hour > 6 )
            {
                stu++;
            }

            if ( debug == DEBUGAGP )
            {
                debug = DEBUGGPS;
                stu = 2;
            }

            break;

        case 2:

            if ( ( AGPS_IDLE == GpsStatues.SgeeState ) && ( GsmSto.updateflag != OK ) && ( NOT_OK == FlashBufRead ( &StuFram ) ) )
            {
                AtNetSgee.currentpackeg = 0;
                AtNetSgee.packegsize = 800;
                AtNetSgee.totalpackeg = 10;
                GpsStatues.SgeeState = AGPS_UPING;
                stu++;
            }

            break;

        case 3:/*ask first packedge*/
            AskSgeePacket();
            AtNetSgee.Sgeedatecome = NOT_OK;
            AtNetSgee.overtime = 0;
            stu++;
            break;

        case 4:/*wait first packeg come*/
            if ( AtNetSgee.Sgeedatecome == OK )
            {
                AtNetSgee.time = 0;
                AtNetSgee.overtime = 0;
                AtNetSgee.Sgeedatecome = NOT_OK;
                stu++;
            }
            else if ( AtNetSgee.overtime > 600 ) /*1min��Ӧ��ʱ����ʧ��*/
            {
                GpsStatues.SgeeState = AGPS_IDLE;
                WriteSGEEHavedate ( NOT_OK );
                stu = 0;
            }
            else if ( AtNetSgee.overtime == 65534 ) /*���ݰ���С����������ʧ��*/
            {
                GpsStatues.SgeeState = AGPS_IDLE;
                stu = 0;
            }

            break;

        case 5:

            if ( AtNetSgee.currentpackeg == AtNetSgee.totalpackeg ) /*�����ɹ����㱨�����ɹ�*/
            {
                /*У����ͨ��֪ͨ����gps���SGEE*/
                if ( AtNetSgee.varity == CalcNetSgeeVarity() )
                {


                    WriteSGEEHavedate ( OK );
                    SgeeReport ( 0 );
                    GpsStatues.SgeeState = AGPS_NEED;
                }
                else
                {
                    SgeeReport ( 2 );
                    WriteSGEEHavedate ( NOT_OK );
                    GpsStatues.SgeeState = AGPS_IDLE;
                }

                stu = 0;
            }
            else if ( AtNetSgee.overtime == 65534 ) /*���ݰ���С����������ʧ��*/
            {
                GpsStatues.SgeeState = AGPS_IDLE;
                stu = 0;
            }

            else if ( AtNetSgee.overtime == 65533 ) /*д�������һ�´���*/
            {
                GpsStatues.SgeeState = AGPS_IDLE;
                stu = 0;
            }
            else if ( AtNetSgee.overtime == 65532 ) /*������ͬ*/
            {
                GpsStatues.SgeeState = AGPS_IDLE;
                stu = 0;
            }
            else if ( AtNetSgee.overtime > 6000 ) /*10min��Ӧ��ʱ����ʧ��*/
            {
                SgeeReport ( 5 );
                WriteSGEEHavedate ( NOT_OK );
                GpsStatues.SgeeState = AGPS_IDLE;
                stu = 0;
            }
            else if ( ( AtNetSgee.Sgeedatecome == OK ) || ( ( AtNetSgee.time > 200 ) && ( NOT_OK == FlashBufRead ( &StuFram ) ) ) ) /*��Ӧ�𣬻��߳�ʱ�ط�*/
            {
                AtNetSgee.Sgeedatecome = NOT_OK;
                AtNetSgee.time = 0;
                /*ask a packeg*/
                AskSgeePacket();
            }

            break;

        default:
            stu = 0;
            break;
    }

}
/*
*********************************************************************************************************
*   �� �� ��u8  GsmSendAtCommand(u8 *AtCommend ,u16 len,u8* flag,u8 times,u16 waittime)
*   ����˵��: GSM����ATָ��
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/

u8  GsmSendAtCommand ( u8 *AtCommend, u16 len, u8 *flag, u8 times, u16 waittimeSecond )
{
    static u8 stu = 0, uwaketimes = 0, errtimes = 0;
    u8 tmp;

    switch ( stu )
    {
        case 0:

            //LEUART_Enable(LEUART0,leuartDisable);
            if ( GsmSta.SendDate != OK )
            {
                GPIO_PinModeSet ( GSM_TX_PORT, GSM_TX_PIN, gpioModePushPull, 0 );
            }


            DTR_ENABLE();
#ifndef CFUN_FIVE
            stu++;
#else
            stu = 3;
#endif
            uwaketimes = 0;
            return AT_WAIT;

        case 1:
            if ( GSM_SLEEP == READ_CTS() )
            {
                stu++;

                if ( errtimes++ > 5 )
                {
                    errtimes = 0;
                    stu = 0;
                    return AT_ERROR;
                }

                uwaketimes = 0;
            }
            else
            {
                RTS_READY_0();
                errtimes = 0;
                stu = 3; /*send at command*/
            }

            return AT_WAIT;

        case 2:
            if ( GSM_WAKE == READ_CTS() )
            {
                RTS_READY_0();
                uwaketimes = 0;
                stu = 3;
                return AT_WAIT;
            }

            if ( ( uwaketimes == 0 ) || ( uwaketimes == 12 ) || ( uwaketimes == 24 ) )
            {
                RTS_UNREADY_1();
            }
            else if ( ( uwaketimes == 6 ) || ( uwaketimes == 18 ) )
            {
                RTS_READY_0();
            }

            if ( uwaketimes++ > 30 )
            {
                RTS_READY_0();
                uwaketimes = 0;
                stu = 0;
                return AT_ERROR;
            }

            return AT_WAIT;

        case 3:/*send at command*/
            tmp = SendAtCommand ( AtCommend, len, flag, times, waittimeSecond, GsmUartSendStr );

            if ( tmp != AT_WAIT ) /*send at command end*/
            {


                stu = 0;
                STU_AtCommand.AtStu = 0;
                DTR_DISABLE();

                if ( GsmSta.SendDate != OK )
                {
                    /* Check that transmit buffer is empty */
                    if ( GSM_TX_PORT == gpioPortE )
                        while ( ! ( LEUART0->STATUS & LEUART_STATUS_TXBL ) );
                    else
                        while ( ! ( USART2->STATUS & USART_STATUS_TXBL ) );

                    GPIO_PinModeSet ( GSM_TX_PORT, GSM_TX_PIN, gpioModeInput, 0 );
                }

            }

            return tmp;

        default:
            stu = 0;
            return AT_ERROR;


    }




}
/*
*********************************************************************************************************
*   �� �� ��u8 GsmM2MAsk(void)
*   ����˵��: ����m2m��γ��
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 GsmM2MAsk ( void )
{
#define GSM_GET_M2M 100  // 20s

    static u8 inter = 250;
    //if( (GsmStatues.connect==CONNECT)&&(BVKstrGpsData.bValidity!='A')&&(GsmStore.SendGsmPosition==SEND_GSM_POSITION))
    /*������������ӵģ�gps����λ������ʱ�䵽*/


#if 1
    inter++ ;

    if ( ( GsmSta.askm2m == 1 ) && ( inter > GSM_GET_M2M ) )
    {
        inter = 0;
        GsmSta.askm2m = 0;

        return OK;
    }

#endif
    return NOT_OK;

}
/*
*********************************************************************************************************
*   �� �� �� u8 MatchApn(u8*oper,u8 len)
*   ����˵��: ƥ��APN
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8 MatchApn ( u8 *oper, u8 len )
{
    u16 i;

    for ( i = 0; i < MAX_APN; i++ )
    {
        if ( OK == Equre ( StuApn[i].oper, oper, len ) )
        {
            GsmSta.operturn = i;
            return OK;
        }

    }

    return NOT_OK;

}
/*
*********************************************************************************************************
*   �� �� ��u8 ATSend(AT_STRUCT AtCommands)
*   ����˵��: GSM����AT
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
//#pragma optimize=none
u8 ATSend ( u8 AtTurn )
{
    u8 i;
    u8 *p, *p1; //,*p2;
    u8 j, k;
    u32 t32mp;
    u8 buf2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static u8 stu = 0;
    static u16 lastPos = 0xFFFF;

    if ( stu == 0 )
    {
        stu = 1;

        if ( ( AtTurn == 48 ) || ( AtTurn == 7 ) || ( AtTurn == 20 ) || ( AtTurn == 21 ) ||
             ( AtTurn == 22 ) || ( AtTurn == 24 ) || ( AtTurn == 30 ) || ( AtTurn == 31 ) ||
             ( AtTurn == 32 ) || ( AtTurn == 34 ) || ( AtTurn == 5 ) )
        {
            InitGsmQueue();
        }

    }

#if 0

    if ( Ue910Mode )
    {
        if ( AtTurn == 13 )
        {
            AtTurn = 1;
        }
        else if ( AtTurn == 6 )
        {
            AtTurn = 36;
        }
    }

#endif

    if ( AtTurn != 42 )
    {
        i = GsmSendAtCommand ( AtCommands[AtTurn].AtCommend,
                               AtCommands[AtTurn].len,
                               AtCommands[AtTurn].flag,
                               AtCommands[AtTurn].times,
                               AtCommands[AtTurn].waittimeSecond );
    }
    else
    {
        i = GsmSendAtCommand ( GsmSto.sosPhone[sosIndex >> 1],
                               GsmSto.sosPhoneLen[sosIndex >> 1],
                               AtCommands[42].flag,
                               AtCommands[42].times,
                               AtCommands[42].waittimeSecond );
    }

    if ( ( stu != 0 ) && ( i != AT_WAIT ) )
    {
        stu = 0;
    }

    if ( i == AT_OK )
    {
        GsmSta.resettime = 0;
        p = (u8 *)&STU_AtCommand.rev;
        j = 0;

        while( j++ < 128 )
        {
            if(   *p++ >= 0x20 )
            {
                break;
            }
        }

        myprintf ( "[%x-%x %x:%x:%x]>3G(%d):%s",
                   timer.time[1], timer.time[2],
                   timer.time[3], timer.time[4], timer.time[5], AtTurn, p);

        if ( 21 == AtTurn ) //CREG
        {

            if ( ( 0 != Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, ",1", 2 ) ) || ( 0 != Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, ",5", 2 ) ) )
            {
                GsmSta.CREG = OK;
            }
            else
            {
                GsmSta.CREG = NOT_OK;
            }
        }
        else if ( 22 == AtTurn ) //CGREG
        {

            if ( ( 0 != Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, ",1", 2 ) ) || ( 0 != Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, ",5", 2 ) ) )
            {
                GsmSta.CGREG = OK;
            }
            else
            {
                GsmSta.CGREG = NOT_OK;
            }
        }
        else if ( 24 == AtTurn ) //cmcc
        {

            if ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "#SERVINFO:", 10 ) ) )
            {
                //AT#SERVINFO
                //#SERVINFO: 122,-74,"China Unicom","46001",77,2539,01,1,,"II",02,6
                //#SERVINFO: 111,-81,"CHN-UNICOM","46001",20,2539,00,1,,"II",02,6
                p = Findbcd ( ( u8 * ) &p[10], 50, ",\"", 2 );
                p = p + 2;

                if ( p != NULL )
                {
                    p1 = Findbcd ( ( u8 * ) p, 50, "\",\"", 3 );
                    p1 = p1 - 1;
                    GsmSta.opeabuflen = p1 - p + 1;

                    if ( GsmSta.opeabuflen > OPEAR_MAX_LEN )
                    {

#if 0
                        GsmSta.opeabuflen = 0;
                        p2 = p;

                        do
                        {
                            p2 = Findbcd ( ( u8 * ) &p2[1], i, " 1", 1 );

                            if ( p2 != NULL )
                            {
                                if ( GsmSta.opeabuflen == 0 )
                                {
                                    buf2[GsmSta.opeabuflen++] = p[0];
                                }
                                else
                                {
                                    buf2[GsmSta.opeabuflen++] = p2[1];
                                }

                            }
                            else
                            {
                                break;

                            }
                        }
                        while ( GsmSta.opeabuflen < OPEAR_MAX_LEN );


                        if ( GsmSta.opeabuflen == 0 )
                        {
                            GsmSta.opeabuflen = OPEAR_MAX_LEN;

                        }

#else
                        GsmSta.opeabuflen = OPEAR_MAX_LEN;
#endif

                    }

                    Mymemcpy ( GsmSta.opeabuf, p, GsmSta.opeabuflen );
                    GsmSta.CSQ++;
                }

                p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "#SERVINFO:", 10 );
                p = Findbcd ( ( u8 * ) &p[10], 50, "\",\"", 3 );

                if ( p != NULL )
                {
                    p1 = Findbcd ( ( u8 * ) &p[3], 80, "\",", 2 );

                    if ( p1 != NULL )
                    {
                        t32mp = p1 - p - 3;

                        if ( t32mp ) /*��ʾ�ҵ�����Ӫ��*/
                        {

                            if ( OK != MatchApn ( &p[3], p1 - p - 3 ) )
                            {
                                GsmSta.operturn = NOT_MATCH;
                            }

                        }

                    }
                }

                p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "\",\"", sizeof ( "\",\"" ) - 1 );

                //  p=  strstr( &p1[0], "\",\"") ;//","46001",53,2539,00,1,,"II",02,6       Mobile","46000",21,2482,01,1,,"II",01,6
                if ( p == NULL )
                {
                    return i;
                }

                t32mp = ( u32 ) strtol ( ( char * ) &p[3], NULL, 10 );
                GsmSta.MNC = t32mp % 100; //(u8)(t32mp&0x000000ff);//(t32mp&10)| (((t32mp%100/10)<<4 )&0x00f0);//�ն���Ӫ�̱�ʶ��MNC��
                // t32mp/=100;
                GsmSta.MCC = t32mp / 100; //(u16)((t32mp>>8)&0x0000ffff);//(t32mp&10)| (((t32mp%100/10)<<4 )&0x00f0)| (((t32mp%1000/100)<<8 )&0x0f00)| (((t32mp/1000)<<12 )&0xf000);;//���ұ�ʶ��MCC��


            }

        }
        else if ( 7 == AtTurn ) //qss
        {
            if ( 0 != Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, ",1", 2 ) )
            {
                GsmSta.QSS = OK;

            }
            else
            {
                GsmSta.QSS = NOT_OK;

            }



        }
        else if ( 20 == AtTurn ) //CSQ
        {
            if ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "+CSQ: ", 6 ) ) )
            {
                GsmSta.CSQ = ( u8 ) strtol ( ( char * ) &p[5], NULL, 10 );

                if ( GsmSta.CSQ == 99 )
                {
                    GsmSta.CSQ = 0;
                }

                myprintf ( "CSQ:%d\r\n", GsmSta.CSQ );

            }
        }
        else if ( 30 == AtTurn )            //Si
        {
            if ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "#SI", 3 ) ) )
            {

                p = Findbcd ( &p[7], MaxRevLen, ", ", 1 ); //#SI: 1,12,

                if ( p != NULL )
                {
                    p = Findbcd ( &p[1], MaxRevLen, ", ", 1 ); //#SI: 1,12,0,

                    if ( p != NULL )
                    {
                        if ( !GsmSta.UnRevLen )
                        {
                            GsmSta.UnRevLen = ( u16 ) strtol ( ( char * ) &p[1], NULL, 10 );
                        }

                        p = Findbcd ( &p[1], MaxRevLen, ", ", 1 ); //#SI: 1,12,0,0,

                        if ( p != NULL )
                        {
                            GsmSta.UnSendLen = ( u16 ) strtol ( ( char * ) &p[1], NULL, 10 );

                        }

                    }

                }

            }

        }
        else if ( 31 == AtTurn ) //SS
        {
            if ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "#SS: ", 5 ) ) ) //#SS 1,    #ss 2,
            {


                GsmSta.SocketState = p[7] - 0x30;

            }

        }
        else if ( 32 == AtTurn )    //SS
        {

            if ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "+CMGL", 5 ) ) ) //+CMGL: 2,"REC READ","10086","","12/08/17,18:48:43+32"
            {

                GsmSta.msgs = ( u16 ) strtol ( ( char * ) &p[7], NULL, 10 );

            }

        }
        else if ( 5 == AtTurn ) /*#CGSN: 357322045735252*/
        {
            if ( 0 != ( p = Findbcd ( ( u8 * ) STU_AtCommand.rev, MaxRevLen, "#CGSN: ", 7 ) ) )
            {
                p1 = Findbcd ( &p[7], 100,  "\r\n", 2 ); //#SI: 1,12,0,0,
                p = &p[7];
                j = p1 - p;

                if ( j > 18 )
                {
                    j = 18;
                }

                Memset ( GsmSta.IMEI, 0, 9 );

                for ( k = 0; k < ( j / 2 ); k++ )
                {
                    GsmSta.IMEI[8 - k] = ( ( acstohex ( * ( p1 - 2 - 2 * k ) ) << 4 ) & 0xf0 ) | ( acstohex ( * ( p1 - 1 - 2 * k ) ) & 0x0f );
                }

                if ( j % 2 )
                {
                    GsmSta.IMEI[8 - k] = acstohex ( * ( p1 - 1 - 2 * k ) ) & 0x0f;
                }


                if ( OK == Equre ( GsmSto.ID, buf2, 10 ) )
                {

                    Mymemcpy ( &GsmSto.ID[1], GsmSta.IMEI, 9 );
                    WriteGsmStoreDate();
                }

            }
        }
        else if ( 43 == AtTurn ) /* Get the phone activity staus */
        {
            if ( 0xFFFF != ( t32mp = FindString ( &GsmRxQueueCtrl, "+CLCC: ", 7 ) ) )
            {
                t32mp = t32mp + 11;

                if ( t32mp >= MaxRevLen )
                {
                    t32mp = t32mp - MaxRevLen;
                }

                if ( lastPos != t32mp )
                {
                    sosStaus = ( ( u8 * ) STU_AtCommand.rev ) [t32mp] - 0x30;
                    lastPos = t32mp;
                }
            }
        }

        else if ( 48 == AtTurn )
        {
            GsmSta.operator_flag = 0;

            if ( 0xFFFF != ( t32mp = FindString ( &GsmRxQueueCtrl, "+COPS: ", 7 ) ) )
            {
                GsmSta.station_count_3g = 0;
                GsmSta.station_count_gsm = 0;
                p1 = (u8 *)&STU_AtCommand.rev;

                for( j = 0; j < 3; j++ )
                {
                    p = strstr ( (char *)p1, "46001" );

                    if ( NULL != p )
                    {
                        GsmSta.operator_flag |= 1;
                        p += 7;

                        if(  *p - 0x30  == 2 )
                        {
                            GsmSta.station_count_3g += 1;
                        }
                        else  if(  *p - 0x30 == 0 )
                        {
                            GsmSta.station_count_gsm += 1;
                        }

                        p1 = p;
                    }
                    else
                    {
                        break;
                    }
                }

                //      if ( 0xFFFF != ( t32mp = FindString ( &GsmRxQueueCtrl, "46001", 5 ) ) )
                //      {
                //          GsmSta.operator_flag |= 1;
                //      }

                if ( 0xFFFF != ( t32mp = FindString ( &GsmRxQueueCtrl, "46000", 5 ) ) )
                {
                    GsmSta.operator_flag |= 2;
                }

                if ( 0xFFFF != ( t32mp = FindString ( &GsmRxQueueCtrl, "45403", 5 ) ) )
                {
                    GsmSta.operator_flag |= 4;
                }

                if ( 0xFFFF != ( t32mp = FindString ( &GsmRxQueueCtrl, "45406", 5 ) ) )
                {
                    GsmSta.operator_flag |= 8;
                }

                if ( 0xFFFF != ( t32mp = FindString ( &GsmRxQueueCtrl, "45413", 5 ) ) )
                {
                    GsmSta.operator_flag |= 16;
                }
            }

#ifdef USE_PRINTF
            myprintf ( "current GsmSta.operator_flag is %d\r\n", GsmSta.operator_flag );
#endif
        }

        /*       else if ( 50 == AtTurn )
               {
                   /// FatQ
                   /// AT#CCLK?
                   // #CCLK:"2/09/07,22:30:25+04,1"   "yy/MM/dd,hh:mm:ss+TZ,DST
                   p = strstr ( ( char* ) STU_AtCommand.rev, "#CCLK:" );

                   if ( NULL != p )
                   {
                       //buf2
                       p1 = ( u8* ) strstr ( ( char* ) p, "\"" );

                   }
               }
        */
    }

    return i;
}

/*
*********************************************************************************************************
*   �� �� ��void InitGsmdate(void)
*   ����˵��: GSM������ʼ��?
*               ����ǰGsmSta.PowerResetStuҪΪ0   500ms����һ��
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: PowerOK��ʾ��λ�ɹ�
*********************************************************************************************************
*/
void InitGsmdate ( void )
{
    u8 i;
    GsmSta.IpOneConnect = NOT_OK;
    RTS_READY_0();
    GsmSta.QSS = NOT_OK;
    GsmSta.CGREG = NOT_OK;
    GsmSta.CSQ = NOT_OK;
    GsmSta.CREG = NOT_OK;
    GsmSta.RevLen = 0;
    GsmSta.DateCome = NOT_OK;
    GsmSta.PhoneCome = NOT_OK;
    GsmSta.MessageCome = OK;
    GsmSta.Battery = 100;
    STU_AtCommand.AtState = AT_STATE_IDLE;
    GsmSta.BasicKeepAlive = timer.counter + 1000;
    GsmSta.BasicPositionInter = timer.counter;

    for ( i = 0; i < MESSAGE_NUMBERS; i++ )
    {
        GsmSta.messagelen[i] = 0;
    }

}
/*
*********************************************************************************************************
*   �� �� ��void GsmInit(void)
*   ����˵��: GSM�������ݽ���
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void GsmInit ( void )
{
    /*
    D1---onoff
    D2:RST
    D3:RING
    D4:tx lu0
    D5:rx Lu0
    D6:DTR
    D7:RTS
    D8:CTS
    */


    InitGsmQueue();/*���г�ʼ��*/

    GPIO_PinModeSet ( RST_PORT, RST_PIN, gpioModePushPull, 0 );
    GPIO_PinModeSet ( CTS_PORT, CTS_PIN, gpioModeInputPull, 1 );
    GPIO_PinModeSet ( DTR_PORT, DTR_PIN, gpioModePushPull, 0 );
    GPIO_PinModeSet ( RTS_PORT, RTS_PIN, gpioModePushPull, 0 );

#ifndef GSM_USE_UART

    initLeuart ( LEUART0, MODEM_IPR, POWER_ON );
#else
    uartSetup ( USART2, 38400, POWER_ON );
#endif
    InitGsmdate();




}

/*
*********************************************************************************************************
*   �� �� ��void GsmReset(void)
*   ����˵��: GSM�������ݽ���
*               ����ǰGsmSta.PowerResetStuҪΪ0   500ms����һ��
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: PowerOK��ʾ��λ�ɹ�
*********************************************************************************************************
*/
//#pragma optimize=none
u8  GsmReset ( void )
{

    if ( GsmSta.PowerResetStu == 0x00 )
    {
        DTR_ENABLE();
        RTS_READY_0();

    }
    else if ( GsmSta.PowerResetStu == 20 )
    {
        GSM_RESET_ON();
#ifdef USE_PRINTF

        if ( debug == DEBUGGSM )
        {
            myprintf ( "reset PIN ON %d\r\n", GsmSta.PowerResetStu );
        }

#endif
    }
    else if ( GsmSta.PowerResetStu == 45 )
    {
        GSM_RESET_OFF();
#ifdef USE_PRINTF

        if ( debug == DEBUGGSM )
        {
            myprintf ( "reset PIN OFF%d\r\n", GsmSta.PowerResetStu );
        }

#endif
    }
    else if ( GsmSta.PowerResetStu > 104 )
    {

#ifdef USE_PRINTF

        if ( debug == DEBUGGSM )
        {
            myprintf ( "reset PIN ON %d\r\n", GsmSta.PowerResetStu );
        }

#endif
        GsmSta.PowerResetStu = 0;
        return PowerOK;
    }


    GsmSta.PowerResetStu++;
    return PowerWait;


}
/*
*********************************************************************************************************
*   �� �� ��u8  PowerOn(void)
*   ����˵��: gsm����
*       ��    �� ��liupeng
*   ��    �Σ�  ����ǰGsmSta.PowerResetStuҪΪ0   500ms����һ��
*       ��    ����version 1.0
*   �� �� ֵ:  PowerOK��ʾ�����ɹ�
*********************************************************************************************************
*/
#if 0

������ĳ���������Ż���
�ں���ǰ����ϣ�
#pragma optimize=none
����
_Pragma ( "optimize=none" )
#endif
#if 0
//#pragma optimize=none
u8  PowerOn ( void )
{

    if ( GsmSta.PowerResetStu == 0x00 )
    {
        DTR_ENABLE();
        RTS_READY_0();
        GSM_POWER_ON();

        if ( debug == DEBUGGSM )
        {
            printf ( "POWER PIN ON %d\r\n", GsmSta.PowerResetStu );
        }

    }
    else if ( ( ( !Ue910Mode ) && ( GsmSta.PowerResetStu == 20 ) ) ||
              ( ( Ue910Mode ) && ( GsmSta.PowerResetStu == 80 ) )
            )
    {
        GSM_POWER_OFF();

        if ( debug == DEBUGGSM )
        {
            printf ( "POWER PIN OFF %d\r\n", GsmSta.PowerResetStu );
        }
    }
    else if ( ( ( !Ue910Mode ) && ( GsmSta.PowerResetStu > 120 ) ) ||
              ( ( Ue910Mode ) && ( GsmSta.PowerResetStu > 160 ) )
            )
    {

        if ( debug == DEBUGGSM )
        {
            printf ( "POWER PIN RETURN %d\r\n", GsmSta.PowerResetStu );
        }

        GsmSta.PowerResetStu = 0;
        return PowerOK;

    }


    GsmSta.PowerResetStu++;
    return PowerWait;


}

/*
*********************************************************************************************************
*   �� �� ��u8  Ue910ShutDown(void)
*   ����˵��: UE910�ػ�
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ:
*********************************************************************************************************
*/

//#pragma optimize=none
u8  Ue910ShutDown ( void )
{

    if ( GsmSta.PowerResetStu == 0x00 )
    {
        DTR_ENABLE();
        RTS_READY_0();
        GSM_SHUTDOWN_ON();

        if ( debug == DEBUGGSM )
        {
            printf ( "shutdown PIN ON %d\r\n", GsmSta.PowerResetStu );
        }

    }
    else if ( GsmSta.PowerResetStu == 20 )

    {
        GSM_SHUTDOWN_OFF();

        if ( debug == DEBUGGSM )
        {
            printf ( "shutdown PIN OFF %d\r\n", GsmSta.PowerResetStu );
        }
    }
    else if ( GsmSta.PowerResetStu == 60 )
    {

        if ( debug == DEBUGGSM )
        {
            printf ( "shutdown PIN return %d\r\n", GsmSta.PowerResetStu );
        }

        GsmSta.PowerResetStu = 0;
        return PowerOK;

    }


    GsmSta.PowerResetStu++;
    return PowerWait;


}
#endif
/*
*********************************************************************************************************
*   �� �� ��:void void LEUART0_IRQHandler(void)
*   ����˵��: luart0�жϽ���
*
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LEUART0_IRQHandler ( void )
{
    u8 RxData;
    u32 leuart_IntFlag;

    leuart_IntFlag = LEUART_IntGet ( LEUART0 );

    if ( leuart_IntFlag & LEUART_IF_RXDATAV )
    {
        RxData = LEUART0->RXDATA;
        RevGsmQueue ( RxData );

    }

    LEUART_IntClear ( LEUART0, ~_LEUART_IFC_RESETVALUE );

}

void USART2_RX_IRQHandler ( void )
{
    // u8 err;

    /* Check for RX data valid interrupt */
    if ( USART2->STATUS & USART_STATUS_RXDATAV )
    {
        /* Copy data into RX Buffer */
        uint8_t rxData = USART_Rx ( USART2 );

        RevGsmQueue ( rxData );

        /* Clear RXDATAV interrupt */
        USART_IntClear ( USART2, USART_IF_RXDATAV );

    }
}





/*
*********************************************************************************************************
*   void ReadGsmStoreDate(void)
*   ����˵��: ��gsmflash����
*       ���У�鲻�ԣ��������е����ݣ�Ȼ���ٰѱ����е�����д��������
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void ReadGsmStoreDate ( void )
{
    // volatile static u16 len;
    // len=sizeof(GsmSto);
    //WriteGsmStoreDateToDefault();
    memset ( &GsmSto, 0, sizeof ( GsmSto ) );
    FLASH_ReadDate ( GsmPara, 1024, ( u8 * ) &GsmSto );
  //   WriteGsmStoreDateToDefault();

    if ( ( GsmSto.varity != CalacXORVarity ( ( u8 * ) &GsmSto, 1023 ) ) || ( GsmSto.first != 0xaa ) )
    {
        //#ifdef USE_PRINTF
        myprintf ( "GSM����У��Ͳ���" );
        //#endif
        /*У��Ͳ��ԣ���������*/
        FLASH_ReadDate ( GsmParaBvk, 1024, ( u8 * ) &GsmSto );

        if ( ( GsmSto.varity != CalacXORVarity ( ( u8 * ) &GsmSto, 1023 ) ) || ( GsmSto.first != 0xaa ) )
        {
            /*У��Ͳ��ԣ���������*/
            WriteGsmStoreDateToDefault();
            //#ifdef USE_PRINTF
            myprintf ( "GSM������������У���Ҳ����,�����ظ�����������\r\n" );
            //#endif

        }
        else
        {
            GsmSto.varity = CalacXORVarity ( ( u8 * ) &GsmSto, 1023 );
            FLASH_eraseOneBlock ( GsmPara );
            FLASH_WriteDate ( GsmPara, 1024, ( u8 * ) &GsmSto );
            FLASH_eraseOneBlock ( GsmParaBvk );
            FLASH_WriteDate ( GsmParaBvk, 1024, ( u8 * ) &GsmSto );
        }

    }

#if 0
    memcpy ( &GsmSto.sosPhone[0], "ATD+8613714338642;\r", 20 );
    GsmSto.sosPhoneLen[0] = 19;
    memcpy ( &GsmSto.sosPhone[1], "ATD+8613682577602;\r", 20 );
    GsmSto.sosPhoneLen[1] = 19;
    memcpy ( &GsmSto.sosPhone[2], "ATD+8613826589923;\r", 20 );
    GsmSto.sosPhoneLen[2] = 19;
#endif

    GsmSto.lcdTemperature[0] = 0;
    GsmSto.lcdTemperature[1] = 25;
    GsmSto.lcdWeather[0] = '0';
    GsmSto.lcdWeather[1] = '1';
    GsmSto.lcdWeather[2] = 'd';
}

/*
*********************************************************************************************************
*   void ReadGsmStoreDate(void)
*   ����˵��: ��gsmflash����
*       ���У�鲻�ԣ��������е����ݣ�Ȼ���ٰѱ����е�����д��������
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void WriteGsmStoreDate ( void )
{
    //GsmPara
#if 0
    u8 flag = 0;
    FLASH_ReadDate ( GsmPara, 1024, ( u8 * ) &GsmSto );

    if ( ( GsmSto.varity != CalacXORVarity ( ( u8 * ) &GsmSto, 1023 ) ) || ( GsmSto.first != 0xaa ) )
    {
        flag = 0xaa;
    }

#endif
    FLASH_ReadDate ( GsmPara, 1024, ( u8 * ) &PbulicBuf );

    if ( ( NOT_OK == Equre ( PbulicBuf, ( u8 * ) &GsmSto, 1023 ) ) ||
         ( GsmSto.varity != CalacXORVarity ( ( u8 * ) &PbulicBuf, 1023 ) )
       )
    {
        GsmSto.varity = CalacXORVarity ( ( u8 * ) &GsmSto, 1023 );
        FLASH_eraseOneBlock ( GsmPara );
        FLASH_WriteDate ( GsmPara, 1024, ( u8 * ) &GsmSto );
        FLASH_eraseOneBlock ( GsmParaBvk );
        FLASH_WriteDate ( GsmParaBvk, 1024, ( u8 * ) &GsmSto );
    }

}





/*
*********************************************************************************************************
*   void WriteGsmStoreDateToDefault(void)
*   ����˵��: GSM��������������

*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void WriteGsmStoreDateToDefault ( void )
{
#if 1

    //const u8 ip[]="shangqiaotang.vicp.cc";
    //
    //const u8 apn[]="cmhk";
    //const u8 apn[]="3gnet";

#else
    //const u8 ip[]="gps.amr333.com";
    const u8 ip[] = "120.25.226.4";
    const u8 apn[] = "unim2m.gzm2mapn";
    const u8  port[] = "9876";

#endif

    //const u8  port[]="6789";
    //
#if 1
    const u16 MoverInterGPS = 900;
    const u16 MoverInterGSM = 900;
    const u16 staticinterval = 900;
    const u16 KeepAliveInter = 300;
#else
    const u16 MoverInterGPS = 900;
    const u16 MoverInterGSM = 900;
    const u16 staticinterval = 900;
    const u16 KeepAliveInter = 900;
#endif
    const u8 BatteryQuatity = 30;
    const u8 NetId = EMPTY_MODE;
    const u8 ID[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const u8 updateflag = NOT_OK;
    const u8 mima[6] = "123456";


    Mymemcpy ( GsmSto.mima, ( u8 * ) mima, 6 );


    GsmSto.iplen = sizeof ( GSM_DEFAULT_URL ) - 1;
    memset ( GsmSto.strip, 0, sizeof(GsmSto.strip) );
    memcpy ( GsmSto.strip, GSM_DEFAULT_URL,  sizeof ( GSM_DEFAULT_URL ) /*GsmSto.iplen*/ );


    GsmSto.apnlen = sizeof ( GSM_DEFAULT_APN ) - 1;
    memset ( GsmSto.strapn, 0, sizeof(GsmSto.strapn) );
    memcpy ( GsmSto.strapn, GSM_DEFAULT_APN, sizeof ( GSM_DEFAULT_APN ) /*GsmSto.apnlen*/ );

    GsmSto.portlen = sizeof ( GSM_DEFAULT_PORT ) - 1;
    memset ( GsmSto.port, 0, GsmSto.portlen + 1 );
    memcpy ( GsmSto.port, GSM_DEFAULT_PORT,  sizeof ( GSM_DEFAULT_PORT )  /*GsmSto.portlen*/ );
    //   Mymemcpy ( GsmSto.ID, ( u8* ) ID, 10 );
    memset ( GsmSto.ID, 0, sizeof ( GsmSto.ID ) );

    GsmSto.moveintervalGPS = MoverInterGPS;
    GsmSto.moveintervalGSM = MoverInterGSM;
    GsmSto.staticinterval = staticinterval;
    GsmSto.KeepAliveInter = KeepAliveInter;
    GsmSto.BatteryQuatity = BatteryQuatity;
    GsmSto.NetId = NetId;
    GsmSto.NetIdNameLen = 0;
    GsmSto.NetIdPsswdLen = 0;
    GsmSto.first = 0xaa;
    GsmSto.updateflag = updateflag;
    GsmSto.MessageCenterLen = 0;
    GsmSto.Timehour = 8;
    GsmSto.Timeminute = 0;
    GsmSto.Is12h = 1;

    GsmSto.lcdTemperature[0] = 0;
    GsmSto.lcdTemperature[1] = 25;
    GsmSto.lcdWeather[0] = '0';
    GsmSto.lcdWeather[1] = '1';
    GsmSto.lcdWeather[2] = 'd';
    WriteGsmStoreDate();



}
/*
*********************************************************************************************************
*   void PackDateToBuf(u8 *indata,u16 inlen,u8 *outbuf,u16*outlen)
*   ����˵��: GSM���ݴ��

*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ:
*********************************************************************************************************
*/
void PackDateToBuf ( u8 *indata, u16 inlen, u8 *outbuf, u16 *outlen )
{
    u16 len = 0, i, k, j;
    U16STU t16mp;
    u8 tmp[50];/**/
    u8 varity = 0;
    static u8 waterid;


    outbuf[len++] = 0xaa;
    //id
    Mymemcpy ( &tmp[0], GsmSto.ID, 10 );

    for ( i = 0; i < 10; i++ )
    {
        varity ^= tmp[i];
    }

    Conver_aa_to_55 ( tmp, 10, &outbuf[len], &i );
    len += i;

    GsmSta.SendCmd[0] = indata[0];
    GsmSta.SendCmd[1] = indata[1];
    GsmSta.SendId = waterid;
    //��ˮ��
    tmp[0] = waterid++;

    //��Ϣ�峤��
    t16mp.total = inlen;
    tmp[1] = t16mp.stu.H;
    tmp[2] = t16mp.stu.L;

    for ( i = 0; i < 3; i++ )
    {
        varity ^= tmp[i];
    }

    Conver_aa_to_55 ( tmp, 3, &outbuf[len], &i );
    len += i;

    //��Ϣ��
    for ( i = 0; i < inlen; i++ )
    {
        varity ^= indata[i];
    }

    i = inlen / 10;
    k = 0;

    if ( i > 0 )
    {
        for ( ; k < i; k++ )
        {
            Conver_aa_to_55 ( &indata[k * 10], 10, &outbuf[len], &j );
            len += j;
        }
    }

    i = inlen % 10;

    if ( i )
    {
        Conver_aa_to_55 ( &indata[k * 10], i, &outbuf[len], &j );
        len += j;
    }

    //����У����
    Conver_aa_to_55 ( &varity, 1, &outbuf[len], &j );
    len += j;
    outbuf[len++] = 0xaa;

    *outlen = len;
}
/*
*********************************************************************************************************
*   void ReadNetDate(void)
*   ����˵��: GSM����������
Mulity Ϊok��ʾ���������
*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ:
*********************************************************************************************************
*/

void ReadNetDate ( u8 Mulity, STRUCT_BLIND_FRAM *StuFram )
{
    u8 i;
    u16  len;
    u16 buflentmp, buflentmp2;
    u8 *p;

    //u8  FlashBufRead(StuFram);
    //u8  FlashBufDelete(void);
    //u8  FlashBufWrite(STRUCT_BLIND_FRAM StuFram,u8 Important);
    do
    {
        i = FlashBufRead ( StuFram );

        if ( i == NOT_OK )
        {
            return  ;
        }

        if ( Mulity )
        {
            if ( GsmSta.SendingLen > 800 )
            {
                return ;

            }

        }

        len = StuFram->len;

        /*��Ϣ���װ��*/
        if ( ( len == 0 ) || ( len > ( A_FRAM_LEN - 1 ) ) )
        {

            //aa lh ll data0 data1...............
            if ( len == 0xaa )
            {
                p = &BlindStu.buflen[0];
                /*��Ϣ���װ��*/
                buflentmp2 = ( ( ( ( u16 ) p[1] ) << 8 ) & 0xff00 ) | ( ( ( u16 ) p[2] ) & 0x00ff );

                PackDateToBuf ( &p[3], buflentmp2, &GsmSta.SendingBuf[GsmSta.SendingLen], &buflentmp );
                FlashBufDelete();
                buflentmp2 -= 37;

                do
                {

                    if ( buflentmp2 >= 40 )
                    {
                        buflentmp2 -= 40;
                    }
                    else
                    {
                        buflentmp2 = 0;
                    }

                    FlashBufDelete();
                }
                while ( buflentmp2 );

                GsmSta.SendingLen += buflentmp;

                if ( Mulity != OK )
                {
                    return ;
                }


            }
            else
            {
                FlashBufDelete();
            }
        }
        else
        {

            PackDateToBuf ( StuFram->buflen, len, &GsmSta.SendingBuf[GsmSta.SendingLen], &buflentmp );
            FlashBufDelete();
            GsmSta.SendingLen +=    buflentmp;

            if ( Mulity != OK )
            {
                return ;
            }
        }


    }
    while ( Mulity == OK );


}


/*
*********************************************************************************************************
*   u8  GetGsmBufData(u8 *outbuf,u16 *outlen)
*   ����˵��: GSM�ӻ����л�ȡ����

*       ��    �� ��liupeng
*   ��    �Σ�
*       ��    ����version 1.0
*   �� �� ֵ: ��
*********************************************************************************************************
*/
u8  GetGsmBufData ( void )
{
#define min2      1200
#define SEC10   100
    static u8 stu = 0;
    static u16 WaitTime = 0, WaitTime2 = 0;
    STRUCT_BLIND_FRAM StuFram;
    u8 i;


    if ( WaitTime )
    {
        WaitTime--;
    }

    if ( WaitTime2 )
    {
        WaitTime2--;
    }

    switch ( stu )
    {

        case 0:
            if ( ( OK == FlashBufRead ( &StuFram ) ) || ( GsmSta.SendingLen ) )
            {
                WaitTime = min2;
                GsmSta.NetAck = NOT_OK;
                stu++;
            }

            return NOT_OK;

        case 1:
            if ( GsmSta.NetAck == OK )
            {
                GsmSta.SendingLen = 0;
                stu = 5;
            }
            else  if ( 0 == WaitTime2 )
            {
                stu++;
            }

            return NOT_OK;

        case 2:
            i = ATSend ( 30 ); //SI

            if ( ( i == AT_OK ) || ( i == AT_ERROR ) )
            {
                stu++;
            }

            return NOT_OK;

        case 3:
            if ( GsmSta.UnSendLen )
            {
                GsmSta.SocketState = 1;
                i = ATSend ( 31 ); //SS

                if ( ( i == AT_OK ) || ( i == AT_ERROR ) )
                {
                    stu++;
                }
            }
            else
            {
                stu++;
            }

            return NOT_OK;

        case 4:/*�����ʱ�Ͷ���,���socket �Ͽ�������,����������(buf�գ�����������ack)����������з���*/
            if ( ( GsmSta.UnSendLen == 0 ) || ( GsmSta.NetAck == OK ) )
            {
                stu++;
                GsmSta.SendingLen = 0;

                if ( GsmSta.updatetime20sec != 255 )
                {
                    GsmSta.updatetime20sec = 0;
                }
            }
            else if ( ( !WaitTime ) || ( GsmSta.SocketState == 0 ) ) /*���糬ʱsocket close*/
            {
                GsmSta.IpOneConnect = NOT_OK;
                stu = 0;
            }
            else if ( GsmSta.SendingLen )
            {
                if ( GsmSto.updateflag == OK )
                {
                    WaitTime2 = 30;
                }
                else
                {
                    WaitTime2 = SEC10;
                }

                stu = 1;
            }

            return NOT_OK;

        case 5:
            if ( GsmSta.SendingLen > SENDING_BUF_LEN )
            {
                GsmSta.SendingLen = 0;
            }

            if ( GsmSta.SendingLen == 0 ) /*����������*/
            {

                ReadNetDate ( OK, &StuFram );

            }

            if ( GsmSta.SendingLen )
            {
                GsmSta.NetAck = NOT_OK;
                WaitTime = min2;

                if ( GsmSto.updateflag == OK )
                {
                    WaitTime2 = 30;
                }
                else
                {
                    WaitTime2 = SEC10;
                }

                stu = 1;
                return OK;
            }

            return NOT_OK;

        default:
            stu = 0;
            return NOT_OK;
    }

}




/*
*********************************************************************************************************
*   �� �� ��: void GsmGetStatues(u8 datain,void(*revnetfunction)(u8 indate))
*   ����˵��:��ö��ŵ绰 �������ѣ�agps��γ��
*
*
*   ��    �Σ�

*   �� �� ֵ: ��
*********************************************************************************************************
*/
void GsmGetStatues ( u8 datain, void ( *revnetfunction ) ( u8 indate ) )
{
    static u8 stu[7] = {0, 0, 0, 0, 0, 0, 0};  //  {5, 0, 0, 0, 0, 0, 0};
    const u8 cpare[7][5] = {"+CLIP", "SRING", "#AGPS", "+CMTI", "ERROR", "NO CA", "SRECV"};
    double dtmp;
    static u8 datebefore;
    u8 i, j;
    u8 *p;

#ifdef  UE866_OPERATE

    for(i = 0; i < 7; i++)
    {
        if(  stu[i] >= 5  )
        {
            stu[i] ++;
            switch(i)
            {
                case 0:// +CLIP
                    if (  datain == 0x0d )
                    {
                        stu[i] = 0;
                    }

                    return;

                case 1:// SRING: 1,8760

                    /// ������Ҫ����
                    
                    if ( stu[i] >= 9  )
                    {
                        if( datain >= 0x30 &&  datain <= 0x39 )
                        {
                            GsmSta.RevLen *= 10;
                            GsmSta.RevLen += ( datain - 0x30 );
                        }
                        else if( 0x0d == datain )
                        {
                            if ( GsmSta.RevLen > 4096 )
                            {
                                GsmSta.RevLen = 4096;
                            }

                            stu[i] = 0;
                            GsmSta.DateCome = OK;
                        }

                    }

                    return;

                case 2:// #AGPS

                    /*#AGPSRING: 200,22.568489,113.864426,   #AGPSRING: 0,,,,,"",0   19--33*/
                    if (  datain == 0x0d )
                    {
                        stu[i] = 0;
                    }

                    return;

                case 3:// +CMTI
                    if (  datain == 0x0d )
                    {
                        stu[i] = 0;

                    }

                    return;

                case 4:// ERROR
                    if (  datain == 0x0d )
                    {
                        stu[i] = 0;
                    }

                    return;

                case 5://  NO CA

                    if (  datain == 0x0d )
                    {
                        stu[i] = 0;
                    }

                    return ;

                case 6://  SRECV
                    //#SRECV: 1,23
                    if( 8 == stu[i] )
                    {
                        // ��ȡ�ĸ�socket��������
                        datebefore = ',';
                    }
                    else if(  ',' == datebefore )
                    {
                        if(  datain >= 0x30 &&  datain <= 0x39 )
                        {
                          GsmSta.asklen *= 10;
                          GsmSta.asklen += ( datain - 0x30 );
                        }
                        else if( 0x0a == datain ) 
                        {
                            datebefore = 0x0a;
                        }
                    }
                    else  if ( 0x0a == datebefore && GsmSta.asklen > 0)
                    {
                        revnetfunction(datain);
                        GsmSta.asklen--;
                    }
                    else if(  stu[i] > 10 &&  0 == GsmSta.asklen  )
                    {
                        stu[i] = 0;
                    }

                    return;
            }
        }
    }

    for(i = 0; i < 7; i++)
    {
        if ( datain == cpare[i][  stu[i]  ]  )
        {
            stu[i] ++ ;

            if( 5 == stu[i]  )
            {
                break; // ����Ҫ���ҵ��ַ���
            }
        }
        else
        {
            stu[i] = 0;
        }
    }

    return;
#else

    for ( i = 0; i < 7; i++ )
    {
        if ( ( i == 1 ) && ( GsmSta.DateCome == OK ) )
        {
            continue;
        }

        switch ( stu[i] )
        {
            case 0:
                if ( datain == cpare[i][stu[i]] )
                {
                    stu[i]++;
                }

                break;

            case 1:
            case 2:
            case 3:
            case 4:
                if ( datain == cpare[i][stu[i]] )
                {

                    if ( stu[i] == 4 )
                    {
                        if ( i == 4 ) /*error*/
                        {
                            STU_AtCommand.AtStu = 1;
                            STU_AtCommand.times = 0;
                            stu[i] = 0;
                        }
                        else if ( i == 3 ) /*CMTI*/
                        {

                            GsmSta.MessageCome = OK;
                            stu[i] = 0;
                        }
                        else if ( i == 5 )           /*NO CARRIER*/
                        {
                            stu[i] = 0;
                        }
                        else if ( i == 0 ) /*+CLIP: "13417483421",129,"",128,"",0 5--9*/
                        {
                            stu[i] = 5;
                            //const u8 msg[]= {0x4F,0x60,0x59,0x7D,0x00,0x31,0x00,0x32,0x00,0x33};
                            //  WriteMsgBuf((u8*)msg,10,"13168094719",11,PDU_HEX);
                            GsmSta.MessageCome = OK;

                        }
                        else if ( i == 1 ) /*SRING: 1,1024   10--18*/
                        {
                            GsmSta.RevLen = 0;
                            stu[i] = 10;
                        }
                        else if ( i == 2 ) /*#AGPSRING: 200,22.568489,113.864426,   #AGPSRING: 0,,,,,"",0   19--33*/
                        {
                            stu[i] = 19;

                        }
                        else if ( i == 6 ) /*SRECV: 1,1024 odoa aa .........aaa   #SRECV: 1,3*/
                        {
                            stu[i] = 34;

                        }

                        break;
                    }

                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 5:/*+CLIP-: "13417483421",129,"",128,"",0   �绰�����ö���Ϣ���� 5--9�绰*/
            case 6:
                stu[i]++;
                break;

            case 7:/*+CLI-P: "13417483421",129,"",128,"",0   �绰�����ö���Ϣ����*/
                if ( datain == '"' )
                {
                    stu[i]++;
                }
                else
                {
                    GsmSta.PhoneCome = OK;
                    stu[i] = 0;
                }

                GsmSta.PnoneNumberLen = 0;
                break;

            case 8:
                if ( ( datain == '"' ) || ( GsmSta.PnoneNumberLen > 19 ) )
                {
                    GsmSta.PhoneCome = OK;
                    stu[i] = 0;
                }
                else
                {

                    GsmSta.PnoneNumber[GsmSta.PnoneNumberLen++] = datain;

                }

                break;


            case 10:/*SRING: 1,1024*/
            case 11:
                stu[i]++;
                break;

            case 12://channel

                // if( (datain>='0')&&(datain<='6'))
                if ( datain >= '1' )
                {
                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 13:
                if ( datain == ',' )
                {
                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 14:
            case 15:
            case 16:
            case 17:
            case 18://1024
                if ( datain == 0x0d )
                {
                    if ( GsmSta.RevLen > 4096 )
                    {
                        GsmSta.RevLen = 4096;
                    }

                    GsmSta.DateCome = OK;
                    stu[i] = 0;
                    break;
                }
                else
                {
                    GsmSta.RevLen *= 10;
                    GsmSta.RevLen += ( datain - 0x30 );
                }

                if ( stu[i] > 17 )
                {
                    GsmSta.RevLen = 4096;
                    GsmSta.DateCome = OK;
                    stu[i] = 0;
                    break;
                }

                stu[i]++;
                break;

            case 19: /* #AGPS-RING: 200,22.568489,113.864426,0.0,520.0,"", #AGPSRING: 0,,,,,"",0   19*/
            case 20:
            case 21:
            case 22:
            case 23:
                stu[i]++;
                break;

            case 24:
                if ( datain == ' ' )
                {
                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 25:
                if ( datain == '2' )
                {
                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 26:
                if ( datain == '0' )
                {
                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 27:
                if ( datain == '0' )
                {
                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 28://,22.568489,113.864426,0.0,520.0,
                if ( datain == ',' )
                {
                    GsmSta.LaloBufLen = 0;
                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 29:
            case 30:
            case 31:
            case 32:
            case 33:
                GsmSta.LaLoBuf[GsmSta.LaloBufLen++] = datain;

                if ( datain == ',' )
                {
                    stu[i]++;

                }
                else if ( ( stu[i] >= 33 ) || ( GsmSta.LaloBufLen >= 50 ) )
                {
                    //22.568489,113.864426,0.0,520.0,
                    p = &GsmSta.LaLoBuf[0];
                    dtmp = atof ( ( char * ) p ); //22.568489

                    if(  ( s32 ) ( dtmp * 1000000 ) != 0 )
                    {
                        GsmSta.Latitude = ( s32 ) ( dtmp * 1000000 );
                        //  if(GsmSta.Latitude == 0)
                        {
                            //     GsmSta.Latitude= 6666666;
                            //    GsmSta.askm2m = 1;
                        }
                    }
                    else
                    {
                        //  GsmSta.askm2m = 1;
                    }


                    p = Findbcd ( &p[1], 49, ", ", 1 ); //,113.864426,0.0,520.0,

                    if ( p != 0 ) /*����*/
                    {
                        dtmp = atof ( ( char * ) &p[1] );
                        GsmSta.longitude = ( s32 ) ( dtmp * 1000000 );

                        if ( GsmSta.longitude == 0 )
                        {
                            GsmSta.Latitude = INVALID_LON_LAT;
                            GsmSta.askm2m = 1;
                        }

                        p = Findbcd ( &p[1], 31, ", ", 1 ); //

                        if ( p != 0 ) //,0.0,520.0,
                        {
                            p = Findbcd ( &p[1], 39, ", ", 1 ); //

                            if ( p != 0 ) /*��ȷ��*/
                            {
                                dtmp = atof ( ( char * ) &p[1] ); //22.568489
                                GsmSta.Uncerten = ( u16 ) ( dtmp * 10 );
                                GsmSta.varitygsmlon = CalacXORVarity ( ( u8 * ) &GsmSta.Latitude, 10 );
                                GsmSta.askm2m = 0;
                            }
                        }

                        GsmSta.askm2malerag = 0;
                        myprintf ( "LBS lon=%d lat=%d\r\n", GsmSta.longitude, GsmSta.Latitude );
                    }

                    stu[i] = 0;
                }

                break;


            case 34:/*SRECV: 1,1024 odoa aa .........aaa  revnetfunction*/
            case 35:
            case 36:
                stu[i]++;
                break;

            case 37:
                if ( datain == ',' )
                {
                    stu[i]++;
                }
                else
                {
                    stu[i] = 0;
                }

                break;

            case 38:/*SRECV: 1,1024 odoa aa .........aaa  revnetfunction*/
            case 39:
            case 40:
            case 41:
            case 42:
                if ( datain == 0x0d )
                {
                    stu[i] = 44;
                    break;
                }

                stu[i]++;
                break;

            case 43:
                stu[i] = 0;
                break;

            case 44:
                if ( datain == 0x0a )
                {
                    if ( debug == DEBUGNET )
                    {
                        //   RTC_Get();
                        //    printf("time:%02d:%02d:%02d",timer.hour,timer.min,timer.sec);
                        printf ( "REV:" );
                        datebefore = 0;
                    }

                    stu[i] = 45;
                }
                else
                {
                    stu[i] = 0;

                }

                break;

            case 45:
                if ( GsmSta.asklen )
                {

                    if ( debug == DEBUGNET )
                    {
                        if ( ( datain == 0xaa ) && ( datebefore == 0xaa ) )
                        {
                            // PrintUsart(0x0d);
                            //  PrintUsart(0x0a);
                            printf ( "\r\n    " );
                        }

                        printf ( "%02x ", datain );
                        // PrintUsart(HexToAscll((datain>>4)&0x0f, 0));
                        // PrintUsart(HexToAscll(datain&0x0f, 0));
                    }


                    revnetfunction ( datain );
                    datebefore = datain;
                    GsmSta.asklen--;
                }

                if ( ! GsmSta.asklen )
                {
                    stu[i] = 0;

                }

                break;

            default:
                stu[i] = 0;
                break;
        }
    }

#endif
}




/*
*********************************************************************************************************
*   �� �� ��: void WriteMsgBuf(u8* data,u8 len, u8*number,u8 numberlen,u8 TYPE)
*   ����˵��: ���Ͷ��ŵ����Ż���
*    ˵��      :123  txt:"123"   pdu:"003000310032"
*
*   ��    �Σ�
*   �� �� ֵ:
*********************************************************************************************************
*/
void WriteMsgBuf ( u8 *data, u8 len, u8 *number, u8 numberlen, u8 TYPE )
{
    u8 i;
    static u8 before = 0;

    if ( ( numberlen == 0 ) || ( numberlen > 140 ) )
    {
        return;
    }

    for ( i = 0; i < MESSAGE_NUMBERS; i++ )
    {
        if ( GsmSta.messagelen[i] == 0 )
        {
            before = i;
            break;
        }
    }

    if ( i >= MESSAGE_NUMBERS )
    {
        i = before % MESSAGE_NUMBERS;
        before++;
    }

    if ( len > 140 )
    {
        len = 140;
    }

    GsmSta.messagelen[i] = len + 1;
    GsmSta.messagetype[i] = TYPE;
    Mymemcpy ( &GsmSta.messagebuf[i][0], data, len );
    GsmSta.msgnumberlen[i] = numberlen;
    Mymemcpy ( &GsmSta.msgnumber[i][0], number, numberlen );
    GsmSta.messagebuf[i][len] = 0x1a;

}






/*
*********************************************************************************************************
*   �� �� ��: void ReadMsg(u8* TYPE)
*   ����˵��: ������
*
*
*   ��    �Σ�
*   �� �� ֵ:����ж��ŷ���ok
*********************************************************************************************************
*/
u8 ReadMsgBuf ( u8 *TYPE )
{
    u8 i;

    for ( i = 0; i < MESSAGE_NUMBERS; i++ )
    {
        if ( GsmSta.messagelen[i] != 0 )
        {
            TYPE[0] = GsmSta.messagetype[i];
            // data=&GsmSta.messagebuf[i][0];
            //  len[0]=GsmSta.messagelen[i];
            return OK;
        }
    }

    return NOT_OK;

}
/*
*********************************************************************************************************
*   �� �� ��: u8 READ_CTS(void)
*   ����˵��:
*
*
*   ��    �Σ�
*   �� �� ֵ:
*********************************************************************************************************
*/

u8 READ_CTS ( void )
{
    return GPIO_PinInGet ( CTS_PORT, CTS_PIN ); /* 1��ʾ�ڹ��� */
}

/*
*********************************************************************************************************
*   �� �� ��: void DeleteMsg(void)
*   ����˵��: ɾ������
*
*
*   ��    �Σ�
*   �� �� ֵ:����ж��ŷ���ok
*********************************************************************************************************
*/

void DeleteMsgBuf ( void )
{
    u8 i;

    for ( i = 0; i < MESSAGE_NUMBERS; i++ )
    {
        if ( GsmSta.messagelen[i] != 0 )
        {
            GsmSta.messagelen[i] = 0;
            return ;
        }
    }

}

const STU_APN StuApn[MAX_APN] =
{
    /*CSL 1010 / One2Free �C Name: hkcsl��APN: mobile; CC:454; MNC:00*/
    {
        .oper = "45400",
        .strapn = "mobile",
        .NetId = EMPTY_MODE,

    },
    /*SmarTone �C Name: smartone��APN: smartone; CC:454; MNC:06*/
    {
        .oper = "45406",
        .strapn = "smartone",
        .NetId = EMPTY_MODE,

    },
    /*3HK �C Name: 3HK��APN: mobile.three.com.hk; CC:454; MNC:03*/
    {
        .oper = "45403",
        .strapn = "mobile.three.com.hk",
        .NetId = EMPTY_MODE,

    },
    /*PCCW �C Name: pccw��APN: pccw; CC:454; MNC:19*/
    {
        .oper = "45419",
        .strapn = "pccw",
        .NetId = EMPTY_MODE,

    },
    /*China Mobile �C Name: cmhk��APN �C cmhk; CC:454; MNC:12*/
    {
        .oper = "45412",
        .strapn = "cmhk",
        .NetId = EMPTY_MODE,

    },
    /*�й���ͨNAPN �C uninet; CC:460; MNC:01*/
    {
        .oper = "46001",
        //.strapn="uninet",
        // .strapn="3gnet",
        .strapn = "unim2m.gzm2mapn",
        .NetId = EMPTY_MODE,

    },
    /*�й��ƶ�APN �C cmnet; CC:460; MNC:00*/
    {
        .oper = "46000",
        .strapn = "cmnet",
        .NetId = EMPTY_MODE,

    }
    ,
    /*�й�����APN �C cmnet; CC:460; MNC:03*/
    {
        .oper = "46003",
        .strapn = "ctnet",
        .NetId = EMPTY_MODE,

    },
};


