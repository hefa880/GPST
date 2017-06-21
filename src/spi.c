/**************************************************************************//**
* @file
* @brief SPI
* @author Energy Micro AS
* @version 1.12
******************************************************************************
* @section License
* <b>(C) Copyright 2013 Energy Micro AS, http://www.energymicro.com</b>
*******************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
* 4. The source and compiled code may only be used on Energy Micro "EFM32"
*    microcontrollers and "EFR4" radios.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
* obligation to support this Software. Energy Micro AS is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Energy Micro AS will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
*****************************************************************************/
#include "em_device.h"
#include "spi.h"
#include "em_gpio.h"



#if 0
/* Buffer pointers and indexes */



u8* masterTxBuffer;
u16 masterTxBufferSize;
volatile u16 masterTxBufferIndex;

u8* masterRxBuffer;
u16 masterRxBufferSize;
volatile u16 masterRxBufferIndex;


/**************************************************************************//**
* @brief USART1 RX IRQ Handler Setup
* @param receiveBuffer points to where to place recieved data
* @param receiveBufferSize indicates the number of bytes to receive
*****************************************************************************/
void SPI1_setupRXInt(char* receiveBuffer, int receiveBufferSize)
{
    USART_TypeDef *spi = USART1;

    /* Setting up pointer and indexes */
    masterRxBuffer      = receiveBuffer;
    masterRxBufferSize  = receiveBufferSize;
    masterRxBufferIndex = 0;

    /* Clear RX */
    spi->CMD = USART_CMD_CLEARRX;

    /* Enable interrupts */
    NVIC_ClearPendingIRQ(USART1_RX_IRQn);
    NVIC_EnableIRQ(USART1_RX_IRQn);
    spi->IEN |= USART_IEN_RXDATAV;
}



/**************************************************************************//**
* @brief USART1 TX IRQ Handler Setup
* @param transmitBuffer points to the data to send
* @param transmitBufferSize indicates the number of bytes to send
*****************************************************************************/
void SPI1_setupTXInt(char* transmitBuffer, int transmitBufferSize)
{
    USART_TypeDef *spi = USART1;

    /* Setting up pointer and indexes */
    masterTxBuffer      = transmitBuffer;
    masterTxBufferSize  = transmitBufferSize;
    masterTxBufferIndex = 0;

    /* Clear TX */
    spi->CMD = USART_CMD_CLEARTX;

    /* Enable interrupts */
    NVIC_ClearPendingIRQ(USART1_TX_IRQn);
    NVIC_EnableIRQ(USART1_TX_IRQn);
    spi->IEN |= USART_IEN_TXBL;
}








/**************************************************************************//**
* @brief USART1 RX IRQ Handler
*****************************************************************************/
void USART1_RX_IRQHandler(void)
{
    USART_TypeDef *spi = USART1;
    uint8_t       rxdata;

    if (spi->STATUS & USART_STATUS_RXDATAV)
    {
        /* Reading out data */
        rxdata = spi->RXDATA;

        if ( masterRxBufferIndex <  masterRxBufferSize)
        {
            /* Store Data */
            masterRxBuffer[ masterRxBufferIndex] = rxdata;
            masterRxBufferIndex++;
        }
    }
}



/**************************************************************************//**
* @brief USART1 TX IRQ Handler
*****************************************************************************/
void USART1_TX_IRQHandler(void)
{
    USART_TypeDef *spi = USART1;

    if (spi->STATUS & USART_STATUS_TXBL)
    {
        /* Checking that valid data is to be transferred */
        if ( masterTxBuffer != 0)
        {
            /* Writing new data */
            spi->TXDATA =  masterTxBuffer[masterTxBufferIndex];
            masterTxBufferIndex++;
            /*Checking if more data is to be transferred */
            if ( masterTxBufferIndex ==  masterTxBufferSize)
            {
                masterTxBuffer = 0;
            }
        }
        else
        {
            /* Sending 0 if no data to send */
            spi->TXDATA = 0;
        }
    }
}

