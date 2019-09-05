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

#include "io430.h"

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
  volatile uint8_t *PxDIR;
  volatile uint8_t *PxSEL;
  uint8_t GPIO_Pin;
}AIO_PortPin_t;

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

// AD0(TP6)
static const AIO_PortPin_t AIN_PORT_PIN[AIO_CH_NUM_IN] = {
  {&P6DIR, &P6SEL,  BIT2},
};

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* ADC sample DMA buffer */
#define AIN_BUFF_SIZE 10
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
 * @brief  ADC sample ON or OFF
 * @param  status: 0=OFF, 1=ON
 * @return None
 */
void AIO_InOnOff(uint8_t status)
{
  uint8_t i;

  for (i = 0; i < AIO_CH_NUM_IN; i++)
  {
    s_usAinBuffer[AIN_BUFF_SIZE - 1][i] &= 0x0FFF;
  }

  if (status)
  {
    ADC12CTL0 |= ADC12ON + ENC + ADC12SC;
  }
  else
  {
    ADC12CTL0 &= ~(ENC);
    ADC12CTL0 &= ~(ADC12ON + ADC12SC);
  }
}


/**
 * @brief  Get analog in voltage valve
 *
 * @param[in]  AIN_TYPE_INx (AIO_TYPE_IN_MIN < x < AIO_TYPE_IN_MAX)
 *
 * @retval mv sampled signal, max 3300mV
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
  ////
  AIO_InOnOff(1);//start AD sample
  LPM3;
  ////
  // Data is valid?
////  if (0 == (s_usAinBuffer[AIN_BUFF_SIZE - 1][ofs] & 0x8000))
////  {
////    return 0;
////  }

  s_usAinBuffer[AIN_BUFF_SIZE - 1][ofs] &= 0x0FFF;

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

  adTotal = ((adTotal - adMax - adMin)>>2);//// / (AIN_BUFF_SIZE - 2);//原理图分压，所以*2.
  return (adTotal * 3300ul) / 0xFFF;
}


/**
 * @brief  ADC In low level init
 * @param  None
 * @return None
 */
static void AIO_InLowLevelInit(void)
{
  uint8_t i;

  /* Configure ADC Channel pin as analog input ******************************/
  for (i = 0; i < AIO_CH_NUM_IN; i++)
  {
    *AIN_PORT_PIN[i].PxDIR &= ~AIN_PORT_PIN[i].GPIO_Pin;
    *AIN_PORT_PIN[i].PxSEL |= AIN_PORT_PIN[i].GPIO_Pin;
  }


  /* ADC Init ****************************************************************/

  ADC12CTL0 = 0;
  ADC12CTL0 = SHT0_15 + SHT1_15 + MSC;

  // ACLK
  ADC12CTL1 = CSTARTADD_0 + SHS_0 + ADC12DIV_0 + ADC12SSEL_1 + SHP + CONSEQ_1;

  ADC12MCTL0 = INCH_2 + SREF_0;//VR+ = AVCC and VR− = AVSS//Vref=3.3v=VR+ - VR−
  ADC12MCTL1 = INCH_2 + SREF_0;
  ADC12MCTL2 = INCH_2 + SREF_0;
  ADC12MCTL3 = INCH_2 + SREF_0;
  ADC12MCTL4 = INCH_2 + SREF_0;
  ADC12MCTL5 = INCH_2 + SREF_0;
  ADC12MCTL6 = INCH_2 + SREF_0;
  ADC12MCTL7 = INCH_2 + SREF_0;
  ADC12MCTL8 = INCH_2 + SREF_0;
  ADC12MCTL9 = INCH_2 + SREF_0 + EOS;

  ADC12IFG = 0;
  ADC12IE  = 0x0200;//enable ADC12IE9.

  /* ADC regular channel configuration **************************************/
}


/**
 * @brief  ADC interrupt handler
 * @param  None
 * @return None
 */

#pragma vector = ADC_VECTOR
__interrupt void ADC12_IRQHandler(void)
{
  if (ADC12IV == ADC12IV_ADC12IFG9)
  {
    s_usAinBuffer[0][0] = ADC12MEM0;
    s_usAinBuffer[1][0] = ADC12MEM1;
    s_usAinBuffer[2][0] = ADC12MEM2;
    s_usAinBuffer[3][0] = ADC12MEM3;
    s_usAinBuffer[4][0] = ADC12MEM4;
    s_usAinBuffer[5][0] = ADC12MEM5;
    s_usAinBuffer[6][0] = ADC12MEM6;
    s_usAinBuffer[7][0] = ADC12MEM7;
    s_usAinBuffer[8][0] = ADC12MEM8;
    s_usAinBuffer[9][0] = ADC12MEM9;

    // Set sample end flag
    s_usAinBuffer[9][0] |= 0x8000;
  }
  else 
  {
    ADC12IFG = 0;////
  }
  LPM3_EXIT;
}

/**
 * @}
 */


