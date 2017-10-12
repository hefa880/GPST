#include "includes.h"
STU_KEY  StuKey;

/*
*********************************************************************************************************
*   函 数 名:void InitGPIOs(void)
*   功能说明: led初始化
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/

extern u32 g_GLedFlag;
extern u8  gGsmInit;
extern u8  gGsmPowerDown;
void InitGPIOs(void)
{

    //GPIO_PinModeSet(CHARGE_PORT, CHARGE_PIN, gpioModeInputPullFilter, 1);
    //GPIO_PinModeSet(EXT_POWER_PORT, EXT_POWER_PIN, gpioModeInputPullFilter, 1);

    /* 设置中断优先级，允许中断 */
    NVIC_SetPriority( GPIO_EVEN_IRQn, 5 );
    NVIC_SetPriority( GPIO_ODD_IRQn,  4 );

    NVIC_ClearPendingIRQ( GPIO_ODD_IRQn );
    NVIC_EnableIRQ( GPIO_ODD_IRQn );

    NVIC_ClearPendingIRQ( GPIO_EVEN_IRQn );
    NVIC_EnableIRQ( GPIO_EVEN_IRQn );

    /* 按键管脚模式设置 */
    //GPIO_PinModeSet( ON_OFF_PORT, ON_OFF_PIN, gpioModeInput, 1 );
    //GPIO_PinModeSet( SOS_PORT, SOS_PIN, gpioModeInputPull, 1 );

    /*add by hardy*/
    GPIO_PinModeSet( CHARGE_DET_PORT, CHARD_PIN, gpioModeInput, 1 ); /*change to 0 by hardy*/


    /* Configure PB 0 interrupt on rising edge */
    //GPIO_IntConfig( ON_OFF_PORT, ON_OFF_PIN, true, false, true );
    /* Configure PB 0 interrupt on falling edge */
    //GPIO_IntConfig( SOS_PORT, SOS_PIN, false, true, true );

    /* GSensor中断管脚模式设置 */
    //GPIO_PinModeSet( GSENSOR_INT1_PORT, GSENSOR_INT1_PIN, gpioModeInput, 1 );
    //GPIO_PinModeSet( GSENSOR_INT2_PORT, GSENSOR_INT2_PIN, gpioModeInput, 1 );

    /* Configure 0 interrupt on rising edge */
    //GPIO_IntConfig( GSENSOR_INT1_PORT, GSENSOR_INT1_PIN, true, false, true );
    /* Configure 1 interrupt on falling edge */
    //GPIO_IntConfig( GSENSOR_INT2_PORT, GSENSOR_INT2_PIN, false, true, true );

    //GPIO_PinModeSet( GSENSOR_SDA_PORT, GSENSOR_SDA_PIN, gpioModeWiredAndPullUp, 1 );
    //GPIO_PinModeSet( GSENSOR_SCL_PORT, GSENSOR_SCL_PIN, gpioModeWiredAndPullUp, 1 );
    GPIO_PinModeSet( GSENSOR_SDA_PORT, GSENSOR_SDA_PIN, gpioModeWiredAnd, 1 );
    GPIO_PinModeSet( GSENSOR_SCL_PORT, GSENSOR_SCL_PIN, gpioModeWiredAnd, 1 );

    //GPIO_PinModeSet( POWER_HOLD_PORT, POWER_HOLD_PIN, gpioModePushPull, 0 );
    GPIO_PinModeSet( GSM_POWER_CONTROL_PORT, GSM_POWER_CONTROL_PIN, gpioModePushPull, 0 );

    //GPIO_PinModeSet( GSM_MUTE_PORT, GSM_MUTE_PIN, gpioModePushPull, 0 );
    //GsmMuteOn();
}