/******************************************************************************
* @brief sends data using USART2
* @param txBuffer points to data to transmit
* @param bytesToSend bytes will be sent
*****************************************************************************/
void USART1_sendBuffer(char* txBuffer, int bytesToSend)
{
    USART_TypeDef *uart = USART1;
    int           ii;

    /* Sending the data */
    for (ii = 0; ii < bytesToSend;  ii++)
    {
        /* Waiting for the usart to be ready */
        while (!(uart->STATUS & USART_STATUS_TXBL)) ;

        if (txBuffer != 0)
        {
            /* Writing next byte to USART */
            uart->TXDATA = *txBuffer;
            txBuffer++;
        }
        else
        {
            uart->TXDATA = 0;
        }
    }

    /*Waiting for transmission of last byte */
    while (!(uart->STATUS & USART_STATUS_TXC)) ;
}

#endif

/**************************************************************************//**
* @brief Setup a USART as SPI
* @param spiNumber is the number of the USART to use (e.g. 1 USART1)
* @param location is the GPIO location to use for the device
* @param master set if the SPI is to be master
*****************************************************************************/
void SPI_setup(uint8_t spiNumber, uint8_t location, bool master)
{
    USART_TypeDef *spi;

    /* Determining USART */
    switch (spiNumber)
    {
    case 0:
        spi = USART0;
        break;
    case 1:
        spi = USART1;
        break;
    case 2:
        spi = USART2;
        break;
    default:
        return;
    }
#define HFRCO_FREQUENCY         1382400
#define SPI_PERCLK_FREQUENCY    HFRCO_FREQUENCY
#define SPI_BAUDRATE            250000
    //    *spi=USART_INITSYNC_DEFAULT;
    /* Setting baudrate */
    spi->CLKDIV = 128 * (SPI_PERCLK_FREQUENCY / SPI_BAUDRATE - 2);

    /* Configure SPI */
    /* Using synchronous (SPI) mode*/
    spi->CTRL = USART_CTRL_SYNC;
    /* Clearing old transfers/receptions, and disabling interrupts */
    spi->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
    spi->IEN = 0;
    /* Enabling pins and setting location */
    // spi->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | (location << 8);
    spi->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN  | (location << 8);

    /* Set GPIO config to slave */
    GPIO_Mode_TypeDef gpioModeMosi = gpioModeInput;
    GPIO_Mode_TypeDef gpioModeMiso = gpioModePushPull;
    GPIO_Mode_TypeDef gpioModeCs   = gpioModeInput;
    GPIO_Mode_TypeDef gpioModeClk  = gpioModeInput;

    /* Set to master and to control the CS line */
    if (master)
    {
        /* Enabling Master, TX and RX */
        spi->CMD   = USART_CMD_MASTEREN | USART_CMD_TXEN | USART_CMD_RXEN;
        spi->CTRL |= USART_CTRL_AUTOCS;

        /* Set GPIO config to master */
        gpioModeMosi = gpioModePushPull;
        gpioModeMiso = gpioModeInput;
        gpioModeCs   = gpioModePushPull;
        gpioModeClk  = gpioModePushPull;
    }
    else
    {
        /* Enabling TX and RX */
        spi->CMD = USART_CMD_TXEN | USART_CMD_RXEN;
    }

    /* Clear previous interrupts */
    spi->IFC = _USART_IFC_MASK;

    /* IO configuration */
    switch(spiNumber)
    {
    case 0:
        switch(location)
        {
        case 0: /* IO configuration (USART 0, Location #0) */
            GPIO_PinModeSet(gpioPortE, 10, gpioModeMosi, 0); /* MOSI */
            GPIO_PinModeSet(gpioPortE, 11, gpioModeMiso, 0); /* MISO */
            GPIO_PinModeSet(gpioPortE, 13, gpioModeCs,   0); /* CS */
            GPIO_PinModeSet(gpioPortE, 12, gpioModeClk,  0); /* Clock */
            break;
        case 1: /* IO configuration (USART 0, Location #1) */
            GPIO_PinModeSet(gpioPortE, 7, gpioModeMosi, 0);  /* MOSI */
            GPIO_PinModeSet(gpioPortE, 6, gpioModeMiso, 0);  /* MISO */
            GPIO_PinModeSet(gpioPortE, 4, gpioModeCs,   0);  /* CS */
            GPIO_PinModeSet(gpioPortE, 5, gpioModeClk,  0);  /* Clock */
            break;
        case 2: /* IO configuration (USART 0, Location #2) */
            #if 0
            GPIO_PinModeSet(gpioPortC, 11, gpioModeMosi, 0); /* MOSI */
            GPIO_PinModeSet(gpioPortC, 10, gpioModeMiso, 0); /* MISO */
            GPIO_PinModeSet(gpioPortC,  8, gpioModeCs,   0); /* CS */
            GPIO_PinModeSet(gpioPortC,  9, gpioModeClk,  0); /* Clock */
            #endif
            break;
        default:
            break;
        }
        break;
    case 1:
        switch(location)
        {
        case 0: /* IO configuration (USART 1, Location #0) */
            GPIO_PinModeSet(gpioPortC, 0, gpioModeMosi, 0);  /* MOSI */
            GPIO_PinModeSet(gpioPortC, 1, gpioModeMiso, 0);  /* MISO */
            GPIO_PinModeSet(gpioPortB, 8, gpioModeCs,   0);  /* CS */
            GPIO_PinModeSet(gpioPortB, 7, gpioModeClk,  0);  /* Clock */
            break;
        case 1: /* IO configuration (USART 1, Location #1) */

            spi->CTRL |=(1<<8);
            spi->CTRL |=((1<<9));
            spi->CTRL |=((1<<10));//msbf first
            GPIO_PinModeSet(gpioPortD, 0, gpioModeMosi, 0);  /* MOSI */
            GPIO_PinModeSet(gpioPortD, 1, gpioModeMiso, 0);  /* MISO */
            GPIO_PinModeSet(gpioPortD, 3, gpioModeCs,   0);  /* CS */
            GPIO_PinModeSet(gpioPortD, 2, gpioModeClk,  0);  /* Clock */
            break;
        default:
            break;
        }
        break;
    case 2:
        switch(location)
        {
        case 0: /* IO configuration (USART 2, Location #0) */

            spi->CTRL &=(~(1<<8));//idle low
            //  spi->CTRL |=(1<<8);//idle hoght
            spi->CTRL |=((1<<9));//sampletrailing
            //  spi->CTRL &=(~(1<<9));//sampletrailing
            spi->CTRL |=((1<<10));//msbf first

            GPIO_PinModeSet(gpioPortC, 2, gpioModeMosi, 0);  /* MOSI */
            GPIO_PinModeSet(gpioPortC, 3, gpioModeMiso, 0);  /* MISO */
            GPIO_PinModeSet(gpioPortC, 5, gpioModeCs,   1);  /* CS */
            GPIO_PinModeSet(gpioPortC, 4, gpioModeClk,  0);  /* Clock */
            break;
        case 1: /* IO configuration (USART 2, Location #1) */
            GPIO_PinModeSet(gpioPortB, 3, gpioModeMosi, 0);  /* MOSI */
            GPIO_PinModeSet(gpioPortB, 4, gpioModeMiso, 0);  /* MISO */
            GPIO_PinModeSet(gpioPortB, 6, gpioModeCs,   0);  /* CS */
            GPIO_PinModeSet(gpioPortB, 5, gpioModeClk,  0);  /* Clock */
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/*
*********************************************************************************************************
*	函 数 名:void Initspi(void)
*	功能说明: SPI初始化
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void Initspi(void)
{


    USART_InitSync_TypeDef SPI_init = USART_INITSYNC_DEFAULT;
    SPI_init.baudrate = 200000;

    USART_InitSync(USART1, &SPI_init);

    GPIO_PinModeSet(gpioPortD,0,gpioModePushPull,1); //tx
    GPIO_PinModeSet(gpioPortD,1,gpioModeInput,1);    //rx
    GPIO_PinModeSet(gpioPortD,2,gpioModePushPull,1); //clk
    GPIO_PinModeSet(gpioPortD,3,gpioModePushPull,1); //cs

    USART1->ROUTE |= USART_ROUTE_CSPEN | USART_ROUTE_CLKPEN | USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_LOCATION_LOC1;
}

/*
*********************************************************************************************************
*	函 数 名:u8  SPI_I2S_SendData(USART_TypeDef *uart , u8 date)
*	功能说明: SPI发送数据
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
u8  SPI_I2S_SendData(USART_TypeDef *uart , u8 date)
{

    return  USART_SpiTransfer(uart, date);
}

/*
*********************************************************************************************************
*	函 数 名:u8 SPI_I2S_RevData(USART_TypeDef *spi )
*	功能说明: SPI接收数据
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

u8 SPI_I2S_RevData(USART_TypeDef *spi )
{

    u8 rxdata;
    if (spi->STATUS & USART_STATUS_RXDATAV)
    {
        /* Reading out data */
        rxdata = spi->RXDATA;
    }
    return rxdata;
}













