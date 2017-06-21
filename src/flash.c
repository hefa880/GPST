#include"includes.h"


//STRUCT_FLASH_CTRL  StuFlashCtrl;
#pragma data_alignment=256
volatile DMA_DESCRIPTOR_TypeDef descr;


/*
*********************************************************************************************************
*	函 数 名:void FLASH_CalcPageSize(void)
*	功能说明: get flash page size
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void FLASH_CalcPageSize(void)
{
    u8 family = *(uint8_t*)0x0FE081FE;
    u32 PageSize;
    if ( ( family == 71 ) || ( family == 73 ) )
    {
        PageSize = 512;                /* Gecko and Tiny, 'G' or 'I' */
    }
    else if ( family == 72 )
    {
        PageSize = 4096;               /* Giant, 'H' */
    }
    else /* if ( family == 74 ) */
    {
        PageSize = 2048;               /* Leopard, 'J' */
    }


}
#endif
/*
*********************************************************************************************************
*	函 数 名:void FLASH_init(void)
*	功能说明: Initializes the Flash programmer
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void FLASH_init(void)
{
    /* Write MSC unlock code to enable interface */
    MSC->LOCK = MSC_UNLOCK_CODE;
    /* Enable memory controller */
    MSC->WRITECTRL |= MSC_WRITECTRL_WREN;
    /* Enable DMA */
    DMA->CONFIG = DMA_CONFIG_EN;
    /* Setup the DMA control block. */
    DMA->CTRLBASE = (uint32_t) &descr;
}
/*
*********************************************************************************************************
*	函 数 名:void FLASH_Deinit(void)
*	功能说明: DeInitializes the Flash programmer
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void FLASH_Deinit(void)
{
    /* Disable writing to the flash */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    /* Lock the MSC */
    MSC->LOCK = 0;
    /* DISABLE DMA */
    DMA->CONFIG &= (~DMA_CONFIG_EN);

}

