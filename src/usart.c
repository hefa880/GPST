#include"usart.h"


#if 0
STRUCT_QUEUE_CTRL GpsRxQueueCtrl;					//gps接收队列控制结构体
volatile u8 GpsRxQueueBuf[GPS_REC_QUEUE_LENGTH];	//gps接收队列缓存
QueueInit(&GpsRxQueueCtrl,(u8 *)GpsRxQueueBuf,GPS_REC_QUEUE_LENGTH);//队列初始化


u8  GetGpsDate(void)
{
    u8 data,err;
    GpsStatues.AnaGps=2;
    do
    {
        data=QueueAccept_OutInt(&GpsRxQueueCtrl,(u8 *)GpsRxQueueBuf,&err);
        if(err==Q_OPT_SUCCEED)
        {
            GetGpsDataPackage(data);
        }
    }
    while ((GpsStatues.AnaGps)&&(err==Q_OPT_SUCCEED));

    return err;
}




err=QueuePost(&GpsRxQueueCtrl,(u8 *)GpsRxQueueBuf,data);

if(err!=Q_OPT_SUCCEED)
{
    QueueFlush(&GpsRxQueueCtrl,(u8 *)GpsRxQueueBuf);
}

#endif
/*
*********************************************************************************************************
*	函 数 名:void initLeuart(LEUART_TypeDef *leuart,u32 baudrate,u8 powerflag)
*	功能说明: luart初始化
*
*       作    者 ：liupeng
*	形    参： LEUART1  LEUART0
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void initLeuart(LEUART_TypeDef *leuart,u32 baudrate,u8 powerflag)
{

    LEUART_Init_TypeDef leuart1Init =
    {
        leuartEnable,      /* Enable RX/TX when init completed. */                                \
        0,                 /* Use current configured reference clock for configuring baudrate. */ \
        2400,              /* 9600 bits/s. */                                                     \
        leuartDatabits8,   /* 8 databits. */                                                      \
        leuartNoParity,    /* No parity. */                                                       \
        leuartStopbits1    /* 1 stopbit. */                                                       \
    };
    if(powerflag==POWER_OFF)
        leuart1Init.enable=leuartDisable;
    /* Reseting and initializing LEUART1 */
    LEUART_Reset(leuart);
    leuart1Init.baudrate=baudrate;
    LEUART_Init(leuart, &leuart1Init);

    if(leuart==LEUART0)
    {
        leuart->ROUTE = LEUART_ROUTE_RXPEN|LEUART_ROUTE_TXPEN|LEUART_ROUTE_LOCATION_LOC2;

        if(powerflag==POWER_ON)
        {

            GPIO_PinModeSet(GSM_TX_PORT, GSM_TX_PIN, gpioModePushPull, 1);
            GPIO_PinModeSet(GSM_RX_PORT, GSM_RX_PIN, gpioModeInputPull,1);  /* Pull direction is set to pull-up */

        }
        else
        {
            GPIO_PinModeSet(GSM_TX_PORT, GSM_TX_PIN, gpioModeInput, 0);
            GPIO_PinModeSet(GSM_RX_PORT, GSM_RX_PIN, gpioModeInput, 0);

        }

        /* Pull direction is set to pull-up */
    }
    else //LEUART1
    {
        leuart->ROUTE = LEUART_ROUTE_RXPEN|LEUART_ROUTE_TXPEN|LEUART_ROUTE_LOCATION_LOC0;

        if(powerflag==POWER_ON)
        {

            GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 1);
            GPIO_PinModeSet(gpioPortC, 7, gpioModeInputPull,1);    /* Pull direction is set to pull-up */
        }
        else
        {
            GPIO_PinModeSet(gpioPortC, 6, gpioModeInput, 0);
            GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 0);    /* Pull direction is set to pull-up */
        }
    }
    if(powerflag==POWER_ON)
        LEUART_IntEnable(leuart, LEUART_IEN_RXDATAV);
    else
    {
        LEUART_IntDisable(leuart, LEUART_IEN_RXDATAV);
        return;
    }
    if(leuart==LEUART0)
    {
        NVIC_SetPriority(LEUART0_IRQn,0);
        NVIC_EnableIRQ(LEUART0_IRQn);

    }
    else
    {
        NVIC_SetPriority(LEUART1_IRQn,1);
        NVIC_EnableIRQ(LEUART1_IRQn);
    }


}



