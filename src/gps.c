#include"includes.h"

STRUCT_QUEUE_CTRL GpsRxQueueCtrl;                   //gps接收队列控制结构体
volatile u8 GpsRxQueueBuf[GPS_REC_QUEUE_LENGTH];    //gps接收队列缓存
u8  B_GpsFrmBuf[MAX_GPS_FRM_LEN];                   //用来存放被正确读取的一帧GPS数据包
OSP_STU OspStu;
GPS_CONTROL_STU GpsControlStu;
GPS_GPGSV GpsGsv;
GPS_STATUES  GpsStatues;
EE_STU eeStu;
STR_GPS_DATA        strGpsData;
BVKSTR_GPS_DATA  bvkstrGpsData;/*上一次发送的点*/
u16 unfixedtime = 200;
u8  LocSuccess;
/*
*********************************************************************************************************
*   函 数 名:void InitGps(void)
*   功能说明: PD0
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void InitGps(void)
{

    LocSuccess     = 0;

    //GPIO_PinModeSet(GPS_RF_POWER_PORT, GPS_RF_POWER_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(GPS_POWER_PORT, GPS_POWER_PIN, gpioModePushPull, 0);
    GpsPowerOff();
    GPIO_PinModeSet(GPS_MON_PORT, GPS_MON_PIN, gpioModeInputPull, 1);
    //   GPIO_PinModeSet(gpioPortC, 11, gpioModeInput,0);
    GPIO_PinModeSet(GPS_ON_OFF_PORT, GPS_ON_OFF_PIN, gpioModePushPull, 0);
    //  initLeuart(GPS_UART,38400,POWER_ON);
    QueueInit(&GpsRxQueueCtrl, (u8 *)GpsRxQueueBuf, GPS_REC_QUEUE_LENGTH); //队列初始化
#ifndef GPS_USE_UART
    initLeuart(GPS_UART, GPS_IPR, POWER_ON);
#else
    uartSetup(GPS_UART, GPS_IPR, POWER_ON);
#endif


}

void UsartSend(USART_TypeDef *usart, u8 *data, u16 len)
{
    u16 i;

    for(i = 0; i < len; i++)
    {
        USART_Tx(usart, data[i]);
    }
}
/*
*********************************************************************************************************
*   函 数 名void GPSUsartSendStr(u8 *str,u16 len)
*   功能说明: GPS发送字符串
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void GPSUsartSendStr(u8 *str, u16 len)
{

    u16 i;

    for(i = 0; i < len; i++)
    {

#ifndef GPS_USE_UART
        LEUART_Tx(GPS_UART, (u8)str[i]);
#else
        USART_Tx(GPS_UART, (u8)str[i]);
#endif

    }

    if( (debug == DEBUGGPS) || (debug == DEBUGANT))
    {
        DebugUartSend(str, len);
    }

}
#if 0
/*
*********************************************************************************************************
*   函 数 名: GetRightGpsDate
*   功能说明:过滤掉过大的经纬度的值
*
*   形    参：

*   返 回 值: 无
*********************************************************************************************************
*/
void GetRightGpsDate(void)
{



    static char bLongitude[4] = {0, 0, 0, 0}, bLatitude[4] = {0, 0, 0, 0};
    static u8 first = 10, vbefore = 0;
    u8 i;
    u32 distancecom;


    if(strGpsData.bValidity == 'A')
    {
        distancecom = GetDistance((u8 *)bLongitude, (u8 *)bLatitude, (u8 *)strGpsData.bLongitude, (u8 *)strGpsData.bLatitude);

        if( (0 == AlarmStu.ShockDecrease) && (ObdStu.ACC != ACC_ON))
        {
            strGpsData.speed = 0;
        }

        if(first)
        {
            first--;
            memcpy((char *)&BVKstrGpsData, (char *)&strGpsData, sizeof(strGpsData));

        }

        if(distancecom < 55) //200km/h /3.6=55.55m/s
        {
            //  if((ObdStu.ACC==ACC_ON)||(AlarmStu.ShockDecrease>24))
            if(ObdStu.ACC == ACC_ON)
            {
                first--;
                memcpy((char *)&BVKstrGpsData, (char *)&strGpsData, sizeof(strGpsData));
            }
        }


        InflexiongPoint(strGpsData.bLongitude, strGpsData.bLatitude, bLongitude, bLatitude);

        for(i = 0; i < 4; i++)
        {
            bLongitude[i] = strGpsData.bLongitude[i];
            bLatitude[i] = strGpsData.bLatitude[i];
        }

    }

    if( (strGpsData.bValidity != vbefore) && (strGpsData.savesecond > 60) && (ObdStu.ACC == ACC_ON))
    {

        WriteGpsDate();
        strGpsData.savesecond = 0;
    }

    vbefore = strGpsData.bValidity;

}
#endif
/*
*********************************************************************************************************
*   函 数 名: void WriteEE(void)
*   功能说明: 定时写星历数据
*
*
*   形    参：
*   返 回 值:
*********************************************************************************************************
*/
void WriteEE(u8 *inbuf, u32 offset, u16 len, u8 *EE)
{
    u16 i;

    if ( (offset + len) > EE_LEN)
    {
        return;
    }

    // eeStu.flag=OK;

    for(i = 0; i < len; i++)
    {
        EE[offset + i] = inbuf[i];
    }
}
/*
*********************************************************************************************************
*   函 数 名: void ReadEE(u8*outbuf ,u32 offset ,u16 len,u8 flag)
*   功能说明: 读EE星历数据
*
*
*   形    参：
*   返 回 值:
*********************************************************************************************************
*/
void ReadEE(u8 *outbuf, u32 offset, u16 len, u8 *EE)
{
    u16 i;



    if ( (offset + len) > EE_LEN)
    {
        /*访问数据出界*/
        Memset(outbuf, 0, len);
        return;
    }

    for(i = 0; i < len; i++)
    {
        outbuf[i] = EE[offset + i];

    }


}
/*
*********************************************************************************************************
*   函 数 名: void ReadSGEE(u8*outbuf ,u32 offset ,u16 len)
*   功能说明: 读SGEE星历数据
*
*
*   形    参：
*   返 回 值:
*********************************************************************************************************
*/
void ReadSGEE(u8 *outbuf, u32 offset, u16 len)
{

    if(  ( (offset + len) > (SGEE_MAX_LEN - 2)) || ( GsmSto.updateflag == OK) )
    {
        /*访问数据出界*/
        Memset(outbuf, 0xff, len);
        return;
    }

    FLASH_ReadDate(SGEE_ADDR + offset, len, outbuf);

}
/*
*********************************************************************************************************
*   函 数 名void WriteSGEE(u8 *datain,u16 datelen,u32 offset )
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void WriteSGEE(u8 *datain, u16 datelen, u32 offset )
{


    if( ( GsmSto.updateflag == OK) || (GpsStatues.SgeeState == AGPS_UPING) || (resetflag == 0xaa))
    {
        return;
    }

    FLASH_WriteDate(SGEE_ADDR + offset, datelen, datain);


}
/*
*********************************************************************************************************
*   函 数 名void EraseSGEE(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void EraseSGEE(void)
{
    u32 i;

    if( ( GsmSto.updateflag == OK) || (GpsStatues.SgeeState == AGPS_UPING) || (resetflag == 0xaa))
    {
        return;
    }

    for(i = 0; i < SGEE_SEC; i++)
    {
        FLASH_eraseOneBlock(SGEE_ADDR + 2048 * i);
    }
}
/*
*********************************************************************************************************
*   函 数 名  void WriteSGEEHavedate(void)
*   功能说明:升级完成后调用 have为OK表示有数据    have为NOT_OK表示没有数据
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void WriteSGEEHavedate(u8 have)
{

    u8 flag[8] = {0xaa, 0xaa, 0xaa, 0xaa};

    if( ( GsmSto.updateflag == OK) || (resetflag == 0xaa))
    {
        return;
    }

    flag[0] = GpsStatues.offset & 0xff;
    flag[1] = (GpsStatues.offset >> 8) & 0xff;
    flag[2] = (GpsStatues.offset >> 16) & 0xff;
    flag[3] = (GpsStatues.offset >> 24) & 0xff;

    if(have == OK)
    {
        flag[4] = 0xaa;
        flag[5] = 0xaa;
        flag[6] = 0xaa;
        flag[7] = 0xaa;
    }
    else
    {

        flag[4] = 0;
        flag[5] = 0;
        flag[6] = 0;
        flag[7] = 0;
    }

    FLASH_WriteDate(NET_SGEE_ADDR + NET_SGEE_MAX_LEN - 8, 8, (u8 *)flag);


}

/*
*********************************************************************************************************
*   函 数 名  u8  ReadSGEEHavedate(void)
*   功能说明:读是否有AGPS数据
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
u8  ReadSGEEHavedate(void)
{

    u8 flag[8];

    if( ( GsmSto.updateflag == OK) || (resetflag == 0xaa))
    {
        return NOT_OK;
    }

    FLASH_ReadDate(NET_SGEE_ADDR + NET_SGEE_MAX_LEN - 8, 8, flag);
    GpsStatues.offset = ((((u32)flag[0]) << 0) & 0x000000ff) | ((((u32)flag[1]) << 8) & 0x0000ff00) | ((((u32)flag[2]) << 16) & 0x00ff0000) | ((((u32)flag[3]) << 24) & 0xff000000);

    if( (flag[4] == 0xaa) &&
        (flag[5] == 0xaa) &&
        (flag[6] == 0xaa) &&
        (flag[7] == 0xaa)
      )
    {
        return OK;
    }

    return NOT_OK;
}



/*
*********************************************************************************************************
*   函 数 名: void InitSendDateToGps(void)
*   功能说明:传输SGEE数据到GPS初始化变量
*
*
*   形    参：

*   返 回 值: 0xaa  ----successs   0x55 ------>timeout   0xa5-----watint
*********************************************************************************************************
*/
void InitSendDateToGps(void)
{

    GpsStatues.U_CurrentPacket = 1;
    GpsStatues.U_TatolPackeg = GpsStatues.offset / SEND_SGEE_FILE_SIZE;

    if(GpsStatues.offset % SEND_SGEE_FILE_SIZE)
    {
        GpsStatues.LastPackegSize = GpsStatues.offset % SEND_SGEE_FILE_SIZE;
        GpsStatues.U_TatolPackeg++;
    }
    else
    {
        GpsStatues.LastPackegSize = SEND_SGEE_FILE_SIZE;
    }
}

