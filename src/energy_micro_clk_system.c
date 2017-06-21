#include "energy_micro_clk_system.h"
/*
*********************************************************************************************************
*	函 数 名:void SystemClockConfig(void)
*	功能说明: 系统时钟配置
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

uint32_t freq;
void SystemClockConfig(u8 USB_ON)
{
#if 0


#else

    //使能晶振
    /* enable HFRCO  如果是hfxo还要设置超时时间CMU_CTRL寄存器默认是使能hfrco*/
    CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
    /* abable LFRCO如果是LFXO还要设置超时时间CMU_CTRL寄存器默认是disable lfrco*/
    //HFCLK LFA  LFB 选择时钟源
    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);/**/
    freq = CMU_ClockFreqGet( cmuClock_CORELE );   //  6m
    if(USB_ON==true)
    {
        CMU_ClockSelectSet( cmuClock_USB, cmuSelect_HFCLK );/*USB使用48M*/
        CMU_ClockEnable(cmuClock_USBC, true);
        //CMU_ClockEnable(cmuClock_USB, true);
        //freq = CMU_ClockFreqGet( cmuClock_USB );
        freq = CMU_ClockFreqGet( cmuClock_USBC );

        CMU_ClockDivSet(cmuClock_HF,cmuClkDiv_2);/*2---HFCLK=24m   8----HFCLK=6m*/   //24m
        freq = CMU_ClockFreqGet( cmuClock_HF );

        CMU_ClockDivSet(cmuClock_HFPER,cmuClkDiv_8);
        CMU_ClockEnable(cmuClock_HFPER, true);
        freq = CMU_ClockFreqGet( cmuClock_HFPER );   //  3m

        CMU_ClockDivSet(cmuClock_CORE,cmuClkDiv_1);
        CMU_ClockEnable(cmuClock_CORE, true);
        freq = CMU_ClockFreqGet( cmuClock_CORE );   //     24m


    }
    else
    {
        CMU_ClockEnable(cmuClock_USBC, false);
        CMU_ClockEnable(cmuClock_USB, false);
        //  freq = CMU_ClockFreqGet( cmuClock_USB );
        //   freq = CMU_ClockFreqGet( cmuClock_USBC );
        CMU_ClockDivSet(cmuClock_HF,cmuClkDiv_4);/*2---HFCLK=24m   8----HFCLK=6m*/
        //  freq = CMU_ClockFreqGet( cmuClock_HF );   //  12m

        CMU_ClockDivSet(cmuClock_HFPER,cmuClkDiv_4);
        CMU_ClockEnable(cmuClock_HFPER, true);
        //  freq = CMU_ClockFreqGet( cmuClock_HFPER );   //  3m

        CMU_ClockDivSet(cmuClock_CORE,cmuClkDiv_1);
        CMU_ClockEnable(cmuClock_CORE, true);
        //   freq = CMU_ClockFreqGet( cmuClock_CORE );   //  12m
    }


    //  CMU_ClockEnable(cmuClock_CORELE, true);
//   freq = CMU_ClockFreqGet( cmuClock_CORELE );   //  6m

    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_CORELEDIV2);/**/
    CMU_ClockSelectSet(cmuClock_LFB,cmuSelect_CORELEDIV2);/**/

    // freq = CMU_ClockFreqGet( cmuClock_LFA );   // 3m
    //  freq = CMU_ClockFreqGet( cmuClock_LFB);  // 3m

    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_LEUART1, true);    /* Enable LEUART1 clock */
    CMU_ClockEnable(cmuClock_LEUART0, true);    /* Enable LEUART0 clock */
    CMU_ClockEnable(cmuClock_USART0, true);     /* Enable LEUART0 clock */
    CMU_ClockEnable(cmuClock_USART1, true);     /* Enable LEUART1 clock */

    CMU_ClockEnable( cmuClock_I2C0, true );


    // CMU_ClockSelectSet( cmuClock_HFPER, cmuClkDiv_2 );/*timer iic ....................*/
    // CMU_ClockDivSet(cmuClock_CORE,cmuClkDiv_4); //222*t
    //  CMU_ClockDivSet(cmuClock_CORELE,cmuClkDiv_4); //222*t
    // CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_CORELEDIV2);/**/
    // CMU_ClockSelectSet(cmuClock_LFB,cmuSelect_CORELEDIV2);/**/
//#ifdef USE_32768
    //CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
    //  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);/*32.768*/
//    CMU_ClockSelectSet(cmuClock_LFB,cmuSelect_LFXO);/*32.768  cmuSelect_CORELEDIV2*/
//#else

//#endif

#endif

}


