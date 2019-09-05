/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     analogio.h
  * @author   ZouZH
  * @version  V1.00
  * @date     03-Nov-2018
  * @brief    Analog input and output.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ANALOGIO_H
#define __ANALOGIO_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/** 
 * @addtogroup AnalogIO
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * @brief Analog in and out error status
 */
typedef enum
{
  AIO_OK = 0,
  AIO_ERROR_Param,
}AIO_ERROR_t;


/**
 * @brief Analog in channel type
 */
typedef enum
{
  AIO_TYPE_IN_MIN,    /*!< Analog input */
  AIO_TYPE_IN1,
  AIO_TYPE_IN2,
  AIO_TYPE_IN3,
  AIO_TYPE_IN4,
  AIO_TYPE_IN5,
  AIO_TYPE_IN6,
  AIO_TYPE_IN7,
  AIO_TYPE_IN8,
  AIO_TYPE_IN9,
  AIO_TYPE_IN10,
  AIO_TYPE_IN11,
  AIO_TYPE_IN12,
  AIO_TYPE_IN13,
  AIO_TYPE_IN_MAX
}AIO_TYPE_t;

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

#define AIO_CH_NUM_IN  (AIO_TYPE_IN_MAX - AIO_TYPE_IN_MIN - 1)

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

AIO_ERROR_t AIO_Init(void);

uint16_t    AIO_GetInVoltage(AIO_TYPE_t AIN_TYPE_x);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __ANALOGIO_H */

/**
 * @}
 */

/***************************** END OF FILE ************************************/


