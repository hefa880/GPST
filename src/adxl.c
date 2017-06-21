#include"includes.h"
ADXL_STU AdxlStu;

/*
*********************************************************************************************************
*	函 数 名:void InitAdxl345(void)
*	功能说明: 加速度传感器初始化
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

void InitAdxl345(void)
{
    volatile  u8  i=0;
    CMU_ClockEnable(cmuClock_USART1, true);
    SPI_setup(1,1,true);








    i=write_byte(XL345_INT_ENABLE,0x00);
    i=read_byte(XL345_INT_ENABLE);
    i=write_byte(XL345_POWER_CTL,0x00);/*电源模式pdf 162d 0x08*/
    i=write_byte(XL345_POWER_CTL,0x08);/*电源模式pdf 162d 0x08*/
    i=read_byte(XL345_POWER_CTL);
    //速度设置
    i=write_byte(XL345_BW_RATE,0x0a);////*速度设置0x0a-->50hz(1s采集100次) pdf 15*////
    i=read_byte(XL345_BW_RATE);
    /*测量范围，16g位模式31*/
    i=write_byte(XL345_DATA_FORMAT,0x0b);//
    i=read_byte(XL345_DATA_FORMAT);

    /*设置32个fifo*/
    i=write_byte(XL345_FIFO_CTL,0x9f);//0x9f
    i=read_byte(XL345_FIFO_CTL);
    /*中断映射2f*/
    /*设置为0发送到中断1              设置为1发送到中断2*/
    write_byte(XL345_INT_MAP,0xff);/*2fDATA_READY SINGLE_TAP DOUBLE_TAP ACTIVITY INACTIVITY FREE_FALL WATERMARK OVERRUN*/


    //使能中断
    i=write_byte(XL345_INT_ENABLE,0x02);/*2e使能DATA_READY中DATA_READY SINGLE_TAP DOUBLE_TAP Activity	 */
    i=read_byte(XL345_INT_ENABLE);
    write_byte(XL345_OFSX,0);//pdf 14
    write_byte(XL345_OFSY,0);	//	-xx[1]/16
    write_byte(XL345_OFSZ,0);	   //-xx[2]/16
    read_mulity_byte(XL345_DATAX0,AdxlStu.stu.buf[0]);
    i=read_byte(XL345_FIFO_STATUS);


    /*INT1*/

    //  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    //    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    GPIO_PinModeSet(INT_PORT, INT_PIN, gpioModeInput, 0);
    /* Configure PB 0 interrupt on falling edge */
    GPIO_IntConfig(INT_PORT, INT_PIN, true,false,  true);
    read_mulity_byte(XL345_DATAX0,AdxlStu.stu.buf[0]);
    i=read_byte(XL345_FIFO_STATUS);

}