/*
*********************************************************************************************************
*	函 数 名:void FlashProcess(u8 Enable_Disable)
*	功能说明:操作flash初始化
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void FlashProcess(u32 ri0,u8 Enable_Disable)
{
    // static u8 flag=0;
    if(Enable_Disable)
    {
        // if(flag==0)
        FLASH_init();
        DisableIrq(ri0);

    }
    else
    {
        FLASH_Deinit();
        EnableIrq(ri0);
    }




}

/*
*********************************************************************************************************
*	函 数 名:void FlashProcess(u8 Enable_Disable)
*	功能说明:中断失能
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void DisableIrq(u32 ri0)
{
    __ASM("MRS     R0, PRIMASK ");
    __ASM("CPSID   I");

}
/*
*********************************************************************************************************
*	函 数 名:void EnableIrq(u32 ri0)
*	功能说明:中断使能
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void EnableIrq(u32 ri0)
{
    __ASM("MSR     PRIMASK, R0 ");
    __ASM(" CPSIE   I");

}
/*
*********************************************************************************************************
*	函 数 名:void FLASH_writeWord(uint32_t adr, uint32_t data)
*	功能说明: DeInitializes the Flash programmer
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void FLASH_writeWord(uint32_t adr, uint32_t data)
{
    u32 ri0;
    UNION_U32_STU outdate,outdate1;
    outdate.total=data;
    outdate1.date[3]=outdate.date[0];
    outdate1.date[2]=outdate.date[1];
    outdate1.date[1]=outdate.date[2];
    outdate1.date[0]=outdate.date[3];
    data=outdate1.total;
    ri0=0;
    // while(GsmSta.DateCome==OK);
    FlashProcess(ri0,0xaa);
    while (DMA->CHENS & DMA_CHENS_CH0ENS) ;

    /* Load address */
    MSC->ADDRB    = adr;
    MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

    /* Load data */
    MSC->WDATA = data;

    /* Trigger write once */
    MSC->WRITECMD = MSC_WRITECMD_WRITEONCE;

    /* Waiting for the write to complete */
    while ((MSC->STATUS & MSC_STATUS_BUSY)) ;
    while (DMA->CHENS & DMA_CHENS_CH0ENS) ;
    FlashProcess(ri0,0);
}
/*
*********************************************************************************************************
*	函 数 名: void FLASH_WriteDate(u32 adr,u32 count, u8  *buffer);
*	功能说明: 必须是4的整数倍个
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void FLASH_WriteDate(u32 adr,u32 count, u8  *buffer)
{
    UNION_U32_STU outdate;
    u16 times;
    u32 outturn=0;
    u8 times2;
    u16 i;
    times=count/4;
    times2=count%4;
#ifdef ENABLE_DOG
    WDOG_Feed();
#endif
    while(times)
    {
        outdate.date[3]=buffer[outturn++];
        outdate.date[2]=buffer[outturn++];
        outdate.date[1]=buffer[outturn++];
        outdate.date[0]=buffer[outturn++];
        FLASH_writeWord(adr,outdate.total);
        adr+=4;
        times--;
    }
    if(times2)
    {
        i=0;
        outdate.total=0xffffffff;
        while(times2)
        {


            outdate.date[3-i]=buffer[outturn++];
            times2--;
            i++;
        }
        FLASH_writeWord(adr,outdate.total);

    }

}
/*
*********************************************************************************************************
*	函 数 名:void FLASH_ReadDate(uint32_t adr,uint32_t count, uint8_t const *buffer)
*	功能说明:
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void FLASH_ReadDate(u32 adr,u32 count, u8  *buffer)
{
    u32 *p;
    UNION_U32_STU outdate;
    u32 outturn;
    u8 i;
    u16 times;
    u8 times2;
    times=count/4;
    times2=count%4;
    p=(u32 *)adr;
    outturn=0;
    while(times)
    {
        outdate.total=*p++;
        buffer[outturn++]=outdate.date[0];
        buffer[outturn++]=outdate.date[1];
        buffer[outturn++]=outdate.date[2];
        buffer[outturn++]=outdate.date[3];
        times--;
    }
    if(times2)
    {
        i=0;
        outdate.total=*p++;
        while(times2)
        {
            buffer[outturn++]=outdate.date[i];
            i++;
            times2--;
        }


    }


}



#if 0
/*
*********************************************************************************************************
*	函 数 名:void FLASH_writeBlock(void *block_start,
uint32_t offset_into_block,
uint32_t count,
uint8_t const *buffer)
*	功能说明: 块写（4倍数数据)
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
__ramfunc void FLASH_writeBlock(u32 block_start,
                                u32 offset_into_block,
                                u32 count,
                                u8  *buffer)
{
    u32 ri0;
    ri0=0;
    FlashProcess(ri0,0xaa);

    /* Check for an active transfer. If a transfer is in progress,
    * we have to delay. Normally, the USART transfer takes more time
    * than writing, but we have to make sure. */
    while (DMA->CHENS & DMA_CHENS_CH0ENS) ;

    /* Set up a basic memory to peripheral DMA transfer. */
    /* Load the start address into the MSC */
    MSC->ADDRB    =  block_start + offset_into_block;
    MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

    /* Set the MSC as the destination. */
    descr.DSTEND = (void *)(&(MSC->WDATA));

    /* Set up the end pointer to copy from the buffer. */
    descr.SRCEND = (void *)(buffer + count - 4);

    /* Control information */
    descr.CTRL = DMA_CTRL_DST_INC_NONE       /* Do not increment destination */
                 | DMA_CTRL_DST_SIZE_WORD    /* Transfer whole words at a time */
                 | DMA_CTRL_SRC_INC_WORD     /* Write one word at the time */
                 | DMA_CTRL_SRC_SIZE_WORD    /* Transfer whole words at a time */
                 | DMA_CTRL_R_POWER_1
                 | DMA_CTRL_CYCLE_CTRL_BASIC /* Basic transfer */
                 /* Number of transfers minus two. */
                 /* This field contains the number of transfers minus 1. */
                 /* Because one word is transerred using WRITETRIG we need to */
                 /* Substract one more. */
                 | (((count / 4) - 2) << _DMA_CTRL_N_MINUS_1_SHIFT);

    /* Set channel to trigger on MSC ready for data */
    DMA->CH[0].CTRL = DMA_CH_CTRL_SOURCESEL_MSC
                      | DMA_CH_CTRL_SIGSEL_MSCWDATA;

    /* Load first word into the DMA */
    MSC->WDATA = *((uint32_t *)(buffer));

    /* Activate channel 0 */
    DMA->CHENS = DMA_CHENS_CH0ENS;

    /* Trigger the transfer */
    MSC->WRITECMD = MSC_WRITECMD_WRITETRIG;
    /* Waiting for the write to complete */
    while ((MSC->STATUS & MSC_STATUS_BUSY)) ;
    while (DMA->CHENS & DMA_CHENS_CH0ENS) ;
    FlashProcess(ri0,0);
}

