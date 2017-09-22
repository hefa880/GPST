#include "includes.h"

#include "em_device.h"
#include "em_chip.h"
#include "em_rtc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"

#include "em_burtc.h"
#include "em_rmu.h"

#include "system_efm32lg.h"

#include "gsensor.h"
u8 VERSION[2] = {0x00, 0x08};
/*修改记录*/
/*
{0x00,0x02};//先读取后发送经纬度
{0x00,0x04};//支持3g 支持动态修改APN
{0x00,0x05};//APN使用手动输入
{0x00,0x06};//不显示运营商，gps关，电量充电的时候显示charging 充满显示FUL ,
*/

/* Defines */
/* When this define is present, the program will enter EN4 at the end. */
/* If it is not present, EM3 is entered. */
//#define ENTER_EM4
/* Clock defines*/
#define LFXO_FREQUENCY         32768
#define RTC_TIMEOUT_S          5
#define RTC_COUNT_TO_WAKEUP    (LFXO_FREQUENCY * RTC_TIMEOUT_S)


/* Structs for modules used */
CMU_TypeDef     *cmu    = CMU;
RTC_TypeDef     *rtc    = RTC;
TIMER_TypeDef   *ptimer  = TIMER1;


/* Global flag to indicate rtc interrupt */
volatile u8  rtcInterrupt = 0;



/******************************************************************************
 * @brief  Start LFXO for RTC
 * Starts the LFXO and routes it to the RTC.
 *****************************************************************************/
void startLFXOForRTC(void)
{
    /* Starting LFXO and waiting until it is stable */
    CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

    /* Routing the LFXO clock to the RTC */
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_RTC, true);

    /* Enabling clock to the interface of the low energy modules */
    CMU_ClockEnable(cmuClock_CORELE, true);
}



/******************************************************************************
 * @brief  Sets up the RTC
 *
 *****************************************************************************/
void setupRTCTimeout(void)
{
    /* Setting up RTC */
    RTC_CompareSet(0, RTC_COUNT_TO_WAKEUP);
    RTC_IntEnable(RTC_IFC_COMP0);
    NVIC_EnableIRQ(RTC_IRQn);
    RTC_Enable(true);
}


/******************************************************************************
 * @brief waits for rtc trigger
 *****************************************************************************/
void waitForRTC(void)
{
    /* Waiting for rtc interrupt */
    rtcInterrupt = 0;
    setupRTCTimeout();

    while (rtcInterrupt == 0) ;
}



/******************************************************************************
 * @brief enables all clocks and waits
 *****************************************************************************/
void enableAllClocks(void)
{
    /* Turning on all oscillators */
    /* Then waiting for all oscillators to stabilize */
    CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
    CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
    CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

    /* Switching the CPU clock source to HFXO */
    /* This will increase current consumtion, since it runs on 32MHz */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

    /* Enabling clocks to all core and peripheral modules */
    cmu->HFCORECLKEN0 = 0xFFFFFFFF;
    cmu->HFPERCLKEN0  = 0xFFFFFFFF;

    /* Wait */
    waitForRTC();
}



/******************************************************************************
 * @brief disables all clocks and waits
 *****************************************************************************/
void disableAllClocks(void)
{
    /* Switching the CPU clock source to HFRCO */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

    /* Disabling every oscillator except hfrco and lfxo */
    /* NOTE: MAKE SURE NOT TO DISABLE THE CURRENT CPU CLOCK!!*/
    CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
    CMU_OscillatorEnable(cmuOsc_HFXO, false, true);
    CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

    /* Disabling all unused clocks. The LE clock must be on in order to use the
     * RTC */
    cmu->HFCORECLKEN0 = CMU_HFCORECLKEN0_LE;
    cmu->HFPERCLKEN0  = 0;

    /* Wait */
    waitForRTC();
}



/******************************************************************************
 * @brief changes band and prescales core clock
 *****************************************************************************/
void downScaleCoreClock(void)
{
    /* Changing the band of the HFRCO */
    CMU_HFRCOBandSet(cmuHFRCOBand_7MHz);

    /* Setting prescaling of the CPU clock*/
    CMU_ClockDivSet(cmuClock_CORE, cmuClkDiv_4);

    /* Wait */
    waitForRTC();
}



/******************************************************************************
 * @brief enters em1 and waits for timer interrupt
 *****************************************************************************/
void prepareEM1(void)
{
    /* Enabling clock to timer0 */
    CMU_ClockEnable(cmuClock_TIMER0, true);

    /* Scaling down the clock of the peripherals */
    /* Remember to enable the peripheral clock */
    CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_512);

    /* Setup timer to give interrupt upon overflow */
    ptimer->IEN = TIMER_IEN_OF;
    ptimer->CMD = TIMER_CMD_START;
    NVIC_EnableIRQ(TIMER0_IRQn);
}



/******************************************************************************
 * @brief enters em2 and waits for rtc interrupt
 *****************************************************************************/
void prepareEM2(void)
{
    /* Enabling clock to timer0. Also DMA clock (see errata)*/
    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockEnable(cmuClock_DMA, true);

    /* Setting up RTC to issue interrupt */
    setupRTCTimeout();
}