/*
*********************************************************************************************************
*   函 数 名:void GpsReadDate(u8 type,u16 datalen,u32 offset,u8*strLen,u8*stroffset)
*   功能说明: GPS读数据
*
*
*   形    参：
*   返 回 值: 无
*********************************************************************************************************
*/
void GpsReadDate(u8 type, u16 datalen, u32 offset, u8 *strLen, u8 *stroffset)
{
    /*若没有数据则回 0*/
    //$PSRF114,1b,1,3,1,a,0,0,0,f,6,0,f0,0,0,4a,0*41
#define GpsReadDate_BUF_LEN 300
    u8 tmp[GpsReadDate_BUF_LEN];
    u16 len = 0;
    u8 datatmp;
    u16 i;

    if((type < 1) || (type > 3))
    {
        return;
    }

    Mymemcpy(&tmp[len], "$PSRF114,1B,1,", 14);
    len += 14;
    tmp[len++] = type + 0x30;
    tmp[len++] = ',';

    /*Num Blocks*/
    tmp[len++] = '1';
    tmp[len++] = ',';

    /*Block Length*/
    strcpy((char *)&tmp[len], (char *)strLen);
    len += strlen((char *)strLen);
    tmp[len++] = ',';
    /*Offset*/
    strcpy((char *)&tmp[len], (char *)stroffset);
    len += strlen((char *)stroffset);
    tmp[len++] = ',';

    /*data*/
    if( (type == 3) || (type == 2) )
    {
        if (type == 3)
        {
            ReadEE(&tmp[GpsReadDate_BUF_LEN - datalen], offset, datalen, eeStu.EEDATA);    /*表示是EE*/
        }
        else
        {
            ReadEE(&tmp[GpsReadDate_BUF_LEN - datalen], offset, datalen, eeStu.CGEEDATA);    /*CGEE无效数据*/
        }
    }
    else if(type == 1) /*SGEE*/
    {
        ReadSGEE(&tmp[GpsReadDate_BUF_LEN - datalen], offset, datalen);

        for(i = 0; i < datalen; i++)
        {
            datatmp = ~tmp[GpsReadDate_BUF_LEN - datalen + i];;
            tmp[GpsReadDate_BUF_LEN - datalen + i] = datatmp;
        }
    }

    for(i = 0; i < datalen; i++)
    {
        datatmp = tmp[GpsReadDate_BUF_LEN - datalen + i];

        if(datatmp > 0x0f)
        {
            tmp[len++] = HexToAscll((datatmp >> 4) & 0x0f, 0);
        }

        tmp[len++] = HexToAscll(datatmp & 0x0f, 0);
        tmp[len++] = ',';
    }

    /* *checksum*/
    datatmp = 0;

    for(i = 1; i < len; i++)
    {
        datatmp ^= tmp[i];
    }

    tmp[len++] = '*';
    tmp[len++] = HexToAscll((datatmp >> 4) & 0x0f, 0);
    tmp[len++] = HexToAscll(datatmp & 0x0f, 0);
    tmp[len++] = 0x0d;
    tmp[len++] = 0x0a;
    tmp[len++] = 0;
    GPSUsartSendStr(tmp, len - 1);

}
/*
*********************************************************************************************************
*   函 数 名:void GpsEraseDate(u8 type)
*   功能说明: GPS擦除数据
*
*
*   形    参：
*   返 回 值: 无
*********************************************************************************************************
*/
void GpsEraseDate(u8 type)
{
    /*$PSRF114,1c,9c,23,0,0*06*/
    u8 tmp[100];
    u16 len = 0;
    u8 datatmp;
    u16 i;

    if((type < 1) || (type > 3))
    {
        return;
    }


    if( (type == 3) || (type == 2))
    {
        Mymemcpy(&tmp[len], "$PSRF114,1C,9C,24,0,0", 21);
        len += 21;

        if(type == 3)
        {
            Memset(eeStu.EEDATA, 0, EE_LEN); /*表示是EE*/
        }
        else
        {
            Memset(eeStu.CGEEDATA, 0, EE_LEN); /*CGEE无效数据*/
        }
    }
    else if(type == 1) /*SGEE*/
    {

        Mymemcpy(&tmp[len], "$PSRF114,1C,9C,24,0,0", 21);
        len += 21;
        EraseSGEE();

    }


    /* *checksum*/
    datatmp = 0;

    for(i = 1; i < len; i++)
    {
        datatmp ^= tmp[i];
    }

    tmp[len++] = '*';
    tmp[len++] = HexToAscll((datatmp >> 4) & 0x0f, 0);
    tmp[len++] = HexToAscll(datatmp & 0x0f, 0);
    tmp[len++] = 0x0d;
    tmp[len++] = 0x0a;
    tmp[len++] = 0;
    GPSUsartSendStr(tmp, len - 1);



}
/*
*********************************************************************************************************
*   函 数 名:void  GpsWriteDate(u8 typr,u16 size,u32 offset,u8*datafrom)
*   功能说明: GPS写数据
*
*
*   形    参：
*   返 回 值: 无
*********************************************************************************************************
*/
void  GpsWriteDate(u8 type, u16 size, u32 offset, u8 *datafrom) /*type size  offset  data*/
{
    /*$PSRF114,1c,9c,23,0,0*06*/
#define GpsWriteDate_BUF_LEN 300
    u8 tmp[GpsWriteDate_BUF_LEN];
    u16 len = 0;
    u8 datatmp;
    u16 i;
    u8 *pbPrePtr, *dateptr;

    if((type < 1) || (type > 3))
    {
        return;
    }


    if( (type == 3) || (type == 2)) /*3表示是EE*/
    {
        Mymemcpy(&tmp[len], "$PSRF114,1C,9C,25,0,0", 21);
        len += 21;
        pbPrePtr = datafrom;
        dateptr = &tmp[GpsWriteDate_BUF_LEN - size];

        for(i = 0; i < size; i++)
        {
            dateptr[i] = strtol((char *)pbPrePtr, NULL, 16);
            pbPrePtr = (u8 *)strchr((char *)pbPrePtr, ',');
            pbPrePtr++;

        }

        if(type == 3)
        {
            WriteEE(dateptr, offset, size, eeStu.EEDATA);
        }
        else
        {
            WriteEE(dateptr, offset, size, eeStu.CGEEDATA);
        }
    }

    else if(type == 1) /*SGEE*/
    {
        Mymemcpy(&tmp[len], "$PSRF114,1C,9C,25,0,0", 21);
        len += 21;
        pbPrePtr = datafrom;
        dateptr = &tmp[GpsWriteDate_BUF_LEN - size];

        for(i = 0; i < size; i++)
        {
            dateptr[i] = strtol((char *)pbPrePtr, NULL, 16);
            pbPrePtr = (u8 *)strchr((char *)pbPrePtr, ',');
            pbPrePtr++;
        }

        for(i = 0; i < size; i++)
        {
            datatmp = ~dateptr[i];
            dateptr[i] = datatmp;
        }

        WriteSGEE(dateptr, size, offset);

    }


    /* *checksum*/
    datatmp = 0;

    for(i = 1; i < len; i++)
    {
        datatmp ^= tmp[i];
    }

    tmp[len++] = '*';
    tmp[len++] = HexToAscll((datatmp >> 4) & 0x0f, 0);
    tmp[len++] = HexToAscll(datatmp & 0x0f, 0);
    tmp[len++] = 0x0d;
    tmp[len++] = 0x0a;
    tmp[len++] = 0;
    GPSUsartSendStr(tmp, len - 1);


}