/*
*********************************************************************************************************
*	函 数 名:void void LEUART1_IRQHandler(void)(void)
*	功能说明: luart1中断接收
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void LEUART1_IRQHandler(void)
{
    u8 RxData;
    u32 leuart_IntFlag;

    leuart_IntFlag = LEUART_IntGet(LEUART1);

    if ( leuart_IntFlag & LEUART_IF_RXDATAV)
    {
        RxData = LEUART1->RXDATA;
        //esPrintChar( RxData );
        GpsDateIn(RxData);
    }

    LEUART_IntClear(LEUART1, ~_LEUART_IFC_RESETVALUE);

}
void USART1_RX_IRQHandler(void)
{

    /* Check for RX data valid interrupt */
    if (USART1->STATUS & USART_STATUS_RXDATAV)
    {
        /* Copy data into RX Buffer */
        uint8_t rxData = USART_Rx(USART1);
        GpsDateIn(rxData);

        /* Clear RXDATAV interrupt */
        USART_IntClear(USART1, USART_IF_RXDATAV);

    }
}


/*
*********************************************************************************************************
*	函 数 名:void void LEUART1_IRQHandler(void)(void)
*	功能说明: luart1中断接收
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void uartSetup(USART_TypeDef *usart, uint32_t board,u8 enable)
{
    USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;
#ifndef ENABLE_DOG
    if(NOT_OK==enable)
        return;
#endif

    if(usart==USART0)
    {
        if(enable==OK)
        {
            /* Enable clock for USART module */
            CMU_ClockEnable(cmuClock_USART0, true);
            /* Configure GPIO pins */
            GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 1);
            GPIO_PinModeSet(gpioPortE, 11, gpioModeInputPull, 1);
            uartInit.enable=usartEnable;
        }
        else
        {
            /* Enable clock for USART module */
            CMU_ClockEnable(cmuClock_USART0, false);
            /* Configure GPIO pins */
            GPIO_PinModeSet(gpioPortE, 10, gpioModeInput, 0);
            GPIO_PinModeSet(gpioPortE, 11, gpioModeInput, 0);
            uartInit.enable=usartDisable;
        }
        QueueInit(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf,DebugUart_REC_QUEUE_LENGTH);//队列初始化

    }

    if(usart==USART1)
    {
        if(enable==OK)
        {
            /* Enable clock for USART module */
            CMU_ClockEnable(cmuClock_USART1, true);
            /* Configure GPIO pins */
            GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 1);
            GPIO_PinModeSet(gpioPortD, 1, gpioModeInputPull, 1);
            uartInit.enable=usartEnable;
        }
        else
        {
            /* Enable clock for USART module */
            CMU_ClockEnable(cmuClock_USART1, false);
            /* Configure GPIO pins */
            GPIO_PinModeSet(gpioPortD, 0, gpioModeInput, 0);
            GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);
            uartInit.enable=usartDisable;
        }

    }
    if(usart==USART2)
    {
        if(enable==OK)
        {
            /* Enable clock for USART module */
            CMU_ClockEnable(cmuClock_USART2, true);
            /* Configure GPIO pins */
            GPIO_PinModeSet(gpioPortC, 2, gpioModePushPull, 1);
            GPIO_PinModeSet(gpioPortC, 3, gpioModeInputPull, 1);
            uartInit.enable=usartEnable;
        }
        else
        {
            /* Enable clock for USART module */
            CMU_ClockEnable(cmuClock_USART2, false);
            /* Configure GPIO pins */
            GPIO_PinModeSet(gpioPortC, 2, gpioModeInput, 0);
            GPIO_PinModeSet(gpioPortC, 3, gpioModeInput, 0);
            uartInit.enable=usartDisable;
        }
        QueueInit(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf,DebugUart_REC_QUEUE_LENGTH);//队列初始化

    }


    uartInit.baudrate     = board;         /* Baud rate */




    /* Initialize USART with uartInit struct */
    USART_InitAsync(usart, &uartInit);

    /* Prepare UART Rx and Tx interrupts */
    USART_IntClear(usart, _USART_IF_MASK);
    if(enable==OK)
        USART_IntEnable(usart, USART_IF_RXDATAV);



    if(usart==USART0)
    {

        if(enable==OK)
        {
            //    NVIC_ClearPendingIRQ(USART0_RX_IRQn);
            //     NVIC_EnableIRQ(USART0_RX_IRQn);
            NVIC_SetPriority(USART0_RX_IRQn,4);
            NVIC_ClearPendingIRQ(USART0_RX_IRQn);
            //     NVIC_ClearPendingIRQ(USART0_TX_IRQn);
            NVIC_EnableIRQ(USART0_RX_IRQn);
            //     NVIC_EnableIRQ(USART0_TX_IRQn);
            /* Enable I/O pins at USART0 location #0 */
            usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC0;
        }
    }

    if(usart==USART1)
    {

        if(enable==OK)
        {
            //    NVIC_ClearPendingIRQ(USART0_RX_IRQn);
            //     NVIC_EnableIRQ(USART0_RX_IRQn);
            NVIC_SetPriority(USART1_RX_IRQn,4);
            NVIC_ClearPendingIRQ(USART1_RX_IRQn);
            //     NVIC_ClearPendingIRQ(USART0_TX_IRQn);
            NVIC_EnableIRQ(USART1_RX_IRQn);
            //     NVIC_EnableIRQ(USART0_TX_IRQn);
            /* Enable I/O pins at USART0 location #0 */
            usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC1;
        }
    }
    if(usart==USART2)
    {

        if(enable==OK)
        {
            //    NVIC_ClearPendingIRQ(USART0_RX_IRQn);
            //     NVIC_EnableIRQ(USART0_RX_IRQn);
            NVIC_SetPriority(USART2_RX_IRQn,4);
            NVIC_ClearPendingIRQ(USART2_RX_IRQn);
            //     NVIC_ClearPendingIRQ(USART0_TX_IRQn);
            NVIC_EnableIRQ(USART2_RX_IRQn);
            //     NVIC_EnableIRQ(USART0_TX_IRQn);
            /* Enable I/O pins at USART0 location #0 */
            usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC0;
        }
    }
    if(enable==OK)
    {
        /* Enable UART */
        USART_Enable(usart, usartEnable);
    }
    else
    {

        /* Enable UART */
        USART_Enable(usart, usartDisable);
    }
}


