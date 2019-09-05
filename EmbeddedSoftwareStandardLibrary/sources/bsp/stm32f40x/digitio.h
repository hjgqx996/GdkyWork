/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     digitio.h
  * @author   ZouZH
  * @version  V1.00
  * @date     04-Nov-2018
  * @brief    Digit In and Out driver.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DIGITIO_H
#define __DIGITIO_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/** 
 * @addtogroup DigitIO
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * @brief Digit in and out error status
 */
typedef enum
{
  DIO_OK = 0,
  DIO_ERROR_Param,
}DIO_ERROR_t;

/**
 * @brief Digit in and out channel type
 */
typedef enum
{
  DIO_TYPE_IN_MIN,    /*!< Digit input type */
  DIO_TYPE_IN1,
  DIO_TYPE_IN2,
  DIO_TYPE_IN_MAX,

  DIO_TYPE_OUT_MIN,    /*!< Digit output type */
  DIO_TYPE_OUT1,
  DIO_TYPE_OUT2,
  DIO_TYPE_OUT3,
  DIO_TYPE_OUT4, 
  DIO_TYPE_OUT5,
  DIO_TYPE_OUT6,
  DIO_TYPE_OUT7,
  DIO_TYPE_OUT8,
  DIO_TYPE_OUT9,
  DIO_TYPE_OUT10,
  DIO_TYPE_OUT11,
  DIO_TYPE_OUT12,
  DIO_TYPE_OUT13,
  DIO_TYPE_OUT_MAX,
}DIO_TYPE_t;

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

#define DIO_CH_NUM_IN  (DIO_TYPE_IN_MAX - DIO_TYPE_IN_MIN - 1)
#define DIO_CH_NUM_OUT (DIO_TYPE_OUT_MAX - DIO_TYPE_OUT_MIN - 1)

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

DIO_ERROR_t DIO_Init(void);
DIO_ERROR_t DIO_Poll(void);

uint32_t    DIO_GetInFreq(DIO_TYPE_t DIO_TYPE_INx);
void        DIO_ClearInFreq(DIO_TYPE_t DIO_TYPE_INx);

uint8_t     DIO_GetInStatus (DIO_TYPE_t DIO_TYPE_INx);
uint8_t     DIO_GetOutStatus(DIO_TYPE_t DIO_TYPE_OUTx);
DIO_ERROR_t DIO_SetOutStatus(DIO_TYPE_t DIO_TYPE_OUTx, uint8_t newStatus);


/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __DIGITIO_H */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

