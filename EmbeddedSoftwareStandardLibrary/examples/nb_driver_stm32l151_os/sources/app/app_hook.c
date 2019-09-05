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

#include "includes.h"
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
  //printf("System Restart Run Now...\r\n");
}

