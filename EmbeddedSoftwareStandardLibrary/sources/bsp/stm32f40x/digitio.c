/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     digitio.h
  * @author   ZouZH
  * @version  V1.00
  * @date     04-Nov-2018
  * @brief    Digit In and Out driver.
  ******************************************************************************
  * @history
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "digitio.h"

#include "stm32f4xx.h"

/** 
 * @defgroup DigitIO
 * @brief Digit In and Out driver
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * @brief Digit in and out status value
 */
typedef union
{
  struct 
  {
    uint32_t IN1  : 1;
    uint32_t IN2  : 1;
    uint32_t IN3  : 1;
    uint32_t IN4  : 1;
    uint32_t IN5  : 1;
    uint32_t IN6  : 1;
    uint32_t IN7  : 1;
    uint32_t IN8  : 1;
    uint32_t IN9  : 1;
    uint32_t IN10 : 1;
    uint32_t IN11 : 1;
    uint32_t IN12 : 1;
  }inbit;
    
  struct
  {
    uint32_t OUT1 : 1;
    uint32_t OUT2 : 1;
    uint32_t OUT3 : 1;
    uint32_t OUT4 : 1;
    uint32_t OUT5 : 1;
    uint32_t OUT6 : 1;
    uint32_t OUT7 : 1;
    uint32_t OUT8 : 1;
  }outbit;

  uint32_t value;
}DIO_Status_t;


/**
 * @brief Digit Port and Pin define
 */
typedef struct
{
  GPIO_TypeDef* GPIOx;
  uint16_t      GPIO_Pin;
}DIO_PortPin_t;


/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

// PD15(LSO1, 0-On), PD10(LSO2, 0-On)
static const DIO_PortPin_t DIN_PORT_PIN[DIO_CH_NUM_IN] = {
  {GPIOD, GPIO_Pin_15}, {GPIOD, GPIO_Pin_10}
};

// PD7(K11,5V->3.3V), PC10(K10),          PB3(K13,5V->3.3V), PB8(K12)
// PE2(K9,220V),      PE4(K2,L1->L2),     PE3(K1,ON->OFF),   PC8(K3),
// PC7(K4),           PD13(K6,L1->L2),    PD14(K5,ON->OFF),  PD11(K7),
// PD12(K8),
static const DIO_PortPin_t DOUT_PORT_PIN[DIO_CH_NUM_OUT] = {
  {GPIOD, GPIO_Pin_7}, {GPIOC, GPIO_Pin_10}, {GPIOB, GPIO_Pin_3}, {GPIOB, GPIO_Pin_8},
  {GPIOE, GPIO_Pin_2}, {GPIOE, GPIO_Pin_4},  {GPIOE, GPIO_Pin_3}, {GPIOC, GPIO_Pin_8},
  {GPIOC, GPIO_Pin_7}, {GPIOD, GPIO_Pin_13}, {GPIOD, GPIO_Pin_14},{GPIOD, GPIO_Pin_11},
  {GPIOD, GPIO_Pin_12}
};

// Timer clock = 10Khz
#define DIO_TIME_FREQ  10000

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* Digit in and out status variable */
DIO_Status_t s_tDInStatus;
DIO_Status_t s_tDOutStatus;

/* Digit PWM input frequency hz */
uint32_t s_ulDInFreq[DIO_CH_NUM_IN];


/* LOCAL FUNCTIONS ---------------------------------------------------------- */
static void DIO_InLowLevelInit(void);
static void DIO_OutLowLevelInit(void);
static void DIO_InTimerLowLevelInit(void);


/**
 * @brief  Init
 * @param  None
 * @return Init status(DIO_OK Init success)
 */
DIO_ERROR_t DIO_Init(void)
{
  DIO_InLowLevelInit();
  DIO_OutLowLevelInit();
  DIO_InTimerLowLevelInit();
  return DIO_OK;
}

/**
 * @brief  Poll digit in or out status check
 * @param  None
 * @return Poll status(DIO_OK success)
 */
DIO_ERROR_t DIO_Poll(void)
{
  uint8_t ofs = 0;

  for (ofs = 0; ofs < DIO_CH_NUM_IN; ofs++)
  {
    if ((DIN_PORT_PIN[ofs].GPIOx->IDR & DIN_PORT_PIN[ofs].GPIO_Pin) != (uint32_t)Bit_RESET)
    {
      s_tDInStatus.value |= (uint32_t)(1 << ofs);
    }
    else
    {
      s_tDInStatus.value &= ~(uint32_t)(1 << ofs);
    }
  }

  return DIO_OK;
}

