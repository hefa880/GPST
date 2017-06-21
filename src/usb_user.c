
#include"includes.h"

EFM32_ALIGN(4)
EFM32_PACK_START(1)
cdcLineCoding_TypeDef __attribute__ ((aligned(4))) cdcLineCoding =
{
    115200, 0, 0, 8, 0
};
EFM32_PACK_END()

STATIC_UBUF(usbRxBuffer0, USB_RX_BUF_SIZ);    /* USB receive buffers.   */
STATIC_UBUF(usbRxBuffer1, USB_RX_BUF_SIZ);


static const u8  *usbRxBuffer[  2 ] = { usbRxBuffer0, usbRxBuffer1 };


static u8            usbRxIndex;/*接收双缓冲的序号*/


static bool           usbRxActive;/*正在接收中*/
static bool           usbTxActive;/*, dmaRxActive;  true表示正在发送中*/



//串口波特率同步用
static int LineCodingReceived(USB_Status_TypeDef status,
                              uint32_t xferred,
                              uint32_t remaining);

/*发送完成后回调函数*/
static int UsbDataTransmitted(USB_Status_TypeDef status,
                              uint32_t xferred,
                              uint32_t remaining);
STRUCT_QUEUE_CTRL DebugUartRxQueueCtrl;					//gps接收队列控制结构体
volatile u8 DebugUartRxQueueBuf[DebugUart_REC_QUEUE_LENGTH];	//gps接收队列缓存

/*
*********************************************************************************************************
*	函 数 名:void UsbSent(u8* data,u16 len)
*	功能说明: USB发送数据函数
*                  一条
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

void UsbSent(u8* data,u16 len)
{

    INT_Disable();
    if (!usbTxActive)
    {
        /* usbTxActive = false means that a new USB packet can be transferred. */
        usbTxActive = true;
        USBD_Write(EP_DATA_IN, (void*) data,
                   len, UsbDataTransmitted);
        // LastUsbTxCnt = len;
    }
    INT_Enable();

}
/*
*********************************************************************************************************
*	函 数 名:void UsbRevDate(u8*date,u16 len)
*	功能说明: USB接收数据
*                  一条
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/
void UsbRevDate(u8*date,u16 len)
{
    u16 i;
    u8 err;
    for(i=0; i<len; i++)
    {

        err=QueuePost(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf,date[i]);

        if(err!=Q_OPT_SUCCEED)
        {
            QueueFlush(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf);
        }

    }
//    UsbSent("12345678901234567890\r\n",22);
}


/*
*********************************************************************************************************
*	函 数 名:void UsbConnectOrNot(u8 true)
*	功能说明: USB插上和取下时动作，true为
*                  一条
*       作    者 ：liupeng
*	形    参：
*       版    本：version 1.0
*	返 回 值: 无
*********************************************************************************************************
*/

void UsbConnectOrNot(u8 connect)
{

    if(connect!=true)
    {
        USBD_Disconnect();
        USBTIMER_DelayMs(1000);
    }
    SystemClockConfig(connect);
    QueueInit(&DebugUartRxQueueCtrl,(u8 *)DebugUartRxQueueBuf,DebugUart_REC_QUEUE_LENGTH);//队列初始化
    if(connect==true)
    {

        USBD_Init(&initstruct);
        USBD_Disconnect();
        USBTIMER_DelayMs(1000);
        USBD_Connect();

    }
    ADCConfig();
    setupRtc();

    //initLeuart(GPS_UART,38400,POWER_ON);
    initLeuart(LEUART0,MODEM_IPR,POWER_ON);


}

/**************************************************************************//**
 * @brief Callback function called whenever a new packet with data is received
 *        on USB.
 *
 * @param[in] status    Transfer status code.
 * @param[in] xferred   Number of bytes transferred.
 * @param[in] remaining Number of bytes not transferred.
 *
 * @return USB_STATUS_OK.
 *****************************************************************************/