/*
*********************************************************************************************************
*   函 数 名:u8 FilterGps(void)
*   功能说明:gps信息滤波
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/

u8 FilterGps(void)
{
    u8 result = OK;

#if 0
    static u16 turn = 0;
    static u32 p = 10;
    static double latb = 0, lonb = 0;
    double latnow = 0, lonnow = 0;
    int  distant, cmp;
    u32 abs;

    result = OK;

    /*查找超时返回*/
    //==========================================================
    if( (GpsControlStu.FindGpsTime++ > (WAKE_TIME - 1)) && (strGpsData.SatelCnt > 2))
    {
        GpsControlStu.FindGpsTime = 0;
        GpsControlStu.Get5Position = OK;
        return result;
    }

    //==========================================================



    /*求平均速度*/
    //==========================================================
    if(turn == 0)
    {
        p = 10;
    }
    else if(turn < 20000)
    {
        p = (p * (turn - 1) + strGpsData.speed) / (turn + 1);
    }
    else
    {
        turn = 0;
    }

    //==========================================================



    if(strGpsData.SatelCnt < 3)
    {
        result = NOT_OK;
    }


    if(result == OK)
    {
        /*判断速度是否合理*/
        //==========================================================
        if(strGpsData.speed  > p)
        {
            abs = strGpsData.speed - p;
        }
        else
        {
            abs = p - strGpsData.speed;
        }

        if(abs > 19)
        {
            result = NOT_OK;
        }

        //==========================================================
    }


    /*判断距离是否合理*/
    //==========================================================
    latnow = (double)strGpsData.Latitude / 1000000;
    lonnow = (double)strGpsData.longitude / 1000000;

    /*1s车的最大距离42m  (150km/h)*/
    /*1s步行最大距离9m  (30km/h)*/
    if(result == OK)
    {
        if(p > 30)
        {
            cmp = 42;
        }
        else
        {
            cmp = 9;
        }

        if( (latb != 0) || (lonb != 0))
        {
            distant = CalcDistance(latb, lonb, latnow, lonnow);
#ifdef USE_PRINTF

            if(debug == DEBUGADC)
            {
                myprintf("距离:%d\r\n", distant);
            }

#endif

            if(distant > cmp)
            {
                result = NOT_OK;
            }

        }


        latb = (double)bvkstrGpsData.Latitude / 1000000;
        lonb = (double)bvkstrGpsData.longitude / 1000000;
        distant = CalcDistance(latb, lonb, latnow, lonnow);

        if(distant > (SEARCH_TIME * cmp))
        {
            result = NOT_OK;
        }
    }

    latb = latnow;
    lonb = lonnow;
    //==========================================================

#endif


    return result;

}

/*
*********************************************************************************************************
*   函 数 名:u8 filetsend(double latnow,double lonnow)
*   功能说明:过滤发送的数据
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
u8 filetsend(double latnow, double lonnow)
{
    u8 i;
    static double latb = 0, lonb = 0;
    i = NOT_OK;


    if( ( 20 < CalcDistance(latb, lonb, latnow, lonnow)) ||
        ( (latb == 0) && (lonb == 0))
      )
    {
        latb = latnow;
        lonb = lonnow;
        i = OK;
    }



    return i;
}
/*
*********************************************************************************************************
*   函 数 名:void AnalysGpsDataPackage(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void AnalysGpsDataPackage(void)
{
    u8 bI;
    u16 tmp;
    u8 time[6];
    char *pbCurrPtr, *pbPrePtr, *LongPtr;
    char bArray[20][20];
    double temp;
    //  static u8 fixtime;
    s32 stmp;
    u8 t8mp;
    char tmpbuf[20];
    pbPrePtr = (char *)(B_GpsFrmBuf);
    pbPrePtr += 7;

    for(bI = 0; bI < 20; bI++)
    {
        pbCurrPtr = strchr(pbPrePtr, ',');

        if(pbCurrPtr == NULL)//不存在
        {
            break;
        }

        t8mp = *pbCurrPtr;
        *pbCurrPtr = 0x0;
        strcpy(bArray[bI], pbPrePtr);
        *pbCurrPtr = t8mp;
        pbCurrPtr++;

        if(bI == 6)
        {
            LongPtr = pbPrePtr;
        }

        pbPrePtr = pbCurrPtr;
    }

    if( (strncmp((char *)B_GpsFrmBuf, "$GPTXT", 6)) == 0 ) //short
    {
        GpsControlStu.result = GPS_SHORT;
        return;
    }

    if( (strncmp((char *)B_GpsFrmBuf, "$PSRF", 5)) == 0 ) //
    {

        if( (strncmp((char *)&B_GpsFrmBuf[5], "150,1", 5)) == 0 ) //
        {
            GpsControlStu.result = GPS_1501;
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "150,0", 5)) == 0 ) //
        {
            GpsControlStu.result = GPS_1500;
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,20,72,16,0", 14)) == 0 ) //
        {
            GpsControlStu.result = GPS_ACK_START;
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,20,72,16,1", 14)) == 0 ) //
        {
            GpsControlStu.result = GPS_NACK_START;
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,20,72,17,0", 14)) == 0 ) //
        {
            GpsControlStu.result = GPS_ACK_FILESIZE;
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,20,72,17,1", 14)) == 0 ) //
        {
            GpsControlStu.result = GPS_NACK_FILESIZE;
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,20,72,18,0", 14)) == 0 ) //
        {
            GpsControlStu.result = GPS_SEND_PACKEG_ACK;
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,20,72,18,1", 14)) == 0 ) //
        {
            GpsControlStu.result = GPS_SEND_PACKEG_NACK;
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,26", 6)) == 0 ) /*读数据$PSRF156,26,3,1,1,4c,0*75<CR><LF> 大小为0x4c  偏移为0*/
        {
            GpsReadDate(strtol(bArray[2], NULL, 16),
                        strtol(bArray[5], NULL, 16),
                        strtol(bArray[6], NULL, 16),
                        (u8 *)bArray[5], (u8 *)bArray[6]);
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,24", 6)) == 0 ) /*擦除数据*/
        {
            GpsEraseDate(strtol(bArray[2], NULL, 16));
        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,25", 6)) == 0 ) /*写数据*/
        {
            GpsWriteDate(strtol(bArray[2], NULL, 16),
                         strtol(bArray[3], NULL, 16),
                         strtol(bArray[4], NULL, 16),
                         (u8 *)LongPtr); /*type size  offset  data*/

        }
        else if( (strncmp((char *)&B_GpsFrmBuf[5], "156,23,1", 6)) == 0 ) /*更新SGEE数据*/
        {
            if((GpsStatues.SgeeState == AGPS_IDLE) && (OK == ReadSGEEHavedate()))
            {
                GpsStatues.SgeeState = AGPS_NEED;
            }
        }

    }

    if( (strncmp((char *)B_GpsFrmBuf, "$GPGGA", 6)) == 0 )
    {

        tmp = (s16)(strtod(bArray[8], NULL));

        bI = (u8)(strtod(bArray[6], NULL));

        if(bI < 13)
        {
            strGpsData.high_m = tmp;
            strGpsData.SatelCnt = bI;
        }
    }

    if( (strncmp((char *)B_GpsFrmBuf, "$GPGSV", 6)) == 0 )
    {

        GpsGsv.gpgsv[3] = tmp = (u8)(strtod(bArray[6], NULL));
        GpsGsv.gpgsv[4] = tmp = (u8)(strtod(bArray[10], NULL));
        GpsGsv.gpgsv[5] = tmp = (u8)(strtod(bArray[14], NULL));

        if(GpsGsv.gpgsv[3] > GpsGsv.gpgsv[0])
        {
            GpsGsv.gpgsv[0] = GpsGsv.gpgsv[3];
        }

        if(GpsGsv.gpgsv[4] > GpsGsv.gpgsv[1])
        {
            GpsGsv.gpgsv[1] = GpsGsv.gpgsv[4];
        }

        if(GpsGsv.gpgsv[5] > GpsGsv.gpgsv[2])
        {
            GpsGsv.gpgsv[2] = GpsGsv.gpgsv[5];
        }

        //  printf("\r\n max :%2d-%2d-%2d\r\n ret :%2d-%2d-%2d",GpsGsv.gpgsv[0],GpsGsv.gpgsv[1],GpsGsv.gpgsv[2],GpsGsv.gpgsv[3],GpsGsv.gpgsv[4],GpsGsv.gpgsv[5]);
    }

    if( (strncmp((char *)B_GpsFrmBuf, "$GPRMC", 6)) == 0 )
    {
        if( OspStu.flagack == OSP_WAIT_NMEA)
        {
            OspStu.flagack = OSP_WAIT_NMEA_OK;
        }

        strGpsData.bValidity = *bArray[1] ; //已定位  41--A

        if(strGpsData.bValidity == 'A')
        {
            GpsControlStu.GpsUnfixedTime = 0;
            LocSuccess = 0xff;
            //  LedFlash(1000,1);
            // unfixedtime=0;

        }

        //  else
        // {
        //   fixtime=0;

        //  }

        if( (strGpsData.bValidity != 'A') && (strGpsData.bValidity != 'V')  )
        {
            return;
        }




        //2012 09 05
        tmpbuf[0] = (((bArray[8][4] - 0x30) << 4) & 0xf0) | ((bArray[8][5] - 0x30) & 0x0f); //year
        tmpbuf[1] = (((bArray[8][2] - 0x30) << 4) & 0xf0) | ((bArray[8][3] - 0x30) & 0x0f); //month
        tmpbuf[2] = (((bArray[8][0] - 0x30) << 4) & 0xf0) | ((bArray[8][1] - 0x30) & 0x0f); //day
        tmpbuf[3] = (((bArray[0][0] - 0x30) << 4) & 0xf0) | ((bArray[0][1] - 0x30) & 0x0f); //hour
        tmpbuf[4] = (((bArray[0][2] - 0x30) << 4) & 0xf0) | ((bArray[0][3] - 0x30) & 0x0f); //minute
        tmpbuf[5] = (((bArray[0][4] - 0x30) << 4) & 0xf0) | ((bArray[0][5] - 0x30) & 0x0f); //second

        for(bI = 0; bI < 6; bI++)
        {
            time[bI] = ((tmpbuf[bI] >> 4) & 0x0f) * 10 + (tmpbuf[bI] & 0x0f);

        }

        if( (time[0] < 17) || (time[0] > 99) || (time[1] > 12) || (time[2] > 31) || (time[3] > 24) || (time[4] > 61) || (time[5] > 61))
        {
            return;
        }

        t8mp = GsmSto.Timehour & 0x80;

        if(t8mp)
        {
            t8mp = 0;
        }
        else
        {
            t8mp = 1;
        }

        if(strGpsData.bValidity == 'A')
        {
            // if(debug==DEBUGGPS)
            // LedFlash(600,3);
            if(timer.rtcflag > 250/*50*/)
            {

                timer.rtcflag = 0;
                UtcToBeijingDate((u8 *)tmpbuf, GsmSto.Timehour & 0x7f, GsmSto.Timeminute, t8mp);
                Mymemcpy((u8 *)strGpsData.bTime, (u8 *)tmpbuf, 6);

            }
        }





        if(   (((*bArray[3]) != 'N') && ((*bArray[3]) != 'S')) || (((*bArray[5]) != 'W') && ((*bArray[5]) != 'E'))     )
        {
            return;
        }


        strncpy(tmpbuf, bArray[2], 9);  //纬度ddmm.mmmm 格式（前导位数不足则补0）
        stmp = ((tmpbuf[0] - 0x30) * 10 + (tmpbuf[1] - 0x30) ) * 1000000 + (s32)(strtod(&tmpbuf[2], NULL) * 100000 / 6);

        if((*bArray[3]) != 'N')
        {
            strGpsData.Latitude = -stmp;
        }
        else
        {
            strGpsData.Latitude = stmp;
        }



        strncpy(tmpbuf, bArray[4], 10); //经度dddmm.mmmm(度分)格式(前面的0也将被传输)
        stmp = ((tmpbuf[0] - 0x30) * 100 + (tmpbuf[1] - 0x30) * 10 + (tmpbuf[2] - 0x30)) * 1000000 + (s32)(strtod(&tmpbuf[3], NULL) * 100000 / 6);

        if((*bArray[5]) != 'E')
        {
            strGpsData.longitude = -stmp;
        }
        else
        {
            strGpsData.longitude = stmp;
        }


        strncpy(tmpbuf, bArray[6], 5);  //地面速度 地面速率(000.0~999.9节，前面的0也将被传输)
        tmpbuf[5] = 0;
        temp = strtod(tmpbuf, NULL);
        temp = (u16)(temp * 1852 / 1000);
        strGpsData.speed = (u16)temp;
        // stmp=temp-strGpsData.speed;
        //strGpsData.AInKmH=(s8)stmp;
        //  strGpsData.a=stmp*10000/3528;
        //  if(temp<200)
        //{
        //    strGpsData.speed=temp;
        //  ObdStu.UpDate._bit.GpsSpeedAlarm=1;
        //  }
        strncpy(tmpbuf, bArray[7], 5); //地面航向     地面航向(000.0~359.9度，以正北为参考基准，前面的0也将被传输)
        tmpbuf[5] = 0;
        temp = strtod(tmpbuf, NULL);

        if(temp <= 360)
        {
            strGpsData.direction = (u16)temp;
        }


        if(strGpsData.bValidity == 'A')
        {
            // if(debug==DEBUGGPS)
            // LedFlash(600,3);

            if(    (
                       (OK == FilterGps()) ||
                       (GsmSto.moveintervalGPS < MIN_GPS_INTER)
                   ) &&
                   (strGpsData.SatelCnt > 2)
              )
            {
                if(
                    (strGpsData.speed >= 0 /*1*/) ||
                    (GsmSto.moveintervalGPS < MIN_GPS_INTER)

                )
                {
                    Mymemcpy((u8 *)&bvkstrGpsData.Latitude, (u8 *)&strGpsData.Latitude, sizeof(bvkstrGpsData) - 1);
                    // bvkstrGpsData.Latitude = strGpsData.Latitude;
                    bvkstrGpsData.varity = CalacXORVarity((u8 *)&strGpsData.Latitude, sizeof(bvkstrGpsData) - 1);
                    unfixedtime = 0;
                    //  bvkstrGpsData.SatelCnt=strGpsData.SatelCnt;
                    //  bvkstrGpsData.speed = strGpsData.speed;
                }
                else if(unfixedtime < 30)
                {
                    unfixedtime = 0;
                }

            }

            //   else
            // {
            //      unfixedtime++;
            //  }
            //GetRightGpsDate();


        }



    }



}

