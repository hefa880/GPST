#ifndef __USB_USER_H
#define __USB_USER_H


//#include"includes.h"

#define EP_DATA_OUT       0x01  /* Endpoint for USB data reception.       */
#define EP_DATA_IN        0x81  /* Endpoint for USB data transmission.    */
#define EP_NOTIFY         0x82  /* The notification endpoint (not used).  */
#define BULK_EP_SIZE     USB_MAX_EP_SIZE  /* This is the max. ep size.    */
#define USB_RX_BUF_SIZ   BULK_EP_SIZE /* Packet size when receiving on USB*/
#define USB_TX_BUF_SIZ   127    /* Packet size when transmitting on USB.  */

#define BULK_EP_SIZE     USB_MAX_EP_SIZE  /* This is the max. ep size.    */
#define USB_RX_BUF_SIZ   BULK_EP_SIZE /* Packet size when receiving on USB*/


void StateChange(USBD_State_TypeDef oldState,
                 USBD_State_TypeDef newState);
int SetupCmd(const USB_Setup_TypeDef *setup);

void UsbConnectOrNot(u8 connect);
EFM32_PACK_START(1)
typedef struct
{
    uint32_t dwDTERate;               /** Baudrate                            */
    uint8_t  bCharFormat;             /** Stop bits, 0=1 1=1.5 2=2            */
    uint8_t  bParityType;             /** 0=None 1=Odd 2=Even 3=Mark 4=Space  */
    uint8_t  bDataBits;               /** 5, 6, 7, 8 or 16                    */
    uint8_t  dummy;                   /** To ensure size is a multiple of 4 bytes.*/
} __attribute__ ((packed)) cdcLineCoding_TypeDef;
EFM32_PACK_END()


void UsbRevDate(u8*date,u16 len);
void UsbSent(u8* data,u16 len);


#endif