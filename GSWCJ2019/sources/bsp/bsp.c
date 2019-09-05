/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY All Rights Reserved
  *
  * @file    bsp.c
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   board support packet.
  ******************************************************************************
  */
#define LOG_TAG    "bsp"

/* INCLUDES ----------------------------------------------------------------- */

#include "includes.h"
#include "bsp.h"

#include "usart0.h"
#include "analogio.h"
#include "lcd.h"
#include "key.h"
#include "TemperatureMeasure.h"
#include "paramConfig.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

/**
 * @brief
 *
 * @param
 *
 * @retval
 */
void BspInitHard(void)
{
  SystemInit();//system clock.
  
  SysTimeInit();//stsytem RTC
  SysTickInit();//timer for ms/sec/delay.
  
  // Power on delay
  SYSDelayMs(100);
  
  AIO_Init();//初始化电源电压AD采集
  DIO_Init();//GPIO init
  lcd_init();
  TemperatureHumiditySensorInit();//传感器初始化
  USART0_Init();   //调试、配置参数串口初始化
  USART0_EnableRx(); //使能接收
  
  #if(DEVICE_TYPE == 3)  //如果为开关型
  {
    AIO_InOnOff(1);//start AD sample
    LPM3;
  }
  #endif
  AllShowLcd();//全显示
  SysTickDelayMs(2000);
  #if(DEVICE_TYPE == 3)  //如果为开关型
  {
    AIO_InOnOff(1);//start AD sample
    LPM3;
  }
  #endif
  
  // Start independent watchdog
  //BspStartIWDG();
  
  clear_lcd();//清屏
  
  #ifdef USE_FULL_ASSERT
    if (elog_init() == ELOG_NO_ERR)
      elog_start();
  #endif
}

/**
 * @brief  start the independent watch dog.
 *
 * @param  none
 *
 * @retval none
 */
void BspStartIWDG(void)
{
}

/**
 * @brief  Restart the independent watch dog.
 *
 * @param  none
 *
 * @retval none
 */
void BspReStartIWDG(void)
{
}


/**
 * @brief
 *
 * @param
 *
 * @retval
 */
void BspEnterStopMode(void)
{
}


/**
 * @brief
 *
 * @param
 *
 * @retval
 */
void BspExitStopMode(void)
{
}


/**
 * @brief  background task run 
 *
 * @param  None
 *
 * @retval None
 */
void BspBackRun(void)
{
  SysTimePoll();      //实时时钟
  DIO_Poll();
  LocalConfigPoll();  //
  KeyPoll();          //按键
}



