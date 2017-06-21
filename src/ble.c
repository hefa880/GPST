#include"includes.h"


extern u8 gsStatus;

STU_BLE StuBle;

/*
*********************************************************************************************************
*	函 数 名:void BleRevData(u8 Datain)
*	功能说明: BLE串口数据接收
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void BleRevData(u8 Datain)
{
    static u8 stu=0,rssi_state[2];
    u8 varity;
    switch(stu)
    {
    case 0:
        if(Datain==0xaa)
            stu++;
        break;
    case 1:
        if(Datain==0x55)
            stu++;
        else
            stu=0;
        break;

    case 2:
        if(Datain==0x01)
            stu++;
        else
            stu=0;
        break;
    case 3:
        if(Datain==0x02)
            stu++;
        else
            stu=0;
        break;
    case 4:
    case 5:
        rssi_state[stu-4]=Datain;
        stu++;
        break;
    case 6:
        varity=0xfc;
        varity^=rssi_state[0];
        varity^=rssi_state[1];
        if(Datain==varity)
        {
            StuBle.RSSI=rssi_state[0];
            StuBle.State=rssi_state[1];
            if(  StuBle.State==2)
                GsmSta.ble=1;
            else
                GsmSta.ble=0;
            StuBle.time=0;
        }
    default:
        stu=0;
        break;

    }


}


/*
*********************************************************************************************************
*	函 数 名:void BleTask(void)
*	功能说明: 蓝牙串口连接
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void BleTask(void)
{
#define Over_time    150     /*15s--150*/
    static u8 stu=0,sta=0;

    if ( ( gsStatus == 0 ) || ( StuKey.SystemState == SYSTEM_OFF ) )
    {
        return;
    }

    switch(stu)
    {
    case 0:
        GPIO_PinModeSet(BLE_ON_OFF_PORT, BLE_ON_OFF_PIN, gpioModePushPull, 1);  /*pwr---0*/
        BLE_POWER_ON();
        StuBle.State=0;
        StuBle.time=0;
        GsmSta.ble=0;
        stu++;
        break;
    case 1:
        if(StuBle.time++>(Over_time-1))
            /*超时重启*/
        {
            StuBle.time=0;
            BLE_POWER_OFF();
            stu++;
        }
        if( StuKey.SystemState==SYSTEM_OFF)
        {
            BLE_POWER_OFF();
            stu=3;
        }
        if(sta!=StuBle.State)
        {
            /* SendPosition(TRIG_SEND_POSITION); */
        }
        sta=StuBle.State;
        break;
    case 3:
        if( StuKey.SystemState==SYSTEM_ON)
        {
            stu=0;
            break;

        }
        break;
    case 2:
        if( StuBle.time++<30)
        {
            break;
        }
    default:
        stu=0;
        break;

    }


}
