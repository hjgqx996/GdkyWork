/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     mbus.h
  * @author   ZouZH
  * @version  V2.02
  * @date     12-June-2016
  * @brief    stm32l1xx usart3 board support package.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __MBUS_H
#define __MBUS_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

#define MBUS_MAX_RLEN        200

#define MBUS_RX_OK           0x8000
#define MBUS_RX_SIZE_MASK    0x3FFF

#define MBUS_RX_START        0x68
#define MBUS_RX_END          0x16
#define MBUS_RX_TAIL         0x16

#define MBUS_RX_TIMEOUT      50      // 50ms

#define MBUS_EN_PRINT        0


/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern uint8_t g_ucMBUSRxBuf[MBUS_MAX_RLEN];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
uint8_t  MBUSInit(void);
uint8_t  MBUSSetParam(uint32_t baud, uint8_t parity);
void     MBUSProtectPoll(void);

uint16_t MBUSSend(const uint8_t *pbuf, uint16_t len);
uint8_t  MBUSCheckReceive(void);
uint16_t MBUSGetReceiveLength(void);
uint32_t MBUSGetLastRecvCharTick(void);

void     MBUSEnableRx(void);
void     MBUSDisableRx(void);

void     MBUSPower(uint8_t onoff);


/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* MBUS_H */

/***************************** END OF FILE *************************************/