/*
*********************************************************************************************************
*   函 数 名:void InitKey(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
#if 0
void GPIO_EVEN_IRQHandler( void )
{
    u32 flag, flag1;

    flag  = GPIO_IntGet();
    flag1 = READ_ON_OFF();

    if ( flag & ((0x00000001) << ON_OFF_PIN) )
    {
        GPIO_IntClear( 1 << ON_OFF_PIN );

        if ( flag1 )
        {
            StuKey.keypress_on = ON_OFF_PRESS;
        }
    }
}
#endif

#if 0
void GPIO_ODD_IRQHandler( void )
{
    u32 flag, flag1;

    flag  = GPIO_IntGet();
    flag1 = READ_SOS();

    if ( flag & ((0x00000001) << SOS_PIN) )
    {
        GPIO_IntClear( 1 << SOS_PIN );

        if ( !flag1 )
        {
            StuKey.keypress_sos = SOS_PRESS;
        }
    }
}
#endif

/*
*********************************************************************************************************
*   函 数 名:void KeyONOff(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/




#if 0
void KeyONOff(void)
{
    static u8 stu = 0;

    switch ( stu )
    {
        case 0:
            if( ((StuKey.keypress_on == ON_OFF_PRESS) || (READ_ON_OFF())) && (StuKey.processend))
            {
                stu = 1;
                StuKey.timeOnOff = 0;
            }

            break;

        case 1:
            StuKey.timeOnOff++;

            /*not press*/
            if ( (READ_ON_OFF() == 0) || (StuKey.timeOnOff > 12) )
            {

                /*进行开关机的操作*/
                if ( StuKey.timeOnOff > 12 )
                {
                    if(StuKey.SystemState != SYSTEM_OFF)
                    {

                        StuKey.SystemState = SYSTEM_OFF;
                        SendPosition(TRIG_SEND_POSITION);

                        g_GLedFlag |= GLED_STOP_STAUTS;
                        /* PowerHoldOff(); */
                    }
                    else
                    {

                        StuKey.SystemState = SYSTEM_ON;
                        SendPosition(TRIG_SEND_POSITION);

                        PowerHoldOn();
                        g_GLedFlag |= GLED_START_STAUTS;
                    }

                    StuKey.timeOnOff = 0;

                }
                else if ( StuKey.SystemState == SYSTEM_ON )
                {
                    GsmSta.LCDState = FLASH_TIME;
                    BacklightOnOff( true );
                    StuKey.processend = 1;

                    if ( GsmSta.voltage > 4050 )
                    {
                        g_GLedFlag |= GLED_BAT4_STAUTS;
                    }
                    else if ( GsmSta.voltage > 3900 )
                    {
                        g_GLedFlag |= GLED_BAT3_STAUTS;
                    }
                    else if ( GsmSta.voltage > 3750 )
                    {
                        g_GLedFlag |= GLED_BAT2_STAUTS;
                    }
                    else
                    {
                        g_GLedFlag |= GLED_BAT1_STAUTS;
                    }
                }

                stu++;

            }

            break;

        default:

            /*如果没有释放*/
            if ( READ_ON_OFF() )
            {
                break;
            }

            StuKey.keypress_on = NOT_PRESS;
            StuKey.timeOnOff = 0;
            stu = 0;
            break;

    }




}
#endif
/*
*********************************************************************************************************
*   函 数 名:void KeySOS(void)
*   功能说明:
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
#if 0
void KeySOS(void)
{
    static u8 stu = 0;

    switch ( stu )
    {
        case 0:
            if ( (StuKey.keypress_sos == SOS_PRESS) || (0 == READ_SOS()) )
            {
                stu = 1;
                StuKey.timeSos = 0;
            }

            break;

        case 1:
            StuKey.timeSos++;

            /*not press*/
            if ( (READ_SOS()) || (StuKey.timeSos > 12) )
            {

                if (StuKey.timeSos > 12)
                {
                    GsmSta.sos = !GsmSta.sos;

                    /*sos被按下的处理函数*/
                    if(GsmSta.sos)
                    {
                        StuKey.SosSendTime = SOS_TIMES;
                    }

                    SendPosition(TRIG_SEND_POSITION);
                    GsmSta.LCDState = FLASH_TIME;
                    BacklightOnOff(true);

                    GsmMuteOff();
                }

                stu++;
            }

            break;

        default:

            /*如果没有释放*/
            if ( (READ_SOS() == 0) && (StuKey.timeSos++ < 10) )
            {
                break;
            }

            StuKey.keypress_sos = NOT_PRESS;
            StuKey.timeSos = 0;
            stu = 0;
            break;


    }




}
#endif


