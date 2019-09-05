/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     uart4.h
  * @author   ZouZH
  * @version  V2.02
  * @date     12-June-2016
  * @brief    stm32f4xx uart4 board support package.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __UART4_H
#define __UART4_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>
/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

#define UART4_MAX_RLEN        100

#define UART4_RX_OK           0x8000
#define UART4_RX_SIZE_MASK    0x3FFF

#define UART4_RX_START        0xFE
#define UART4_RX_END          0xED
#define UART4_RX_TAIL         0xED

#define UART4_RX_TIMEOUT      20      // 20ms Char gap time

#define UART4_EN_PRINT        0


/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern uint8_t g_ucUART4RxBuf[UART4_MAX_RLEN];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
uint8_t UART4Init(void);
uint8_t UART4SetParam(uint32_t baud, uint8_t parity);

uint16_t UART4Send(const uint8_t *pbuf, uint16_t len);
uint8_t UART4CheckReceive(void);
uint16_t UART4GetReceiveLength(void);
uint32_t UART4GetLastRecvCharTick(void);

void UART4EnableRx(void);
void UART4DisableRx(void);

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __UART4_H */

/***************************** END OF FILE *************************************/