/**
 * @brief  Get in pin frequency
 * @param  DIO_TYPE_INx (DIO_TYPE_IN_MIN < x < DIO_TYPE_IN_MAX)
 * @return In pin frequency(Hz)
 */
uint32_t DIO_GetInFreq(DIO_TYPE_t DIO_TYPE_INx)
{
  uint8_t ofs = 0;
  
  if ((DIO_TYPE_INx <= DIO_TYPE_IN_MIN) || (DIO_TYPE_INx >= DIO_TYPE_IN_MAX))
  {
    return 0;
  }

  ofs = DIO_TYPE_INx - DIO_TYPE_IN1;
  
  return s_ulDInFreq[ofs];
}

/**
 * @brief  Clear in pin frequency
 * @param  DIO_TYPE_INx (DIO_TYPE_IN_MIN < x < DIO_TYPE_IN_MAX)
 * @return In pin frequency(Hz)
 */
void DIO_ClearInFreq(DIO_TYPE_t DIO_TYPE_INx)
{
  uint8_t ofs = 0;
  
  if ((DIO_TYPE_INx <= DIO_TYPE_IN_MIN) || (DIO_TYPE_INx >= DIO_TYPE_IN_MAX))
  {
    return;
  }

  ofs = DIO_TYPE_INx - DIO_TYPE_IN1;

  s_ulDInFreq[ofs] = 0;
}


/**
 * @brief  Get in pin status
 * @param  DIO_TYPE_INx (DIO_TYPE_IN_MIN < x < DIO_TYPE_IN_MAX)
 * @return In pin status(0 or 1)
 */
uint8_t DIO_GetInStatus(DIO_TYPE_t DIO_TYPE_INx)
{
  uint8_t ofs = 0;
  
  if ((DIO_TYPE_INx <= DIO_TYPE_IN_MIN) || (DIO_TYPE_INx >= DIO_TYPE_IN_MAX))
  {
    return 0;
  }

  ofs = DIO_TYPE_INx - DIO_TYPE_IN1;
  
  if ((DIN_PORT_PIN[ofs].GPIOx->IDR & DIN_PORT_PIN[ofs].GPIO_Pin) != (uint32_t)Bit_RESET)
  {
    s_tDInStatus.value |= (uint32_t)(1 << ofs);
    return 1;
  }
  else
  {
    s_tDInStatus.value &= ~(uint32_t)(1 << ofs);
    return 0;
  }
}


/**
 * @brief  Get out pin status
 * @param  DIO_TYPE_OUTx (DIO_TYPE_OUT_MIN < x < DIO_TYPE_OUT_MAX)
 * @return Out pin status(0 or 1)
 */
uint8_t DIO_GetOutStatus(DIO_TYPE_t DIO_TYPE_OUTx)
{
  uint8_t ofs = 0;
  
  if ((DIO_TYPE_OUTx <= DIO_TYPE_OUT_MIN) || (DIO_TYPE_OUTx >= DIO_TYPE_OUT_MAX))
  {
    return 0;
  }

  ofs = DIO_TYPE_OUTx - DIO_TYPE_OUT1;
  
  if ((DOUT_PORT_PIN[ofs].GPIOx->ODR & DOUT_PORT_PIN[ofs].GPIO_Pin) != (uint32_t)Bit_RESET)
  {
    s_tDOutStatus.value |= (uint32_t)(1 << ofs);
    return 1;
  }
  else
  {
    s_tDOutStatus.value &= ~(uint32_t)(1 << ofs);
    return 0;
  }
}


/**
 * @brief  Set out pin status
 * @param  DIO_TYPE_OUTx (DIO_TYPE_OUT_MIN < x < DIO_TYPE_OUT_MAX)
 * @param  newStatus (0 or 1)
 * @return Operation status(DIO_OK Success)
 */
DIO_ERROR_t DIO_SetOutStatus(DIO_TYPE_t DIO_TYPE_OUTx, uint8_t newStatus)
{
  uint8_t ofs = 0;
  
  if ((DIO_TYPE_OUTx <= DIO_TYPE_OUT_MIN) || (DIO_TYPE_OUTx > DIO_TYPE_OUT_MAX))
  {
    return DIO_ERROR_Param;
  }

  if (DIO_TYPE_OUTx == DIO_TYPE_OUT_MAX)
  {
    if (newStatus)
    {
      for (ofs = 0; ofs < DIO_CH_NUM_OUT; ofs++)
      {
        s_tDOutStatus.value |= (uint32_t)(1 << ofs);
        DOUT_PORT_PIN[ofs].GPIOx->BSRRL = DOUT_PORT_PIN[ofs].GPIO_Pin;
      }
    }
    else
    {
      for (ofs = 0; ofs < DIO_CH_NUM_OUT; ofs++)
      {
        s_tDOutStatus.value &= ~(uint32_t)(1 << ofs);
        DOUT_PORT_PIN[ofs].GPIOx->BSRRH = DOUT_PORT_PIN[ofs].GPIO_Pin ;
      }
    }

    return DIO_OK;
  }

  ofs = DIO_TYPE_OUTx - DIO_TYPE_OUT1;

  if (newStatus)
  {
    s_tDOutStatus.value |= (uint32_t)(1 << ofs);
    DOUT_PORT_PIN[ofs].GPIOx->BSRRL = DOUT_PORT_PIN[ofs].GPIO_Pin;
  }
  else
  {
    s_tDOutStatus.value &= ~(uint32_t)(1 << ofs);
    DOUT_PORT_PIN[ofs].GPIOx->BSRRH = DOUT_PORT_PIN[ofs].GPIO_Pin ;
  }

  return DIO_OK;
}