#endif
/*
*********************************************************************************************************
*	 void FLASH_eraseOneBlock(uint32_t blockStart)
*	功能说明: 块擦除 (按照页大小擦除)
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void FLASH_eraseOneBlock(uint32_t blockStart)
{
    uint32_t acc = 0xFFFFFFFF;
    uint32_t *ptr;
    volatile  u32 ri0;
    ri0=0;
    FlashProcess(ri0,0xaa);
    while (DMA->CHENS & DMA_CHENS_CH0ENS) ;
    /* Optimization - check if block is allready erased.
    * This will typically happen when the chip is new. */
    for (ptr = (uint32_t *) blockStart; ptr < (uint32_t *)(blockStart +  PAGE_SIZE); ptr++)
        acc &= *ptr;

    /* If the accumulator is unchanged, there is no need to do an erase. */
    if (acc == 0xFFFFFFFF)
    {
        FlashProcess(ri0,0);
        return;
    }

    /* Load address */
    MSC->ADDRB    = blockStart;
    MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

    /* Send Erase Page command */
    MSC->WRITECMD = MSC_WRITECMD_ERASEPAGE;

    /* Waiting for erase to complete */
    while ((MSC->STATUS & MSC_STATUS_BUSY)) ;
    while (DMA->CHENS & DMA_CHENS_CH0ENS) ;
    FlashProcess(ri0,0);
}




/*
*********************************************************************************************************
*	void TestFlash(void)
*	功能说明:
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void TestFlash(void)
{
    //#define FLASH_START_ADDR   (256-64)*1024             /*使用最后的64k数据*/
#if 0
    static u8 readbuf[128];
    u8 i,writebuf[128];
    for(i=0; i<128; i++)
    {
        writebuf[i]=i;
    }

    FLASH_ReadDate(FLASH_START_ADDR,128,readbuf);
    FLASH_eraseOneBlock(FLASH_START_ADDR);
    FLASH_ReadDate(FLASH_START_ADDR,128,readbuf);
    FLASH_WriteDate(FLASH_START_ADDR,128,writebuf);
    FLASH_ReadDate(FLASH_START_ADDR,128,readbuf);
    for(i=0; i<128; i++)
    {
        printf("第%2d的个：%2d\r\n" ,i,readbuf[i]);
    }
#endif
}