/*
*********************************************************************************************************
*   函 数 名: void GetGpsDataPackage(u8 Data)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
//#pragma optimize=none
void GetGpsDataPackage(u8 Data)
{
    static u16 idx;
    static u8 xor = 0;
    static u8 GPSUARTState = 0;

    switch(GPSUARTState)
    {
        case 0: //'$'
            if(Data == '$')
            {
                idx = 0;
                B_GpsFrmBuf[idx++] = Data;
                GPSUARTState = 1;
            }
            else   if(Data == 0xa0)
            {
                idx = 0;
                B_GpsFrmBuf[idx++] = Data;
                GPSUARTState = 3;
            }

            break;

        case 1:
            B_GpsFrmBuf[idx++] = Data;

            if((Data == '\r'))
            {
                GPSUARTState = 2;

            }
            else
            {
                xor ^= Data;

                if(idx >= (MAX_GPS_FRM_LEN - 1))
                {
                    GPSUARTState = 0;
                    idx = 0;
                    xor = 0;
                }
            }

            break;

        case 2:
            if(Data == '\n' )
            {
                B_GpsFrmBuf[idx] = Data;
                B_GpsFrmBuf[idx + 1] = 0;
                xor ^= B_GpsFrmBuf[idx - 2];
                xor ^= B_GpsFrmBuf[idx - 3];
                xor ^= B_GpsFrmBuf[idx - 4];
                GPSUARTState = ((acstohex(B_GpsFrmBuf[idx - 3]) & 0x0f) << 4) | (acstohex(B_GpsFrmBuf[idx - 2]) & 0x0f);

                if(xor == GPSUARTState)
                {
                    GpsControlStu.GpsNoDateTime = 0;

                    AnalysGpsDataPackage();
                    //myprintf( "Chenyong: %s\r\n", B_GpsFrmBuf );
                }
            }

            GPSUARTState = 0;
            idx = 0;
            xor = 0;
            break;

        case 3:
            if(Data == 0xa2)
            {
                B_GpsFrmBuf[idx++] = Data;
                GPSUARTState = 4;
            }
            else
            {
                GPSUARTState = 0;
                idx = 0;
            }

            break;

        case 4:
        case 5:
            B_GpsFrmBuf[idx++] = Data;
            GPSUARTState++;

            if(GPSUARTState == 6)
            {
                OspStu.len = ((((u16)B_GpsFrmBuf[2]) << 8) & 0xff00) | (((u16)B_GpsFrmBuf[3]) & 0x00ff);

                if(OspStu.len > 280)
                {
                    OspStu.len = 280;
                }

                OspStu.checksum = 0;
            }

            break;

        case 6:
            B_GpsFrmBuf[idx++] = Data;
            OspStu.pdata[idx - 5] = Data;
            OspStu.checksum += Data;
            OspStu.checksum &= 0x7fff;

            if(idx >= (OspStu.len + 4))
            {
                GPSUARTState = 7;
            }

            break;

        case 7:
            xor = (u8)((OspStu.checksum >> 8) & 0x00ff);

            if(xor == Data)
            {
                GPSUARTState = 8;
            }
            else
            {
                GPSUARTState = 0;
                idx = 0;
                xor = 0;
            }

            break;

        case 8:
            xor = (u8)(OspStu.checksum & 0x00ff);

            if(xor == Data)
            {


                if((OspStu.pdata[0] == 0x0b) && (OspStu.pdata[1] == 0x92))
                {
                    OspStu.flag = OSP_FRAM_ACK;
                }
                else if((OspStu.pdata[0] == 0x0c) && (OspStu.pdata[1] == 0x92))
                {
                    OspStu.flag = OSP_FRAM_NCK;
                }
                else if((OspStu.pdata[0] == 0x0b) && (OspStu.pdata[1] == 0x88))
                {
                    OspStu.flag = OSP_FRAM_SMOOTH_ACK;
                }
                else
                {
                    OspStu.flag = OSP_FRAM_OTHER;

                }


                if( (OspStu.flagack == OSP_WAIT_TO_TRIC) && (OspStu.pdata[0] == 0x5a) && (OspStu.pdata[2] == 0x00))
                {

                    OspStu.flag = OSP_TRICK_OK;





                }

            }

            GPSUARTState = 0;
            idx = 0;
            xor = 0;
            break;

        default:
            GPSUARTState = 0;
            idx = 0;
            xor = 0;
            break;

    }



}



/*
*********************************************************************************************************
*   函 数 名:u8  GetGpsDate(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
u8  GetGpsDate(void)
{
    u8 data, err;

    do
    {
        data = QueueAccept_OutInt(&GpsRxQueueCtrl, (u8 *)GpsRxQueueBuf, &err);

        if(err == Q_OPT_SUCCEED)
        {

            if( (debug == DEBUGGPS) || (debug == DEBUGANT))
            {
                PrintUsart(data);
            }

            GetGpsDataPackage(data);


        }
    }
    while (err == Q_OPT_SUCCEED);

    return err;
}
/*
*********************************************************************************************************
*   函 数 名: u8 ChangeGpsToHostMode(void)
*   功能说明:模式切换
*
*
*   形    参：

*   返 回 值: 0xaa  ----successs   0x55 ------>timeout   0xa5-----watint
*********************************************************************************************************
*/

