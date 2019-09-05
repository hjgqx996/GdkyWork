/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 ZouZH  All Rights Reserved
  *
  * @file     rtc.h
  * @author   ZouZH
  * @version  V1.00
  * @date     13-July-2018
  * @brief    stm32l1xx RTC Service.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */
  
/* RTC backup register address */
#define RTC_BKP_ADDR  RTC_BKP_DR0

/* RTC backup register value */
#define RTC_BKP_VALUE 0x32F2

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/*
 * Initialize RTC Service.
 */
uint8_t RTCInit( void );

/*
 * Set the RTC date and time.
 */
uint8_t RTCSetTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);


/*
 * Get RTC date and time.
 */
uint8_t RTCGetTime(SysTime_t *ptime);


/*
 * Set the RTC alarm.
 */
uint8_t RTCSetAlarm(uint8_t hour, uint8_t minute);


/*
 * Set the RTC wakeup period.
 */
uint8_t RTCSetWakeup(uint16_t seconds);

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RTC_H */

/***************************** END OF FILE *************************************/

