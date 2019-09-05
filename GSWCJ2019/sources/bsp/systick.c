/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     systick.c
  * @author   scf
  * @version  V1.00
  * @date     8-May-2019
  * @brief    System tick for application.
  ******************************************************************************
  * @history
  */
#define LOG_TAG    "systick"

/* INCLUDES ----------------------------------------------------------------- */
#include "systick.h"
#include "includes.h"
//#include "rtc.h"

/**
 * @defgroup SystemTick
 * @brief 系统滴答定时器通用接口
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

SysTick_t g_tSysTick;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* system tick Count */
static volatile uint32_t s_ulTickCnt = 0;

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


/**
 * @brief  Init system tick
 *
 * @param  void
 *
 * @retval 0-Success.
 */
 uint8_t SysTickInit(void)
 {
    // TODO: Add init time driver here

    // 00 Stop mode: the timer is halted
    TACTL_bit.MC1 = 0;
    TACTL_bit.MC0 = 0;

    //Timer_A clear. Setting this bit resets TAR, the clock divider, and the count
    //direction. The TACLR bit is automatically reset and is always read as zero.
    TACTL_bit.TACLR = 1;

    // Input divider. 00 /1
    TACTL_bit.ID1 = 0;
    TACTL_bit.ID0 = 0;

    // 01 ACLK
    TACTL_bit.TASSEL1 = 0;
    TACTL_bit.TASSEL0 = 1;

    // Period 1 ms
    TACCR0 = 33;//32768 - 1;

    TACCTL0_bit.CCIFG = 0;
    TACCTL0_bit.CCIE = 1;//1 Interrupt enabled. Capture/compare interrupt enable. 

    // 01 Up mode: the timer counts up to TACCR0
    TACTL_bit.MC0 = 1;
    
    SYSExitCritical();// 开启全局中断
    
    g_tSysTick.ms = 0;
    g_tSysTick.sec = 0;

    return 0;
 }


/**
 * @brief  Get system tick.
 *
 * @param  void
 *
 * @retval system tick value.
 */
uint32_t SysTickGet(void)
{
  uint32_t tmpTime;
  
	// 获取当前毫秒计数
	tmpTime = s_ulTickCnt;
  
#ifndef DEVICE_32BIT
	// 这里是为了处理非32位处理器，在上一步赋值
	// 过程中被中断导致返回毫秒数错误的情况
	// 对32位CPU不需以下代码
  while ( tmpTime != s_ulTickCnt )
  {
		tmpTime = s_ulTickCnt;
	}
#endif
  
  return s_ulTickCnt;
}

/**
 * @brief  Get Ms time.
 *
 * @param  void
 *
 * @retval current Ms time.
 */
uint32_t SysTickGetMs(void)
{
  uint32_t tmpTime;
  
	// 获取当前毫秒计数
	tmpTime = g_tSysTick.ms;
  
#ifndef DEVICE_32BIT
	// 这里是为了处理非32位处理器，在上一步赋值
	// 过程中被中断导致返回毫秒数错误的情况
	// 对32位CPU不需以下代码
  while ( tmpTime != g_tSysTick.ms )
  {
		tmpTime = g_tSysTick.ms;
	}
#endif
  
  return tmpTime;
}

/**
 * @brief  Get second time.
 *
 * @param  void
 *
 * @retval current second time.
 */
uint32_t SysTickGetSec(void)
{
  uint32_t tmpTime;
  
	tmpTime = SysTickGetMs();// 取得当前毫秒数

	return (tmpTime/1000);
}


/**
 * @brief  Get Ms interval, uint ms.
 * @param[in]   bTime:begin time
 * @param[in]   eTime:end time
 * @retval      Ms interval.
 */
uint32_t SysTickGetMsDif(uint32_t bTime, uint32_t eTime)
{
	if ( eTime >= bTime )
		return (eTime - bTime);
	else
		return ((0xFFFFFFFF - bTime) + eTime);	
}

/**
 * @brief  Get second interval, uint s.
 * @param[in]   bTime:begin time
 * @param[in]   eTime:end time
 * @retval      second interval.
 */
uint32_t SysTickGetSecDif(uint32_t bTime, uint32_t eTime)
{
	if ( eTime >= bTime )
		return (eTime - bTime);
	else
		return ((0xFFFFFFFF - bTime) + eTime);	
}


/**
 * @brief  delay function, uint ms.
 * @param[in]   ntime: delay time value
 * @retval  None
 */
void SysTickDelayMs(uint16_t ntime)
{
	uint32_t cur_time = 0;

	cur_time = SysTickGetMs();
	
	do {
//		WDT_Feed();
	} while ( SysTickGetMsDif(cur_time, SysTickGetMs()) < ntime);
}

/**
 * @brief  Timer_A interrupt handler
 *
 * @param  None
 *
 * @retval None
 */
#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA_IRQHandler( void )
{
    s_ulTickCnt++;
    g_tSysTick.ms++;
}

/**
 * @}
 */

