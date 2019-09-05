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
#define LOG_TAG    "rtc"

/* INCLUDES ------------------------------------------------------------------- */
#include "includes.h"
#include "rtc.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* Debug info output */
#define RTC_TRACE	log_i

/* Select the RTC Clock source */
#define RTC_CLOCK_SOURCE_LSE
//#define RTC_CLOCK_SOURCE_LSI


/* Enable interrupt mode */
//#define RTC_INT_SOURCE_WAKEUP
//#define RTC_INT_SOURCE_ALARMA

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */
static uint8_t RTCConfiguration(void);

/**
 * @brief  Initialize RTC Service
 *
 * @param  None
 *
 * @retval 0-Success.
 */
uint8_t RTCInit(void)
{
    /* INITS is reset when the calendar year field is set to 0x00 (power-on reset value) */
    if ((RTC_BKP_VALUE != RTC_ReadBackupRegister(RTC_BKP_ADDR)) || (RTC_GetFlagStatus(RTC_FLAG_INITS) == RESET))
    {
        /* Backup data register value is not correct or not yet programmed (when
         the first time the program is executed) */
        RTC_TRACE("RTC not yet configured");
        /* RTC Configuration */
        if(RTCConfiguration())
        {
            RTC_TRACE("RTC configure fail");
            return 2;
        }

        RTC_TRACE("Default Set_date and Set_time: 2018-10-01 00:00:00");
        if (RTCSetTime(0x2018, 0x10, 0x01, 1, 0, 0))
            return 3;
    }
    else
    {
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
          RTC_TRACE("Power On Reset occurred....");
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
          RTC_TRACE("External Reset occurred....");
        }

        RTC_TRACE("No need to configure RTC....");

        /* Enable the PWR clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

        /* Allow access to RTC */
        PWR_RTCAccessCmd(ENABLE);

        /* Wait for RTC APB registers synchronisation */
        if (RTC_WaitForSynchro() != SUCCESS)
            return 4;
    }

#ifdef RTC_INT_SOURCE_ALARMA
{
    EXTI_InitTypeDef  EXTI_InitStructure;
    /* RTC Alarm A Interrupt Configuration */
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    RTC_ITConfig(RTC_IT_ALRA, ENABLE);
    
    /* Enable the RTC Alarm A Interrupt */
    RTC_ClearFlag(RTC_FLAG_ALRAF);
    EXTI_ClearITPendingBit(EXTI_Line17);
}
#endif /* RTC_INT_SOURCE_ALARMA */

#ifdef RTC_INT_SOURCE_WAKEUP
{
    EXTI_InitTypeDef  EXTI_InitStructure;
    EXTI_ClearITPendingBit(EXTI_Line20);
    EXTI_InitStructure.EXTI_Line = EXTI_Line20;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    if (RTC_WakeUpCmd(DISABLE) != SUCCESS)
        return 5;
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
    RTC_SetWakeUpCounter(RTC_PERIOD_WAKEUP);

    RTC_ClearFlag(RTC_FLAG_WUTF);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);

    if (RTC_WakeUpCmd(ENABLE) != SUCCESS)
        return 6;
}
#endif /* RTC_INT_SOURCE_WAKEUP */


    return 0;
}


/**
 * @brief  Configures the RTC.
 *
 * @param  None
 *
 * @retval 0-Success.
 */
