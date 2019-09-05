/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     systime.h
  * @author   ZouZH
  * @version  V1.00
  * @date     13-July-2018
  * @brief    System Real-Time Clock for application.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTIME_H
#define __SYSTIME_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/**
 * @addtogroup SystemTime
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * System time(Hex)
 */
typedef struct  {
  uint16_t year;          /* 1970 ~ 2099 */
  uint8_t  month;         /* 1 - 12 */
  uint8_t  week;          /* 0(Sunday) - 6 */
  uint8_t  day;           /* 1 - 31 */
  uint8_t  hour;          /* 0 - 23 */
  uint8_t  minute;        /* 0 - 59 */
  uint8_t  second;        /* 0 - 59 */
  uint8_t  millisec;      /* 0 - 99 */
  uint32_t runsec;        /* hex */
} SysTime_t;

/* MACROS  ------------------------------------------------------------------ */

/*
 * Determine this year is a leap year
 */
#define IS_LEAP_YEAR(y)  (((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)


/* CONSTANTS  --------------------------------------------------------------- */

/* Period Wakeup count(S) */
#define SYS_PERIOD_WAKEUP  15


/* GLOBAL VARIABLES --------------------------------------------------------- */

extern SysTime_t g_tSysTime;
extern const uint8_t sucMontable[12];

/* GLOBAL FUNCTIONS --------------------------------------------------------- */
uint8_t  SysTimeInit(void);
uint8_t  SysTimeSet(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
uint8_t  SysTimeSetAlarm(uint8_t hour, uint8_t minute);
uint8_t  SysTimeSetWakeup(uint16_t second);

uint8_t  SysTimeUpdate(void);
uint32_t SysTimeGetSecond(void);

uint32_t SysMakeSecond(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
uint8_t  SysMakeDateTime(uint32_t timesec, SysTime_t *ptime);
uint8_t  SysGetWeekFromDate(uint16_t year, uint8_t month, uint8_t day);

void SysTimePoll(void);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __SYSTIME_H */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

