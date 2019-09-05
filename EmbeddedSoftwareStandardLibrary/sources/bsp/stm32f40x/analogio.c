/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     analogio.c
  * @author   ZouZH
  * @version  V1.00
  * @date     03-Nov-2018
  * @brief    Analog input and output.
  ******************************************************************************
  * @history
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "analogio.h"

#include "stm32f4xx.h"

/** 
 * @defgroup AnalogIO
 * @brief Analog input and output
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * @brief Analog Port and Pin define
 */
typedef struct
{
  GPIO_TypeDef* GPIOx;
  uint16_t      GPIO_Pin;
}AIO_PortPin_t;

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

// AD0(TP6),  1(TP4),  2(TP7),  3
// AD7(TP11), 10,      11,      12(TP9)
// AD13(TP5), 14(TP10),15(TP8)
static const AIO_PortPin_t AIN_PORT_PIN[AIO_CH_NUM_IN] = {
  {GPIOA, GPIO_Pin_0}, {GPIOA, GPIO_Pin_1}, {GPIOA, GPIO_Pin_2}, {GPIOA, GPIO_Pin_3},
  {GPIOA, GPIO_Pin_7}, {GPIOC, GPIO_Pin_0}, {GPIOC, GPIO_Pin_1}, {GPIOC, GPIO_Pin_2},
  {GPIOC, GPIO_Pin_3}, {GPIOC, GPIO_Pin_4}, {GPIOC, GPIO_Pin_5}, {GPIOC, GPIO_Pin_5},
  {GPIOC, GPIO_Pin_5}
};

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* ADC sample DMA buffer */
#define AIN_BUFF_SIZE 16
static volatile uint16_t s_usAinBuffer[AIN_BUFF_SIZE][AIO_CH_NUM_IN];

/* LOCAL FUNCTIONS ---------------------------------------------------------- */
static void AIO_InLowLevelInit(void);


/**
 * @brief  Init
 * @param  None
 * @return Init status
 * @retval AIO_OK Init success
 */
AIO_ERROR_t AIO_Init(void)
{
  AIO_InLowLevelInit();
  
  return AIO_OK;
}


/**
 * @brief  Get analog in voltage valve
 *
 * @param[in]  AIN_TYPE_INx (AIO_TYPE_IN_MIN < x < AIO_TYPE_IN_MAX)
 *
 * @retval mv sampled signal, max 3000mV
 */
uint16_t AIO_GetInVoltage(AIO_TYPE_t AIN_TYPE_INx)
{
  uint32_t adTotal = 0;
  uint16_t adValue = 0;
  uint16_t adMax = 0;
  uint16_t adMin = 0xFFFF;

  uint8_t i = 0;
  uint8_t ofs = 0;

  if ((AIN_TYPE_INx <= AIO_TYPE_IN_MIN) || (AIN_TYPE_INx >= AIO_TYPE_IN_MAX) || (AIN_BUFF_SIZE <= 2))
  {
    return 0;
  }

  ofs = AIN_TYPE_INx - AIO_TYPE_IN1;

  for (i = 0; i < AIN_BUFF_SIZE; i++)
  {
    adValue = s_usAinBuffer[i][ofs];
    if (adValue > adMax)
    {
      adMax = adValue;
    }
    
    if (adValue < adMin)
    {
      adMin = adValue;
    }

    adTotal += adValue;
  }

  adTotal = (adTotal - adMax - adMin) / (AIN_BUFF_SIZE - 2);
  return (adTotal * 3000ul) / 0xFFF;
}


/**
 * @brief  ADC In low level init
 * @param  None
 * @return None
 */
static void AIO_InLowLevelInit(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  uint8_t               i = 0;

  /* Enable ADCx, DMA and GPIO clocks ****************************************/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);  

  /* DMA2 Stream0 channel0 configuration **************************************/
  DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&s_usAinBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = AIN_BUFF_SIZE * AIO_CH_NUM_IN;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /* Configure ADC1 Channel pin as analog input ******************************/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  for (i = 0; i < AIO_CH_NUM_IN; i++)
  {
    GPIO_InitStructure.GPIO_Pin = AIN_PORT_PIN[i].GPIO_Pin;
    GPIO_Init(AIN_PORT_PIN[i].GPIOx, &GPIO_InitStructure);
  }

  /* ADC Common Init **********************************************************/
  ADC_CommonStructInit(&ADC_CommonInitStructure);
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC Init ****************************************************************/
  ADC_DeInit();
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = AIO_CH_NUM_IN;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC regular channel configuration **************************************/
  // AD0,  1,  2,  3
  // AD7,  10, 11, 12
  // AD13, 14, 15
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0,  1, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  2, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2,  3, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3,  4, ADC_SampleTime_480Cycles);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_7,  5, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 6, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 7, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 8, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 9, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 10, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 11, ADC_SampleTime_480Cycles);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 12, ADC_SampleTime_480Cycles);
  ADC_TempSensorVrefintCmd(ENABLE);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_Vbat, 13, ADC_SampleTime_480Cycles);
  ADC_VBATCmd(ENABLE);

  /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  /* Enable ADC DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC */
  ADC_Cmd(ADC1, ENABLE);

  /* Start ADC Software Conversion */ 
  ADC_SoftwareStartConv(ADC1);
}

/**
 * @}
 */


