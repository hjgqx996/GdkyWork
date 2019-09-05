/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     PowerManage.h
  * @author   scf
  * @version  V1.00
  * @date     03-June-2019
  * @brief    Power management, low power process..
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __POWER_MANAGE_H
#define __POWER_MANAGE_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
   
#include "includes.h"
#include "analogio.h"
   
/** 
 * @addtogroup PowerManage
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */
   
   
void PM_Init(void);   
uint8_t PM_GetFlagLowVolt(void);
uint8_t PM_PowerAbnormalDetect(uint32_t u32StartTime);
void PM_Poll(void);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __POWER_MANAGE_H */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

