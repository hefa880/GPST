#ifndef ADXL__H
#define ADXL__H


#define INT_PORT gpioPortC
#define INT_PIN  15
/* ------- Register names ------- */
#define XL345_DEVID           0x00
#define XL345_RESERVED1       0x01
#define XL345_THRESH_TAP      0x1d//敲击中断阀值
#define XL345_OFSX            0x1e
#define XL345_OFSY            0x1f
#define XL345_OFSZ            0x20
#define XL345_DUR             0x21//时间
#define XL345_LATENT          0x22
#define XL345_WINDOW          0x23
#define XL345_THRESH_ACT      0x24
#define XL345_THRESH_INACT    0x25
#define XL345_TIME_INACT      0x26
#define XL345_ACT_INACT_CTL   0x27
#define XL345_THRESH_FF       0x28
#define XL345_TIME_FF         0x29
#define XL345_TAP_AXES        0x2a
#define XL345_ACT_TAP_STATUS  0x2b
#define XL345_BW_RATE         0x2c
#define XL345_POWER_CTL       0x2d
#define XL345_INT_ENABLE      0x2e
#define XL345_INT_MAP         0x2f
#define XL345_INT_SOURCE      0x30
#define XL345_DATA_FORMAT     0x31
#define XL345_DATAX0          0x32
#define XL345_DATAX1          0x33
#define XL345_DATAY0          0x34
#define XL345_DATAY1          0x35
#define XL345_DATAZ0          0x36
#define XL345_DATAZ1          0x37
#define XL345_FIFO_CTL        0x38
#define XL345_FIFO_STATUS     0x39


#define ADXL_MOVE   0xaa
#define ADXL_STATIC 0x55
#define MOVE_TIME 30
typedef __packed struct
{
    union
    {
        u8 buf[32][6];
        s16 result[32][3];
    } stu;
   // union
   // {
   //     u8 buf[32][6];
   //     s16 result[32][3];
   // } stubvk;
 u8 flag;
u8 state;
u8 time;

}ADXL_STU;
extern ADXL_STU AdxlStu;

#define ADXLCS_HiGHT()  GPIO_PinOutSet(gpioPortD, 3)
#define ADXLCS_LOW()  GPIO_PinOutClear(gpioPortD, 3)




void Initspi(void);


void InitAdxl345(void);
void Adxl345Irq(void);
u8 read_byte(u8 add);
u8 write_byte(u8 add,s8 val);
void read_mulity_byte(u8 add,u8*data);
void PrintAdxlDate(void);
void Adxl345Irq(void);
void TaskAdxl(void);
void AdxlPowerMode(u8 poweranable);
#endif



