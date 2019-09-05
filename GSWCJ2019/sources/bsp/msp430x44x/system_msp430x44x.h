/**
  ******************************************************************************
  *               Copyright(C) 2019-2029 GDKY All Rights Reserved
  *
  * @file     system_msp430x44x.h
  * @author   ZouZH
  * @version  V1.00
  * @date     21-March-2019
  * @brief    System clock configuration.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTEM_MSP430X44X_H
#define __SYSTEM_MSP430X44X_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/**
 * @addtogroup msp430x44x_system
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __SYSTEM_MSP430X44X_H */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