/*
*********************************************************************************************************
*   函 数 名:void KeyTask(void)
*   功能说明: 按键时间统计
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
#if 0
void KeyTask(void)
{
    KeyONOff();

    if( StuKey.SystemState == SYSTEM_ON)
    {
        KeySOS();
    }
}
#endif

/*
*********************************************************************************************************
*   函 数 名:void ADCConfig(void)
*   功能说明: PD0
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void ADCConfig(void)
{
    ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
    ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;
    singleInit.prsSel = adcPRSSELCh7;
    CMU_ClockEnable(cmuClock_ADC0, true);
    /* Init common settings for both single conversion and scan mode */
    init.timebase = ADC_TimebaseCalc(0);
    /* Might as well finish conversion as quickly as possibly since polling */
    /* for completion. */
    /* Set ADC clock to 7 MHz, use default HFPERCLK */
    init.prescale = ADC_PrescaleCalc(500000, 0);

    /* WARMUPMODE must be set to Normal according to ref manual before */
    /* entering EM2. In this example, the warmup time is not a big problem */
    /* due to relatively infrequent polling. Leave at default NORMAL, */

    ADC_Init(ADC0, &init);

    /* Init for single conversion use, measure VDD/3 with 2.5 reference. */
    singleInit.reference  = adcRef2V5;
    singleInit.input      = adcSingleInpCh7;
    singleInit.resolution = adcRes12Bit;

    /* The datasheet specifies a minimum aquisition time when sampling vdd/3 */
    /* 32 cycles should be safe for all ADC clock frequencies */
    singleInit.acqTime = adcAcqTime128;
    ADC_InitSingle(ADC0, &singleInit);
}
/*
*********************************************************************************************************
*   函 数 名:u8 GetBatteryLever(u16 voltage)
*   功能说明: 获得电量
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
//#pragma optimize=none
u8 GetBatteryLever(u16 voltage)
{
    u8 lever, i;
    /*
    const STU_VOL_LEVER StuLever[15]=
    {
        {4182 ,100},
        {4032,93 },
        {3966,85},
        {3918,78},
        {3864,70},
        {3810,63},
        {3780,55},
        {3730,48},
        {3726,40},
        {3708,33},
        {3684,25},
        {3642,18},
        {3618,9},
        {3600,3},
        {3400,0}
    };
    */
    const STU_VOL_LEVER StuLever[6] =
    {
        {4130, 95},
        {3900, 70 },
        {3750, 45 },
        {3660, 20 },
        {3600, 3 },
        {3550, 0 }
    };

    for ( i = 1; i < 6; i ++ )
    {
        if ( ( voltage <= StuLever[i - 1].vol ) && ( voltage > StuLever[i].vol ) )
        {
            lever = StuLever[i].basiclever;
            break;
        }
    }

    if ( voltage > 4130 )
    {
        lever = 100;
    }
    else if ( voltage <= LOW_VOLTAGE/*3600*/ )
    {
        lever = 0;
    }

    return lever;

#if 0
    u8 lever;

    if( GsmSta.ful == true)
    {
        voltage = 4200;
    }

    if(voltage >= 4032)
    {

        lever = 93 + 3 * (voltage - 4032) / 150;

        if(voltage > 4181)
        {
            lever = 100;
        }
    }
    else     if(voltage >= 3966)
    {
        lever = 85 + 8 * (voltage - 3966) / 66;

    }
    else     if(voltage >= 3760)
    {
        lever = 40 + 40 * (voltage - 3760) / 210;

    }
    else
    {
        lever = 40 * (voltage - 3600) / 160;

        if(lever < 1)
        {
            lever = 0;
        }

    }

    return lever;
