#ifndef GPIO_H
#define GPIO_H


#include "date_type.h"
#define SYSTEM_ON   0xaa
#define SYSTEM_OFF  0x55


#define ON_OFF_PRESS            1
#define SOS_PRESS               2
#define NOT_PRESS               0
#define MODEM_IPR               115200
#define GPS_IPR                 38400
#define LOW_VOLTAGE             3560
#define LOW_VOLTAGE_PROTECT     3400


#define SOS_TIMES  3
typedef __packed struct
{
    u8 keypress_sos:4;/*按下返回OK*/
    u8 keypress_on:4;/*按下返回OK*/
    u8 timeOnOff;/*按键时长*/
	u8 timeSos;/*按键时长*/
	u8 SosSendTime;/*上报的次数*/
    u8 SystemState;/*系统开关机状态*/
	u8 processend;/*界面显示结束0xaa*/
} STU_KEY;
extern STU_KEY  StuKey;

typedef __packed struct{
u16 vol;
u8 basiclever;
}STU_VOL_LEVER;
#if 0
#define CHARGE_PORT  gpioPortA    /*配置为上拉输入，当usb连接，并且为低时表示充满*/
#define CHARGE_PIN   10
#define READ_CHARGE()             GPIO_PinInGet(CHARGE_PORT,CHARGE_PIN)
#endif
#if 0
#define EXT_POWER_PORT  gpioPortA  /*配置为上拉输入，当usb连接时为低*/
#define EXT_POWER_PIN   9
#define READ_EXPOWER()            GPIO_PinInGet(EXT_POWER_PORT,EXT_POWER_PIN)
#endif
#if 0
#define ON_OFF_PORT  gpioPortA
#define ON_OFF_PIN   4
#define READ_ON_OFF()             GPIO_PinInGet(ON_OFF_PORT,ON_OFF_PIN)
#endif
#if 0
#define SOS_PORT  gpioPortA
#define SOS_PIN   5
#define READ_SOS()                GPIO_PinInGet(SOS_PORT,SOS_PIN)
#endif
#define CHARGE_DET_PORT  gpioPortA
#define CHARD_PIN   6
#define READ_CHARD()                GPIO_PinInGet(CHARGE_DET_PORT,CHARD_PIN)
#if 0
#define POWER_HOLD_PORT           gpioPortA
#define POWER_HOLD_PIN            6
#define PowerHoldOn()             GPIO_PinOutSet( POWER_HOLD_PORT, POWER_HOLD_PIN )
#define PowerHoldOff()            GPIO_PinOutClear( POWER_HOLD_PORT, POWER_HOLD_PIN )
#endif

#define GSM_POWER_CONTROL_PORT    gpioPortA
#define GSM_POWER_CONTROL_PIN     8
#define GSM_POWER_ON()            GPIO_PinOutSet( GSM_POWER_CONTROL_PORT,GSM_POWER_CONTROL_PIN )
#define GSM_POWER_OFF()           GPIO_PinOutClear( GSM_POWER_CONTROL_PORT,GSM_POWER_CONTROL_PIN )
#define READ_GSMPOWER_STATUS()    GPIO_PinInGet( GSM_POWER_CONTROL_PORT, GSM_POWER_CONTROL_PIN )
#if 0
#define GSM_MUTE_PIN              12
#define GSM_MUTE_PORT             gpioPortE
#define GsmMuteOff()              GPIO_PinOutSet( GSM_MUTE_PORT, GSM_MUTE_PIN )
#define GsmMuteOn()               GPIO_PinOutClear( GSM_MUTE_PORT, GSM_MUTE_PIN )
#endif

#define GSENSOR_SDA_PORT          gpioPortA
#define GSENSOR_SDA_PIN           0
#define GSENSOR_SCL_PORT          gpioPortA
#define GSENSOR_SCL_PIN           1
#if 0
#define GSENSOR_INT1_PORT         gpioPortA
#define GSENSOR_INT1_PIN          2
#endif
#define GSENSOR_INT2_PORT         gpioPortA
#define GSENSOR_INT2_PIN          3


#define NOT_CHARGE 0
#define CHARGE25 1
#define CHARGE50 2
#define CHARGE75 3
#define CHARGE100 4
#define CHARGEFUL 5



void InitGPIOs(void);
//void KeyTask(void);
void ADCGetVoltage(void);
void ADCConfig(void);
#endif


