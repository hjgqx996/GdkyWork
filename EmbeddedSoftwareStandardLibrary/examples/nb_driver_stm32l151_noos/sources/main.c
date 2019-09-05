/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file    main.c
  * @author  ZouZH
  * @version V1.03
  * @date    12-July-2018
  * @brief   main
  ******************************************************************************
  * 2013-12-10 V1.00  First create the file
  * 2018-07-12 V1.03  Replace Jlink RTT with elog output
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "includes.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

/**
 * @brief  Main
 * @param  None
 * @retval None
 */
int main(void)
{
    // Start independent watchdog
    BspStartIWDG();

  #ifdef USE_IAP_UPDATE
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
  #endif

    // 4 bits for pre-emption priority 0 bits for subpriority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  #ifdef USE_FULL_ASSERT
    DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP, ENABLE);
    DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP, ENABLE);
    if (elog_init() == ELOG_NO_ERR)
      elog_start();
  #endif

  #if (configUSE_TRACE_FACILITY == 1)
    vTraceEnable(TRC_INIT);
  #endif

    // Create first task
    //xTaskCreate(vAppTaskInit, "InitTask", APP_TASK_STK_INIT, NULL, APP_TASK_PRIO_INIT, &g_hTaskInit);

    // Start the scheduler.
    //if (g_hTaskInit)
    //    vTaskStartScheduler();

    // Will only get here if there was not enough heap space to create the idle task.
    while(1)
    {
    }
}

/**
 * @brief  Reports the name of the source file and the source line number
 *          where the assert_param error has occurred.
 *
 * @param  file: pointer to the source file name
 *         line: assert_param error line source n
 *
 * @retval None
 */
void assert_failed(const char* file, uint32_t line)
{
    //taskDISABLE_INTERRUPTS();
    printf("[%s][%d]Assert failed!\r\n", file, line);

    while (1)
    {
    }
}

int32_t fputc(int32_t c, FILE *stream)
{
#ifdef USE_FULL_ASSERT
  extern unsigned SEGGER_RTT_Write(unsigned BufferIndex, const void* pBuffer, unsigned NumBytes);
  char ch = c;

  if (sizeof(ch) == SEGGER_RTT_Write(0, (const char*)&ch, sizeof(ch)))
    return c;
#endif

  return EOF;
}
