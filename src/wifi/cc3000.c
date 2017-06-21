#include"includes.h"

STU_CC300 StuCc300;
void CC3000PowerOff(void);
STU_WIFI_AP StuWifiAp,bvkStuWifiAp;/*bvk动态*/
/*
*********************************************************************************************************
*	函 数 名:  void CC3000InitHardware(void)
*	功能说明: wifi的spi初始化
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void CC3000InitHardware(void)
{


  GPIO_PinModeSet(CC3000_PWR_PORT, CC3000_PWR_Pin, gpioModePushPull, 1);
  GPIO_PinModeSet(CC3000_SW_PORT, CC3000_SW_Pin, gpioModePushPull, 0);

  /*INT1*/
  GPIO_PinModeSet(CC3000_IRQ_PORT, CC3000_IRQ_Pin, gpioModeInput, 0);
  GPIO_IntConfig(CC3000_IRQ_PORT, CC3000_IRQ_Pin,false,true, false);


}


/*
*********************************************************************************************************
*	函 数 名:void CC3000PowerOn(void)
*	功能说明: wifi的开电源
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void CC3000PowerOn(void)
{

  GPIO_PinOutClear(CC3000_PWR_PORT, CC3000_PWR_Pin);
  GPIO_PinOutSet(CC3000_SW_PORT, CC3000_SW_Pin);

}

/*
*********************************************************************************************************
*	函 数 名:void CC3000PowerOff(void)
*	功能说明: wifi
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void CC3000PowerOff(void)
{

  GPIO_PinOutSet(CC3000_PWR_PORT, CC3000_PWR_Pin);
  GPIO_PinOutClear(CC3000_SW_PORT, CC3000_SW_Pin);
  //  GPIO_IntConfig(CC3000_IRQ_PORT, CC3000_IRQ_Pin,false,true, false);
  //  GPIO_IntConfig(CC3000_IRQ_PORT, CC3000_IRQ_Pin,true,true, false);
}

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected,ulCC3000DHCP,OkToDoShutDown;
/*
*********************************************************************************************************
*	函 数 名:void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length)
*	功能说明: wifi事件响应
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length)
{
  if (lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE)
  {
    ulSmartConfigFinished = 1;
  }

  if (lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT)
  {
    ulCC3000Connected = 1;

  }

  if (lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT)
  {
    ulCC3000Connected = 0;


  }

  if (lEventType == HCI_EVNT_WLAN_UNSOL_DHCP)
  {
    ulCC3000DHCP = 1;
  }

  if (lEventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN)
  {
    OkToDoShutDown = 1;
  }

}


//*****************************************************************************
//
//! sendDriverPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the driver patch: since there is no patch yet -
//!				it returns 0
//
//*****************************************************************************
char *sendDriverPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}


//*****************************************************************************
//
//! sendBootLoaderPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the boot loader patch: since there is no patch yet -
//!				it returns 0
//
//*****************************************************************************
char *sendBootLoaderPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! sendWLFWPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the FW patch: since there is no patch yet - it returns 0
//
//*****************************************************************************

char *sendWLFWPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}
//*****************************************************************************
//
//! ReadWlanInterruptPin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  return wlan interrup pin
//
//*****************************************************************************

long ReadWlanInterruptPin(void)
{
  long a;
  a=CC3000_READ_IRQ_PIN();
  return a;
}

//*****************************************************************************
//
//! Enable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
//
//*****************************************************************************

void WlanInterruptEnable()
{
  GPIO_IntEnable(1<<CC3000_IRQ_Pin);
}

//*****************************************************************************
//
//! Disable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
//
//*****************************************************************************


void WlanInterruptDisable()
{
  GPIO_IntDisable(1<<CC3000_IRQ_Pin);
}


//*****************************************************************************
//
//! WriteWlanPin
//!
//! \param  new val
//!
//! \return none
//!
//! \brief  void
//
//*****************************************************************************

void WriteWlanPin( unsigned char val )
{
  if (val)
  {
    CC3000PowerOn();
  }
  else
  {
    CC3000PowerOff();
  }
}
//=====================================================================================================================================================




//==============================================================================================================================================

//*****************************************************************************
//
//! \addtogroup link_buff_api
//! @{
//
//*****************************************************************************
#include "hci.h"
#include "spi.h"
#include "os.h"
#include "evnt_handler.h"
//#include "board.h"
#include "includes.h"


#define READ                    3
#define WRITE                   1

#define HI(value)               (((value) & 0xFF00) >> 8)
#define LO(value)               ((value) & 0x00FF)

#define ASSERT_CS()          CC3000_LOW()

#define DEASSERT_CS()        CC3000_HiGHT()

#define HEADERS_SIZE_EVNT       (SPI_HEADER_SIZE + 5)

#define SPI_HEADER_SIZE			(5)

#define 	eSPI_STATE_POWERUP 				 (0)
#define 	eSPI_STATE_INITIALIZED  		 (1)
#define 	eSPI_STATE_IDLE					 (2)
#define 	eSPI_STATE_WRITE_IRQ	   		 (3)
#define 	eSPI_STATE_WRITE_FIRST_PORTION   (4)
#define 	eSPI_STATE_WRITE_EOT			 (5)
#define 	eSPI_STATE_READ_IRQ				 (6)
#define 	eSPI_STATE_READ_FIRST_PORTION	 (7)
#define 	eSPI_STATE_READ_EOT				 (8)







typedef struct
{
  gcSpiHandleRx  SPIRxHandler;
  unsigned short usTxPacketLength;
  unsigned short usRxPacketLength;
  unsigned long  ulSpiState;
  unsigned char *pTxPacket;
  unsigned char *pRxPacket;

} tSpiInformation;


tSpiInformation sSpiInformation;

//
// Static buffer for 5 bytes of SPI HEADER
//
unsigned char tSpiReadHeader[] = {READ, 0, 0, 0, 0};


void SpiWriteDataSynchronous(unsigned char *data, unsigned short size);
void SpiWriteAsync(const unsigned char *data, unsigned short size);
void SpiPauseSpi(void);
void SpiResumeSpi(void);
void SSIContReadOperation(void);

// The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
// for the purpose of detection of the overrun. The location of the memory where the magic number
// resides shall never be written. In case it is written - the overrun occured and either recevie function
// or send function will stuck forever.
#define CC3000_BUFFER_MAGIC_NUMBER (0xDE)



u8 spi_buffer[CC3000_RX_BUFFER_SIZE];
u8 wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];
#if 0
//*****************************************************************************
//
//!  This function get the reason for the GPIO interrupt and clear cooresponding
//!  interrupt flag
//!
//!  \param  none
//!
//!  \return none
//!
//!  \brief  This function This function get the reason for the GPIO interrupt
//!          and clear cooresponding interrupt flag
//
//*****************************************************************************
void
SpiCleanGPIOISR(void)
{
  //  SPI_IFG_PORT &= ~SPI_IRQ_PIN;
  GPIO_IntClear(1<<CC3000_IRQ_Pin);
}
#endif


//*****************************************************************************
//
//!  SpiClose
//!
//!  \param  none
//!
//!  \return none
//!
//!  \brief  Cofigure the SSI
//
//*****************************************************************************
void
SpiClose(void)
{
  if (sSpiInformation.pRxPacket)
  {
    sSpiInformation.pRxPacket = 0;
  }

  //
  //	Disable Interrupt in GPIOA module...
  //
  tSLInformation.WlanInterruptDisable();
}


//*****************************************************************************
//
//!  SpiClose
//!
//!  \param  none
//!
//!  \return none
//!
//!  \brief  Cofigure the SSI
//
//*****************************************************************************
void
SpiOpen(gcSpiHandleRx pfRxHandler)
{
  sSpiInformation.ulSpiState = eSPI_STATE_POWERUP;

  sSpiInformation.SPIRxHandler = pfRxHandler;
  sSpiInformation.usTxPacketLength = 0;
  sSpiInformation.pTxPacket = NULL;
  sSpiInformation.pRxPacket = (unsigned char *)spi_buffer;
  sSpiInformation.usRxPacketLength = 0;
  spi_buffer[CC3000_RX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;
  wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;



  //
  // Enable interrupt on the GPIOA pin of WLAN IRQ
  //
  tSLInformation.WlanInterruptEnable();
}




//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
long
SpiFirstWrite(unsigned char *ucBuf, unsigned short usLength)
{
  //
  // workaround for first transaction
  //
  ASSERT_CS();

  // Assuming we are running on 24 MHz ~50 micro delay is 1200 cycles;
  // __delay_cycles(1200);
  Delayms(30);

  // SPI writes first 4 bytes of data
  SpiWriteDataSynchronous(ucBuf, 4);

  //__delay_cycles(1200);
  Delayms(30);

  SpiWriteDataSynchronous(ucBuf + 4, usLength - 4);

  // From this point on - operate in a regular way
  sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

  DEASSERT_CS();

  return(0);
}

#pragma optimize=none
u8 WaitSpi(u8*flag)
{

  if(Rtc)
  {
    Rtc=0;
    if(flag[0]++>50)
    {
      StuCc300.waiteven=NOT_OK;
      return NOT_OK;
    }

  }
  return OK;

}
//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
long
SpiWrite(unsigned char *pUserBuffer, unsigned short usLength)
{
  unsigned char ucPad = 0;
  u8 i;
  //
  // Figure out the total length of the packet in order to figure out if there is padding or not
  //
  if(!(usLength & 0x0001))
  {
    ucPad++;
  }


  pUserBuffer[0] = WRITE;
  pUserBuffer[1] = HI(usLength + ucPad);
  pUserBuffer[2] = LO(usLength + ucPad);
  pUserBuffer[3] = 0;
  pUserBuffer[4] = 0;

  usLength += (SPI_HEADER_SIZE + ucPad);

  // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
  // for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun
  // occurred - and we will stuck here forever!
  i=0;
  if (wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
  {
    if(OK!=WaitSpi(&i))
      return NOT_OK;

  }

  if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
  {
    i=0;
    while (sSpiInformation.ulSpiState != eSPI_STATE_INITIALIZED)
    {
      if(OK!=WaitSpi(&i))
        return NOT_OK;

    }
  }

  if (sSpiInformation.ulSpiState == eSPI_STATE_INITIALIZED)
  {
    //
    // This is time for first TX/RX transactions over SPI: the IRQ is down - so need to send read buffer size command
    //
    SpiFirstWrite(pUserBuffer, usLength);
  }
  else
  {
    //
    // We need to prevent here race that can occur in case 2 back to back packets are sent to the
    // device, so the state will move to IDLE and once again to not IDLE due to IRQ
    //
    tSLInformation.WlanInterruptDisable();

    while (sSpiInformation.ulSpiState != eSPI_STATE_IDLE)
    {
      ;
    }


    sSpiInformation.ulSpiState = eSPI_STATE_WRITE_IRQ;
    sSpiInformation.pTxPacket = pUserBuffer;
    sSpiInformation.usTxPacketLength = usLength;

    //
    // Assert the CS line and wait till SSI IRQ line is active and then initialize write operation
    //
    ASSERT_CS();

    //
    // Re-enable IRQ - if it was not disabled - this is not a problem...
    //
    tSLInformation.WlanInterruptEnable();
  }


  //
  // Due to the fact that we are currently implementing a blocking situation
  // here we will wait till end of transaction
  //

  while (eSPI_STATE_IDLE != sSpiInformation.ulSpiState)
    ;

  return(0);
}




//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
void
SpiWriteDataSynchronous(unsigned char *data, unsigned short size)
{

  USART_TypeDef *usart=USART2;
  while (size)
  {
    while (!(usart->STATUS & USART_STATUS_TXBL));

    usart->TXDATA =*data;
    while (!(usart->STATUS & USART_STATUS_TXC));
    (usart->RXDATA);
    size --;
    data++;
  }








}

//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
void
SpiReadDataSynchronous(unsigned char *data, unsigned short size)
{
  long i = 0;
  unsigned char *data_to_send = tSpiReadHeader;
  USART_TypeDef *usart=USART2;

  for (i = 0; i < size; i ++)
  {
    while (!(usart->STATUS & USART_STATUS_TXBL));
    usart->TXDATA = (u32)data_to_send[i];
    while (!(usart->STATUS & USART_STATUS_TXC));
    data[i]= (unsigned char)(usart->RXDATA);
  }





}



//*****************************************************************************
//
//! This function enter point for read flow: first we read minimal 5 SPI header bytes and 5 Event
//!	Data bytes
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
void
SpiReadHeader(void)
{
  SpiReadDataSynchronous(sSpiInformation.pRxPacket, 10);
}


//*****************************************************************************
//
//! This function processes received SPI Header and in accordance with it - continues reading
//!	the packet
//!
//!  \param  None
//!
//!  \return None
//!
//!  \brief  ...
//
//*****************************************************************************
long
SpiReadDataCont(void)
{
  long data_to_recv;
  unsigned char *evnt_buff, type;


  //
  //determine what type of packet we have
  //
  evnt_buff =  sSpiInformation.pRxPacket;
  data_to_recv = 0;
  STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_PACKET_TYPE_OFFSET, type);

  switch(type)
  {
  case HCI_TYPE_DATA:
    {
      //
      // We need to read the rest of data..
      //
      STREAM_TO_UINT16((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_DATA_LENGTH_OFFSET, data_to_recv);
      if (!((HEADERS_SIZE_EVNT + data_to_recv) & 1))
      {
        data_to_recv++;
      }

      if (data_to_recv)
      {
        SpiReadDataSynchronous(evnt_buff + 10, data_to_recv);
      }
      break;
    }
  case HCI_TYPE_EVNT:
    {
      //
      // Calculate the rest length of the data
      //
      STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_EVENT_LENGTH_OFFSET, data_to_recv);
      data_to_recv -= 1;

      //
      // Add padding byte if needed
      //
      if ((HEADERS_SIZE_EVNT + data_to_recv) & 1)
      {

        data_to_recv++;
      }

      if (data_to_recv)
      {
        SpiReadDataSynchronous(evnt_buff + 10, data_to_recv);
      }

      sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
      break;
    }
  }

  return (0);
}


//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SpiPauseSpi
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for
//
//*****************************************************************************
void
SpiPauseSpi(void)
{
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);

}


//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SpiResumeSpi
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for
//
//*****************************************************************************

void
SpiResumeSpi(void)
{
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}



//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SpiTriggerRxProcessing
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for
//
//*****************************************************************************
void
SpiTriggerRxProcessing(void)
{
  //
  // Trigger Rx processing
  //
  SpiPauseSpi();
  DEASSERT_CS();

  // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
  // for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun
  // occurred - and we will stuck here forever!
  if (sSpiInformation.pRxPacket[CC3000_RX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
  {
    while (1)
      ;
  }

  sSpiInformation.ulSpiState = eSPI_STATE_IDLE;
  sSpiInformation.SPIRxHandler(sSpiInformation.pRxPacket + SPI_HEADER_SIZE);
}



//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SSIContReadOperation
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for
//
//*****************************************************************************

void
SSIContReadOperation(void)
{
  //
  // The header was read - continue with  the payload read
  //
  if (!SpiReadDataCont())
  {


    //
    // All the data was read - finalize handling by switching to teh task
    //	and calling from task Event Handler
    //
    SpiTriggerRxProcessing();
  }
}



/*
*********************************************************************************************************
*	函 数 名:void CC3000IRQPing(void)
*	功能说明: wifi的开电源
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void CC3000IRQPing(void)
{





  if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
  {
    /* This means IRQ line was low call a callback of HCI Layer to inform on event */
    sSpiInformation.ulSpiState = eSPI_STATE_INITIALIZED;
  }
  else if (sSpiInformation.ulSpiState == eSPI_STATE_IDLE)
  {
    sSpiInformation.ulSpiState = eSPI_STATE_READ_IRQ;

    /* IRQ line goes down - we are start reception */
    ASSERT_CS();

    //
    // Wait for TX/RX Compete which will come as DMA interrupt
    //
    SpiReadHeader();

    sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;

    //
    //
    //
    SSIContReadOperation();
  }
  else if (sSpiInformation.ulSpiState == eSPI_STATE_WRITE_IRQ)
  {
    SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

    sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

    DEASSERT_CS();
  }




}













