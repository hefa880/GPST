#ifndef LCD_IO_H
#define LCD_IO_H

#include "includes.h"

#define LCD_FOUR_GRAY
#define LCD_128X128

/*hard ware defines*/
#define SPI_SCLK_PORT  gpioPortD
#define SPI_SCLK_PIN   2
#define SPI_SID_PORT   gpioPortD
#define SPI_SID_PIN    0
#define SCL_HIGHT()    GPIO_PinOutSet( SPI_SCLK_PORT, SPI_SCLK_PIN )
#define SCL_LOW()      GPIO_PinOutClear( SPI_SCLK_PORT, SPI_SCLK_PIN )

#define SPI_A0_PORT         gpioPortD
#define SPI_A0_PIN          4
#define SPI_COMMAND_SEND()  GPIO_PinOutClear( SPI_A0_PORT, SPI_A0_PIN )
#define SPI_DATA_SEND()     GPIO_PinOutSet( SPI_A0_PORT, SPI_A0_PIN )

#define SPI_CSB_PORT        gpioPortD
#define SPI_CSB_PIN         3
#define SPI_CSB_VALID()     GPIO_PinOutClear( SPI_CSB_PORT, SPI_CSB_PIN )
#define SPI_CSB_INVALID()   GPIO_PinOutSet( SPI_CSB_PORT, SPI_CSB_PIN )

#define LCD_RST_PORT        gpioPortD
#define LCD_RST_PIN         5
#define LCD_RST_LOW()       GPIO_PinOutClear( LCD_RST_PORT, LCD_RST_PIN )
#define LCD_RST_HIGH()      GPIO_PinOutSet( LCD_RST_PORT, LCD_RST_PIN )

#define LCD_BL_PORT         gpioPortB
#define LCD_BL_PIN          11
#define LCD_BL_OFF()        GPIO_PinOutClear( LCD_BL_PORT, LCD_BL_PIN )
#define LCD_BL_ON()         GPIO_PinOutSet( LCD_BL_PORT, LCD_BL_PIN )

#define LCD_POWER_PORT      gpioPortD
#define LCD_POWER_PIN       8
#define LCD_POWER_OFF()     GPIO_PinOutSet( LCD_POWER_PORT, LCD_POWER_PIN )
#define LCD_POWER_ON()      GPIO_PinOutClear( LCD_POWER_PORT, LCD_POWER_PIN )


#define Slave_Address              0x78       //拉低sa0 0x78      拉高---0x7a
#define OP_Command     0x00                //写命令
#define OP_Data             0x40                    //写数据
#define OP_Data_Two     0xc0                         //写2数据

#define RLED_GPIO_PIN         0
#define RLED_GPIO_PORT        gpioPortC
#define GLED_GPIO_PIN         1
#define GLED_GPIO_PORT        gpioPortC
#define GLED_OFF()            GPIO_PinOutSet( GLED_GPIO_PORT, GLED_GPIO_PIN )
#define GLED_ON()             GPIO_PinOutClear( GLED_GPIO_PORT, GLED_GPIO_PIN )
#define READ_GLED_STATUS()    GPIO_PinInGet( GLED_GPIO_PORT, GLED_GPIO_PIN )

#define GLED_START_STAUTS     ( 0x1 << 8 )
#define GLED_STOP_STAUTS      ( 0x1 << 9 )
#define GLED_BAT_STAUTS       ( 0x0F << 12 )
#define GLED_BAT1_STAUTS      ( 0x1 << 12 )
#define GLED_BAT2_STAUTS      ( 0x1 << 13 )
#define GLED_BAT3_STAUTS      ( 0x1 << 14 )
#define GLED_BAT4_STAUTS      ( 0x1 << 15 )
#define GLED_POWEROFF_ACTION  1


void All_Screen(void);
void Write_Command(u8 command);
void Write_Data(u8 data);
void InitST7571(void);
void ResetLcd(void);
void InitSpiModule(void);
void LcdInit(void);

#define GPS_POSITION  38
#define BLE_POSITION  73
#define BAT_POSITION  108
#define SOS_POSITION  94

#define GPS_COIN_LEN  32
#define BLE_COIN_LEN  32
#define BAT_COIN_LEN  32
#define SOS_COIN_LEN  880


extern const u8 gpscoin[GPS_COIN_LEN];
extern const u8 blecoin[BLE_COIN_LEN];

void FillLcd(u8 flag);  /* Flag=0,Clear LCD; Flaf=0xFF,Full LCE */
void BacklightOnOff(u8 on);
void LCDWriteGsm(u8 qss,u8 csq);
void LCDWritePowerONOff(void);
void LCDWriteBattery(u8 power);
void LCDWriteTime(u8 hour,u8 min,u8 is12Hour,u8 flag);
void LCDWritePowerLever(u8 powerlever);
void LCDWriteCoin(const u8 *coin,u8 coinlen,u8 position,u8 enable);
void LCDWriteSos(const u8 *coin, u8 enable);
void TaskLcd(void);

void LED_Init( void );
void LED_Task( void );
#endif


