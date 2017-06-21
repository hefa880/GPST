#include"timer.h"

/*
*********************************************************************************************************
*	函 数 名:void TIMER0_IRQHandler(void)
*	功能说明: timer0中断入口
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void TIMER0_IRQHandler(void)
{
    /* Clear flag for TIMER0 overflow interrupt */
    TIMER_IntClear(TIMER0, TIMER_IF_OF);
   // LedsTrig();

}



/*
*********************************************************************************************************
*	函 数 名:void Timer0Init(void)
*	功能说明: timer0初始化
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void Timer0Init(void)
{
   // static u32 fre;
    /* 13761 Hz -> 14Mhz (clock frequency) / 1024 (prescaler)
      Setting TOP to 27342 results in an overflow each 2 seconds */
    //定时时长为2000ms
#define TOP (	CMU_ClockFreqGet(cmuClock_HFPER)*2000)/1024000

 //   fre=	CMU_ClockFreqGet(cmuClock_HFPER);/**/



    /* Enable clock for TIMER0 module */
    CMU_ClockEnable(cmuClock_TIMER0, true);

    /* Select TIMER0 parameters */
    TIMER_Init_TypeDef timerInit =
    {
        .enable     = true,
        .debugRun   = true,
        .prescale   = timerPrescale1024,
        .clkSel     = timerClkSelHFPerClk,
        .fallAction = timerInputActionNone,
        .riseAction = timerInputActionNone,
        .mode       = timerModeUp,
        .dmaClrAct  = false,
        .quadModeX4 = false,
        .oneShot    = false,
        .sync       = false,
    };

    /* Enable overflow interrupt */
    TIMER_IntEnable(TIMER0, TIMER_IF_OF);

    /* Enable TIMER0 interrupt vector in NVIC */
    NVIC_EnableIRQ(TIMER0_IRQn);

    /* Set TIMER Top value */
    TIMER_TopSet(TIMER0, TOP);

    /* Configure TIMER */
    TIMER_Init(TIMER0, &timerInit);

}