#endif
}
/*
*********************************************************************************************************
*   函 数 名:vu8 CalacB(void)
*   功能说明: 电量统计
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void CalacB(void)
{
#if 0
    u16 tmp;
    u8 Battery, cmp;

#define LOWEST 3600
    tmp = GsmSta.voltage;

    if(tmp > 4200)
    {
        tmp = 4200;
    }

    if(tmp < LOWEST)
    {
        Battery = 0;
    }
    else
    {

        Battery = (tmp - LOWEST) * 100 / (4200 - LOWEST);
    }

#else
    u8 Battery, cmp;
    Battery = GetBatteryLever(GsmSta.voltage);
#endif


    if(Battery > GsmSta.Battery)
    {
        cmp = Battery - GsmSta.Battery;
    }
    else
    {
        cmp = GsmSta.Battery - Battery;
    }

    if(GsmSta.charging == BATTERY_CHARGE)
    {
        if( ( (Battery > GsmSta.Battery) || (cmp > 10)) && (GsmSta.Battery < 100))
        {
            GsmSta.Battery++;

        }
    }
    else
    {

        if( ( (Battery < GsmSta.Battery) || (cmp > 10)) && (GsmSta.Battery > 0))
        {
            GsmSta.Battery--;
        }
        else if(GsmSta.Battery == 0)
        {
            GsmSta.Battery = Battery;
        }
    }

    if(cmp > 15)
    {
        GsmSta.Battery = Battery;
    }

}
/*
*********************************************************************************************************
*   函 数 名:u16 GetVoltage(u16 *voltagebuf,u16 times)
*   功能说明: 对电压排序，然后去掉最大最小的几个值，最后求平均值
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
u16 GetVoltage(u16 *voltagebufp, u16 times)
{
    u16 *p;
    u32 tmp;
    u8 i;
    BubbleSort(voltagebufp, times);

    if(GsmSta.charging != BATTERY_CHARGE)
    {
        p = voltagebufp + 2;
    }
    else
    {
        p = voltagebufp + 10;
    }

    tmp = 0;

    for(i = 0; i < (times - 12); i++)
    {
        tmp += p[i];
    }

    tmp = tmp / (times - 12);
    return (u16)tmp;

}
/*
*********************************************************************************************************
*   函 数 名:void ExPower(void)
*   功能说明: 外部电源检测
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
#if 0
void ExPower(void)
{

    static u8 batterybefore = 0xaa;

    if(READ_EXPOWER())
    {
        GsmSta.charging = BATTERY_NOT_CHARGE;
    }
    else
    {
        GsmSta.charging = BATTERY_CHARGE;
    }




    if(batterybefore != GsmSta.charging)
    {
        // if(GsmSta.charging==BATTERY_NOT_CHARGE)
        //   UsbConnectOrNot(false);
        //  else
        //    UsbConnectOrNot(true);
        SendPosition(TRIG_SEND_POSITION);
    }

    batterybefore = GsmSta.charging;

}
#endif
/*
*********************************************************************************************************
*   函 数 名: void lowVoltageDetecate(void)
*   功能说明: 低电压检测
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/

void lowVoltageDetecate(void)
{
    static u8 stu = 0;
    u8 low;
    low = (GsmSta.charging == BATTERY_NOT_CHARGE) && (GsmSta.Battery < GsmSto.BatteryQuatity);

    switch(stu)
    {
        case 0:/*如果没有欠压，进入欠压判断*/
            if(low)
            {
                stu++;
            }

            break;

        case 1:/*连续三次欠压*/
        case 2:
        case 3:
            if(low)
            {
                stu++;
            }
            else
            {
                stu = 0;
            }

            break;

        case 4:/*发送欠压信息*/
            GsmSta.batterylowtrig = 1;
            //SendPosition(TRIG_SEND_POSITION);
            stu++;
            break;

        case 5:/*电压高于%25 或者重新充电*/
            if( (GsmSta.Battery > 25) || (GsmSta.charging != BATTERY_NOT_CHARGE))
            {
                stu = 0;
            }

            break;

        default:
            stu = 0;
            break;

    }



}