/*
*********************************************************************************************************
*	函 数 名:void AdxlPowerMode(u8 poweranable)
*	功能说明: 加速度传感器省电，OK----->正常工作模式
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
//#pragma optimize=none
void AdxlPowerMode(u8 poweranable)
{
    u8 i,j;



    if(OK!=poweranable)
    {

        for(j=0; j<3; j++)
        {
            i=write_byte(XL345_POWER_CTL,0x00);/*待机模式*/
            i=read_byte(XL345_POWER_CTL);
            if(i==0)
            {
                GPIO_IntConfig(INT_PORT, INT_PIN, true,false,  false);
                CMU_ClockEnable(cmuClock_USART1, false);
                GPIO_PinModeSet(gpioPortD, 0, gpioModeInput, 0);  /* MOSI */
                GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);  /* MISO */
                GPIO_PinModeSet(gpioPortD, 3, gpioModeInput,   0);  /* CS */
                GPIO_PinModeSet(gpioPortD, 2, gpioModeInput,  0);  /* Clock */
                Delayms(1000);
                return ;
            }
        }

    }
    else
    {
        SPI_setup(1,1,true);
        CMU_ClockEnable(cmuClock_USART1, true);
        GPIO_IntConfig(INT_PORT, INT_PIN, true,false,  true);
        Delayms(1000);
        for(j=0; j<3; j++)
        {
            i=write_byte(XL345_POWER_CTL,0x08);/*电源模式pdf 162d 0x08*/
            i=read_byte(XL345_POWER_CTL);
            if(i==0x08)
                return ;
        }

    }


}
/*
*********************************************************************************************************
*	函 数 名:void Adxl345Irq(void)
*	功能说明: 加速度传感器中断处理
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void Adxl345Irq(void)
{


    static u8 i=0;
    u8 k;
    do
    {
        i=i%32;
        read_mulity_byte(XL345_DATAX0,AdxlStu.stu.buf[i]);
        k=read_byte(XL345_FIFO_STATUS);
        i++;
    }
    while(0x3f&k);
    AdxlStu.flag=0xaa;


}
/**********************************************************************
* 名    称：void PrintAdxlDate(void)
* 功    能：打印加速度传感器的平均值
* 入口参数：
* 出口参数：
-----------------------------------------------------------------------
***********************************************************************/
void PrintAdxlDate(void)
{

    //    static u8 time=0;
    //   u8 i,j;
    //    s32 result[3]= {0,0,0};
    //   if(time++<20) return;
    //   time=0;
    u8 i;
    for(i=0; i<32; i++)
    {
        //   for(j=0; j<3; j++)
        //		{
        //     result[j]+=AdxlStu.stu.result[i][j];
        //		}
        printf("ADXL 轴方向的平均值-------->%d,%d,%d\r\n",AdxlStu.stu.result[i][0],AdxlStu.stu.result[i][1],AdxlStu.stu.result[i][2]);



    }
    // for(j=0; j<3; j++)
    //    result[j]/=32;
    //  printf("ADXL 轴方向的平均值-------->%d,%d,%d\r\n",result[0],result[1],result[2]);

}
/*
*********************************************************************************************************
*	函 数 名: u8 ShockCheck(s16 shockcmpvalue)
*	功能说明:判断震动是否到达shockcmpvalue，到达就返回0xaa
*
*	形    参：

*	返 回 值: 无
*********************************************************************************************************
*/
u8 ShockCheck(s16 shockcmpvalue)
{
    u8 i,j;
    s16 tmp;
    u8 result=0;
    for(j=0; j<32; j++)
    {
        for(i=0; i<3; i++)
        {
            tmp=AdxlStu.stu.result[(j+1)%32][i]-AdxlStu.stu.result[j][i];
            if(tmp<0)
                tmp=-tmp;

            /*变化值达到，并且间隔有3s*/
            if(tmp>shockcmpvalue)
            {
                if(debug==DEBUGADD)
                    printf("comp:%d--%d 差值:%d\r\n",AdxlStu.stu.result[(j+1)%32][i],AdxlStu.stu.result[j][i],tmp);
                result=0xaa;
            }

        }

    }

    if( (debug==DEBUGADD)&&(result==0xaa))
        printf("--------------->当前差值:%d\r\n",tmp);
    return result;


}
#if 0
/*
*********************************************************************************************************
*	函 数 名:void AdxlMoveStatic(void)
*	功能说明: 加速度移动静止判断
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void AdxlMoveStatic(void)
{

#define MOVE_VALUE  8
#define MOVE_TIME     30           // 10/0.3
#define MOVE_TIMES    2
    static u16 times=0;
    static u8 MoveTimes=0;
    /*
    移动策略:
    10s内没有震动认为静止
    10内有震动认为在移动

    */






    if(AdxlStu.flag!=0xaa)  return;



    if(times<0xfffd)
        times++;
    if(0xaa== ShockCheck(MOVE_VALUE))
    {
        MoveTimes++;
        if(MoveTimes>MOVE_TIMES)
        {
            AdxlStu.state=ADXL_MOVE;
            if(debug==DEBUGADD)
                printf("--------------------------->运动\r\n");
        }
    }


    if(times>MOVE_TIME)
    {
        if(MoveTimes<MOVE_TIMES)
        {
            AdxlStu.state=ADXL_STATIC;
            if(debug==DEBUGADD)
                printf("--------------------------->静止\r\n");
        }
        if(debug==DEBUGADD)
            printf("移动次数--------------------------->:%d\r\n",MoveTimes);
        MoveTimes=0;
        times=0;
    }


    // if(debug==DEBUGADD)
    //  PrintAdxlDate();





    AdxlStu.flag=0;

}

#endif

