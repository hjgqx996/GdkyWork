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
#include "io430.h"
#include "systick.h"

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
    uint8_t IN1  : 1;
    uint8_t IN2  : 1;
    uint8_t IN3  : 1;
    uint8_t IN4  : 1;
    uint8_t IN5  : 1;
    uint8_t IN6  : 1;
    uint8_t IN7  : 1;
    uint8_t IN8  : 1;
  }inbit;

  struct
  {
    uint8_t OUT1 : 1;
    uint8_t OUT2 : 1;
    uint8_t OUT3 : 1;
    uint8_t OUT4 : 1;
    uint8_t OUT5 : 1;
    uint8_t OUT6 : 1;
    uint8_t OUT7 : 1;
    uint8_t OUT8 : 1;
  }outbit;

  uint8_t value;
}DIO_Status_t;


/**
 * @brief Digit Port and Pin define
 */
typedef struct
{
  volatile uint8_t *PxDIR;
  volatile uint8_t *PxSEL;
  const volatile uint8_t *GPIOx;
  uint8_t GPIO_Pin;
}DIN_PortPin_t;

typedef struct
{
  volatile uint8_t *PxDIR;
  volatile uint8_t *PxSEL;
  volatile uint8_t *GPIOx;
  uint8_t GPIO_Pin;
}DOUT_PortPin_t;



/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

// P1.0,P1.1(JTAG), P2.0(LSO1, 0-On)
// P2.7(K1),P2.1(K2),P2.2(K3) 0-Down
// P2.3(DYD, 1-Low Power)
static const DIN_PortPin_t DIN_PORT_PIN[DIO_CH_NUM_IN] = {
  {&P1DIR, &P1SEL, &P1IN, BIT0}, {&P1DIR, &P1SEL, &P1IN, BIT1}, {&P2DIR, &P2SEL, &P2IN, BIT0},
  {&P2DIR, &P2SEL, &P2IN, BIT7}, {&P2DIR, &P2SEL, &P2IN, BIT1}, {&P2DIR, &P2SEL, &P2IN, BIT2},
  {&P2DIR, &P2SEL, &P2IN, BIT3},
};

// P6.3(GPRSVDD_ON, 1-On),P6.4(CONTROL),P6.5(GPRS_ON)
// P6.6(GPRS_RST, 1-On)
static const DOUT_PortPin_t DOUT_PORT_PIN[DIO_CH_NUM_OUT] = {
  {&P6DIR, &P6SEL, &P6OUT, BIT3}, {&P6DIR, &P6SEL, &P6OUT, BIT4}, {&P6DIR, &P6SEL, &P6OUT, BIT5},
  {&P6DIR, &P6SEL, &P6OUT, BIT6},
};

uint32_t m_PIN_Jitter[DIO_CH_NUM_IN] = {0};//输入引脚消抖时间

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* Digit in and out status variable */
DIO_Status_t s_tDInStatus;
DIO_Status_t s_tDOutStatus;


/* LOCAL FUNCTIONS ---------------------------------------------------------- */
static void DIO_InLowLevelInit(void);
static void DIO_OutLowLevelInit(void);


/**
 * @brief  Init
 * @param  None
 * @return Init status(DIO_OK Init success)
 */
DIO_ERROR_t DIO_Init(void)
{
  DIO_InLowLevelInit();
  DIO_OutLowLevelInit();
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
  uint8_t u8Status  = 0;
  uint8_t u8PreStatus  = 0;

  for (ofs = 0; ofs < DIO_CH_NUM_IN; ofs++)
  {
    u8Status = (0!=(*DIN_PORT_PIN[ofs].GPIOx & DIN_PORT_PIN[ofs].GPIO_Pin));
    u8PreStatus = ((s_tDInStatus.value>>ofs) & 0x01);
    if(u8Status != u8PreStatus) {
			if (SysTickGetMsDif(m_PIN_Jitter[ofs], SysTickGetMs() ) >= 20 )// 消抖时间 20ms
			{	
        s_tDInStatus.value &= ~(uint32_t)(1 << ofs);
        s_tDInStatus.value |= (uint32_t)(u8Status << ofs);// 更新IO口状态
				m_PIN_Jitter[ofs] = SysTickGetMs();
			}
    }
    else {
      m_PIN_Jitter[ofs] = SysTickGetMs();
    }
  }

  return DIO_OK;
}


/**
 * @brief  Get in pin status
 * @param[in]  DIO_TYPE_INx (DIO_TYPE_IN_MIN < x < DIO_TYPE_IN_MAX)
 * @return In pin status(0 or 1)
 */
