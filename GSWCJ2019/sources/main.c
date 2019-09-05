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
#include "system_msp430x44x.h"
#include "bsp.h"
#include "nb_app.h"
#include "PowerManage.h"
#include "lcd.h"
#include "key.h"
#include "TemperatureMeasure.h"
#include "record_app.h"
#include "paramConfig.h"

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
  BspInitHard();
  vAppTaskInit(0);
  
  while(1)
  {
    BspBackRun();       //后台运行任务
    
    PM_Poll();          //电源管理
    SensorSamplePoll(); //传感器数据采集
    DisplayPoll();      //显示
    
    nbAppPoll();        //NB application.
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
    //printf("[%s][%d]Assert failed!\r\n", file, line);

    while (1)
    {
    }
}