/*
*********************************************************************************************************
*	函 数 名:void USART0_RX_IRQHandler(void)
*	功能说明: USART0串口接收函数
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/



void USART0_RX_IRQHandler(void)
{
    u8 err;

    /* Check for RX data valid interrupt */
    if (USART0->STATUS & USART_STATUS_RXDATAV)
    {
        /* Copy data into RX Buffer */
        uint8_t rxData = USART_Rx(USART0);
        BleRevData(rxData);

        err=QueuePost(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf,rxData);

        if(err!=Q_OPT_SUCCEED)
        {
            QueueFlush(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf);
        }


        /* Clear RXDATAV interrupt */
        USART_IntClear(USART0, USART_IF_RXDATAV);

    }
}



/*
*********************************************************************************************************
*	函 数 名:void DebugUart(u8 datain)
*	功能说明:调试串口处理函数
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
usart1_stu Usart1Stu;


void DebugUart(u8 bt)
{
    static u8 stu=0,checksum;
    static u16 message_len,message_len_tmp;
    if( (Usart1Stu.updateflag==0xaa)&&( (Usart1Stu.Overtime%30)==0)&&(Usart1Stu.Overtime>10))
    {
        stu=0;
    }
    switch(stu)
    {
    case 0:
        if(bt==0xaa)
        {
            stu++;
        }
        break;
    case 1:
        if(bt==0x55)
        {
            stu++;
            checksum=0;
            message_len=0;
        }
        else
            stu=0;
        break;
    case 2:
        message_len=bt;
        stu++;
        break;
    case 3:
        message_len=((message_len<<8)&0xff00)|((u16)bt&0x00ff);
        message_len_tmp=message_len;
        stu++;
        break;
    case 4:
        Usart1Stu.usart1_receive_buf[message_len-message_len_tmp]=bt;
        checksum^=bt;
        if(message_len>1124)
            stu=0;
        if(message_len_tmp==1)
        {
            stu++;
        }
        message_len_tmp--;
        break;
    case 5:
        if(bt==checksum)
        {

            ana_message(Usart1Stu.usart1_receive_buf,message_len);

        }
        stu=0;
        break;
    default:
        stu=0;
        break;

    }


}
/*
*********************************************************************************************************
*	函 数 名:void DebugUartUpdataPackegToUsart(u8  *Mydate,u16 datelen)
*	功能说明:打包发送
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
void DebugUartUpdataPackegToUsart(u8  *Mydate,u16 datelen)
{
#define mycharLen   512
    u8  mychar[mycharLen];
    u16 Sendlen;
    u8 VAR;
    u16 i;

    Sendlen=0;
    mychar[Sendlen++]=0xaa;
    mychar[Sendlen++]=0x55;

    mychar[Sendlen++]=(u8)( (datelen>>8)&0x00ff);
    mychar[Sendlen++]=(u8)(datelen&0x00ff);

    VAR=0;
    for(  i=0; i<datelen; i++)
    {
        mychar[Sendlen++]=Mydate[i];
        VAR^=Mydate[i];;
    }

    mychar[Sendlen++]=VAR;

    DebugUartSend(mychar,Sendlen);



}

/*
*********************************************************************************************************
*	函 数 名:void DebugUartUpdataSendPackegResult(u8 result)
*	功能说明:发送包升级结果
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
void DebugUartUpdataSendPackegResult(u8 result)
{
    u8 buf[3];
    buf[0]=0x00;
    buf[1]=0x02;
    buf[2]=result;
    DebugUartUpdataPackegToUsart(buf,3);


}


/*
*********************************************************************************************************
*	函 数 名:void DebugUartUpdataSendAck(u8 *idbody,u8 result)
*	功能说明:发送应答
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
void DebugUartUpdataSendAck(u8 *idbody,u8 result)
{
    u8 buf[5];
    buf[0]=0x00;
    buf[1]=0x04;
    buf[2]=idbody[0];
    buf[3]=idbody[1];
    buf[4]=result;
    DebugUartUpdataPackegToUsart(buf,5);
}
/*
*********************************************************************************************************
*	函 数 名: void ana_message(u8 *datain,u16 len)
*	功能说明:接收到一帧串口数据
*
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
void ana_message(u8 *rxdate,u16 message_len)
{

    u16  cmd,packegsize,i,j,k,len,turn;
    u8 buf[256];
    u8 *message_body;//只包含数据部分
    message_len-=2;
    message_body=&rxdate[2];//只包含数据部分
    cmd=((((u16)rxdate[0])<<8)&0xff00)|(((u16)rxdate[1])&0x00ff);
    switch(cmd)
    {
    case 0x8001:/*固件升级请求*/
        if( (GsmSto.updateflag==OK)||(resetflag==0xaa))
        {
            return;
        }
        if(Usart1Stu.updateflag!=0xaa)
        {
            Usart1Stu.updateflag=0xaa;
            GsmSto.currentpackeg=0;
            GsmSto.Softwarelen=((((u32)message_body[0])<<24)&0xff000000)|((((u32)message_body[1])<<16)&0x00ff0000)|((((u32)message_body[2])<<8)&0x0000ff00)|((((u32)message_body[3])<<0)&0x000000ff);
            GsmSto.updatevarity=message_body[4];
            GsmSto.totalpackeg=10;
            if(( GsmSto.Softwarelen<(10*1024))||( GsmSto.Softwarelen>(58*1024)))/*升级的数据包大小有问题*/
            {
                Usart1Stu.updateflag=0;
                DebugUartUpdataSendPackegResult(5);
                return ;
            }
        }
        Usart1Stu.Overtime=0;
        break;
    case 0x8002:/*固件包*/
        Usart1Stu.Overtime=0;
        /*获得包序号*/
        turn=((((u16)message_body[0])<<8)&0xff00)|(((u16)message_body[1])&0x00ff);
        if(turn!=GsmSto.currentpackeg)
            break;
        /*获得包长度*/
        packegsize=((((u16)message_body[2])<<8)&0xff00)|(((u16)message_body[3])&0x00ff);
        if(turn==0)/*第一包*/
        {
            GsmSto.totalpackeg= GsmSto.Softwarelen/packegsize;
            GsmSto.packetsize=packegsize;
            if(GsmSto.Softwarelen%packegsize)
            {
                GsmSto.totalpackeg++;

            }
            if( (GsmSto.updateflag==OK)||(resetflag==0xaa))
            {
                return;
            }
            /*擦除整个备份区*/
            EraseApp(GsmSto.Softwarelen);
        }
        WriteApp(turn*GsmSto.packetsize,&message_body[4],packegsize);
        j=packegsize/256;
        k=packegsize%256;
        len=256;
        for(i=0; i<=j; i++)
        {
            if(i==j)
            {
                if(k)
                    len =k;
                else
                    break;
            }
            ReadApp(turn*GsmSto.packetsize+i*256,buf,len);
            if(EQUER!=EqureOrNot(buf,&message_body[4+i*256],len))
            {
                Usart1Stu.updateflag=0;
                /*写flash错误*/
                DebugUartUpdataSendPackegResult(1);
                break;
            }
        }
        GsmSto.currentpackeg++;
        DebugUartUpdataSendPackegTurn();
        break;
    case 0x8003:/*参数设置*/
        if(OK==McuAnswerSetPara(message_body[0],&message_body[1]))
            DebugUartUpdataSendAck(rxdate,0);
        else
            DebugUartUpdataSendAck(rxdate,1);
        break;
    case 0x8004:/*参数查询*/
        McuAnswerAskPara(message_body[0],&message_body[1],0);
        break;
    default:
        break;



    }


}
/*
*********************************************************************************************************
*	函 数 名: u8  GetUsart1Date(void)
*	功能说明:接收串口1数据
*
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
u8  GetDebugUartDate(void)
{
    u8 data,err;

    do
    {
        data=QueueAccept_OutInt(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf,&err);
        if(err==Q_OPT_SUCCEED)
        {
            DebugProcess(data);/*调试指令*/
            DebugUart(data);
        }
    }
    while (err==Q_OPT_SUCCEED);

    return err;
}
/*
*********************************************************************************************************
*	函 数 名:void DebugUartUpdataSendPackegTurn(void)
*	功能说明:发送包序号
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
void DebugUartUpdataSendPackegTurn(void)
{
    u8 buf[4];
    if(GsmSto.currentpackeg<GsmSto.totalpackeg)
    {
        buf[0]=0x00;
        buf[1]=0x01;
        buf[2]=(u8)((GsmSto.currentpackeg>>8)&0x00ff);
        buf[3]=(u8)(GsmSto.currentpackeg&0x00ff);
        DebugUartUpdataPackegToUsart(buf,4);
    }

}

/*
*********************************************************************************************************
*	函 数 名: void DebugUartAskPacked(void)
*	功能说明:调试串口处理请求升级包
*
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
void DebugUartAskPacked(void)
{
    u8 PROGRAM[12]="PROGRAM";




    if(Usart1Stu.updateflag==0xaa)
    {
        if(Rtc)
        {
            FeedTaskDog();
            if(Usart1Stu.Overtime++>240)/*超时失败*/
            {
                Usart1Stu.updateflag=0;
                DebugUartUpdataSendPackegResult(3);
            }

            if( (Usart1Stu.Overtime>1)&&((Usart1Stu.Overtime%5==0)))
                DebugUartUpdataSendPackegTurn();

            if(GsmSto.totalpackeg==GsmSto.currentpackeg)/*计算校验码*/
            {

                if(GsmSto.updatevarity==CalcUpdateVarity())
                {

                    PROGRAM[7]=GsmSto.updatevarity;
                    PROGRAM[8]=(u8)((GsmSto.Softwarelen>>0)&0x000000ff);
                    PROGRAM[9]=(u8)((GsmSto.Softwarelen>>8)&0x000000ff);
                    PROGRAM[10]=(u8)((GsmSto.Softwarelen>>16)&0x000000ff);
                    PROGRAM[11]=(u8)((GsmSto.Softwarelen>>24)&0x000000ff);
                    FLASH_eraseOneBlock(UpdatePara);
                    FLASH_WriteDate(UpdatePara,12,PROGRAM);
                    Usart1Stu.updateflag=0;
                    DebugUartUpdataSendPackegResult(0);
                    Delayms(3000);
                    NVIC_SystemReset();

                }
                else
                {

                    Usart1Stu.updateflag=0;
                    DebugUartUpdataSendPackegResult(4);
                }
            }
            Rtc=0;
        }
    }

}
/*
*********************************************************************************************************
*	函 数 名: void DebugUartTask(void)
*	功能说明:调试串口处理函数
*
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
void DebugUartTask(void)
{
    do
    {
        GetDebugUartDate();
        DebugUartAskPacked();

    }
    while(Usart1Stu.updateflag==0xaa);
}

/*
*********************************************************************************************************
*	函 数 名:void DebugUartSend(u8 *in,u16 len)
*	功能说明:
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void DebugUartSend(u8 *in,u16 len)
{

#ifdef USE_USART0_DEBUG
    u16 i;
    for(i=0; i<len; i++)
        PrintUsart(in[i]);
#else
    if(GsmSta.charging==BATTERY_CHARGE)
    {

        //UsbSent(in,len);
        UsbSendTobuf(in,len);

    }
#endif
}

/*
*********************************************************************************************************
*	函 数 名:void PrintUsart(u8 ch)
*	功能说明:串口打印
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void PrintUsart(u8 ch)
{
    if(GsmSta.charging==BATTERY_CHARGE)
    {
#ifdef USE_USART0_DEBUG
        USART_Tx(USART0,ch);
#else
        UsbSendTobuf(&ch,1);
#endif


    }
}

/*
*********************************************************************************************************
*	函 数 名:void DebugProcess(u8 datein)
*	功能说明:处理调试命令
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

#if 0
#define DEBUGGSM 0xaa
#define DEBUGGPS DEBUGGSM+1
#define DEBUGADD DEBUGGPS+1
#define DEBUGWIF DEBUGADD+1
#define DEBUGADC DEBUGWIF+1
#endif
void DebugProcess(u8 datein)
{
    static u8 stu=0;//,stu1=0;
    const u8 buf[5]="DEBUG";
    static u8 bufr[3];//,time[6];

    switch(stu)
    {
    case 0:
        if(datein==buf[stu])
            stu++;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        if(datein==buf[stu])
            stu++;
        else
            stu=0;
        break;
    case 5:
    case 6:
    case 7:
        bufr[stu-5]=datein;

        if(stu==7)
        {
            if(OK== Equre(bufr, "GSM",3))
            {
                debug=DEBUGGSM;
            }
            else   if(OK== Equre(bufr, "GPS",3))
            {
                debug=DEBUGGPS;
            }
            else   if(OK== Equre(bufr, "WIF",3))
            {
                debug=DEBUGWIF;
            }
            else   if(OK== Equre(bufr, "ADD",3))
            {
                debug=DEBUGADD;
#if 0
                if(AdxlStu.state==ADXL_STATIC)
                {
                    AdxlStu.state=ADXL_MOVE;
                    printf("------->move\r\n");
                }
                else
                {
                    AdxlStu.state=ADXL_STATIC;
                    printf("------->sleepr\n");
                }
#endif
            }
            else   if( (OK== Equre(bufr, "ADC",3))||(OK== Equre(bufr, "VOL",3)))
            {
                debug=DEBUGADC;
            }
            else   if(OK== Equre(bufr, "TIM",3))
            {
                debug=DEBUGTIM;
            }
            else   if(OK== Equre(bufr, "NET",3))
            {
                debug=DEBUGNET;
            }
            else   if(OK== Equre(bufr, "ANT",3))
            {
                debug=DEBUGANT;
            }
            else   if(OK== Equre(bufr, "RON",3))
            {
                GPS_RF_ON();
#ifdef USE_PRINTF
                myprintf("RF ON\r\n");
#endif
            }
            else   if(OK== Equre(bufr, "ROF",3))
            {
                GPS_RF_OFF();
#ifdef USE_PRINTF
                myprintf("RF OFF\r\n");
#endif
            }
            stu=0;
        }
        stu++;
        break;
    default:
        stu=0;
        break;

    }

#if 0
    switch(stu1)
    {
    case 0:
        if(0xaa==datein)
        {
            stu1++;
        }
        break;
    case 1:

        if(0xaa==datein)
        {
            stu1=3;
        }
        else
        {
            stu1=0;
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        time[stu1-3]=datein;
        if(stu1==8)
        {
            stu1=0;
            SetRtcTime(time);
            printf("time:year:%02d month:%02ddate :%02d h:%02d min:%02d sec:%02d \r\n",(u8)(timer.w_year-2000),timer.w_month,timer.w_date,timer.hour,timer.min,timer.sec);
            break;
        }
        stu1++;
        break;
    default:
        stu1=0;
        break;



    }
#endif
}


/*
*********************************************************************************************************
*	函 数 名:打印函数
*	功能说明:
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
int putchar(int ch)
{

    PrintUsart(ch);
    return ch;



}



/*
*********************************************************************************************************
*	函 数 名:void UartSendStr(LEUART_TypeDef *leuart,u8 * str,u16 len)
*	功能说明: luart字符串发送
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void UartSendStr(LEUART_TypeDef *leuart,u8 * str,u16 len)
{
    u16 i;
    for(i=0; i<len; i++)
    {
        LEUART_Tx(leuart, (u8)str[i]);
    }
    if( (debug==DEBUGGSM)&&(leuart==LEUART0))
        DebugUartSend(str,len);

}



/*
*********************************************************************************************************
*	函 数 名:void UsbSendTobuf(u8*datain,u16 len)
*	功能说明: 入USB发送缓冲
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
#define USB_BUF_LEN 1224
u8 usbbuf[USB_BUF_LEN];
u16 usbbuflen=0;

void UsbSendTobuf(u8*datain,u16 len)
{
    u16 addlen;
    addlen=len;
    if((usbbuflen+len)>USB_BUF_LEN)
    {
        addlen-=(usbbuflen+len-USB_BUF_LEN);
    }

    Mymemcpy(&usbbuf[usbbuflen],datain, addlen);
    usbbuflen+=addlen;

}

/*
*********************************************************************************************************
*	函 数 名:void TaskUsbSend(u8*datain,u16 len)
*	功能说明: 入USB发送缓冲
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

void TaskUsbSend(void)
{
    u16 SendLen;
    if( (0==usbbuflen)||(GsmSta.charging!=BATTERY_CHARGE)) return;


    SendLen=usbbuflen;
    if(usbbuflen>1023)
    {
        SendLen=1023;
    }
    UsbSent(usbbuf,SendLen);
    usbbuflen-=SendLen;


}