u8 ChangeGpsToHostMode(void)
{
    const u8 code1[] = "$PSRF120,F,H*2A\r\n";

    switch(GpsControlStu.ConTrolStu)
    {
        case 0:
        case 1:
        case 2:
            if( (GpsControlStu.ConTrolStu > 0) && (GpsControlStu.result == GPS_1501))
            {
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.result = GPS_EMPTY;
                return RETURN_SUCCESS;
            }

            if(GpsControlStu.addsecond == 0)
            {
                GPSUsartSendStr((u8 *)code1, sizeof(code1) - 1);
                GpsControlStu.addsecond = 10;
                GpsControlStu.ConTrolStu++;
            }

            return RETURN_WAITING;

        default:
            GpsControlStu.ConTrolStu = 0;
            return RETURN_TIMEOUT;

    }


}




/*
*********************************************************************************************************
*   函 数 名: u8 ChangeGpsToOSP(void)
*   功能说明:切换到OSP模式
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/
u8 GetOspCode(u8 *cod1)
{


    u8 len = 0;
    u8 checksum = 0, i;


    Mymemcpy(&cod1[len], "$PSRF100,0,", 11);
    len = 11;
    Mymemcpy(&cod1[len], "38400,", 6);
    len += 6;

    Mymemcpy(&cod1[len], "8,1,0*", 6);
    len += 6;

    for(i = 1; i < (len - 1); i++)
    {
        checksum ^= cod1[i];
    }

    cod1[len++] = HexToAscll((checksum >> 4) & 0x0f, 0);
    cod1[len++] = HexToAscll(checksum & 0x0f, 0);
    cod1[len++] = 0x0d;
    cod1[len++] = 0x0a;
    return len;
}

/*
*********************************************************************************************************
*   函 数 名: void  BackToNMEA(u32 band,u8 interval)
*   功能说明:切换到NMEA
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/
void  BackToNMEA(u32 band, u8 interval)
{

#if 0
    u8 cod1[32] = {0xA0, 0xA2, 0x00, 0x18, 0x81, 0x02, 0x01, 0x01, 0x00, 0x01,
                   0x01, 0x01, 0x05, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01,
                   0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0xb0, 0xb3
                  };
    uc8  cos4800[4] = {0x12, 0xc0, 0x01, 0x65};
    uc8  cos9600[4] = {0x25, 0x80, 0x01, 0x38};
    uc8  cos115200[4] = {0xc2, 0x00, 0x01, 0x55};
#endif

    u8 buf[35];
    u8 len = 0;
    u8 i;
    U32STU tmp;
    u16 checksum;
    //head
    buf[len++] = 0xa0;
    buf[len++] = 0xa2;

    //len
    buf[len++] = 0x00;
    buf[len++] = 0x18;

    //body
    buf[len++] = 0x81;
    buf[len++] = 0x01;/*00--->ENABLE NMEA DEBUG MSG
  01--->DISABLE NMEA DEBUG MSG
  02--->Do Not Change last-set value for  NMEA DEBUG MSG
  */

    buf[len++] = interval * 2; /*GGA */
    buf[len++] = 0x01;

    buf[len++] = 0x00; /*GLL*/
    buf[len++] = 0x01;

    buf[len++] = 0x00; /*GSA*/
    buf[len++] = 0x01;
    buf[len++] = interval * 2; /*GSV*/
    buf[len++] = 0x01;
    buf[len++] = interval; /*RMC*/
    buf[len++] = 0x01;
    buf[len++] = 0x00; /*VTG*/
    buf[len++] = 0x01;
    buf[len++] = 0x00; /*MSS*/
    buf[len++] = 0x01;
    buf[len++] = 0x00; /*EPE*/
    buf[len++] = 0x01;
    buf[len++] = 0x00; /*ZDA*/
    buf[len++] = 0x01;

    buf[len++] = 0x00; /*resv*/
    buf[len++] = 0x00; /*resv*/

    buf[len++] = (u8)(band >> 8); /*baud*/
    buf[len++] = (u8)band;

    //checksum
    checksum = 0;

    for(i = 4; i < len; i++)
    {
        checksum += buf[i];
        checksum &= 0x7fff;

    }

    tmp.total = checksum;
    buf[len++] = tmp.stu.Lower; //period
    buf[len++] = tmp.stu.Low; //period

    //end
    buf[len++] = 0xb0; //period
    buf[len++] = 0xb3; //period
    GPSUsartSendStr(buf, len);
}

