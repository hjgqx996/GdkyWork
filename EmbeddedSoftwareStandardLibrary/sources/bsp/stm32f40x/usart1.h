/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     usart1.h
  * @author   ZouZH
  * @version  V2.02
  * @date     12-June-2016
  * @brief    stm32f10x usart1 board support package.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __USART1_H
#define __USART1_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

#define USART1_MAX_RLEN        100

#define USART1_RX_OK           0x8000
#define USART1_RX_SIZE_MASK    0x3FFF

#define USART1_RX_START        0x68
#define USART1_RX_END          0x16
#define USART1_RX_TAIL         0x16

#define USART1_RX_TIMEOUT      20      // 20ms

#define USART1_EN_PRINT        0


/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern uint8_t g_ucUSART1RxBuf[USART1_MAX_RLEN];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
uint8_t USART1Init(void);
uint8_t USART1SetParam(uint32_t baud, uint8_t parity);

uint16_t USART1Send(const uint8_t *pbuf, uint16_t len);
uint8_t USART1CheckReceive(void);
uint16_t USART1GetReceiveLength(void);
uint32_t USART1GetLastRecvCharTick(void);

void USART1EnableRx(void);
void USART1DisableRx(void);

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __USART1_H */

/***************************** END OF FILE *************************************/


