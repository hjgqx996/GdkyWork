/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     systick.h
  * @author   scf
  * @version  V1.00
  * @date     8-May-2019
  * @brief    System tick for application.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTICK_H
#define __SYSTICK_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/**
 * @addtogroup SystemTick
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * System tick
 */
typedef struct  {
  uint32_t ms;            /* millin  */
  uint8_t sec;            /* second  */
} SysTick_t;

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern SysTick_t g_tSysTick;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */
uint8_t  SysTickInit(void);
uint32_t SysTickGet(void);
uint32_t SysTickGetMs(void);
uint32_t SysTickGetSec(void);
uint32_t SysTickGetMsDif(uint32_t bTime, uint32_t eTime);
uint32_t SysTickGetSecDif(uint32_t bTime, uint32_t eTime);
void SysTickDelayMs(uint16_t ntime);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __SYSTICK_H */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

