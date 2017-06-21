#include "energy_micro_clk_system.h"
/*
*********************************************************************************************************
*	�� �� ��:void SystemClockConfig(void)
*	����˵��: ϵͳʱ������
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
*********************************************************************************************************
*/

uint32_t freq;
void SystemClockConfig(u8 USB_ON)
{
#if 0


#else

    //ʹ�ܾ���
    /* enable HFRCO  �����hfxo��Ҫ���ó�ʱʱ��CMU_CTRL�Ĵ���Ĭ����ʹ��hfrco*/
    CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
    /* abable LFRCO�����LFXO��Ҫ���ó�ʱʱ��CMU_CTRL�Ĵ���Ĭ����disable lfrco*/
    //HFCLK LFA  LFB ѡ��ʱ��Դ
    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);/**/
    freq = CMU_ClockFreqGet( cmuClock_CORELE );   //  6m
    if(USB_ON==true)
    {
        CMU_ClockSelectSet( cmuClock_USB, cmuSelect_HFCLK );/*USBʹ��48M*/
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