//*****************************************************************************
//
//! StartSmartConfig
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  The function triggers a smart configuration process on CC3000.
//!			it exists upon completion of the process
//
//*****************************************************************************

void StartSmartConfig(void)
{
  char aucCC3000_prefix[] = {'T', 'T', 'T'};
  ulSmartConfigFinished = 0;
  ulCC3000Connected = 0;
  ulCC3000DHCP = 0;
  OkToDoShutDown=0;
  //
  // Reset all the previous configuration
  //
  wlan_ioctl_set_connection_policy(NOT_OK, NOT_OK, NOT_OK);

  //
  // Trigger the Smart Config process
  //

  // Start blinking LED6 during Smart Configuration process
  //  turnLedOn(6);
  wlan_first_time_config_set_prefix(aucCC3000_prefix);
  //  turnLedOff(6);
  //
  // Start the First Time config process
  //
  wlan_first_time_config_start();

  //turnLedOn(6);


  //
  // Wait for First Time config finished
  //
  while (ulSmartConfigFinished == 0)
  {
    // __delay_cycles(6000000);

    //
    //   turnLedOff(6);


    // __delay_cycles(6000000);

    //   turnLedOn(6);

  }

  // turnLedOn(6);

  //
  // Configure to connect automatically to the AP retrieved in the
  // First Time config process
  //
  wlan_ioctl_set_connection_policy(NOT_OK, NOT_OK, NOT_OK);

  //
  // reset the CC3000
  //
  wlan_stop();

  // DispatcherUartSendPacket(pucUARTCommandSimpleConfigDoneString, sizeof(pucUARTCommandSimpleConfigDoneString));

  wlan_start(0);

  //
  // Mask out all non-required events
  //
  wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
}