static int UsbDataReceived(USB_Status_TypeDef status,
                           uint32_t xferred,
                           uint32_t remaining)
{
//   u16 i;
    (void) remaining;            /* Unused parameter */

    if ((status == USB_STATUS_OK) && (xferred > 0))
    {
        //   p=(u8*)usbRxBuffer[usbRxIndex ];

        usbRxIndex ^= 1;
        if (!usbRxActive)
        {
            /* dmaTxActive = false means that a new UART Tx DMA can be started. */
            usbRxActive = true;
            /*USB 接收数据*/
            UsbRevDate( (u8*)usbRxBuffer[ usbRxIndex^1 ],xferred);
            usbRxActive = false;
            /* Start a new USB receive transfer. */
            USBD_Read(EP_DATA_OUT, (void*) usbRxBuffer[ usbRxIndex ],
                      USB_RX_BUF_SIZ, UsbDataReceived);
        }

    }
    return USB_STATUS_OK;
}
/**************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *   Starts CDC operation when device has been configured by USB host.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 *****************************************************************************/
void StateChange(USBD_State_TypeDef oldState,
                 USBD_State_TypeDef newState)
{
    if (newState == USBD_STATE_CONFIGURED)
    {
        /* We have been configured, start CDC functionality ! */

        if (oldState == USBD_STATE_SUSPENDED)   /* Resume ?   */
        {
        }

        /* Start receiving data from USB host. */
        usbRxIndex  = 0;
        usbRxActive= false;
        usbTxActive    = false;
        USBD_Read(EP_DATA_OUT, (void*) usbRxBuffer[ usbRxIndex ],
                  USB_RX_BUF_SIZ, UsbDataReceived);

    }

    else if ((oldState == USBD_STATE_CONFIGURED) &&
             (newState != USBD_STATE_SUSPENDED))
    {
        /* We have been de-configured, stop CDC functionality */
        USBTIMER_Stop(0);
        DMA->CHENC = 3;     /* Stop DMA channels 0 and 1. */
    }

    else if (newState == USBD_STATE_SUSPENDED)
    {
        /* We have been suspended, stop CDC functionality */
        /* Reduce current consumption to below 2.5 mA.    */
        USBTIMER_Stop(0);
        DMA->CHENC = 3;     /* Stop DMA channels 0 and 1. */
    }
}

/**************************************************************************//**
 * @brief
 *   Handle USB setup commands. Implements CDC class specific commands.
 *
 * @param[in] setup Pointer to the setup packet received.
 *
 * @return USB_STATUS_OK if command accepted.
 *         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
 *         stack will handle the request.
 *****************************************************************************/

int SetupCmd(const USB_Setup_TypeDef *setup)
{
    int retVal = USB_STATUS_REQ_UNHANDLED;

    if ((setup->Type == USB_SETUP_TYPE_CLASS) &&
            (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE))
    {
        switch (setup->bRequest)
        {
        case USB_CDC_GETLINECODING:
            /********************/
            if ((setup->wValue == 0) &&
                    (setup->wIndex == 0) &&               /* Interface no.            */
                    (setup->wLength == 7) &&              /* Length of cdcLineCoding  */
                    (setup->Direction == USB_SETUP_DIR_IN))
            {
                /* Send current settings to USB host. */
                USBD_Write(0, (void*) &cdcLineCoding, 7, NULL);
                retVal = USB_STATUS_OK;
            }
            break;

        case USB_CDC_SETLINECODING:
            /********************/
            if ((setup->wValue == 0) &&
                    (setup->wIndex == 0) &&               /* Interface no.            */
                    (setup->wLength == 7) &&              /* Length of cdcLineCoding  */
                    (setup->Direction != USB_SETUP_DIR_IN))
            {
                /* Get new settings from USB host. */
                USBD_Read(0, (void*) &cdcLineCoding, 7, LineCodingReceived);
                retVal = USB_STATUS_OK;
            }
            break;

        case USB_CDC_SETCTRLLINESTATE:
            /********************/
            if ((setup->wIndex == 0) &&               /* Interface no.  */
                    (setup->wLength == 0))                /* No data        */
            {
                /* Do nothing ( Non compliant behaviour !! ) */
                retVal = USB_STATUS_OK;
            }
            break;
        }
    }

    return retVal;
}



/**************************************************************************//**
 * @brief
 *   Callback function called when the data stage of a CDC_SET_LINECODING
 *   setup command has completed.
 *
 * @param[in] status    Transfer status code.
 * @param[in] xferred   Number of bytes transferred.
 * @param[in] remaining Number of bytes not transferred.
 *
 * @return USB_STATUS_OK if data accepted.
 *         USB_STATUS_REQ_ERR if data calls for modes we can not support.
 //串口波特率同步用
 *****************************************************************************/
