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
  DIO_IN_P10,
  DIO_IN_P11,
  DIO_IN_LOS1,
  DIO_IN_K1,
  DIO_IN_K2,
  DIO_IN_K3,
  DIO_IN_DYD,
  DIO_TYPE_IN_MAX,

  DIO_TYPE_OUT_MIN,    /*!< Digit output type */
  DIO_OUT_GPRS_VDD,
  DIO_OUT_CONTROL,
  DIO_OUT_GPRS_ON,
  DIO_OUT_GPRS_RST,
  DIO_TYPE_OUT_MAX,
}DIO_TYPE_t;

/* MACROS  ------------------------------------------------------------------ */
// Macros for hardware access
#define HWREG32(x) (*((volatile uint32_t *)((uint16_t)x)))
#define HWREG16(x) (*((volatile uint16_t *)((uint16_t)x)))
#define HWREG8(x)  (*((volatile uint8_t *)((uint16_t)x)))
          
/* CONSTANTS  --------------------------------------------------------------- */

#define DIO_CH_NUM_IN  (DIO_TYPE_IN_MAX - DIO_TYPE_IN_MIN - 1)
#define DIO_CH_NUM_OUT (DIO_TYPE_OUT_MAX - DIO_TYPE_OUT_MIN - 1)

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

DIO_ERROR_t DIO_Init(void);
DIO_ERROR_t DIO_Poll(void);

uint8_t     DIO_GetInStatus (DIO_TYPE_t DIO_TYPE_INx);
uint8_t     DIO_GetOutStatus(DIO_TYPE_t DIO_TYPE_OUTx);
DIO_ERROR_t DIO_SetOutStatus(DIO_TYPE_t DIO_TYPE_OUTx, uint8_t newStatus);

uint8_t DIO_GetRegBit(const volatile uint8_t * regAddr, uint8_t bit);
void    DIO_SetRegBit(volatile uint8_t * regAddr, uint8_t bit);
void    DIO_ClrRegBit(volatile uint8_t * regAddr, uint8_t bit);

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