uint8_t DIO_GetInStatus(DIO_TYPE_t DIO_TYPE_INx)
{
  uint8_t ofs = 0;

  if ((DIO_TYPE_INx <= DIO_TYPE_IN_MIN) || (DIO_TYPE_INx >= DIO_TYPE_IN_MAX))
  {
    return 0;
  }

  ofs = DIO_TYPE_INx - DIO_IN_P10;
  if(((s_tDInStatus.value >> ofs)&0x01) == 1) {
    return 1;
  }
  else {
    return 0;
  }
}


/**
 * @brief  Get out pin status
 * @param[in]  DIO_TYPE_OUTx (DIO_TYPE_OUT_MIN < x < DIO_TYPE_OUT_MAX)
 * @return Out pin status(0 or 1)
 */
uint8_t DIO_GetOutStatus(DIO_TYPE_t DIO_TYPE_OUTx)
{
  uint8_t ofs = 0;

  if ((DIO_TYPE_OUTx <= DIO_TYPE_OUT_MIN) || (DIO_TYPE_OUTx >= DIO_TYPE_OUT_MAX))
  {
    return 0;
  }

  ofs = DIO_TYPE_OUTx - DIO_OUT_GPRS_VDD;
  if(((s_tDOutStatus.value >> ofs)&0x01) == 1) {
    return 1;
  }
  else {
    return 0;
  }
}


/**
 * @brief  Set out pin status
 * @param[in]  DIO_TYPE_OUTx (DIO_TYPE_OUT_MIN < x < DIO_TYPE_OUT_MAX)
 * @param[in]  newStatus (0 or 1)
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
        *DOUT_PORT_PIN[ofs].GPIOx |= DOUT_PORT_PIN[ofs].GPIO_Pin;
      }
    }
    else
    {
      for (ofs = 0; ofs < DIO_CH_NUM_OUT; ofs++)
      {
        s_tDOutStatus.value &= ~(uint32_t)(1 << ofs);
        *DOUT_PORT_PIN[ofs].GPIOx &= ~DOUT_PORT_PIN[ofs].GPIO_Pin ;
      }
    }

    return DIO_OK;
  }

  ofs = DIO_TYPE_OUTx - DIO_OUT_GPRS_VDD;

  if (newStatus)
  {
    s_tDOutStatus.value |= (uint32_t)(1 << ofs);
    *DOUT_PORT_PIN[ofs].GPIOx |= DOUT_PORT_PIN[ofs].GPIO_Pin;
  }
  else
  {
    s_tDOutStatus.value &= ~(uint32_t)(1 << ofs);
    *DOUT_PORT_PIN[ofs].GPIOx &= ~DOUT_PORT_PIN[ofs].GPIO_Pin ;
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

  for (i = 0; i < DIO_CH_NUM_IN; i++)
  {
    *DIN_PORT_PIN[i].PxDIR &= ~DIN_PORT_PIN[i].GPIO_Pin;
    *DIN_PORT_PIN[i].PxSEL &= ~DIN_PORT_PIN[i].GPIO_Pin;
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

  for (i = 0; i < DIO_CH_NUM_OUT; i++)
  {
    *DOUT_PORT_PIN[i].PxDIR |= DOUT_PORT_PIN[i].GPIO_Pin;
    *DOUT_PORT_PIN[i].PxSEL &= ~DOUT_PORT_PIN[i].GPIO_Pin;

    *DOUT_PORT_PIN[i].GPIOx &= ~DOUT_PORT_PIN[i].GPIO_Pin;
  }
}

/**
 * @brief  Get status of regist's bit
 * @param[in]  regAddr: address of register
 * @param[in]  bit    : bit position
 * @return bit status
 */
uint8_t DIO_GetRegBit(const volatile uint8_t * regAddr, uint8_t bit) {
  if(0 == (HWREG8(regAddr) & bit)) {
    return 0;
  }
  else {
    return 1;
  }
}

/**
 * @brief  Set status of regist's bit
 * @param[in]  regAddr: address of register
 * @param[in]  bit    : bit position
 * @return None
 */
void DIO_SetRegBit(volatile uint8_t * regAddr, uint8_t bit) { 
  HWREG8(regAddr) |= bit;
}

/**
 * @brief  clear status of regist's bit
 * @param[in]  regAddr: address of register
 * @param[in]  bit    : bit position
 * @return None
 */
void DIO_ClrRegBit(volatile uint8_t * regAddr, uint8_t bit) {
  HWREG8(regAddr) &= ~bit;
}


/**
 * @}
 */


