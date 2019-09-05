/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 ZouZH  All Rights Reserved
  *
  * @file     pwrmgr.c
  * @author   ZouZH
  * @version  V1.00
  * @date     13-Aug-2018
  * @brief    Power Management.
  ******************************************************************************
  */

/* INCLUDES ------------------------------------------------------------------- */
#include "pwrmgr.h"

#include "includes.h"
#include "stm32l1xx.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */
static SemaphoreHandle_t hSemaphTaskApp;
static SemaphoreHandle_t hSemaphTaskCom;

/* LOCAL FUNCTIONS ------------------------------------------------------------ */



/**
 * @brief  电源管理初始化
 * @param  无
 * @retval 无
 */
void PMInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* Battery Voltage under 3.3V pin  PC0 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  if (NULL == hSemaphTaskApp)
    hSemaphTaskApp = xSemaphoreCreateBinary();

  if (NULL == hSemaphTaskCom)
    hSemaphTaskCom = xSemaphoreCreateBinary();
}


/**
 * @brief  等待唤醒信号
 * @param  无
 * @retval 无
 */
void PMWaitWakeupApp(void)
{
  xSemaphoreTake(hSemaphTaskApp, portMAX_DELAY);
}

/**
 * @brief  释放唤醒信号
 *
 * @param  无
 *
 * @retval 无
 */
void PMGiveWakeupApp(void)
{
  xSemaphoreGive(hSemaphTaskApp);
}

/**
 * @brief  等待唤醒信号
 * @param  无
 * @retval 无
 */
void PMWaitWakeupCom(void)
{
  xSemaphoreTake(hSemaphTaskCom, portMAX_DELAY);
}

/**
 * @brief  释放唤醒信号
 *
 * @param  无
 *
 * @retval 无
 */
void PMGiveWakeupCom(void)
{
  xSemaphoreGive(hSemaphTaskCom);
}


/**
 * @brief  电池是否低电
 * @param  无
 * @retval 1低电,0正常
 */
uint8_t PMIsBatLowVol(void)
{
  return (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == Bit_RESET);
}
