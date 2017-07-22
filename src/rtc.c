#include"includes.h"









/* Structs for modules used */
//CMU_TypeDef     *cmu = CMU;
//RTC_TypeDef     *rtc = RTC;




/* extern u8  g_time; */
/*
*********************************************************************************************************
*	函 数 名:void startLfxoForRtc(void)
*	功能说明: RTC时钟设置
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
u32 rtcinter;
void setupRtc(void)
{
    /* Configuring clocks in the Clock Management Unit (CMU) */
    CMU_ClockEnable(cmuClock_RTC, true);

    // rtcinter= CMU_ClockFreqGet( cmuClock_LFA )/10;  // 3m 6m
    rtcinter=272801;//329167;
    /* Setting up RTC */
// RTC_CompareSet(0, RTC_COUNT_BETWEEN_WAKEUP);
    RTC_CompareSet(0, rtcinter);
    RTC_IntEnable(RTC_IFC_COMP0);
    NVIC_SetPriority(RTC_IRQn,4);
    /* Enabling Interrupt from RTC */
    NVIC_EnableIRQ(RTC_IRQn);

    /* Enabling the RTC */
    RTC_Enable(true);

}






/*
*********************************************************************************************************
*	函 数 名:void RTC_IRQHandler(void)
*	功能说明: RTC中断入口
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
u8 Rtc=0;
void RTC_IRQHandler(void)
{
    u32 currentTime, nextTime;
    static u8 second=0;
    /* Clear interrupt source */
    RTC_IntClear(RTC_IFC_COMP0);

    /* Calculating new wake-up time */
    currentTime = RTC_CounterGet();
// nextTime    = currentTime + RTC_COUNT_BETWEEN_WAKEUP;
    nextTime    = currentTime + rtcinter;

    /* If an overflow will occur, the compare value is adjusted */
    if (nextTime > _RTC_CNT_MASK)
        nextTime = nextTime - _RTC_CNT_MASK;
#ifdef ENABLE_DOG
    WDOG_Feed();
    ManageMyDog();
#endif
    Rtc++;
    if(second++>9)
    {
        second=0;
        RtcInter();
    }
    /* if ( g_time )  g_time --; */
  /*  if ( ( GsmSta.sos ) && ( second % 5 == 0 ) )
    {
        GPIO_PinOutToggle(RLED_GPIO_PORT, RLED_GPIO_PIN);
    }
    */
    /* Setting the new compare value */
    RTC_CompareSet(0, nextTime);
}


/*
*********************************************************************************************************
*	函 数 名:void Delayms(u32 ms)
*	功能说明:延时时长为10ms的整数倍，至少10ms
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
#pragma optimize=none
void Delayms( volatile u32 ms)
{

    while(ms>2)
    {

        ms-=1;

    }


}
