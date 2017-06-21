#ifndef __CC3000__H__
#define __CC3000__H__

#define CC3000_CS_PORT gpioPortC
#define CC3000_CS_Pin    5
#define CC3000_PWR_PORT gpioPortA
#define CC3000_PWR_Pin    5
#define CC3000_SW_PORT gpioPortC
#define CC3000_SW_Pin     1

#define CC3000_IRQ_PORT gpioPortC
#define CC3000_IRQ_Pin     0

#define CC3000_HiGHT()  GPIO_PinOutSet(CC3000_CS_PORT, CC3000_CS_Pin)
#define CC3000_LOW()  GPIO_PinOutClear(CC3000_CS_PORT, CC3000_CS_Pin)

#define CC3000_READ_IRQ_PIN()   GPIO_PinInGet(CC3000_IRQ_PORT,CC3000_IRQ_Pin)


#define CC3000_POWER_ON   0xaa
#define CC3000_POWR_OFF  0x55


typedef __packed struct
{
    u8 cc3000stu;



} STU_WIFI;
extern STU_WIFI StuWifi;

typedef __packed struct _wlan_full_scan_results_args_t
{
    /*
        4 Bytes: number of networks found
        4 Bytes: The status of the scan: 0 - agged results, 1 - results valid, 2 - no results
        {
            1 bit isValid - is result valid or not
            7 bits rssi - RSSI value;
        }
        {
            2 bits: securityMode - security mode of the AP: 0 - Open, 1 - WEP, 2 WPA, 3 WPA2
            6 bits: SSID name length
        }
        2 bytes: the time at which the entry has entered into scans result table
        32 bytes: SSID name
        6 bytes: BSSID
    */
    uint32_t ap_count;
    uint32_t ap_state;
    uint32_t ap_vaild    : 1;
    uint32_t ap_rssi     : 7;
    uint32_t ap_security : 2;
    uint32_t ap_ssidlen  : 6;
    uint16_t ap_time;
    uint8_t  ap_ssid[32];
    uint8_t  ap_bssid[6];
} wlan_scan_result;




typedef __packed struct
{
    u8 Ap_count;
    
    u8 apmac0[6];
    u8 aprssi0;
    u8 ssid0len;
    u8 ssid0[32];
    
    u8 apmac1[6];
    u8 aprssi1;
    u8 ssid1len;
    u8 ssid1[32];
    
    u8 apmac2[6];
    u8 aprssi2;
    u8 ssid2len;
    u8 ssid2[32];
    
    u8 apmac3[6];
    u8 aprssi3;
    u8 ssid3len;
    u8 ssid3[32];

    
    u8 apmac4[6];
    u8 aprssi4;
    u8 ssid4len;
    u8 ssid4[32];


} STU_WIFI_AP;
extern STU_WIFI_AP StuWifiAp,bvkStuWifiAp;
void WifiTask(void);


typedef __packed struct
{
      u8 apmac[6];
    u8 aprssi;
    u8 ssidlen;
    u8 ssid[32];
}STU_WIFI_MSG;




typedef __packed struct
{

    u8 cc3000stu;
    unsigned long ulSpiIRQState;
    u8 waiteven;
    u16 times;
} STU_CC300;
extern STU_CC300 StuCc300;



//===============================================================================================================================================
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




typedef void (*gcSpiHandleRx)(void *p);
typedef void (*gcSpiHandleTx)(void);

extern unsigned char wlan_tx_buffer[];
//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void SpiOpen(gcSpiHandleRx pfRxHandler);
extern void SpiClose(void);
extern long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength);
extern void SpiResumeSpi(void);
extern void SpiConfigureHwMapping(	unsigned long ulPioPortAddress,
                                    unsigned long ulPort,
                                    unsigned long ulSpiCs,
                                    unsigned long ulPortInt,
                                    unsigned long uluDmaPort,
                                    unsigned long ulSsiPortAddress,
                                    unsigned long ulSsiTx,
                                    unsigned long ulSsiRx,
                                    unsigned long ulSsiClck);
//extern void SpiCleanGPIOISR(void);
extern long TXBufferIsEmpty(void);
extern long RXBufferIsEmpty(void);
void CC3000IRQPing(void);
u8 WaitSpi(u8*flag);
void CommandInit(void);


#endif




