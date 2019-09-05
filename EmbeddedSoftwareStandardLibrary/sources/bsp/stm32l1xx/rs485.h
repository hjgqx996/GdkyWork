/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     rs485.h
  * @author   ZouZH
  * @version  V1.02
  * @date     12-June-2016
  * @brief    stm32l1xx uart5 board support package.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __RS485_H
#define __RS485_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

#define RS485_MAX_RLEN        200

#define RS485_RX_OK           0x8000
#define RS485_RX_SIZE_MASK    0x3FFF

#define RS485_RX_START        0x68
#define RS485_RX_END          0x16
#define RS485_RX_TAIL         0x16

#define RS485_RX_TIMEOUT      50      // 50ms

#define RS485_EN_PRINT        0


/* GLOBAL VARIABLES ----------------------------------------------------------- */
extern uint8_t g_ucRS485RxBuf[RS485_MAX_RLEN];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
uint8_t  RS485Init(void);
uint8_t  RS485SetParam(uint32_t baud, uint8_t parity);

uint16_t RS485Send(const uint8_t *pbuf, uint16_t len);
uint8_t  RS485CheckReceive(void);
uint16_t RS485GetReceiveLength(void);
uint32_t RS485GetLastRecvCharTick(void);

void     RS485EnableRx(void);
void     RS485DisableRx(void);

void     RS485Power(uint8_t onoff);


/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RS485_H */

/***************************** END OF FILE *************************************/


