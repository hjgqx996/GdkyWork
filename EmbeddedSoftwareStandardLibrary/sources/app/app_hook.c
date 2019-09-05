/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     app_hook.c
  * @author   ZouZH
  * @version  V1.00
  * @date     12-July-2018
  * @brief    application hook function.
  ******************************************************************************
  * @history
  */

/* INCLUDES ----------------------------------------------------------------- */

#include "led.h"
#include "key.h"
#include "pwrmgr.h"

#include "includes.h"
#include "record_app.h"
#include "nb.h"


/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

void vApplicationStackOverflowHook( TaskHandle_t * pxTask, signed char *pcTaskName );
void vApplicationMallocFailedHook( TaskHandle_t * pxTask, signed char *pcTaskName );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );

/* LOCAL VARIABLES ---------------------------------------------------------- */

extern AppRunObj_t s_tAppObj;

static uint16_t s_usRTCWkupCnt = 0;
static uint16_t s_usRTCErrCnt = 0;


/* LOCAL FUNCTIONS ---------------------------------------------------------- */


/**
 * @brief Task stack overflow hook
 *
 * @param[in] Task handler
 * @param[in] Task name string
 *
 * @retval None
 */
void vApplicationStackOverflowHook( TaskHandle_t * pxTask, signed char *pcTaskName )
{
  ( void )pxTask;

  xprintf("[Error][%s]Task stack overflow!\r\n", pcTaskName);

  taskDISABLE_INTERRUPTS();
  for( ;; )
  {
  }
}

/**
 * @brief Heap malloc failed hook
 *
 * @param[in] Task handler
 * @param[in] Task name
 *
 * @retval None
 */
void vApplicationMallocFailedHook( TaskHandle_t * pxTask, signed char *pcTaskName )
{
  ( void )pxTask;

  xprintf("[Error][%s]Heap malloc failed!\r\n", pcTaskName);

  taskDISABLE_INTERRUPTS();
  for( ;; )
  {
  }
}

/**
 * @brief  Rtos idle hook
 *
 * @param  None
 *
 * @retval None
 */
void vApplicationIdleHook( void )
{
}


/**
 * @brief  Rtos tick interrupt hook
 *
 * @param  None
 *
 * @retval None
 */
void vApplicationTickHook( void )
{
  nbUpdateTime(portTICK_PERIOD_MS);
}



/**
 * @brief  Before the system goes into low power mode
 *
 * @param  None
 *
 * @retval None
 */
void vApplicationPreStopHook( void )
{
  if (s_usRTCWkupCnt && NVIC_GetEnableIRQ(EXTI4_IRQn) && NVIC_GetEnableIRQ(RTC_WKUP_IRQn) )
    return;
    
  // NB RESET
  nbHWSetReset(0);
  LedSet(LED_ALL, LED_MODE_OFF);
  
  BspEnterStopMode();
  
  //printf("System Stop Run Now...\r\n");
}

/**
* @brief  After the system exits the low power mode
*
* @param  None
*
* @retval None
*/
void vApplicationPostStopHook( void )
{
  s_usRTCWkupCnt += 1;
  
  // 重置看门狗
  if ((s_usRTCErrCnt < 5760) && (s_usRTCWkupCnt < 11520))
    BspReStartIWDG();
    
  // RTC状态
  RTC_WaitForSynchro();
  if (SysTimeUpdate())
    s_usRTCErrCnt += 1;
  else
    s_usRTCErrCnt = 0;

  // 是否按键唤醒
  if (!KeyIsDwon())
  {
    if (s_usRTCErrCnt)
      return;
    
    // 清除分摊标志: 分摊点1min外
    if (s_tAppObj.mode && (bcd2dec(g_tSysTime.minute) != s_tAppObj.allocMinute))
    {
      s_tAppObj.mode = 0;
      return;
    }

    // 检查是否分摊: 分摊点1min内, 空闲状态
    if (s_tAppObj.allocMinute > 59)
      s_tAppObj.allocMinute = 0;

    if (s_tAppObj.allocHour >= 24)
      s_tAppObj.allocHour = 00;
      
    if ((bcd2dec(g_tSysTime.minute) != s_tAppObj.allocMinute) || \
        (bcd2dec(g_tSysTime.hour) != s_tAppObj.allocHour))
      return;
  }

  s_usRTCWkupCnt = 0;
  
  // Reset system clock
  extern void SetSysClock(void);
  SetSysClock();
  
  BspExitStopMode();

  PMGiveWakeupApp();
  //printf("System Restart Run Now...\r\n");
}

