#include"includes.h"
#include "ue866_gpio.h"

#define UE866_GPIO_RST_PIN   15
#define UE866_GPIO_RST_PORT  gpioPortA

#define UE866_GPIO_DTR_PIN   13
#define UE866_GPIO_DTR_PORT  gpioPortE

#define UE866_GPIO_RTS_PIN   8
#define UE866_GPIO_RTS_PORT  gpioPortE

#define UE866_GPIO_CTS_PIN   9
#define UE866_GPIO_CTS_PORT  gpioPortE


#define UE866_GPIO_TX_PIN   14
#define UE866_GPIO_TX_PORT  gpioPortE
#define UE866_GPIO_RX_PIN   15
#define UE866_GPIO_RX_PORT  gpioPortE


//#define UE866_GPIO_DTR_ENABLE()    GPIO_PinOutClear(UE866_GPIO_DTR_PORT,UE866_GPIO_DTR_PIN)
//#define UE866_GPIO_DTR_DISABLE()   GPIO_PinOutSet(UE866_GPIO_DTR_PORT,UE866_GPIO_DTR_PIN)

#define UE866_GPIO_GSM_RESET_ON()  GPIO_PinOutSet(UE866_GPIO_RST_PORT,UE866_GPIO_RST_PIN)
#define UE866_GPIO_GSM_RESET_OFF()  GPIO_PinOutClear(UE866_GPIO_RST_PORT,UE866_GPIO_RST_PIN)

//#define UE866_GPIO_RTS_READY_0()       GPIO_PinOutClear(UE866_GPIO_RTS_PORT,UE866_GPIO_RTS_PIN)
//#define UE866_GPIO_RTS_UNREADY_1()       GPIO_PinOutSet(UE866_GPIO_RTS_PORT,UE866_GPIO_RTS_PIN)



#define UE866_GPIO_POWER_CONTROL_PORT    gpioPortA
#define UE866_GPIO_POWER_CONTROL_PIN     8
//#define UE866_GPIO_POWER_ON()            GPIO_PinOutSet( UE866_GPIO_POWER_CONTROL_PORT,UE866_GPIO_POWER_CONTROL_PIN )
//#define UE866_GPIO_POWER_OFF()           GPIO_PinOutClear( UE866_GPIO_POWER_CONTROL_PORT,UE866_GPIO_POWER_CONTROL_PIN )
// #define UE866_GPIO_READ_GSMPOWER_STATUS()    GPIO_PinInGet( UE866_GPIO_POWER_CONTROL_PORT, UE866_GPIO_POWER_CONTROL_PIN )




void ue866_gpio_int(void )
{

}

void ue866_gpio_uart_init(void )
{

    GPIO_PinModeSet ( UE866_GPIO_RST_PORT, UE866_GPIO_RST_PIN, gpioModePushPull, 0 );
    GPIO_PinModeSet ( UE866_GPIO_CTS_PORT, UE866_GPIO_CTS_PIN, gpioModeInputPull, 1 );
    GPIO_PinModeSet ( UE866_GPIO_DTR_PORT, UE866_GPIO_DTR_PIN, gpioModePushPull, 0 );
    GPIO_PinModeSet ( UE866_GPIO_RTS_PORT, UE866_GPIO_RTS_PIN, gpioModePushPull, 0 );
}


//  开启或是关闭物理电源
int ue866_gpio_power(bool bEnable )
{
    int ret = 0;

    ue866_gpio_uart_init();
    if( true == bEnable  )
    {
        GPIO_PinOutSet( UE866_GPIO_POWER_CONTROL_PORT, UE866_GPIO_POWER_CONTROL_PIN );
    }
    else
    {
        /* 需要执行以下两个操作才可以移除电源 */
        // 1,  AT#SYSHALT
        //  2, 10s Timeout
        GPIO_PinOutClear( UE866_GPIO_POWER_CONTROL_PORT, UE866_GPIO_POWER_CONTROL_PIN );

    }

    return ret;
}

int ue866_gpio_cts( void )
{

    return GPIO_PinInGet ( UE866_GPIO_CTS_PORT, UE866_GPIO_CTS_PIN ); /* 1表示在工作 */;

}


int ue866_gpio_rts(bool bEnable)
{
    int ret = 0;

    if( true == bEnable )
    {
        // ready to rev data
        GPIO_PinOutClear(UE866_GPIO_RTS_PORT, UE866_GPIO_RTS_PIN);
    }
    else
    {
        GPIO_PinOutSet(UE866_GPIO_RTS_PORT, UE866_GPIO_RTS_PIN);
    }

    return ret;

}


int ue866_gpio_dtr(bool bEnable)
{
    int ret = 0;

    if( true == bEnable )
    {
        GPIO_PinOutClear(UE866_GPIO_DTR_PORT, UE866_GPIO_DTR_PIN);
    }
    else
    {
        GPIO_PinOutSet(UE866_GPIO_DTR_PORT, UE866_GPIO_DTR_PIN);
    }

    return ret;

}



// 复位模块
int ue866_gpio_reset(void)
{
    int ret = 0;
    // u32 delayTime = get_system_time ();

    ue866_gpio_power(false);
    Rtc = 0;

    while( 1 )
    {
        if(Rtc > 10)
        {
            break;
        }
    };

    //  ret = 1;
    ue866_gpio_power(true);

    GsmInit();

#if 0
    // RESET* = LOW
    GPIO_PinOutClear(UE866_GPIO_RST_PORT, UE866_GPIO_RST_PIN);

    // Delay = 200ms
    Rtc = 0;

    while( 1 )
    {
        if(Rtc > 0x04)
        {
            break;
        }
    };

    Rtc = 0;

    // RESET* = HIGH
    GPIO_PinOutSet(UE866_GPIO_RST_PORT, UE866_GPIO_RST_PIN);

    while( 1 )
    {
        if(Rtc > 10)
        {
            break;
        }
    }

    Rtc = 0;
    //  Delay = 1s
    //      GPIO_PinOutClear(UE866_GPIO_RST_PORT,UE866_GPIO_RST_PIN);
#endif
    return ret;
}

//获取模块上电状态
bool ue866_gpio_power_state(void)
{
    // 这里获取只是有没有开启电源开关而已，并没有真正检测到系统已经开机成功，需要AT指令检测
    //
    int ret =  GPIO_PinInGet( UE866_GPIO_POWER_CONTROL_PORT, UE866_GPIO_POWER_CONTROL_PIN );

    return (ret == 0 ? false : true);
}


