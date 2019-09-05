/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     key.c
  * @author   ZouZH
  * @version  V1.00
  * @date     27-July-2018
  * @brief    Wake up key for NB-IoT Collector.
  ******************************************************************************
  * @history
  */
#define LOG_TAG    "KEY"

/* INCLUDES ----------------------------------------------------------------- */
#include "key.h"
#include "includes.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* Key Press or Release continue time count */
static uint32_t sulKeyDepressCnt;
static uint32_t sulKeyReleaseCnt;

/* Key status */
static KeyStatus_t stKeyStatus;

/* LOCAL FUNCTIONS ---------------------------------------------------------- */



/**
 * @brief  Key init
 * @param  None
 * @return
 * @retval 0-Success
 */
uint8_t KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    /* Configure PA4 pin  as input floating  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* Connect EXTI4 Line to PA4 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);

    /* Configure EXTI4 line */
    EXTI_ClearITPendingBit(EXTI_Line4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    stKeyStatus = KEY_RELEASE;
    sulKeyDepressCnt = 0;
    sulKeyReleaseCnt = 0;

    return 0;
}

/**
 * @brief  Key is down
 * @param  None
 * @return Key Status
 * @retval 0up 1down
 */
uint8_t KeyIsDwon(void)
{
  return (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == (uint8_t)Bit_RESET);
}


/**
 * @brief  Get key status
 * @param  None
 * @return Key Status
 * @retval KEY_RELEASE or KEY_DEPRESS
 */
KeyStatus_t KeyGetStatus(void)
{
  return stKeyStatus;
}

/**
 * @brief  Key poll scan
 * @param  None
 * @return None
 * @retval None
 */
void KeyPoll(void)
{
    static uint8_t step = 0;
    static uint32_t lastTick = 0;

    uint32_t curtick = 0;

    // scan period
    if (!step)
    {
      curtick = SYSGetTickCount();
      if (curtick - lastTick < SYS_MS_TO_TICKS(KEY_POLL_CYCLE))
        return;
      lastTick = curtick;
    }

    switch (step)
    {
      case 0:
        step = 1;
        break;

      case 1:
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == (uint8_t)Bit_RESET)
        {
            if (++sulKeyDepressCnt > KEY_VALID_TIME)
            {
                sulKeyReleaseCnt = 0;
                stKeyStatus = KEY_DEPRESS;
            }
        }
        else
        {
            if (++sulKeyReleaseCnt > KEY_RELEASE_TIME)
            {
                sulKeyDepressCnt = 0;
                stKeyStatus = KEY_RELEASE;
            }
        }
        step = 0;
        break;

      default:
        step = 0;
        break;
    }
}



