#ifndef _GSENSOR_H_
#define _GSENSOR_H_

/* ”ÎC++ºÊ»› */
#ifdef __cplusplus
extern "C" {
#endif

#define GSENSOR_I2C_ADDR    0x98

typedef enum
{
  GSensRegTapStatus  = 3,
  GSensRegOpStat     = 4,
  GSensRegINTEn      = 6,
  GSensRegMode       = 7,
  GSensRegODRtr      = 8,
  GSensRegTapEn      = 9,
  GSensRegXTapTT     = 0x0A,
  GSensRegYTapTT     = 0x0B,
  GSensRegZTapTT     = 0x0C,
  GSensRegCntXL      = 0x0D,
  GSensRegCntXM      = 0x0E,
  GSensRegCntYL      = 0x0F,
  GSensRegCntYM      = 0x10,
  GSensRegCntZL      = 0x11,
  GSensRegCntZM      = 0x12,
  GSensRegOutCfg     = 0x20,
  GSensRegOffXL      = 0x21,
  GSensRegOffXM      = 0x22,
  GSensRegOffYL      = 0x23,
  GSensRegOffYM      = 0x24,
  GSensRegOffZL      = 0x25,
  GSensRegOffZM      = 0x26,
  GSensRegGainX      = 0x27,
  GSensRegGainY      = 0x28,
  GSensRegGainZ      = 0x29
} GSENS_Register_TypeDef;


void initGSensor( void );
int GSensorRegisterGet( I2C_TypeDef *i2c, u8 addr, GSENS_Register_TypeDef reg, u8 *data, u8 dataLength );
int GSensorRegisterSet( I2C_TypeDef *i2c, u8 addr, GSENS_Register_TypeDef reg, u8 *data, u8 dataLength );

void GSensorTask( void );
#ifdef __cplusplus
}  /* end of extern "C" */
#endif

#endif /* _GSENSOR_H_ */
