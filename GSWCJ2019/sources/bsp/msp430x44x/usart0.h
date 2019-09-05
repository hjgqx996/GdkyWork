/**
  ******************************************************************************
  *               Copyright(C) 2019-2029 GDKY All Rights Reserved
  *
  * @file     usart0.h
  * @author   ZouZH
  * @version  V1.01
  * @date     14-Feb-2019
  * @brief    msp430f448 usart0 board support package.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __USART0_H
#define __USART0_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

#define USART0_MAX_RLEN        100u

#define USART0_RX_OK           0x8000u
#define USART0_RX_SIZE_MASK    0x3FFFu

#define USART0_RX_START        '+'
#define USART0_RX_END          0x0D
#define USART0_RX_TAIL         0x0A

#define USART0_RX_TIMEOUT      20      // ms

#define USART0_EN_PRINT        0


/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern uint8_t g_ucUSART0RxBuf[USART0_MAX_RLEN];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
uint8_t  USART0_Init(void);
uint8_t  USART0_SetParam(uint32_t baud, uint8_t parity);

uint16_t USART0_Send(const uint8_t *pbuf, uint16_t len);
uint8_t  USART0_CheckReceive(void);
void     USART0_ClearRxStatus(void);
uint16_t USART0_GetReceiveLength(void);
uint32_t USART0_GetLastRecvCharTick(void);

void     USART0_EnableRx(void);
void     USART0_DisableRx(void);

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __USART0_H */

/***************************** END OF FILE *************************************/


