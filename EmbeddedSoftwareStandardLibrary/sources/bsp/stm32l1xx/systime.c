/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     systime.c
  * @author   ZouZH
  * @version  V1.00
  * @date     13-July-2018
  * @brief    System Real-Time Clock for application.
  ******************************************************************************
  * @history
  */
#define LOG_TAG    "systime"

/* INCLUDES ----------------------------------------------------------------- */
#include "systime.h"
#include "includes.h"
#include "rtc.h"

/** 
 * @defgroup SystemTime
 * @brief 系统时间通用接口
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* The month table of nonleap year */
static const uint8_t sucMontable[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* GLOBAL VARIABLES --------------------------------------------------------- */

SysTime_t g_tSysTime;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


/**
 * @brief  Init system time
 *
 * @param  void
 *
 * @retval 0-Success.
 */
 uint8_t SysTimeInit(void)
 {
     uint8_t retErr = 0;

     // TODO: Add init time driver here
     retErr = RTCInit();

     if (retErr)
     {
        log_w("TimeInit Fail!");
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        PWR_RTCAccessCmd(ENABLE);
        RTC_WriteBackupRegister(RTC_BKP_ADDR, 0x0000);
        RTCInit();
     }

     SysTimeUpdate();

     return retErr;
 }


/**
 * @brief  Set the RTC date and time(BCD formate).
 *
 * @param  year (1970~2099)
 * @param  month (1~12)
 * @param  day (1~31)
 * @param  hour (0~23)
 * @param  minute (0~59)
 * @param  second (0~59)
 *
 * @retval 0-Success.
 */
uint8_t SysTimeSet(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint8_t ret = 0;
    
    if ((year < 0x1970) || (year > 0x2099) || (month < 0x01) || (month > 0x12) || \
       (day < 0x01) || (day > 0x31) || (hour > 0x23) || (minute > 0x59) || (second > 0x59))
    {
        return 1;
    }

    // TODO: Add set time driver here
    ret = RTCSetTime(year, month, day, hour, minute, second);
    
    return ret;
}

/**
 * @brief  Set the RTC alarm.
 *
 * @param  hour:   Time input BCD hour(00-23)
 * @param  minute: Time input BCD minute(00-59)
 *
 * @retval 0-Success.
 */
uint8_t SysTimeSetAlarm(uint8_t hour, uint8_t minute)
{
    uint8_t ret = RTCSetAlarm(hour, minute);
    if (ret)
      log_w("SetAlarm Fail!");
    return ret;
}

/**
 * @brief  Set the RTC Wakeup.
 *
 * @param  second: Time input second(<0xFFFF)
 *
 * @retval 0-Success.
 */
uint8_t SysTimeSetWakeup(uint16_t second)
{
    uint8_t ret = RTCSetWakeup(second);
    if (ret)
      log_w("SetWakeup Fail!");
    return ret;
}


/**
 * @brief  Update system time(Call cycle < 1s)
 *
 * @param  void
 *
 * @retval 0-Success.
 */
uint8_t SysTimeUpdate(void)
{
    static uint8_t stopcnt = 0;

    SysTime_t tm;

    // TODO: Add Get time driver here
    if (RTCGetTime(&tm) || (g_tSysTime.second == tm.second))
    {
        if (++stopcnt > 240)
        {
            stopcnt = 0;
            log_w("TimeUpdate Fail!, Reinit system clock...");
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
            PWR_RTCAccessCmd(ENABLE);
            RTC_WriteBackupRegister(RTC_BKP_ADDR, 0x0000);
            RTCInit();
        }
        return 1;
    }

    stopcnt = 0;

    SYSEnterCritical();
    g_tSysTime.runsec += 1;
    g_tSysTime.year = tm.year;
    g_tSysTime.month = tm.month;
    g_tSysTime.day = tm.day;
    g_tSysTime.week = tm.week;
    g_tSysTime.hour = tm.hour;
    g_tSysTime.minute = tm.minute;
    g_tSysTime.second = tm.second;
    SYSExitCritical();

    return 0;

}


/**
 * @brief  Make the second count of date and time.
 *
 * @param  void
 *
 * @return Seconds from 1970.
 */
uint32_t SysTimeGetSecond(void)
{
    uint16_t i = 0;
    uint16_t tmp = 0;
    uint32_t secnum = 0;

    /* The year is beyond the scope rules */
    if ((g_tSysTime.year < 0x1970) || (g_tSysTime.year > 0x2099))
        return 0;

    SYSTaskSuspendAll();

    /* The number of seconds  years */
    tmp = bcd2dec(UINT16_HI(g_tSysTime.year)) * 100 + bcd2dec(UINT16_LO(g_tSysTime.year));
    for (i = 1970; i < tmp; i++)
    {
        if (IS_LEAP_YEAR(i))
            secnum += 31622400UL;
        else
            secnum += 31536000UL;
    }

    /* The number of seconds months */
    tmp = bcd2dec(g_tSysTime.month);
    for ( i = 1; i < tmp; i++)
    {
        secnum += (uint32_t)sucMontable[i - 1] * 86400UL;
        if (IS_LEAP_YEAR(bcd2dec(UINT16_HI(g_tSysTime.year)) * 100 + bcd2dec(UINT16_LO(g_tSysTime.year))) && (i == 2))
            secnum += 86400UL;
    }

    secnum += (uint32_t)(bcd2dec(g_tSysTime.day) - 1) * 86400UL;
    secnum += (uint32_t)bcd2dec(g_tSysTime.hour) * 3600UL;
    secnum += (uint32_t)bcd2dec(g_tSysTime.minute) * 60UL;
    secnum += bcd2dec(g_tSysTime.second);

    SYSTaskResumeAll();

    return secnum;
}


/**
 * @brief  Make the second count of date and time(Hex formate).
 *
 * @param  year (1970~2099)
 * @param  month (1~12)
 * @param  day (1~31)
 * @param  hour (0~23)
 * @param  minute (0~59)
 * @param  second (0~59)
 *
 * @return Seconds from 1970.
 */
uint32_t SysMakeSecond(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint16_t i = 0;
    uint32_t secnum = 0;

    /* The year is beyond the scope rules */
    if ((year < 1970) || (year > 2099))
        return 0;

    /* The number of seconds  years */
    for (i = 1970; i < year; i++)
    {
        if (IS_LEAP_YEAR(i))
            secnum += 31622400UL;
        else
            secnum += 31536000UL;
    }

    /* The number of seconds months */
    for ( i = 1; i < month; i++)
    {
        secnum += (uint32_t)sucMontable[i - 1] * 86400UL;
        if (IS_LEAP_YEAR(year) && (i == 2))
            secnum += 86400UL;
    }

    secnum += (uint32_t)(day - 1) * 86400UL;
    secnum += (uint32_t)hour *3600UL;
    secnum += (uint32_t)minute * 60UL;
    secnum += second;

    return secnum;
}

/**
 * @brief  Make the date and time(BCD formate)  of second count.
 *
 * @param[in]  timesec: Seconds count from 1970.
 * @param[out] ptime:   Output @ref SysTime_t format time.
 *
 * @retval 0-Success.
 */
uint8_t SysMakeDateTime(uint32_t timesec, SysTime_t *ptime)
{
    uint16_t i = 0;
    uint16_t year = 1970;
    uint32_t tmpsec = 0;

    if (NULL == ptime)
        return 1;

    /* The number of seconds  years */
    for (i = year; i <= 2099; i++)
    {
        if (IS_LEAP_YEAR(i))
        {
            if (timesec < 31622400UL)
                break;

            timesec -= 31622400UL;
        }
        else
        {
            if (timesec < 31536000UL)
                break;

            timesec -= 31536000UL;
        }

        year++;
    }
    ptime->year = UINT16_BUILD(dec2bcd(year % 100), dec2bcd(year / 100));

    /* The number of seconds months */
    for ( i = 1; i <= 12; i++)
    {
        tmpsec = (uint32_t)sucMontable[i - 1] * 86400UL;
        if (IS_LEAP_YEAR(year) && (i == 2))
            tmpsec += 86400UL;

        if (timesec < tmpsec)
            break;

        timesec -= tmpsec;
    }
    ptime->month = dec2bcd(i);

    /* The number of seconds days */
    for ( i = 1; i <= 31; i++)
    {
        if (timesec < 86400UL)
            break;
        timesec -= 86400UL;
    }
    ptime->day = dec2bcd(i);

    /* The number of seconds hours */
    for (i = 0; i <= 23; i++)
    {
        if (timesec < 3600UL)
            break;

        timesec -= 3600UL;
    }
    ptime->hour = dec2bcd(i);

    /* The number of seconds minutes */
    for ( i = 0; i <= 59; i++)
    {
        if (timesec < 60UL)
            break;
        timesec -= 60UL;
    }
    ptime->minute = dec2bcd(i);

    ptime->second = dec2bcd(timesec);

    return 0;
}


/**
 * @brief  According to the date to calculate the week(Hex format).
 *
 * @param  year (1970~2099)
 * @param  month (1~12)
 * @param  day (1~31)
 *
 * @retval 0-6. 0-Sunday
 */
uint8_t SysGetWeekFromDate(uint16_t year, uint8_t month, uint8_t day)
{
    if ((month == 1) || (month == 2))
    {
        year -= 1;
        month += 12;
    }

    return ((day + 1 + (2 * month) + (3 * (month + 1)) / 5 + year + (year / 4) - (year / 100) + (year / 400)) % 7);
}

/**
 * @}
 */ 