static uint8_t RTCConfiguration(void)
{
    RTC_InitTypeDef RTC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    uint32_t AsynchPrediv = 0;
    uint32_t SynchPrediv = 0;

    uint32_t count = 0x100000;

    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_RTCAccessCmd(ENABLE);

    /* Reset RTC Domain */
    RCC_RTCResetCmd(ENABLE);
    RCC_RTCResetCmd(DISABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
    /* The RTC Clock may varies due to LSI frequency dispersion. */

    /* Enable the LSI OSC */
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */
    while((RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) && count--);
    if (!count)
        return 1;

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    SynchPrediv = 0x120; /* (37KHz / 128) - 1 = 0x120*/
    AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */

    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */
    while((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && count--);
    if (!count)
        return 2;

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#else
    #error Please select the RTC Clock source inside the rtc.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    if (RTC_WaitForSynchro() != SUCCESS)
        return 3;

    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;

    /* Check on RTC init */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      RTC_TRACE("RTC Prescaler Config failed!");
      return 4;
    }

    return 0;
}


/**
 * @brief  Set the RTC date and time(BCD formate).
 *
 * @param  year: 00-99, month: 1-12, day: 1-31, hour: 0-23, minute: 0-59, second: 0-59
 *
 * @retval 0-Success.
 */
uint8_t RTCSetTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    RTC_DateTypeDef RTC_DateStructure;
    RTC_TimeTypeDef RTC_TimeStructure;
    uint8_t tmp = 0;

    RTC_DateStructure.RTC_Year = UINT16_LO(year);
    RTC_DateStructure.RTC_Month = month;
    RTC_DateStructure.RTC_Date = day;

    tmp = SysGetWeekFromDate(bcd2dec(UINT16_HI(year)) * 100 + bcd2dec(UINT16_LO(year)), bcd2dec(month), bcd2dec(day));
    if (!tmp)
        RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Sunday;
    else
        RTC_DateStructure.RTC_WeekDay = tmp;

    /* if use STOP low power mode,must run this step */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_RTCAccessCmd(ENABLE);
    if (RTC_WaitForSynchro() != SUCCESS)
        return 1;
            
    if (RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure) != SUCCESS)
    {
        RTC_TRACE("RTC set date failed!");
        return 2;
    }

    RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours = hour;
    RTC_TimeStructure.RTC_Minutes = minute;
    RTC_TimeStructure.RTC_Seconds = second;
    if (RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure) != SUCCESS)
    {
        RTC_TRACE("RTC set time failed!");
        return 3;
    }

    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_RTCAccessCmd(ENABLE);

    RTC_WriteBackupRegister(RTC_BKP_ADDR, RTC_BKP_VALUE);

    return 0;
}

/**
 * @brief  Uptate the RTC date and time.
 *
 * @param  Time out
 *
 * @retval 0-Success.
 */
uint8_t RTCGetTime(SysTime_t *ptime)
{
    RTC_DateTypeDef RTC_DateStructure;
    RTC_TimeTypeDef RTC_TimeStructure;

    if (NULL == ptime)
        return 1;

    RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
    RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);

    ptime->year = 0x2000 | RTC_DateStructure.RTC_Year;
    ptime->month = RTC_DateStructure.RTC_Month;
    ptime->day = RTC_DateStructure.RTC_Date;
    if (RTC_Weekday_Sunday == RTC_DateStructure.RTC_WeekDay)
        ptime->week = 0;
    else
        ptime->week = RTC_DateStructure.RTC_WeekDay;

    ptime->hour = RTC_TimeStructure.RTC_Hours;
    ptime->minute = RTC_TimeStructure.RTC_Minutes;
    ptime->second = RTC_TimeStructure.RTC_Seconds;

    return 0;
}


/**
 * @brief  Set the RTC alarm.
 *
 * @param  Time input BCD hour(00-23) and minute(00-59)
 *
 * @retval 0-Success.
 */
uint8_t RTCSetAlarm(uint8_t hour, uint8_t minute)
{
    RTC_AlarmTypeDef  RTC_AlarmStructure;

    if (hour > 0x23 || minute > 0x59)
        return 1;

    /* if use STOP low power mode,must run this step */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_RTCAccessCmd(ENABLE);
    if (RTC_WaitForSynchro() != SUCCESS)
        return 2;

    RTC_ClearFlag(RTC_FLAG_ALRAF);
    EXTI_ClearITPendingBit(EXTI_Line17);

    if (RTC_AlarmCmd(RTC_Alarm_A, DISABLE) != SUCCESS)
        return 3;

    RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = hour;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = minute;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x00;

    RTC_AlarmStructure.RTC_AlarmDateWeekDay = 31;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

    /* Configure the RTC Alarm A register */
    RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

    RTC_ClearFlag(RTC_FLAG_ALRAF );
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);
    
    /* Enable the alarm  A */
    if (RTC_AlarmCmd(RTC_Alarm_A, ENABLE) != SUCCESS)
        return 4;

    RTC_ClearFlag(RTC_FLAG_ALRAF );
    EXTI_ClearITPendingBit(EXTI_Line17);
    
    return 0;
}


/**
 * @brief  Set the RTC Wakeup Period.
 *
 * @param  Time in sec
 *
 * @retval 0-Success.
 */
uint8_t RTCSetWakeup(uint16_t seconds)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* if use STOP low power mode,must run this step */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_RTCAccessCmd(ENABLE);
    if (RTC_WaitForSynchro() != SUCCESS)
        return 1;
        
    EXTI_ClearITPendingBit(EXTI_Line20);
    EXTI_InitStructure.EXTI_Line = EXTI_Line20;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    if (RTC_WakeUpCmd(DISABLE) != SUCCESS)
        return 2;
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
    RTC_SetWakeUpCounter(seconds);

    RTC_ClearFlag(RTC_FLAG_WUTF);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);

    if (RTC_WakeUpCmd(ENABLE) != SUCCESS)
        return 3;
        
    return 0;
}






