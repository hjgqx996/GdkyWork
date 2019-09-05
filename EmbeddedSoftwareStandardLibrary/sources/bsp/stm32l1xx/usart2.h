/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     usart2.h
  * @author   ZouZH
  * @version  V2.02
  * @date     12-June-2016
  * @brief    stm32f10x usart2 board support package.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __USART2_H
#define __USART2_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

#define USART2_MAX_RLEN        1

#define USART2_RX_OK           0x8000
#define USART2_RX_SIZE_MASK    0x3FFF

#define USART2_RX_START        0x68
#define USART2_RX_END          0x16
#define USART2_RX_TAIL         0x16

#define USART2_RX_TIMEOUT      50      // 50ms

#define USART2_EN_PRINT        0


/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern uint8_t g_ucUSART2RxBuf[USART2_MAX_RLEN];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
uint8_t USART2Init(void);
uint8_t USART2SetParam(uint32_t baud, uint8_t parity);

uint16_t USART2Send(const uint8_t *pbuf, uint16_t len);
uint8_t USART2CheckReceive(void);
uint16_t USART2GetReceiveLength(void);
uint32_t USART2GetLastRecvCharTick(void);

void USART2EnableRx(void);
void USART2DisableRx(void);

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __USART2_H */

/***************************** END OF FILE *************************************/


