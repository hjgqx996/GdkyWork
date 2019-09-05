/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_driver.h
  * @author   ZouZH
  * @version  V1.00
  * @date     31-July-2018
  * @brief    NB-IoT driver port.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NB_HW_H
#define __NB_HW_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @addtogroup NB-Hardware
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */
uint8_t  nbHWInit(uint32_t baudrate);
uint16_t nbHWSend(const void* pbuf, uint16_t len);
void     nbHWSetPower(uint8_t stat);
void     nbHWSetReset(uint8_t stat);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __NB_HW_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/