/*
*********************************************************************************************************
*   函 数 名: u8 ChangeGpsToTricklePowerMode(u8 period_sec,u8 ontime_ms,u8 sleeptime_sec,u8 searchtime_sec)
*   功能说明:改变电源模式
*
*ChangeGpsToTricklePowerMode(500,900,30,120);
ChangeGpsToTricklePowerMode(1000,200,5,300);
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/
//ChangeGpsToTricklePowerMode(500,1000,30,120);
void  ChangeGpsToTricklePowerMode(u16 period_sec, u16 ontime_ms, u8 sleeptime_sec, u16 searchtime_sec)
{
#if 0
    u8 buf[30];
    u8 len = 0;
    u8 i;
    U32STU tmp;
    u16 checksum;
    //head
    buf[len++] = 0xa0;
    buf[len++] = 0xa2;

    //len
    buf[len++] = 0x00;
    buf[len++] = 0x10;

    //body
    buf[len++] = 0xda; //mid
    buf[len++] = 0x03; //sid

    if(period_sec != 1000)
    {
        tmp.total = period_sec;
    }
    else
    {
        tmp.total = 1000;
    }

    buf[len++] = tmp.stu.Lower; //period
    buf[len++] = tmp.stu.Low; //period

    tmp.total = ontime_ms;
    buf[len++] = tmp.stu.Highter; //on_time
    buf[len++] = tmp.stu.Hight; //On_time
    buf[len++] = tmp.stu.Lower; //on_time
    buf[len++] = tmp.stu.Low; //On_time

    tmp.total = sleeptime_sec;
    tmp.total *= 1000;
    buf[len++] = tmp.stu.Highter; //Sleep
    buf[len++] = tmp.stu.Hight; //Sleep
    buf[len++] = tmp.stu.Lower; //Sleep
    buf[len++] = tmp.stu.Low; //Sleep
    tmp.total = searchtime_sec;
    tmp.total *= 1000;
    buf[len++] = tmp.stu.Highter; //search
    buf[len++] = tmp.stu.Hight; //search
    buf[len++] = tmp.stu.Lower; //search
    buf[len++] = tmp.stu.Low; //search


    //checksum
    checksum = 0;

    for(i = 4; i < len; i++)
    {
        checksum += buf[i];
        checksum &= 0x7fff;

    }

    tmp.total = checksum;
    buf[len++] = tmp.stu.Lower; //period
    buf[len++] = tmp.stu.Low; //period

    //end
    buf[len++] = 0xb0; //period
    buf[len++] = 0xb3; //period
    GPSUsartSendStr(buf, len);
#else
    const u8 buf[17] = {0xa0, 0x02, 0x00, 0x09, 0x97, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0xc8, 0x01, 0x73, 0xb0, 0xb3};
    GPSUsartSendStr((u8 *)buf, 17);

#endif

}

/*
*********************************************************************************************************
*   函 数 名: void  SendSmoothMode(STU_SMOOTH StuSmooth)
*   功能说明:smoothmode
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/


void  SendSmoothMode(STU_SMOOTH StuSmooth)
{

    u8 buf[30];
    u8 len = 0;
    u8 i;
    U32STU tmp;
    u16 checksum;
    //head
    buf[len++] = 0xa0;
    buf[len++] = 0xa2;

    //len
    buf[len++] = 0x00;
    buf[len++] = 0x0e;

    //mid
    buf[len++] = 0x88;

    //reserve
    buf[len++] = 0x00;
    buf[len++] = 0x00;

    //degragemode
    buf[len++] = StuSmooth.degragemode;
    //position_calc_mode
    buf[len++] = StuSmooth.position_calc_mode;

    //reserve
    buf[len++] = 0x00;

    //altitude
    buf[len++] = (u8)(StuSmooth.altitude >> 8);
    buf[len++] = (u8)(StuSmooth.altitude >> 0);

    //alt_hold_mode
    buf[len++] = StuSmooth.alt_hold_mode;
    //alt_hold_source
    buf[len++] = StuSmooth.alt_hold_source;

    //reserve
    buf[len++] = 0x00;
    //degrade_time_out
    buf[len++] = StuSmooth.degrade_time_out;

    //dr_time_out
    buf[len++] = StuSmooth.dr_time_out;
    //mersurement_track_smoothing
    buf[len++] = StuSmooth.mersurement_track_smoothing;




    //checksum
    checksum = 0;

    for(i = 4; i < len; i++)
    {
        checksum += buf[i];
        checksum &= 0x7fff;

    }

    tmp.total = checksum;
    buf[len++] = tmp.stu.Lower; //period
    buf[len++] = tmp.stu.Low; //period

    //end
    buf[len++] = 0xb0; //period
    buf[len++] = 0xb3; //period
    GPSUsartSendStr(buf, len);


}

/*
*********************************************************************************************************
*   函 数 名: u8 ChangeGpsToOSP(void)
*   功能说明:切换到OSP模式
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/
u8 ChangeGpsToOSP(void)
{
    u8 data[50];
    u8 len;

    //GpsControlStu.GpsNoDateTime
    switch(GpsControlStu.ConTrolStu)
    {
        case 0:
        case 1:
        case 2:
            if( (GpsControlStu.ConTrolStu > 0) && ( (OspStu.flag != OSP_NO_FRAM) || (GpsControlStu.GpsNoDateTime > 10)))
            {
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.result = GPS_EMPTY;
                return OK;
            }

            if(GpsControlStu.addsecond == 0)
            {
                len = GetOspCode(data);
                GPSUsartSendStr(data, len);
                GpsControlStu.GpsNoDateTime = 0;
                GpsControlStu.addsecond = 10;
                GpsControlStu.ConTrolStu++;
            }

            return WAIT;

        default:
            GpsControlStu.ConTrolStu = 0;
            return NOT_OK;

    }


}
/*
*********************************************************************************************************
*   函 数 名: u8 ChangeGpsToOSP(void)
*   功能说明:切换到OSP模式
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/
u8 ChangeGpsToTricmode(void)
{


    //GpsControlStu.GpsNoDateTime
    switch(GpsControlStu.ConTrolStu)
    {
        case 0:
        case 1:
        case 2:
            if( (GpsControlStu.ConTrolStu > 0) && ( (OspStu.flag == OSP_TRICK_OK) || (GpsControlStu.GpsNoDateTime > 10)))
            {
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.result = GPS_EMPTY;
                return OK;
            }

            if(GpsControlStu.addsecond == 0)
            {
                OspStu.flag = OSP_WAIT_ACK;
                ChangeGpsToTricklePowerMode(300, 300, 60, 120);
                GpsControlStu.GpsNoDateTime = 0;
                GpsControlStu.addsecond = 10;
                GpsControlStu.ConTrolStu++;
            }

            return WAIT;

        default:
            GpsControlStu.ConTrolStu = 0;
            return NOT_OK;

    }


}

/*
*********************************************************************************************************
*   函 数 名: u8 ChangeGpsToSmoothMode(void)
*   功能说明:切换到OSP模式
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/
u8 ChangeGpsToSmoothMode(void)
{
    STU_SMOOTH StuSmooth =
    {

        .degragemode = 0x04,
        .position_calc_mode = 0x03,
        .altitude = 0x0000,
        .alt_hold_mode = 0x00,
        .alt_hold_source = 0x00,
        .degrade_time_out = 0x05,
        .dr_time_out = 0x02,
        .mersurement_track_smoothing = 0x1
    };

    //GpsControlStu.GpsNoDateTime
    switch(GpsControlStu.ConTrolStu)
    {
        case 0:
        case 1:
        case 2:
            if( (GpsControlStu.ConTrolStu > 0) && ( (OspStu.flag == OSP_FRAM_SMOOTH_ACK) || (GpsControlStu.GpsNoDateTime > 10)))
            {
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.result = GPS_EMPTY;
                return OK;
            }

            if(GpsControlStu.addsecond == 0)
            {
                OspStu.flag = OSP_WAIT_ACK;
                SendSmoothMode(StuSmooth);
                GpsControlStu.GpsNoDateTime = 0;
                GpsControlStu.addsecond = 10;
                GpsControlStu.ConTrolStu++;
            }

            return WAIT;

        default:
            GpsControlStu.ConTrolStu = 0;
            return NOT_OK;

    }


}

/*
*********************************************************************************************************
*   函 数 名: u8 ChangeGpsToOSP(void)
*   功能说明:切换到OSP模式
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/
u8 ChangeNmeaMode(void)
{


    //GpsControlStu.GpsNoDateTime
    switch(GpsControlStu.ConTrolStu)
    {
        case 0:
        case 1:
        case 2:
            if( (GpsControlStu.ConTrolStu > 0) && ( (OspStu.flagack != OSP_WAIT_NMEA_OK) || (GpsControlStu.GpsNoDateTime < 10)))
            {
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.result = GPS_EMPTY;
                return OK;
            }

            if(GpsControlStu.addsecond == 0)
            {
                //BackToNMEA(38400,1);
                BackToNMEA(GPS_IPR, 1);
                GpsControlStu.GpsNoDateTime = 50;
                GpsControlStu.addsecond = 10;
                GpsControlStu.ConTrolStu++;
            }

            return WAIT;

        default:
            GpsControlStu.ConTrolStu = 0;
            return NOT_OK;

    }


}



/*
*********************************************************************************************************
*   函 数 名: u8 GpsStardown(void)
*   功能说明:启动SGEE
*
*
*   形    参：

*   返 回 值: 0xaa  ----successs   0x55 ------>timeout   0xa5-----watint
*********************************************************************************************************
*/
u8 GpsStardown(void)
{
    const u8  code1[] = "$PSRF114,16,0*14\r\n";

    switch(GpsControlStu.ConTrolStu)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            if( (GpsControlStu.ConTrolStu > 0) && (GpsControlStu.result == GPS_ACK_START))
            {
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.addsecond = 0;
                GpsControlStu.result = GPS_EMPTY;
                return RETURN_SUCCESS;
            }

            if(GpsControlStu.addsecond == 0)
            {
                GPSUsartSendStr((u8 *)code1, sizeof(code1) - 1);
                GpsControlStu.addsecond = 3;
                GpsControlStu.ConTrolStu++;
                GpsControlStu.result = GPS_EMPTY;
            }

            return RETURN_WAITING;

        default:
            GpsControlStu.ConTrolStu = 0;
            return RETURN_TIMEOUT;

    }


}
/*
*********************************************************************************************************
*   函 数 名: void SendGpsFileSize(void)
*   功能说明:发送请求包数据
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/
void SendGpsFileSize(void)
{
    u8 len, checksum, i;
    u8 code1[30] = "$PSRF114,17,";

    len = 12;
    /*lenth*/
    code1[len++] = HexToAscll(( GpsStatues.offset >> 12) & 0x0f, 0);
    code1[len++] = HexToAscll(( GpsStatues.offset >> 8) & 0x0f, 0);
    code1[len++] = HexToAscll(( GpsStatues.offset >> 4) & 0x0f, 0);
    code1[len++] = HexToAscll(( GpsStatues.offset >> 0) & 0x0f, 0);
    /*checksum*/
    code1[len++] = '*';
    checksum = 0;

    for(i = 1; i < (len - 1); i++)
    {
        checksum ^= code1[i];
    }

    code1[len++] = HexToAscll((checksum >> 4) & 0x0f, 0);
    code1[len++] = HexToAscll(checksum & 0x0f, 0);
    code1[len++] = 0x0d;
    code1[len++] = 0x0a;
    GPSUsartSendStr((u8 *)code1, len);

}

/*
*********************************************************************************************************
*   函 数 名: u8 GpsFileSize(void)
*   功能说明:告诉要传输的文件的大小
*
*
*   形    参：

*   返 回 值: 0xaa  ----successs   0x55 ------>timeout   0xa5-----watint
*********************************************************************************************************
*/
u8 GpsFileSize(void)
{
    switch(GpsControlStu.ConTrolStu)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            if( (GpsControlStu.ConTrolStu > 0) && (GpsControlStu.result == GPS_ACK_FILESIZE))
            {
                GpsControlStu.addsecond = 0;
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.result = GPS_EMPTY;
                return RETURN_SUCCESS;
            }

            if(GpsControlStu.addsecond == 0)
            {
                SendGpsFileSize();
                GpsControlStu.addsecond = 3;
                GpsControlStu.ConTrolStu++;
                GpsControlStu.result = GPS_EMPTY;
            }

            return RETURN_WAITING;

        default:
            GpsControlStu.ConTrolStu = 0;
            return RETURN_TIMEOUT;

    }


}
/*
*********************************************************************************************************
*   函 数 名: void ReadNetSGEE(u8*outbuf ,u32 offset ,u16 len)///
*   功能说明: 读原始SGEE星历数据
*
*
*   形    参：
*   返 回 值:
*********************************************************************************************************
*/
void ReadNetSGEE(u8 *outbuf, u32 offset, u16 len) ///
{

    FLASH_ReadDate(NET_SGEE_ADDR + offset, len, outbuf);

}

/*
*********************************************************************************************************
*   函 数 名: void WriteNetSGEEToFlash(u8 *datain,u16 datelen,u32 offset )///
*   功能说明:
*
*
*   形    参：
*   返 回 值:
*********************************************************************************************************
*/
void WriteNetSGEEToFlash(u8 *datain, u16 datelen, u32 offset ) ///
{

    FLASH_WriteDate(NET_SGEE_ADDR + offset, datelen, datain);
}

/*
*********************************************************************************************************
*   函 数 名: void EraseNetSGEEToFlash(void)
*   功能说明:
*
*
*   形    参：
*   返 回 值:
*********************************************************************************************************
*/
void EraseNetSGEEToFlash(void)
{

    u32 i;

    for(i = 0; i < NET_SGEE_SEC; i++)
    {
        FLASH_eraseOneBlock(NET_SGEE_ADDR + 2048 * i);
    }

}


/*
*********************************************************************************************************
*   函 数 名: void SendGpsFileInTurn(u16 packegturn)
*   功能说明:发送请求包数据
*
*
*   形    参：

*   返 回 值:
*********************************************************************************************************
*/

