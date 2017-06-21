#ifndef __BLE_H__
#define __BLE_H__


#define BLE_ON_OFF_PORT  gpioPortC
#define BLE_ON_OFF_PIN   4

#define BLE_WAKE_PORT    gpioPortC
#define BLE_WAKE_PIN     5

#define BLE_POWER_ON()   GPIO_PinOutSet(BLE_ON_OFF_PORT, BLE_ON_OFF_PIN)
#define BLE_POWER_OFF()  GPIO_PinOutClear(BLE_ON_OFF_PORT, BLE_ON_OFF_PIN)



typedef struct
{
    u8 RSSI;
    u8 State;
    u8 time;

} STU_BLE;

extern STU_BLE StuBle;
void BleTask(void);
void BleRevData(u8 Datain);

#endif


