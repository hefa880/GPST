#include "includes.h"
#include "gsensor.h"
u8 VERSION[2]= {0x00,0x08};
/*修改记录*/
/*
{0x00,0x02};//先读取后发送经纬度
{0x00,0x04};//支持3g 支持动态修改APN
{0x00,0x05};//APN使用手动输入
{0x00,0x06};//不显示运营商，gps关，电量充电的时候显示charging 充满显示FUL ,


*/



/*
*********************************************************************************************************
*	函 数 名:int main(void)
*	功能说明: 程序入口
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
// volatile u16 len=0;
// u32 g_time=0xffffffff;
u32 g_GLedFlag = 0;
int main(void)
{
    u32 time=0x4ffff; /* */
    //  u32 resetCause;
    //  while(1);
    SCB->VTOR=AppStart;
    while(time--);
    Inithardware();
    DateInit();
    //   resetCause = RMU_ResetCauseGet();
    //   RMU_ResetCauseClear();

#ifdef ENABLE_DOG
    InitDog();
#endif

// GsmSto.iplen=sizeof("116.24.26.55")-1;
// Mymemcpy(GsmSto.strip,(u8*)("116.24.26.55"),GsmSto.iplen);
    AskTime();
    //PowerHoldOn();
    GSM_POWER_ON();
    initGSensor();
    time=0xfffff; 
     while(time--);
    while(1)
    {
    
       if(Rtc)
        {
          //myprintf("current Rtc is on \r\n");
            //TaskUsbSend();

            FeedTaskDog();
            ResetSystem();
            //KeyTask();
            //TaskLcd();
            //BleTask();
           Rtc=0;
           GsmTask();
	/**/
            if(GsmSto.updateflag!=OK)
            {

                GpsTask();
	   
                TimeTask();
                DebugUartTask();
                ADCGetVoltage();
                InTime();
            }

            //LED_Task();

            GSensorTask();
        
        }
        if(GsmSto.updateflag!=OK)
        {
//#ifdef ENABLE_DOG

            EMU_EnterEM1();

//#endif
        }
    }

}


/*
*********************************************************************************************************
*	函 数 名:硬件初始化
*	功能说明: 程序入口
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void Inithardware(void)
{
    u32 i=0;
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    RTC->CTRL  = _RTC_CTRL_RESETVALUE;
    RTC->COMP0 = _RTC_COMP0_RESETVALUE;
    RTC->IEN   = _RTC_IEN_RESETVALUE;
    /* Reset GPIO settings */
    GPIO->P[5].MODEL = _GPIO_P_MODEL_RESETVALUE;
    /* Disable RTC clock */
    CMU->LFACLKEN0 = _CMU_LFACLKEN0_RESETVALUE;
    CMU->LFCLKSEL  = _CMU_LFCLKSEL_RESETVALUE;
    /* Disable LFRCO */
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
    /* Disable LE interface */
    CMU->HFCORECLKEN0 = _CMU_HFCORECLKEN0_RESETVALUE;
    /* Reset clocks */
    CMU->HFPERCLKDIV = _CMU_HFPERCLKDIV_RESETVALUE;
    CMU->HFPERCLKEN0 = _CMU_HFPERCLKEN0_RESETVALUE;
    NVIC_SetPriorityGrouping(6);
        i=0xffff;
    while(i--);
#ifndef USE_USART0_DEBUG
    SystemClockConfig(true);
#else

    SystemClockConfig(false);
    uartSetup( USART0, 38400, OK );
#endif
        i=0x1ffff;
    while(i--);
    InitGPIOs();
 //   LED_Init();
    TimerRtcInit();

#ifndef USE_USART0_DEBUG
    USBD_Init(&initstruct);
    USBD_Disconnect();
    USBTIMER_DelayMs(1000);
    USBD_Connect();
    QueueInit(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf,DebugUart_REC_QUEUE_LENGTH);//队列初始化
#endif

#if 0
    if(READ_EXPOWER())
    {
        // LCDWriteBattery(50);
        //UsbConnectOrNot(false);
        GsmSta.charging=BATTERY_NOT_CHARGE;
    }
    else
    {
        // LCDWriteBattery(50);
        //  UsbConnectOrNot(true);
        GsmSta.charging=BATTERY_CHARGE;
    }
#endif

    i=0x1ffff;
    while(i--);
    ADCConfig();
    setupRtc();
    InitGps();
    GsmInit();



    ReadGsmStoreDate();
    ReadBlind();

    GiveStoreApn();
}