/*
*********************************************************************************************************
*	void EraseApp(u32 applen)
*	功能说明:擦除app备份区
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void EraseApp(u32 applen)
{
    u16 i,j;
    j=applen/S_2__K;
    if(applen%S_2__K)
        j++;
    WriteFlag=0xaa;
    BlindStu.unit=0;
    WriteWhenNoBlindDate();
#ifdef ENABLE_DOG
    WDOG_Feed();
#endif
    for(i=0; i<AppSEC; i++)
    {

        FLASH_eraseOneBlock(AppbvkStart+2048*i);
    }


}



/*
*********************************************************************************************************
*	void WriteApp(u32 addr,u16 len)
*	功能说明:地址从0开始，已经偏移过
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void WriteApp(u32 addr,u8*readbuf,u16 len)
{
    FLASH_WriteDate(AppbvkStart+addr,len,readbuf);
}


/*
*********************************************************************************************************
*	void ReadApp(u32 addr,u16 len)
*	功能说明:地址从0开始，已经偏移过
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void ReadApp(u32 addr,u8*writebuf,u16 len)
{

    FLASH_ReadDate(AppbvkStart+addr,len,writebuf);

}






STRUCT_BLIND_BUF BlindStu;
u8 WriteFlag=0x55;






//关键数据的时候保
void WriteBlind(void)
{

    BlindStu.varity=CalacXORVarity((u8*)&BlindStu,2047);
    FLASH_eraseOneBlock(BLIND_STU);
    FLASH_WriteDate(BLIND_STU,2048,(u8*)&BlindStu);
    WriteFlag=0xaa;

}


//开机的时候读一次
void ReadBlind(void)
{
    FLASH_ReadDate(BLIND_STU,2048,(u8*)&BlindStu);


    if( (BlindStu.varity!=CalacXORVarity((u8*)&BlindStu,2047))||(BlindStu.unit>BLIND_MAXTIMES))
    {
        BlindStu.unit=0;
        WriteBlind();


    }

}

//当判断缓冲中没有数据的时候保存
void WriteWhenNoBlindDate(void)
{
    if( (WriteFlag==0x55)|(WriteFlag==0xaa))
    {
        FLASH_ReadDate(BLIND_STU,2048,(u8*)&BlindStu);
        if(0!=BlindStu.unit)
        {
            BlindStu.unit=0;
            WriteBlind();
        }
        WriteFlag=0;
    }


}



u8 WriteBlindDatToFlash(u32 Address,u8 *dat,u16 len)
{
    //u8 Tmp[MAXHISBUFLEN*MAXHISBUFTIMES+9];
    u8 Tmp[8];
    u32 wCount;
    u8 xor=0;
    u16 j;
    //AA 55  xor  time1 time2 time3  DATLEN1 DATLEN2 [DAT....]
    FLASH_ReadDate(Address,8,&Tmp[0]);


    wCount=((u32)Tmp[3]<<16)+((u32)Tmp[4]<<8)+Tmp[5];
    if ((Tmp[0]!=0xaa)||(Tmp[1]!=0x55)||(wCount>65535))
    {
        Tmp[0]=0xaa;
        Tmp[1]=0x55;
        Tmp[3]=Tmp[4]=Tmp[5]=0; //写入次数清0
        wCount=0;
    }


    if( (len==0)||(len==2040))
    {
        wCount++;

        Tmp[3]=(wCount>>16)&0xff;
        Tmp[4]=(wCount>>8)&0xff;
        Tmp[5]=(wCount&0xff);
        Tmp[6]=(len>>8)&0xff;
        Tmp[7]=(len&0xff);

        for(j=0; j<len; j++) xor^=dat[j];
        Tmp[2]=xor;


        if(len==2040)
            FLASH_eraseOneBlock(Address);
        FLASH_WriteDate(Address,8,Tmp);
        if(len==2040)
            FLASH_WriteDate(Address+8,2040,dat);

        return(0);//写入成功
    }
    return(1);//写入失败
}


u16 ReadBlindData(u8 *dat)
{
    u32 i;
    u32 Address;
    u16 j,datLen;
    u8 tmp[8];

    for(i=0; i<DATE_BUF_LEN_SEC; i++)
    {

        //AA 55  xor  time1 time2 time3  DATLEN1 DATLEN2 [DAT....]
        Address=DateBufStart+(u16)i*2048;
        FLASH_ReadDate(Address,8,&tmp[0]);

        if( (tmp[0]==0xaa)&&(tmp[1]==0x55))
        {
            datLen=((u16)tmp[6]<<8)+tmp[7];
            if(datLen==2040)
            {
                FLASH_ReadDate(Address+8,2040,&dat[0]);

                for(j=0; j<2040; j++)   tmp[2]^=dat[j];

                tmp[6]=0;
                tmp[7]=0;
                //WriteBlindDatToFlash(Address,tmp,8);
                FLASH_WriteDate(Address,8,tmp);
                FLASH_ReadDate(Address,8,tmp);
                if( tmp[2]==0) return(datLen);
            }
        }
    }
    return(0);
}

void next_gps(u8 count)
{
    u8 i;
    u16 j;
    if(count==0)
    {
        j= ReadBlindData(&BlindStu.buflen[0]);
        if(j>0)
            BlindStu.unit=j/A_FRAM_LEN;
    }
    else
    {
        for(i=1; i<=count; i++) //  1    ~100
        {
            Mymemcpy(&BlindStu.buflen[(i-1)*A_FRAM_LEN],&BlindStu.buflen[i*A_FRAM_LEN],A_FRAM_LEN);
        }
    }
}






u32 FineBlindDatAddr(void)
{
    u8 Tmp[8];
    u32 Address,wCount=0,ACount=0xffffffff;
    u32 i,rAdd=0xffffffff;


    for(i=0; i<DATE_BUF_LEN_SEC; i++)
    {

        //AA 55 xor LEN2 LEN3 LEN4 DATLEN1 DATLEN2 [DAT....]
        Address=DateBufStart+i*2048;
        FLASH_ReadDate(Address,8,&Tmp[0]);
        wCount=((u32)Tmp[3]<<16)+((u32)Tmp[4]<<8)+Tmp[5];
        if((Tmp[0]!=0xaa)||(Tmp[1]!=0x55))
        {
            return(Address);
        }

        if (wCount<ACount)
        {
            ACount=wCount;
            rAdd=Address;
        }
    }
    return(rAdd);//如查rAdd=0 说明写入次数已到上限 或 都已存满
}


u8 SaveBlindDatToFlash(void)
{
    u8 i=1;
    u32 wAddr;
    wAddr=FineBlindDatAddr();
    if(wAddr!=0xffffffff)
    {
        i=WriteBlindDatToFlash(wAddr,BlindStu.buflen,2040);
    }
    return(i);
}


u8  FlashBufWrite(STRUCT_BLIND_FRAM StuFram,u8 Important)
{
    u8 i;

    if( (StuFram.len>(A_FRAM_LEN-1))&&( (Important==0)||(Important==Flash_IMPORT)))
    {


        return NOT_OK;
    }

    if(BlindStu.unit>BLIND_MAXTIMES)
    {
        BlindStu.unit=0;
        WriteBlind();
    }
    if(BlindStu.unit==BLIND_MAXTIMES)
    {

        if( (GsmSto.updateflag==NOT_OK)&&(GpsStatues.SgeeState!=AGPS_UPING)&&(resetflag!=0xaa))
        {
            i=SaveBlindDatToFlash();

            if(i==0) BlindStu.unit=0;//成功清0
            else
            {
                next_gps(--BlindStu.unit);//往前移
            }
        }
        else
        {
            next_gps(--BlindStu.unit);//往前移
        }
    }
    Mymemcpy(&BlindStu.buflen[BlindStu.unit*A_FRAM_LEN],(u8*)&StuFram,A_FRAM_LEN);
    BlindStu.unit++;

    if(Important==Flash_IMPORT)
    {
        WriteBlind();

    }
    return OK;


}

u8  FlashBufDelete(void)
{

    if(BlindStu.unit>0)
    {
        if(1==BlindStu.unit)
        {
            BlindStu.unit--;

        }
        else
        {
            next_gps(--BlindStu.unit);//往前移
        }

        return OK;
    }


    return NOT_OK;


}

//8+2040
//aa55xx xx xx xx ff ff (ff ff 表示没有读)
u8  FlashBufRead(STRUCT_BLIND_FRAM *StuFram)
{
    u16 j;
    if(BlindStu.unit)
    {
        Mymemcpy((u8*)StuFram,&BlindStu.buflen[0],A_FRAM_LEN);
        return OK;

    }
    else  if( (GsmSto.updateflag==NOT_OK)&&(GpsStatues.SgeeState!=AGPS_UPING)&&(resetflag!=0xaa))
    {
        j= ReadBlindData(&BlindStu.buflen[0]);
        if(j>0)
        {
            BlindStu.unit=j/A_FRAM_LEN;
            Mymemcpy((u8*)StuFram,&BlindStu.buflen[0],A_FRAM_LEN);
            return OK;
        }


    }
    WriteWhenNoBlindDate();
    return NOT_OK;

}

//清空盲点数据
void DeleteAllDate(void)
{
    u8 i;
    BlindStu.unit=0;
    WriteWhenNoBlindDate();
#ifdef ENABLE_DOG
    WDOG_Feed();
#endif
    for(i=0; i<DATE_BUF_LEN_SEC; i++)
    {

        FLASH_eraseOneBlock(DateBufStart+2048*i);
    }
}