#if 0
/******************************************************************************
 * @brief RTC Interrupt Handler. Sets interrupt flag and disables the RTC.
 *        The interrupt table is in assembly startup file startup_efm32.s
 *****************************************************************************/
void RTC_IRQHandler(void)
{
    /* Clear interrupt source */
    RTC_IntClear(RTC_IFC_COMP0);
    /* Flushing instructions to make sure that the interrupt is not re-triggered*/
    /* This may be required when the peripheral clock is slower than the core */
    __DSB();

    /* Disabling interrupts from RTC */
    NVIC_DisableIRQ(RTC_IRQn);

    /* Disabling RTC */
    RTC_Enable(false);

    /* Asserting that an RTC interrupt has occured. */
    rtcInterrupt = 1;
}

#endif

/******************************************************************************
 * @brief TIMER0 Interrupt Handler. Clears interrupt flag.
 *        The interrupt table is in assembly startup file startup_efm32.s
 *****************************************************************************/
void TIMER1_IRQHandler(void)
{
    /* Clear interrupt source */
    ptimer->IFC = TIMER_IFC_OF;
    /* Flushing instructions to make sure that the interrupt is not re-triggered*/
    /* This may be required when the peripheral clock is slower than the core */
    __DSB();

    /* Stopping timer */
    ptimer->CMD = TIMER_CMD_STOP;

    /* Disabling interrupts from TIMER */
    NVIC_DisableIRQ(TIMER0_IRQn);
}




int test_main(void)
{
    /* Initialize chip */
    CHIP_Init();

    /* Initalizing */
    startLFXOForRTC();

    /* Starting to enter different energy-modes and  various clock settings */
    /************************************************************************/

    /* Turn on all clocks and oscillators. Then wait in EM0 */
    enableAllClocks();

    /* Turn off all clocks and wait in EM0 */
    /* Current consumption drops drastically */
    disableAllClocks();

    /* Adjusting core clock down to further reduce current consumption */
    downScaleCoreClock();

    /* Enter EM1. Using Timer0 to wake up */
    prepareEM1();
    EMU_EnterEM1();

    /* Only enable RTC and wait in EM2 */
    prepareEM2();
    EMU_EnterEM2(false);

#ifdef ENTER_EM4
    /* Enter EM4 */
    EMU_EnterEM4();
#else
    /* Enter EM3 */
    EMU_EnterEM3(false);
#endif

    while(1);

    return 0;
}


/*
*********************************************************************************************************
*   函 数 名:int main(void)
*   功能说明: 程序入口
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
// volatile u16 len=0;
// u32 g_time=0xffffffff;
u32 g_GLedFlag = 0;
int main(void)
{
    u32 time = 0x4ffff; /* */

    //  u32 resetCause;
    //  while(1);
    SCB->VTOR = AppStart;

    //  test_main();


    while(time--);

    Inithardware();
    DateInit();
    //   resetCause = RMU_ResetCauseGet();
    //   RMU_ResetCauseClear();
#ifdef ENABLE_DOG
    InitDog();
#endif
   
    ue866_gpio_power(false);
    while(Rtc < 20 );

    AskTime();
    //PowerHoldOn();
    ue866_gpio_power(true);
    initGSensor();
    time = 0xfffff;

    while(time--);

    myprintf ("---Booting...\r\n");

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
            Rtc = 0;
           GsmTask();

            /**/
            if(GsmSto.updateflag != OK)
            {

          
        //   GpsTask();

                TimeTask();
                DebugUartTask();
                ADCGetVoltage();
                InTime();
            }

            //LED_Task();

            GSensorTask();

        }

        if(GsmSto.updateflag != OK)
        {
            //#ifdef ENABLE_DOG

            EMU_EnterEM1();

            //#endif
        }
    }

}


/*
*********************************************************************************************************
*   函 数 名:硬件初始化
*   功能说明: 程序入口
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void Inithardware(void)
{
    u32 i = 0;
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
    i = 0xffff;

    while(i--);

#ifndef USE_USART0_DEBUG
    SystemClockConfig(true);
#else

    SystemClockConfig(false);
    uartSetup( USART0, 38400, OK );
#endif
    i = 0x1ffff;

    while(i--);

    InitGPIOs();
    //   LED_Init();
    TimerRtcInit();

#ifndef USE_USART0_DEBUG
    USBD_Init(&initstruct);
    USBD_Disconnect();
    USBTIMER_DelayMs(1000);
    USBD_Connect();
    QueueInit(&DebugUartRxQueueCtrl, (u8 *)DebugUartRxQueueBuf, DebugUart_REC_QUEUE_LENGTH); //队列初始化
#endif

#if 0

    if(READ_EXPOWER())
    {
        // LCDWriteBattery(50);
        //UsbConnectOrNot(false);
        GsmSta.charging = BATTERY_NOT_CHARGE;
    }
    else
    {
        // LCDWriteBattery(50);
        //  UsbConnectOrNot(true);
        GsmSta.charging = BATTERY_CHARGE;
    }

#endif

    i = 0x1ffff;

    while(i--);

    ADCConfig();
    setupRtc();
    InitGps();
    GsmInit();



    ReadGsmStoreDate();
    ReadBlind();

    GiveStoreApn();
}