/*
*********************************************************************************************************
*   函 数 名:void ADCConfig(void)
*   功能说明: PD0
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/

void ADCGetVoltage(void)
{
#define CAPTURE_TIME_500MS  4
#define CAPTURE_TIMES      20
    static u8 volzero = 0, stu = 0, second = 0, firstTime = 0, prinfFlag = 0;
    static u16 voltagebuf[CAPTURE_TIMES];

    u32 sample;

    //ExPower(); /*hardy delte *
    /*500ms检测一次电压，关机的时候不检测电压*/
    //if(  (second++<CAPTURE_TIME_500MS)||( (StuKey.SystemState==SYSTEM_OFF)&&(GsmSta.charging==BATTERY_NOT_CHARGE)))  return;
    if(second++ < CAPTURE_TIME_500MS)
    {
        return;
    }

    second = 0;

   if( /**/timer.counter %  2 ==  0 || firstTime == 0/* */ )
    {

        ADC_Start(ADC0, adcStartSingle);

        /* Wait while conversion is active */
        while (ADC0->STATUS & ADC_STATUS_SINGLEACT);

        /* Get ADC result */
        sample = ADC_DataSingleGet(ADC0);

        /* Calculate supply voltage relative to 1.25V reference */
        //sample= (sample * 2500 * 147) / (4096*47);
        sample = sample * 1908972 / 1000000;
        if( sample > 1000 )
        {
       //     return;
        }
        voltagebuf[stu++] = (u16)sample;
    }

    if(stu > (CAPTURE_TIMES - 1))
    {
        firstTime = 1;
        stu = 0;
        GsmSta.voltage = GetVoltage(voltagebuf, CAPTURE_TIMES);
        /*
        myprintf ( "[%x-%x %x:%x:%x] voltage is %d\r\n",
                   timer.time[1], timer.time[2],
                   timer.time[3], timer.time[4], timer.time[5],
                   GsmSta.voltage
                 );
*/
        //        myprintf("current Battery voltage is %d\r\n", GsmSta.voltage);
        CalacB();

        if(GsmSta.Battery == 0)
        {
            if(volzero++ > 4)
            {
                StuKey.SystemState = SYSTEM_OFF;
                GsmSta.nopower = 1;
                SendPosition(TRIG_SEND_POSITION);
                //PowerHoldOff();
            }
        }
        else
        {
            volzero = 0;
        }

#ifdef USE_PRINTF

        if(debug == DEBUGADC)
        {
            myprintf("电池电压:%d 电量:%d --charge=%d  \r\n", GsmSta.voltage, GsmSta.Battery, GsmSta.charging);
        }

#endif

    }

    if( timer.counter % 10  == 0  && 0 == prinfFlag)
    {
        
        myprintf ( "[%x-%x %x:%x:%x]Voltage:%d ",
                      timer.time[1], timer.time[2],
                      timer.time[3], timer.time[4], timer.time[5], GsmSta.voltage);
        ue866_operate_status_print();
        ue880_operate_status_print();
        protocol_send_print();
        prinfFlag = 1;
    }
    else if( prinfFlag < 5 && prinfFlag > 0 )
    {
        prinfFlag++;
    }
    else
    {
        prinfFlag= 0;
    }
       

    if(READ_CHARD())
    {
         return;/// For Test By FatQ
        GsmSta.ful = false;

        if(StuKey.SystemState != SYSTEM_OFF)
        {
            GsmSta.charge_shutdown = 0xFF;
            StuKey.SystemState = SYSTEM_OFF;
            gGsmPowerDown = 0xAA;
            //GSM_POWER_OFF();//add by hardy 20170111
            GsmSta.gsm_p = 0x01;
            GsmSta.gps_p = 0x01;
            GpsPowerOff();
            myprintf ( "[%x-%x %x:%x:%x] charger is in, power off \r\n\r\n",
                       timer.time[1], timer.time[2],
                       timer.time[3], timer.time[4], timer.time[5]
                     );
            // myprintf("charger is in, power off \r\n");
        }
    }
    else
    {
        GsmSta.ful = true;

        if(StuKey.SystemState != SYSTEM_ON)
        {
            if(GsmSta.voltage > LOW_VOLTAGE)
            {
                if ( READ_GSMPOWER_STATUS() == 0 )
                {
                    gGsmInit = 0xAA;
                }

                GSM_POWER_ON();
                GpsPowerOn();
                GsmSta.charge_shutdown = 0x10;
                StuKey.SystemState = SYSTEM_ON;
                GsmSta.gsm_p = 0x02;
                GsmSta.gps_p = 0x02;
                gGsmPowerDown = 0;


                //  myprintf("charger is off , power on \r\n");
                myprintf ( "[%x-%x %x:%x:%x] charger is off , power on \r\n\r\n",
                           timer.time[1], timer.time[2],
                           timer.time[3], timer.time[4], timer.time[5] );
                WaitToResetSystem ( 20 );
            }
            else
            {
                // myprintf("Low Battery voltage:%d\r\n,can not startup", GsmSta.voltage);
                myprintf ( "[%x-%x %x:%x:%x] Low Battery voltage:%d\r\n,can not startup \r\n\r\n",
                           timer.time[1], timer.time[2],
                           timer.time[3], timer.time[4], timer.time[5], GsmSta.voltage);

                if( GsmSta.voltage < LOW_VOLTAGE && GsmSta.voltage > LOW_VOLTAGE_PROTECT)
                {
                    if(StuKey.SystemState != SYSTEM_OFF)
                    {
                        GsmSta.charge_shutdown = 0xFF;
                        StuKey.SystemState = SYSTEM_OFF;
                        GsmSta.gsm_p = 0x01;
                        GsmSta.gps_p = 0x01;
                        GpsPowerOff();
                        myprintf ( "[%x-%x %x:%x:%x] low power and power off \r\n\r\n",
                                   timer.time[1], timer.time[2],
                                   timer.time[3], timer.time[4], timer.time[5]
                                 );
                    }
                }
            }
        }
    }

    //}


    /*欠压检测*/
    lowVoltageDetecate();

}