/*
*********************************************************************************************************
*	函 数 名:void TaskAdxl(void)
*	功能说明: 加速度传感器写数据
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void TaskAdxl(void)
{
#define ON_TME   20
#define OFF_TIME  20


#define MOVE_VALUE  15
#define MOVE_TIMES   2 // 4
    static u8 stu=0,MoveTimes=0;
    static u16 ontime=0,offtime=0;

#if 0
    if(AdxlStu.state!=ADXL_MOVE)
    {
        AdxlStu.state=ADXL_MOVE;

    }
    return;
#endif
    switch(stu)
    {
    case 0:
        if(offtime) offtime--;
        if( (offtime==0)&&(StuKey.SystemState!=SYSTEM_OFF))/*关机的时候进入待机模式*/
        {
            AdxlPowerMode(OK);
            ontime=ON_TME;
            MoveTimes=0;
            stu=3;
        }
        break;
    case 1:/*进入待机模式*/
        AdxlPowerMode(NOT_OK);
        stu=2;
        break;
    case 2:/*等待开机*/
        if(StuKey.SystemState!=SYSTEM_OFF)
        {
            stu=0;
        }
        break;
    case 3:/*工作*/
    case 4:
    case 5:
    case 6:
        stu++;
        break;
    case 7:
        if(ontime)
        {
            ontime--;
            if(0xaa== ShockCheck(MOVE_VALUE))
            {
                if(MoveTimes<0xfd)
                    MoveTimes++;
            }
        }
        else
        {

            if(MoveTimes>=MOVE_TIMES)
            {

                AdxlStu.time=MOVE_TIME;
                if(AdxlStu.state!=ADXL_MOVE)
                {
                    AdxlStu.state=ADXL_MOVE;
                }
                if(debug==DEBUGADD)
                    printf("--------------------------->运动\r\n");
            }






            AdxlPowerMode(NOT_OK);
            offtime=OFF_TIME;
            stu=0;
        }
        break;
    default:
        stu=0;
        break;


    }


}
/*
*********************************************************************************************************
*	函 数 名:u8 write_byte(u8 add,s8 val)
*	功能说明: 加速度传感器写数据
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

u8 write_byte(u8 add,s8 val)
{
    USART_TypeDef *usart=USART1;
    U16STU j;
    j.stu.H=val;
    j.stu.L=add&0x7f;
    ADXLCS_LOW();
    //  j.total=0xaa55;
    while (!(usart->STATUS & USART_STATUS_TXBL));

    usart->TXDOUBLE= (uint32_t)j.total;
    //   usart->TXDOUBLEX = (uint32_t)j.total;
    // usart->TXDATA =
    while (!(usart->STATUS & USART_STATUS_TXC));
    // return (uint8_t)(usart->RXDATA);
    j.total=usart->RXDOUBLE;
    ADXLCS_HiGHT();
    return j.stu.H;
    //   return (uint8_t)(usart->RXDOUBLEX);

}
/*
*********************************************************************************************************
*	函 数 名:u8 write_byte(u8 add,s8 val)
*	功能说明: 加速度传感器写数据
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
//u16 tt;
u8 read_byte(u8 add)
{
    USART_TypeDef *usart=USART1;
    U16STU j;
    j.stu.H=0xff;
    j.stu.L=add|0x80;
    ADXLCS_LOW();
    while (!(usart->STATUS & USART_STATUS_TXBL));
    // usart->TXDATA = (uint32_t)j.total;
    usart->TXDOUBLE= (uint32_t)j.total;
    while (!(usart->STATUS & USART_STATUS_TXC));
    //  return (uint8_t)(usart->RXDATA);
    j.total=usart->RXDOUBLE;
    // tt=j.total;
    ADXLCS_HiGHT();
    return j.stu.H;

}
/*
*********************************************************************************************************
*	函 数 名:void read_mulity_byte(u8 add,u8*data)
*	功能说明: 多字节读
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void read_mulity_byte(u8 add,u8*data)
{

    u8 i;
    U16STU j;
    j.stu.H=0xff;
    j.stu.L=add|0xc0;
    USART_TypeDef *usart=USART1;

    ADXLCS_LOW();
    for(i=0; i<4; i++)
    {

        while (!(usart->STATUS & USART_STATUS_TXBL));
        if(i==0)
            usart->TXDOUBLE= (uint32_t)j.total;
        else
            usart->TXDOUBLE=0x0000ffff;

        while (!(usart->STATUS & USART_STATUS_TXC));

        j.total=usart->RXDOUBLE;
        if(i==0)
        {
            data[0]= j.stu.H;

        }
        else if(i==1)
        {
            data[1]= j.stu.L;
            data[2]= j.stu.H;
        }
        else if(i==2)
        {
            data[3]= j.stu.L;
            data[4]= j.stu.H;
        }
        else if(i==3)
        {
            data[5]= j.stu.L;
        }
    }

    ADXLCS_HiGHT();

}