/*
*********************************************************************************************************
*	函 数 名:void ReSort(wlan_scan_result result);
*	功能说明: 重新排列mac
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
//#pragma optimize=none
void ReSort(wlan_scan_result result)
{
  STU_WIFI_AP aptmp;
  STU_WIFI_MSG StuWifiMsg;

   Mymemcpy(StuWifiMsg.apmac,result.ap_bssid, 6);
  StuWifiMsg.aprssi=result.ap_rssi;
  StuWifiMsg.ssidlen=result.ap_ssidlen;
  Mymemcpy(StuWifiMsg.ssid,result.ap_ssid, 32);

  Mymemcpy((u8*)&aptmp,(u8*)&bvkStuWifiAp, sizeof(aptmp));
  if(result.ap_rssi>aptmp.aprssi0)
  {
    // buf[0]=5;
    // buf[1]=0;
    // buf[2]=1;
    // buf[3]=2;
    // buf[4]=3;

    Mymemcpy(bvkStuWifiAp.apmac0,StuWifiMsg.apmac, 40);
    Mymemcpy(bvkStuWifiAp.apmac1,aptmp.apmac0, 40);
    Mymemcpy(bvkStuWifiAp.apmac2,aptmp.apmac1, 40);
    Mymemcpy(bvkStuWifiAp.apmac3,aptmp.apmac2, 40);
    Mymemcpy(bvkStuWifiAp.apmac4,aptmp.apmac3, 40);
  }
  else if(result.ap_rssi>aptmp.aprssi1)
  {
    //  buf[0]=0;
    //   buf[1]=5;
    //   buf[2]=1;
    //   buf[3]=2;
    //  buf[4]=3;

    Mymemcpy(bvkStuWifiAp.apmac1,StuWifiMsg.apmac, 40);
    Mymemcpy(bvkStuWifiAp.apmac2,aptmp.apmac1, 40);
    Mymemcpy(bvkStuWifiAp.apmac3,aptmp.apmac2, 40);
    Mymemcpy(bvkStuWifiAp.apmac4,aptmp.apmac3, 40);
  }
  else if(result.ap_rssi>aptmp.aprssi2)
  {
    //buf[0]=0;
    //buf[1]=1;
    //buf[2]=5;
    //buf[3]=2;
    //buf[4]=3;
    Mymemcpy(bvkStuWifiAp.apmac2,StuWifiMsg.apmac, 40);
    Mymemcpy(bvkStuWifiAp.apmac3,aptmp.apmac2, 40);
    Mymemcpy(bvkStuWifiAp.apmac4,aptmp.apmac3, 40);

  }
  else if(result.ap_rssi>aptmp.aprssi3)
  {
    //buf[0]=0;
    //buf[1]=1;
    //buf[2]=2;
    //buf[3]=5;
    //buf[4]=3;
    Mymemcpy(bvkStuWifiAp.apmac3,StuWifiMsg.apmac, 40);
    Mymemcpy(bvkStuWifiAp.apmac4,aptmp.apmac3, 40);
  }
  else
  {
    //buf[0]=0;
    //buf[1]=1;
    //buf[2]=2;
    //buf[3]=3;
    //buf[4]=5;
   Mymemcpy(bvkStuWifiAp.apmac4,StuWifiMsg.apmac, 40);
  }









}







//=====================================================================================================================================================================================================================================================================================================================================

/*
*********************************************************************************************************
*	函 数 名:void WifiTask(void)
*	功能说明: wifi任务管理
*
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

STU_WIFI StuWifi;
u8 version[2];
u8 state;
u8 ssid[6]= {0xEC,0x17,0x2F,0x41,0xEB,0x40};
unsigned long list[17]= {2000,2000,2000,2000,2000,2000,2000,2000,2000,2000,2000,2000,2000,2000,2000,2000,2000};
wlan_scan_result result;
void WifiTask(void)
{
  static u8  times=254;
  u8 i;
  // Handle any un-solicited event if required - the function shall be triggered
  // few times in a second
  //
  // u8 buf[10]= {1,0,5,0,0,1,0,0x40,1,0};




  if(StuCc300.waiteven!=WAIT)
    hci_unsolicited_event_handler();
  switch(StuWifi.cc3000stu)
  {
  case 0:
    CC3000InitHardware();
    StuWifi.cc3000stu++;
    StuCc300.cc3000stu=0;
    StuCc300.waiteven=NOT_OK;
    break;
  case 1:
    CMU_ClockEnable(cmuClock_USART2, false);
    WlanInterruptDisable();
    CC3000PowerOff();
    StuWifi.cc3000stu++;
    break;
  case 2:
   // if( StuKey.SystemState==SYSTEM_OFF)
   // {

    //  break;
   // }



    if(GsmSta.AskAp==0)
    {
     // if(AdxlStu.state==ADXL_STATIC) break;
       if(times++<250) break;
      if( GsmSta.askwifi==0)break;

    }
    CMU_ClockEnable(cmuClock_USART2, true);
    SPI_setup(2,0,true);
    //  CC3000PowerOn();
    // WlanInterruptEnable();
    if(debug==DEBUGWIF)
      printf("开始扫描\r\n");
    StuWifi.cc3000stu=3;

    break;
  case 3:
    wlan_init( CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch, sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);
    StuWifi.cc3000stu++;
    StuCc300.cc3000stu=0;
    StuCc300.waiteven=NOT_OK;
    break;
  case 4:
    i=cc3000wlan_start(0);
    if(i==OK)
    {
      StuWifi.cc3000stu++;
      StuCc300.cc3000stu=0;
      StuCc300.waiteven=NOT_OK;
    }
    else if(i==NOT_OK)
    {
        StuWifi.cc3000stu=1;
              GsmSta.askwifi=0;

    }
    break;
  case 5:
    i= cc3000wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
    if(i==OK)
    {
      StuWifi.cc3000stu++;
      StuCc300.cc3000stu=0;
      StuCc300.waiteven=0;
      StuCc300.waiteven=NOT_OK;
    }
    else if(i==NOT_OK)
    {
      StuWifi.cc3000stu=1;
       GsmSta.askwifi=0;
    }
    break;
  case 6:

    //  state = nvmem_read_sp_version (version);
    i =cc3000wlan_ioctl_set_scan_params(1, 100, 100, 5, 0x7FF, -80, 0, 205, list);//////////////////////
    if(i==OK)
    {
      state = nvmem_get_mac_address (GsmSta.mac);
      //   printf("ID:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",GsmSto.ID[0],GsmSto.ID[1],GsmSto.ID[2],GsmSto.ID[3],GsmSto.ID[4],GsmSto.ID[5],GsmSto.ID[6],GsmSto.ID[7],GsmSto.ID[8],GsmSto.ID[9]);
      StuWifi.cc3000stu++;
      StuCc300.cc3000stu=0;
      StuCc300.waiteven=NOT_OK;
      result.ap_count=0;
      times=0;
      Memset(&bvkStuWifiAp.Ap_count, 0, 36);
    }
    else if(i==NOT_OK)
    {
      StuWifi.cc3000stu=1;
       GsmSta.askwifi=0;
    }
    break;
  case 7:
    i =cc3000wlan_ioctl_get_scan_results(1,(u8*)&result);
    if(i==OK)
    {
      StuWifi.cc3000stu++;
      StuCc300.cc3000stu=0;
      StuCc300.waiteven=NOT_OK;
    }
    else if(i==NOT_OK)
    {
      StuWifi.cc3000stu=1;
       GsmSta.askwifi=0;
    }
    break;
  case 8:
    if( (debug==DEBUGWIF)&&(result.ap_rssi))
    {

      //  printf("ap_count=(---%d---)\r\nap_state=%d\r\nap_vaild: 1=%d\r\nap_rssi : 7=%d\r\nap_security: 2=%d\r\nap_ssidlen: 6=%d\r\nap_time=%d\r\nap_ssid=%s\r\nap_bssid=%02x-%02x-%02x-%02x-%02x-%02x\r\n",
      //       result.ap_count, result.ap_state,result.ap_vaild,result.ap_rssi,result.ap_security,result.ap_ssidlen,result.ap_time,result.ap_ssid,result.ap_bssid[0],result.ap_bssid[1],result.ap_bssid[2],result.ap_bssid[3],result.ap_bssid[4],result.ap_bssid[5]
      //      );

      printf("ap_count=(---%d---)\r\n ap_ssid------------->%s\r\n ap_rssi : 7=%d\r\nap_bssid=%02x-%02x-%02x-%02x-%02x-%02x\r\n",
             result.ap_count,result.ap_ssid,result.ap_rssi,result.ap_bssid[0],result.ap_bssid[1],result.ap_bssid[2],result.ap_bssid[3],result.ap_bssid[4],result.ap_bssid[5]
               );


    }
    if(0==times)
    {
       Memset((u8*)&bvkStuWifiAp, 0,sizeof(bvkStuWifiAp) );
      if(result.ap_count>5)
        bvkStuWifiAp.Ap_count=5;
      else
      {
        bvkStuWifiAp.Ap_count=result.ap_count;
      }

    }

    if(result.ap_rssi>bvkStuWifiAp.aprssi4)
    {

      ReSort(result);
    }
    times++;
    if(result.ap_count)
    {
      StuWifi.cc3000stu--;
    }

    if((result.ap_count==0)||(times>50))
    {
      Mymemcpy((u8*)&StuWifiAp, (u8*)&bvkStuWifiAp,sizeof(bvkStuWifiAp) );
      if (debug==DEBUGWIF)
        printf("\r\n-->rssi0=%d,rssi1=%d,rssi2=%d,rssi3=%d,rssi4=%d,\r\n",StuWifiAp.aprssi0,StuWifiAp.aprssi1,StuWifiAp.aprssi2,StuWifiAp.aprssi3,StuWifiAp.aprssi4);
      times=0;
      StuWifi.cc3000stu=1;
       GsmSta.askwifi=0;
      if(GsmSta.AskAp==1)
      {
        GsmSta.AskAp=0;
        SendAp();
      }

    }
    break;
  default:
    StuWifi.cc3000stu=0;
    break;



  }






}