/**
 * @brief  Digit out low level init
 * @param  None
 * @return None
 */
static void DIO_InLowLevelInit(void)
{
  uint8_t i = 0;

  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  for (i = 0; i < DIO_CH_NUM_IN; i++)
  {
    GPIO_InitStructure.GPIO_Pin = DIN_PORT_PIN[i].GPIO_Pin;
    GPIO_Init(DIN_PORT_PIN[i].GPIOx, &GPIO_InitStructure);
    GPIO_ResetBits(DIN_PORT_PIN[i].GPIOx, DIN_PORT_PIN[i].GPIO_Pin);
  }
}


/**
 * @brief  Digit out low level init
 * @param  None
 * @return None
 */
static void DIO_OutLowLevelInit(void)
{
  uint8_t i = 0;

  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | \
                         RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  for (i = 0; i < DIO_CH_NUM_OUT; i++)
  {
    GPIO_InitStructure.GPIO_Pin = DOUT_PORT_PIN[i].GPIO_Pin;
    GPIO_Init(DOUT_PORT_PIN[i].GPIOx, &GPIO_InitStructure);
    GPIO_ResetBits(DOUT_PORT_PIN[i].GPIOx, DOUT_PORT_PIN[i].GPIO_Pin);
  }
}


/**
 * @brief  Digit in timer count low level init
 * @param  None
 * @return None
 */
static void DIO_InTimerLowLevelInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef       TIM_ICInitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  // TIM1 and TIM8 clock enable
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_TIM8, ENABLE);

  // GPIOA, GPIOC clock enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
  
  // PA8(TP12,TIM1-CH1), PC6(TP13,TIM8-CH1)
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  // Connect TIM1 pin to AF1
  // Connect TIM8 pin to AF1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);

  // Enable the TIM1 and TIM8 global Interrupt
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;
  NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = ( uint16_t ) ( SystemCoreClock / DIO_TIME_FREQ );
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;

  // TIM1-CH1
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_PWMIConfig(TIM1, &TIM_ICInitStructure);

  // TIM8-CH1
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_PWMIConfig(TIM8, &TIM_ICInitStructure);
  
  // Select the TIM Input Trigger
  TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
  TIM_SelectInputTrigger(TIM8, TIM_TS_TI1FP1);

  // Select the slave Mode: Reset Mode
  TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(TIM1,TIM_MasterSlaveMode_Enable);

  TIM_SelectSlaveMode(TIM8, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(TIM8,TIM_MasterSlaveMode_Enable);

  // TIM enable counter
  TIM_Cmd(TIM1, ENABLE);
  TIM_Cmd(TIM8, ENABLE);

  // Enable the CC Interrupt Request
  TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
  TIM_ITConfig(TIM8, TIM_IT_CC1, ENABLE);
}


/**
  * @brief  Handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM1_CC_IRQHandler(void)
{
  uint16_t cc_value = 0;

  // Clear TIM Capture compare interrupt pending bit
  TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);

  // Get the Input Capture value
  cc_value = TIM_GetCapture1(TIM1);

  if (cc_value != 0)
  {
    s_ulDInFreq[0] = DIO_TIME_FREQ / cc_value;
  }
  else
  {
    s_ulDInFreq[0] = 0;
  }
}


/**
  * @brief  Handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM8_CC_IRQHandler(void)
{
  uint16_t cc_value = 0;

  // Clear TIM Capture compare interrupt pending bit
  TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);

  // Get the Input Capture value
  cc_value = TIM_GetCapture1(TIM8);

  if (cc_value != 0)
  {
    s_ulDInFreq[1] = DIO_TIME_FREQ / cc_value;
  }
  else
  {
    s_ulDInFreq[1] = 0;
  }
}

/**
 * @}
 */