void SendGpsFileInTurn(u16 packegturn)
{
#define BUF_LEN  500
    u16 len, i;
    u16 packegsize;
    u8 checksum;
    u8 code1[BUF_LEN] = "$PSRF114,18,";
    len = 12;


    /*squence decim*/
    if(packegturn > 99)
    {
        code1[len++] = HexToAscll(packegturn / 100, 0);
    }

    if(packegturn > 9)
    {
        code1[len++] = HexToAscll(packegturn % 100 / 10, 0);
    }

    code1[len++] = HexToAscll(packegturn % 10, 0);



    code1[len++] = ',';

    /*length of this packeg*/
    if(GpsStatues.U_TatolPackeg == packegturn)
    {
        packegsize = GpsStatues.LastPackegSize;
    }
    else
    {
        packegsize = SEND_SGEE_FILE_SIZE;
    }

    code1[len++] = HexToAscll(packegsize / 100, 0);
    code1[len++] = HexToAscll(packegsize % 100 / 10, 0);
    code1[len++] = HexToAscll(packegsize % 10, 0);
    code1[len++] = ',';
    /*packeg data*/
    ReadNetSGEE(&code1[BUF_LEN - packegsize], (packegturn - 1)*SEND_SGEE_FILE_SIZE, packegsize);

    for(i = 0; i < packegsize; i++)
    {
        checksum = code1[BUF_LEN - packegsize + i];

        if(checksum > 0x0f)
        {
            code1[len++] = HexToAscll((checksum >> 4) & 0x0f, 0);
        }

        code1[len++] = HexToAscll(checksum & 0x0f, 0);
        code1[len++] = ',';

    }

    len--;
    /*checksum*/
    code1[len++] = '*';
    checksum = 0;

    for(i = 1; i < (len - 1); i++)
    {
        checksum ^= code1[i];
    }

    code1[len++] = HexToAscll((checksum >> 4) & 0x0f, 0);
    code1[len++] = HexToAscll(checksum & 0x0f, 0);
    code1[len++] = 0x0d;
    code1[len++] = 0x0a;
    GPSUsartSendStr((u8 *)code1, len);


}

/*
*********************************************************************************************************
*   函 数 名: u8 GpsSendFile(void)
*   功能说明:传输SGEE文件到GPS
*
*
*   形    参：

*   返 回 值: 0xaa  ----successs   0x55 ------>timeout   0xa5-----watint
*********************************************************************************************************
*/
u8 GpsSendFile(void)
{

    switch(GpsControlStu.ConTrolStu)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            if(GpsControlStu.result == GPS_SEND_PACKEG_ACK)
            {
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.addsecond = 5;
                GpsControlStu.result = GPS_EMPTY;
                GpsStatues.U_CurrentPacket++;

                if(GpsStatues.U_CurrentPacket < (GpsStatues.U_TatolPackeg + 1))
                {
                    SendGpsFileInTurn(GpsStatues.U_CurrentPacket);
                    return RETURN_WAITING;
                }

                return RETURN_SUCCESS;
            }

            if(GpsControlStu.addsecond == 0)
            {
                GpsControlStu.result = GPS_EMPTY;
                SendGpsFileInTurn(GpsStatues.U_CurrentPacket);
                GpsControlStu.addsecond = 4;
                GpsControlStu.ConTrolStu++;
            }

            return RETURN_WAITING;

        default:
            GpsControlStu.ConTrolStu = 0;
            return RETURN_TIMEOUT;

    }



}
/*
*********************************************************************************************************
*   函 数 名:void CommandInit(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void CommandInit(void)
{
    GpsControlStu.result = GPS_EMPTY;
    GpsControlStu.ConTrolStu = 0;
    GpsControlStu.addsecond = 0;
    OspStu.flag = OSP_NO_FRAM;
    GpsControlStu.stu = 0;



}
/*
*********************************************************************************************************
*   函 数 名:u8 GetGsv(void)
*   功能说明: 获得gps信号强度
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
u8 GetGsv(void)
{
    //(strGpsData.bValidity=='A') //GpsGsv
    switch(GpsControlStu.stu)
    {
        case 0:
            if(strGpsData.bValidity != 'A')
            {
                Memset(&GpsGsv.maxsv[0], 0, 3);
                GpsControlStu.FindTime = FIND_GSV_TIME;
                GpsControlStu.stu++;
            }

            return NOT_OK;

        case 1:
            if(strGpsData.bValidity == 'A')
            {
                GpsControlStu.stu = 0;
            }
            else if(!GpsControlStu.FindTime)  /*查找信号强度的时间到*/
            {
                GpsControlStu.stu = 0;
                GpsControlStu.Low10Gsv = NOT_OK;
                GpsControlStu.HightGsv = NOT_OK;
                GpsControlStu.Low20Gsv = NOT_OK;

                if( (GpsGsv.maxsv[0] < 10) && (GpsGsv.maxsv[1] < 10) && (GpsGsv.maxsv[2] < 10))
                {
                    GpsControlStu.Low10Gsv = OK;
                    return OK;
                }
                else        if( (GpsGsv.maxsv[0] < 20) && (GpsGsv.maxsv[1] < 20) && (GpsGsv.maxsv[2] < 20))
                {
                    GpsControlStu.Low20Gsv = OK;
                    return OK;
                }
                else
                {
                    GpsControlStu.HightGsv = OK;
                    return OK;
                }


            }

            return NOT_OK;

        default :
            GpsControlStu.stu = 0;
            return NOT_OK;


    }



}

