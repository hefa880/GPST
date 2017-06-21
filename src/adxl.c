#include"includes.h"
ADXL_STU AdxlStu;

/*
*********************************************************************************************************
*	�� �� ��:void InitAdxl345(void)
*	����˵��: ���ٶȴ�������ʼ��
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void InitAdxl345(void)
{
    volatile  u8  i=0;
    CMU_ClockEnable(cmuClock_USART1, true);
    SPI_setup(1,1,true);








    i=write_byte(XL345_INT_ENABLE,0x00);
    i=read_byte(XL345_INT_ENABLE);
    i=write_byte(XL345_POWER_CTL,0x00);/*��Դģʽpdf 162d 0x08*/
    i=write_byte(XL345_POWER_CTL,0x08);/*��Դģʽpdf 162d 0x08*/
    i=read_byte(XL345_POWER_CTL);
    //�ٶ�����
    i=write_byte(XL345_BW_RATE,0x0a);////*�ٶ�����0x0a-->50hz(1s�ɼ�100��) pdf 15*////
    i=read_byte(XL345_BW_RATE);
    /*������Χ��16gλģʽ31*/
    i=write_byte(XL345_DATA_FORMAT,0x0b);//
    i=read_byte(XL345_DATA_FORMAT);

    /*����32��fifo*/
    i=write_byte(XL345_FIFO_CTL,0x9f);//0x9f
    i=read_byte(XL345_FIFO_CTL);
    /*�ж�ӳ��2f*/
    /*����Ϊ0���͵��ж�1              ����Ϊ1���͵��ж�2*/
    write_byte(XL345_INT_MAP,0xff);/*2fDATA_READY SINGLE_TAP DOUBLE_TAP ACTIVITY INACTIVITY FREE_FALL WATERMARK OVERRUN*/


    //ʹ���ж�
    i=write_byte(XL345_INT_ENABLE,0x02);/*2eʹ��DATA_READY��DATA_READY SINGLE_TAP DOUBLE_TAP Activity	 */
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
*	�� �� ��:void AdxlPowerMode(u8 poweranable)
*	����˵��: ���ٶȴ�����ʡ�磬OK----->��������ģʽ
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
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
            i=write_byte(XL345_POWER_CTL,0x00);/*����ģʽ*/
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
            i=write_byte(XL345_POWER_CTL,0x08);/*��Դģʽpdf 162d 0x08*/
            i=read_byte(XL345_POWER_CTL);
            if(i==0x08)
                return ;
        }

    }


}
/*
*********************************************************************************************************
*	�� �� ��:void Adxl345Irq(void)
*	����˵��: ���ٶȴ������жϴ���
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
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
* ��    �ƣ�void PrintAdxlDate(void)
* ��    �ܣ���ӡ���ٶȴ�������ƽ��ֵ
* ��ڲ�����
* ���ڲ�����
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
        printf("ADXL �᷽���ƽ��ֵ-------->%d,%d,%d\r\n",AdxlStu.stu.result[i][0],AdxlStu.stu.result[i][1],AdxlStu.stu.result[i][2]);



    }
    // for(j=0; j<3; j++)
    //    result[j]/=32;
    //  printf("ADXL �᷽���ƽ��ֵ-------->%d,%d,%d\r\n",result[0],result[1],result[2]);

}
/*
*********************************************************************************************************
*	�� �� ��: u8 ShockCheck(s16 shockcmpvalue)
*	����˵��:�ж����Ƿ񵽴�shockcmpvalue������ͷ���0xaa
*
*	��    �Σ�

*	�� �� ֵ: ��
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

            /*�仯ֵ�ﵽ�����Ҽ����3s*/
            if(tmp>shockcmpvalue)
            {
                if(debug==DEBUGADD)
                    printf("comp:%d--%d ��ֵ:%d\r\n",AdxlStu.stu.result[(j+1)%32][i],AdxlStu.stu.result[j][i],tmp);
                result=0xaa;
            }

        }

    }

    if( (debug==DEBUGADD)&&(result==0xaa))
        printf("--------------->��ǰ��ֵ:%d\r\n",tmp);
    return result;


}
#if 0
/*
*********************************************************************************************************
*	�� �� ��:void AdxlMoveStatic(void)
*	����˵��: ���ٶ��ƶ���ֹ�ж�
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
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
    �ƶ�����:
    10s��û������Ϊ��ֹ
    10��������Ϊ���ƶ�

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
                printf("--------------------------->�˶�\r\n");
        }
    }


    if(times>MOVE_TIME)
    {
        if(MoveTimes<MOVE_TIMES)
        {
            AdxlStu.state=ADXL_STATIC;
            if(debug==DEBUGADD)
                printf("--------------------------->��ֹ\r\n");
        }
        if(debug==DEBUGADD)
            printf("�ƶ�����--------------------------->:%d\r\n",MoveTimes);
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
*	�� �� ��:void TaskAdxl(void)
*	����˵��: ���ٶȴ�����д����
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
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
        if( (offtime==0)&&(StuKey.SystemState!=SYSTEM_OFF))/*�ػ���ʱ��������ģʽ*/
        {
            AdxlPowerMode(OK);
            ontime=ON_TME;
            MoveTimes=0;
            stu=3;
        }
        break;
    case 1:/*�������ģʽ*/
        AdxlPowerMode(NOT_OK);
        stu=2;
        break;
    case 2:/*�ȴ�����*/
        if(StuKey.SystemState!=SYSTEM_OFF)
        {
            stu=0;
        }
        break;
    case 3:/*����*/
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
                    printf("--------------------------->�˶�\r\n");
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
*	�� �� ��:u8 write_byte(u8 add,s8 val)
*	����˵��: ���ٶȴ�����д����
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
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
*	�� �� ��:u8 write_byte(u8 add,s8 val)
*	����˵��: ���ٶȴ�����д����
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
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
*	�� �� ��:void read_mulity_byte(u8 add,u8*data)
*	����˵��: ���ֽڶ�
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
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