//串口波特率同步用
static int LineCodingReceived(USB_Status_TypeDef status,
                              uint32_t xferred,
                              uint32_t remaining)
{
#if 0
    uint32_t frame = 0;
#endif
    (void) remaining;

    /* We have received new serial port communication settings from USB host */
    if ((status == USB_STATUS_OK) && (xferred == 7))
    {
#if 0
        /* Check bDataBits, valid values are: 5, 6, 7, 8 or 16 bits */
        if (cdcLineCoding.bDataBits == 5)
            frame |= UART_FRAME_DATABITS_FIVE;

        else if (cdcLineCoding.bDataBits == 6)
            frame |= UART_FRAME_DATABITS_SIX;

        else if (cdcLineCoding.bDataBits == 7)
            frame |= UART_FRAME_DATABITS_SEVEN;

        else if (cdcLineCoding.bDataBits == 8)
            frame |= UART_FRAME_DATABITS_EIGHT;

        else if (cdcLineCoding.bDataBits == 16)
            frame |= UART_FRAME_DATABITS_SIXTEEN;

        else
            return USB_STATUS_REQ_ERR;

        /* Check bParityType, valid values are: 0=None 1=Odd 2=Even 3=Mark 4=Space  */
        if (cdcLineCoding.bParityType == 0)
            frame |= UART_FRAME_PARITY_NONE;

        else if (cdcLineCoding.bParityType == 1)
            frame |= UART_FRAME_PARITY_ODD;

        else if (cdcLineCoding.bParityType == 2)
            frame |= UART_FRAME_PARITY_EVEN;

        else if (cdcLineCoding.bParityType == 3)
            return USB_STATUS_REQ_ERR;

        else if (cdcLineCoding.bParityType == 4)
            return USB_STATUS_REQ_ERR;

        else
            return USB_STATUS_REQ_ERR;

        /* Check bCharFormat, valid values are: 0=1 1=1.5 2=2 stop bits */
        if (cdcLineCoding.bCharFormat == 0)
            frame |= UART_FRAME_STOPBITS_ONE;

        else if (cdcLineCoding.bCharFormat == 1)
            frame |= UART_FRAME_STOPBITS_ONEANDAHALF;

        else if (cdcLineCoding.bCharFormat == 2)
            frame |= UART_FRAME_STOPBITS_TWO;

        else
            return USB_STATUS_REQ_ERR;

        /* Program new UART baudrate etc. */
        UART1->FRAME = frame;
        USART_BaudrateAsyncSet(UART1, 0, cdcLineCoding.dwDTERate, usartOVS16);
#endif
        return USB_STATUS_OK;
    }
    return USB_STATUS_REQ_ERR;
}

/**************************************************************************//**
 * @brief Callback function called whenever a packet with data has been
 *        transmitted on USB
 *
 * @param[in] status    Transfer status code.
 * @param[in] xferred   Number of bytes transferred.
 * @param[in] remaining Number of bytes not transferred.
 *
 * @return USB_STATUS_OK.
 *****************************************************************************/
/*发送完成后回调函数*/
static int UsbDataTransmitted(USB_Status_TypeDef status,
                              uint32_t xferred,
                              uint32_t remaining)
{
    (void) xferred;              /* Unused parameter */
    (void) remaining;            /* Unused parameter */

    if (status == USB_STATUS_OK)
    {
#if 0
        if (!dmaRxActive)
        {
            /* dmaRxActive = false means that a new UART Rx DMA can be started. */

            USBD_Write(EP_DATA_IN, (void*) uartRxBuffer[ uartRxIndex ^ 1],
                       uartRxCount, UsbDataTransmitted);
            LastUsbTxCnt = uartRxCount;

            dmaRxActive    = true;
            dmaRxCompleted = true;
            DMA_ActivateBasic(1, true, false,
                              (void *) uartRxBuffer[ uartRxIndex ],
                              (void *) &(UART1->RXDATA),
                              USB_TX_BUF_SIZ - 1);
            uartRxCount = 0;
            USBTIMER_Start(0, RX_TIMEOUT, UartRxTimeout);
        }
        else
#endif
        {
            /* The UART receive DMA callback function will start a new DMA. */
            usbTxActive = false;
        }
    }
    return USB_STATUS_OK;
}