/*
*********************************************************************************************************
*   函 数 名:u8 SetGpsStatus(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
u8 SetGpsStatus(void)
{
    u8   gpsStu, GpsMonitor;
    u8   interval  = GsmSto.moveintervalGPS >  60 ? 60 : 30;

    if ((timer.counter > GsmSta.BasicPositionInter) && ((timer.counter - GsmSta.BasicPositionInter + 30) >= (u32)GsmSto.moveintervalGPS))
        // if(timer.counter - GsmSta.BasicPositionInter + interval >= (u32)GsmSto.moveintervalGPS)
    {
        gpsStu = 7;
    }
    else
    {
        gpsStu = 5;
    }

    GpsMonitor = GPIO_PinInGet( GPS_MON_PORT, GPS_MON_PIN );

    if (((GpsMonitor == GPS_ON) && (gpsStu == 7)) || ((GpsMonitor == GPS_OFF) && (gpsStu == 5)))
    {
        gpsStu = 0;
    }

    return gpsStu;
}

/*
*********************************************************************************************************
*   函 数 名:void GpsTask(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
//#pragma optimize=none

void GpsTask(void)
{
    static u8 stuask = 0, stu = 0;
    static u16 waittime = 0;
    u8 i;
#ifdef USE_PRINTF
    static u8 ime5s = 0;

    if((debug == DEBUGGPS) && (ime5s++ > 49))
    {
        ime5s = 0;
        myprintf("debug gps.................\r\n");
    }

#endif
    GetGpsDate();
    //记住进入ldo mode 35ma   switch mode 28ma

    if ((GsmSto.updateflag == OK) || (resetflag == 0xaa))
    {
        GpsStatues.SgeeState = AGPS_IDLE;
        return;
    }

    if((GsmSta.gps_p & 0x02) == 0x02)
    {
        GsmSta.gps_p = 0;
        stu = 100; /*enter reset mode*/
    }
    else if(  (GsmSta.gps_p & 0x04) == 0x04)
    {
        // GpsPowerOff();
        GpsControlStu.GpsNoDateTime = 0;

        if( 5 != stu )
        {
            stu = 5;
            //    GsmSta.gps_p = 0;
        }
        else
        {
            return;
        }
    }
    else if(  (GsmSta.gps_p & 0x08) == 0x08)
    {
        stu = 7;
        GpsControlStu.GpsNoDateTime = 0;
        GsmSta.gps_p = 0;
    }

    switch(stu)
    {
        case 0:/*open gps*/
            if ((ReadPower() == 0) && (GPS_OFF == GpsReadMon()))
            {
                GpsPowerOn();
            }

            stu++;
            waittime = 50;
            stuask = 2;
            break;

        case 1:/*wait process*/
            if(waittime)
            {
                waittime--;
            }
            else
            {
                stu = stuask;
            }

            break;

        case 2:/*wakeup gps*/
            i = WakeUpGps(GPS_WAKEUP);

            if (OK == i)
            {
                GPS_RF_ON();

                stu = 1;
                waittime = 30;
                //if(debug!=DEBUGANT)
                //    stuask=11;
                //else
                stuask = 3;
                CommandInit();
#ifdef DEBUG_GPS_ANT
                stuask = 3;
#endif

            }
            else if (NOT_OK == i)
            {
                stu = 100; /*restart gps*/
            }

            break;

        case 11://进入osp
            i = ChangeGpsToOSP();

            if(OK == i)
            {
                stu = 12;
                CommandInit();
                OspStu.flagack = OSP_WAIT_TO_TRIC;
                OspStu.flag = OSP_NO_FRAM;
            }
            else   if(NOT_OK == i)
            {
                stu = 100; /*restart gps*/
            }

            break;

        case 12://进入tric mode
            i = ChangeGpsToTricmode();

            if(OK == i)
            {
                stu = 13;
                CommandInit();
                OspStu.flag = OSP_WAIT_ACK;
            }
            else   if(NOT_OK == i)
            {
                stu = 100; /*restart gps*/
            }

            break;

        case 13://进入smooth模式
            i = ChangeGpsToSmoothMode();

            if(OK == i)
            {
                stu = 14;
                CommandInit();
                OspStu.flagack = OSP_WAIT_NMEA;
            }
            else   if(NOT_OK == i)
            {
                stu = 100; /*restart gps*/
            }

            break;


        case 14://进入tric mode
            i = ChangeNmeaMode();

            if(OK == i)
            {
                stu = 3;
                CommandInit();
                GpsControlStu.GpsNoDateTime = 0;
            }
            else   if(NOT_OK == i)
            {
                stu = 100; /*restart gps*/
            }

            break;

        case 3:/*切换保存EE数据的位置*/
            if(OK == ReadSGEEHavedate())
            {
                i = ChangeGpsToHostMode();
            }
            else
            {
                i = RETURN_SUCCESS;
            }

            if(i == RETURN_SUCCESS)
            {
                GPS_RF_ON();
                CommandInit();
                GpsStatues.SgeeState = AGPS_IDLE;
                stu++;
            }
            else  if(i == RETURN_TIMEOUT)
            {
                stu = 100;
                CommandInit();
            }

            break;

        case 4:/*gps管理中心*/
#ifndef DEBUG_GPS_ANT

            /*休眠管理*/
            if (LocSuccess && (debug != DEBUGANT) && (stu = SetGpsStatus())) /* stu==5 Enter sleep; 7 Enter wakeup */
            {
                GpsControlStu.ConTrolStu = 0;
                GpsControlStu.result = GPS_EMPTY;
                break;
            }

            stu = 4;
#endif
            /*重启管理*/
#if 1

            if ((LocSuccess == 0) && ((GpsControlStu.GpsNoDateTime > GsmSto.moveintervalGPS) || (GpsControlStu.GpsUnfixedTime > 600))) /*50s   600s/60=12min*/
            {
                GPS_RF_OFF();
                GpsPowerOff();
                stu = 1;
                waittime = 3000; /* 5min */
                stuask = 100;
                break;
            }

#endif

            /*灌AGPS数据到gps*/
            if ((GpsStatues.SgeeState == AGPS_NEED) && (GsmSto.updateflag != OK))
            {
                /*当刚刚更新完agps文件，或者有请求且有数据*/
                stu = 8; /*Down Agps to Gps Mode*/
                CommandInit();
                break;
            }

            break;

        case 5:/*进入到休眠*/
            i = WakeUpGps(GPS_SLEEP);

            if (OK == i)
            {
#ifndef GPS_USE_UART
                initLeuart(GPS_UART, GPS_IPR, POWER_OFF);
#else
                uartSetup(GPS_UART, GPS_IPR, POWER_OFF);
#endif

                GPS_RF_OFF() ;
                stu = 4;/*返回*/
                CommandInit();
#ifdef USE_PRINTF

                if(debug == DEBUGGPS)
                {
                    myprintf("GPS success goto sleep\r\n");
                }

#endif
            }
            else if (NOT_OK == i)
            {
                stu = 4; /*返回*/
#ifdef USE_PRINTF

                if(debug == DEBUGGPS)
                {
                    myprintf("GPS fail goto sleep\r\n");
                }

#endif
            }

            break;

        case 6:

#if 0
            if(GpsControlStu.sleepManageTime == NO_SLEEP)
            {
                // if( (AdxlStu.state==ADXL_MOVE)&&(StuKey.SystemState!=SYSTEM_OFF))/*切换到休眠模式*/
                if (StuKey.SystemState != SYSTEM_OFF)
                {
                    GPS_RF_ON() ;

#ifndef GPS_USE_UART
                    initLeuart(GPS_UART, 38400, POWER_ON);
#else
                    uartSetup(GPS_UART, 38400, POWER_ON);
#endif
                    stu++;/*goto GPS WakeUp*/
                    CommandInit();

                }
            }
            else if(GpsControlStu.sleepManageTime == 0)
            {
                GPS_RF_ON() ;
#ifndef GPS_USE_UART
                initLeuart(GPS_UART, 38400, POWER_ON);
#else
                uartSetup(GPS_UART, 38400, POWER_ON);
#endif
                stu++;/*goto GPS WakeUp*/
                CommandInit();

            }

#endif

            stu = 1;
            waittime = 3200;
            stuask = 7;

#ifndef GPS_USE_UART
            initLeuart(GPS_UART, GPS_IPR, POWER_ON);
#else
            uartSetup(GPS_UART, GPS_IPR, POWER_ON);
#endif

            break;


        case 7:/*wakeup gps*/
            i = WakeUpGps(GPS_WAKEUP);

            if(OK == i)
            {
#ifndef GPS_USE_UART
                initLeuart(GPS_UART, GPS_IPR, POWER_ON);
#else
                uartSetup(GPS_UART, GPS_IPR, POWER_ON);
#endif

                GPS_RF_ON();

                if(GpsControlStu.sleepManageTime == NO_SLEEP)
                {
                    stu = 4; /*back to normal mode 4*/
                    CommandInit();
                }
                else
                {
                    GpsControlStu.sleepManageTime = 0xfd;
                    GpsControlStu.GpsNoDateTime = 0;
                    GpsControlStu.GpsUnfixedTime = 0;
                    stu = 4; /*back to normal mode 4*/
                    CommandInit();
                }

#ifdef USE_PRINTF

                if(debug == DEBUGGPS)
                {

                    myprintf("GPS success wake up\r\n");
                }

#endif
            }
            else   if(NOT_OK == i)
            {
                stu = 100; /*restart gps*/
#ifdef USE_PRINTF

                if(debug == DEBUGGPS)
                {

                    myprintf("GPS fali to wake up,reset gps\r\n");
                }

#endif
            }

            break;

        case 8:/*Down Agps to Gps Mode 1*/
            i = GpsStardown();

            if(i == RETURN_SUCCESS)
            {
                stu++;
                CommandInit();
            }
            else if(i == RETURN_TIMEOUT)
            {

                stu = 4; /*back to manage state*/
                CommandInit();
                GpsStatues.SgeeState = AGPS_IDLE;
            }

            break;

        case 9:/*Down Agps to Gps Mode 2*/
            i = GpsFileSize();

            if(i == RETURN_SUCCESS)
            {
                stu++;
                CommandInit();
                EraseSGEE();
                InitSendDateToGps();
            }
            else if(i == RETURN_TIMEOUT)
            {
                stu = 4; /*back to manage state*/
                CommandInit();
                GpsStatues.SgeeState = AGPS_IDLE;
            }

            break;

        case 10:/*Down Agps to Gps Mode 3*/
            i = GpsSendFile();

            if(i != RETURN_WAITING)
            {
                stu = 4; /*back to manage state*/
                CommandInit();

                GpsStatues.SgeeState = AGPS_IDLE;
            }

            break;

        case 100:/*restart gps*/
            GpsPowerOff();
            waittime = 50;
            stuask = 0;
            GpsControlStu.GpsNoDateTime = 0;
            GpsControlStu.GpsUnfixedTime = 0;
            CommandInit();
            GpsStatues.SgeeState = AGPS_IDLE;
            stu = 1;
            break;

        default:
            stu = 0;
            GpsControlStu.ConTrolStu = 0;
            break;
    }

}
/*
*********************************************************************************************************
*   函 数 名:u8  WakeUpGps(u8 wakeup)
*   功能说明:  GPS_SLEEP  GPS_WAKEUP
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: OK wakeup ok   NOT_OK wakeup fail
*********************************************************************************************************
*/
//#pragma optimize=none
u8  WakeUpGps(u8 wakeup)
{

    static  u8 i;
    i = GpsReadMon();

    if (((GPS_ON == i) && (wakeup == GPS_WAKEUP)) || ((GPS_ON != i) && (wakeup == GPS_SLEEP)))
    {
        GpsControlStu.ConTrolStu = 0;
        GpsOnOff_0();
        return OK;
    }

    if (GpsControlStu.ConTrolStu == 0)
    {
        GpsOnOff_1();
        GpsControlStu.ConTrolStu++;
    }
    else if (GpsControlStu.ConTrolStu < 30)
    {
        GpsControlStu.ConTrolStu++;
        return WAIT;
    }
    else
    {
        GpsControlStu.ConTrolStu = 0;
        GpsOnOff_0();
        return NOT_OK;
    }

    return WAIT;
}

/*
*********************************************************************************************************
*   函 数 名:void GpsDateIn(u8 datein)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/

void GpsDateIn(u8 datein)
{

    u8 err;




    //==================================================
    err = QueuePost(&GpsRxQueueCtrl, (u8 *)GpsRxQueueBuf, datein);

    if(err != Q_OPT_SUCCEED)
    {
        QueueFlush(&GpsRxQueueCtrl, (u8 *)GpsRxQueueBuf);
    }


}
/*
*********************************************************************************************************
*   函 数 名:double rad(double d)
*   功能说明:计算弧度
*
*
*   形    参：
*   返 回 值:
*********************************************************************************************************
*/

double rad(double d)
{
    const double pi = 3.1415926535898;
    return d * pi / 180.0;
}

/*
*********************************************************************************************************
*   函 数 名:int CalcDistance(double fLati1, double fLong1, double fLati2, double fLong2)
*   功能说明://从两个gps坐标点（经纬度）获得两点的直线距离，单位是米
*
*
*   形    参：
*   返 回 值:
*********************************************************************************************************
*/
int CalcDistance(double fLati1, double fLong1, double fLati2, double fLong2)
{
    double s = 0, lambda1 = 0, lambda2 = 0, phi1 = 0, phi2 = 0, x1 = 0, x2 = 0, y1 = 0, y2 = 0, z1 = 0, z2 = 0, A = 0, tmp;
    int distance = 0;
    const float EARTH_RADIUS = 6378.137;

    phi1 = rad(fLati1);
    lambda1 = rad(fLong1);
    phi2 = rad(fLati2);
    lambda2 = rad(fLong2);
    x1 = cos(phi1) * cos(lambda1);
    y1 = cos(phi1) * sin(lambda1);
    z1 = sin(phi1);
    x2 = cos(phi2) * cos(lambda2);
    y2 = cos(phi2) * sin(lambda2);
    z2 = sin(phi2);
    tmp = (x1 * x2 + y1 * y2 + z1 * z2);
    A = acos(tmp);
    s = A * EARTH_RADIUS;
    distance = (int)(s * 1000);

    if(distance < 0)
    {
        distance = -distance;
    }

    return distance;
}






