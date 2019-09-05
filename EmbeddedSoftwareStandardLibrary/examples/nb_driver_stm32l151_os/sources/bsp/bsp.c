/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 ZouZH All Rights Reserved
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

#include "led.h"
#include "key.h"
#include "pwrmgr.h"
#include "fm24clxx.h"
#include "usart2.h"
#include "mbus.h"
#include "rs485.h"

#include "nb_app.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */
static void BspConfigNVIC(void);


/**
 * @brief
 *
 * @param
 *
 * @retval
 */
void BspInitHard(void)
{
  BspConfigNVIC();
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
  // Tout=((4×2^pre) ×rlr) / 40 = (12.8 * 2)S
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  IWDG_SetReload(0x0FFF);
  IWDG_Enable();
  IWDG_ReloadCounter();
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
  IWDG_ReloadCounter();
}


/**
 * @brief  configures the nested vectored interrupt controller.
 *
 * @param  none
 *
 * @retval none
 */
void BspConfigNVIC(void)
{
    //NVIC_InitTypeDef NVIC_InitStructure;
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
  GPIO_InitTypeDef GPIO_InitStructure;


  /////////////////////////// Config All Pin Analog input mode ////////////////
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD | \
                        RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  // Config All Pin Analog input mode
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, DISABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ((uint16_t)(~(GPIO_Pin_4 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)));
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ((uint16_t)(~(GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_12)));
  GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifndef USE_FULL_ASSERT
  // SWD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // JNTRST
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif /* USE_FULL_ASSERT */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ((uint16_t)(~(GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_10 | GPIO_Pin_11)));
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  // FRAM
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  //GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);

  // U2-Tx U1-Tx
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);

  // U3-Tx
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);


  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  // U2-Rx, U1-Rx
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);

  // U3-Rx
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);


  /////////////////////////// Config Wakeup interrupt /////////////////////////
  
  // Wakeup Key
  KeyInit();

  // RTC WakeUp
  SysTimeSetWakeup(SYS_PERIOD_WAKEUP);

  PWR_FastWakeUpCmd(ENABLE);
  PWR_UltraLowPowerCmd(ENABLE);
  
  //PWR_EnterSTOPMode( PWR_Regulator_LowPower, PWR_SLEEPEntry_WFI );
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
  NVIC_DisableIRQ(EXTI4_IRQn);
  NVIC_DisableIRQ(RTC_WKUP_IRQn);

  LedInit();
  KeyInit();
  PMInit();
  FRAM_Init();
  USART2Init();
  USART2SetParam(NB_CFG_BAUDRATE, 0);
  MBUSInit();
  RS485Init();
}


