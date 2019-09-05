/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_os.h
  * @author   ZouZH
  * @version  V1.00
  * @date     31-July-2018
  * @brief    NB-IoT os port.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NB_OS_H
#define __NB_OS_H 100

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>

/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @addtogroup NB-OS
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */
uint32_t nbOsTimeGet(void);

uint8_t  nbOsMutexInit(void);
uint8_t  nbOsMutexUninit(void);
uint8_t  nbOsMutexAcquire(uint32_t timout);
void     nbOsMutexRelease(void);


void     *nbOsMemAlloc(size_t size);
void     nbOsMemFree(void *p);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __NB_OS_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

