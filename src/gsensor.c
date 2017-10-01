
#include <stdlib.h>
#include "em_i2c.h"
#include "em_gpio.h"

#include "includes.h"
#include "gpio.h"
#include "gsensor.h"

u8 gsStatus = 0xA1;
u8 gGsmInit = 0;

extern u8 gGsmPowerDown;

void initGSensor( void )
{
    u8               nTmp;
    I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

    /* In some situations (after a reset during an I2C transfer), the slave */
    /* device may be left in an unknown state. Send 9 clock pulses just in case. */
    for ( nTmp = 0; nTmp < 9; nTmp++ )
    {
        GPIO_PinOutSet( GSENSOR_SCL_PORT, GSENSOR_SCL_PIN );
        GPIO_PinOutClear( GSENSOR_SCL_PORT, GSENSOR_SCL_PIN );
    }

    I2C0->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | ( 0 << _I2C_ROUTE_LOCATION_SHIFT );

    I2C_Init( I2C0, &i2cInit );

    /* DI3133 GSensor */
    /* 1. Set Mode 0x7, Power off. (Default) */
    nTmp = 0x0;
    GSensorRegisterSet( I2C0, GSENSOR_I2C_ADDR, GSensRegMode, &nTmp, 1 );
    /* 2. Set ODR 0x8. (Default) */
    nTmp = 0x80;
    GSensorRegisterSet( I2C0, GSENSOR_I2C_ADDR, GSensRegODRtr, &nTmp, 1 );
    /* 3. Set Output CFG 0x20. (0x84 允许低通；2G；12位精度) */
    nTmp = 0x84;
    GSensorRegisterSet( I2C0, GSENSOR_I2C_ADDR, GSensRegOutCfg, &nTmp, 1 );

    /* n. Set TAPEN 0x9 & TAP_TH 0xA~0xC. */
    nTmp = 0xFF;
    GSensorRegisterSet( I2C0, GSENSOR_I2C_ADDR, GSensRegTapEn, &nTmp, 1 );
    nTmp = 0x0A;
    GSensorRegisterSet( I2C0, GSENSOR_I2C_ADDR, GSensRegXTapTT, &nTmp, 1 );
    GSensorRegisterSet( I2C0, GSENSOR_I2C_ADDR, GSensRegYTapTT, &nTmp, 1 );
    GSensorRegisterSet( I2C0, GSENSOR_I2C_ADDR, GSensRegZTapTT, &nTmp, 1 );

    /* 4. Set Mode 0x7, Power on. (0x81) */
    nTmp = 0x1;
    GSensorRegisterSet( I2C0, GSENSOR_I2C_ADDR, GSensRegMode, &nTmp, 1 );
}

int GSensorRegisterGet( I2C_TypeDef *i2c, u8 addr, GSENS_Register_TypeDef reg, u8 *data, u8 dataLength )
{
    I2C_TransferSeq_TypeDef    seq;
    I2C_TransferReturn_TypeDef ret;
    u8                         regid[1];
    u32                        timeout = 300000;

    seq.addr  = addr;
    seq.flags = I2C_FLAG_WRITE_READ;
    /* Select register to be read */
    regid[0]        = (u8)reg;
    seq.buf[0].data = regid;
    seq.buf[0].len  = 1;
    /* Select location/length to place register */
    seq.buf[1].data = data;
    seq.buf[1].len  = dataLength;

    /* Do a polled transfer */
    ret = I2C_TransferInit( i2c, &seq );

    while ( ret == i2cTransferInProgress && timeout-- )
    {
        ret = I2C_Transfer( i2c );
    }

    return ret;
}

int GSensorRegisterSet( I2C_TypeDef *i2c, u8 addr, GSENS_Register_TypeDef reg, u8 *data, u8 dataLength )
{
    I2C_TransferSeq_TypeDef    seq;
    I2C_TransferReturn_TypeDef ret;
    u8                         regid[1];
    u32                        timeout = 300000;

    seq.addr  = addr;
    seq.flags = I2C_FLAG_WRITE_WRITE;
    /* Select register to be write */
    regid[0]        = (u8)reg;
    seq.buf[0].data = regid;
    seq.buf[0].len  = 1;
    /* Select location/length to place register */
    seq.buf[1].data = data;
    seq.buf[1].len  = dataLength;

    /* Do a polled transfer */
    ret = I2C_TransferInit( i2c, &seq );

    while ( ret == i2cTransferInProgress && timeout-- )
    {
        ret = I2C_Transfer( i2c );
    }

    return ret;
}
#define GS_STOP 3000
void GSensorTask( void )
{
    s32        ret;
    static u8  nTmp;
    static u8  nValue[6];
    static s16 nX = 0x7FFF, nY = 0x7FFF, nZ = 0x7FFF;
    static s16 nNewX, nNewY, nNewZ;
    static u32 gsCounter = 0;

    if ( StuKey.SystemState == SYSTEM_OFF )
    {
        return;
    }



    return; //  disable



    gsCounter ++;

    /* 5. Read 0xD */
    ret = GSensorRegisterGet( I2C0, GSENSOR_I2C_ADDR, GSensRegCntXL, nValue, 6 );

    if ( ret != 0 )
    {
        return;
    }

    nNewX = (s16)(nValue[1] << 8 | nValue[0]);
    nNewY = (s16)(nValue[3] << 8 | nValue[2]);
    nNewZ = (s16)(nValue[5] << 8 | nValue[4]);

    if (( nX == 0x7FFF ) && ( nY == 0x7FFF ) && ( nZ == 0x7FFF ))
    {
        nX = nNewX;
        nY = nNewY;
        nZ = nNewZ;
    }

    //myprintf( "Chenyong: %d, %d, %d\r\n", nNewX, nNewY, nNewZ );

    if ( ( gsStatus == 0xA1 ) && (( abs(nNewX - nX) > 120 ) || ( abs(nNewY - nY) > 120 ) || ( abs(nNewZ - nZ) > 120 )) )
    {
        gsStatus  = 0xA2;
        gsCounter = 0;

        //GsmSta.LCDState = FLASH_TIME;
        //BacklightOnOff( true );
#ifdef USE_PRINTF
        myprintf( "Chenyong: Back light On.\r\n" );
#endif
    }
    else
    {
        /* 6. Read 0x3 */
        ret = GSensorRegisterGet( I2C0, GSENSOR_I2C_ADDR, GSensRegTapStatus, &nTmp, 1 );

        if (( ret == 0 ) && ( nTmp > 0x80 ) && (GsmSta.voltage > 3600))
        {
            gsStatus  = 0xA1;
            gsCounter = 0;

            if ( READ_GSMPOWER_STATUS() == 0 )
            {

                gGsmInit = 0xAA;
                //  GSM_POWER_ON();
                gGsmPowerDown = 0;
                // GpsPowerOn();

                NVIC_SystemReset();  // Add FatQ 20170930
                myprintf( "Gsensor: System On.\r\n" );
            }
        }
    }

    nX = nNewX;
    nY = nNewY;
    nZ = nNewZ;

    if (( gsCounter > GS_STOP ) && ( gsStatus != 0 ))
    {
        gsStatus  = 0;
        gsCounter = 0;

        GpsPowerOff();

        //BLE_POWER_OFF();
        //  GSM_POWER_OFF(); /**/
        GsmSta.gsm_p = MASK_POWER_STATUS_OFF;

        gGsmPowerDown = 0xAA;
        myprintf( "Gsensor: System Off.\r\n" );
    }
}

