/**
  ******************************************************************************
  *               Copyright(C) 2019-2029 GDKY All Rights Reserved
  *
  * @file     system_msp430x44x.h
  * @author   ZouZH
  * @version  V1.00
  * @date     21-March-2019
  * @brief    System clock configuration.
  ******************************************************************************
  * @history
  */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>
#include <io430.h>

/**
 * @defgroup msp430x44x_system
 * @brief 系统时钟配置
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* 外部低频晶振 */
#define LSE_ACLK ((uint16_t)(32768))

/* GLOBAL VARIABLES --------------------------------------------------------- */

uint32_t SystemCoreClock = 7995392;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */
static void SetSysClockTo8(void);
static void SetSysClockToExt4(void);

/**
 * @brief  Setup the microcontroller system.
 * @param  None
 * @retval None
 */
void SystemInit (void)
{
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;

  if(1) {
    SetSysClockTo8();
  }
  else {
    SetSysClockToExt4();
  }
}


/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 * @param  None
 * @retval None
 */
void SystemCoreClockUpdate(void)
{
  uint8_t freq_mul = (SCFQCTL & 0x7F) + 1;
  uint8_t flldx = ((SCFI0 & 0xC0) >> 6) + 1;

  __disable_interrupt();
  {
    SystemCoreClock = LSE_ACLK * freq_mul;
    if (FLL_CTL0 & DCOPLUS)
    {
      SystemCoreClock *= flldx;
    }
  }
  __enable_interrupt();
}


/**
 * @brief  Sets System clock frequency to 8MHz.
 * @param  None
 * @retval None
 */
static void SetSysClockTo8(void)
{
  /**FLL+, Runs Internal DCO at 8MHz
   * Setting the internal DCO to run at 8MHz with auto-calibration by the FLL+.
   * ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = DCO = (121+1) x 2 x ACLK = 7995392Hz
   * An external watch crystal between XIN & XOUT is required for ACLK
   *           -----------------
   *       /|\|              XIN|-
   *        | |                 | 32kHz
   *        --|RST          XOUT|-
   *          |                 |
   *          |             P1.1|--> MCLK = 8MHz
   *          |                 |
   *          |             P1.5|--> ACLK = 32kHz
   *          |                 |
   */


  /**FLL_CTL0 003h
   * DCOPLUS: [0]DCO output is divided fDCOCLK = (N+1)xfcrystal [1]DCO output is not divided fDCOCLK = FLLDx(N+1)xfcrystal
   * DCO output pre-divider. This bit selects if the DCO output is pre-divided
   * before sourcing MCLK or SMCLK. The division rate is selected with the FLL_D bits
   *
   * XTS_FLL: [0]Low frequency mode [1]High frequency mode or external clock
   *
   * XCAPxPF: Oscillator capacitor selection. These bits select the effective capacitance
   * seen by the LFXT1 crystal or resonator. Should be set to 00 if the
   * high-frequency mode is selected for LFXT1 with XTS_FLL = 1.
   *
   * XT2OF: XT2 oscillator fault [0] No fault condition present [1] Fault condition present
   * XT1OF: LFXT1 high-frequency oscillator fault
   * LFOF: LFXT1 low-frequency oscillator fault
   * DCOF: DCO oscillator fault
   */
  FLL_CTL0 |=  DCOPLUS + XCAP10PF;
  //FLL_CTL0 &= ~(XT2OF + XT1OF + LFOF + DCOF);

  /**FLL_CTL1 00h
   * LFXT1DIG
   * SMCLKOFF: [0]SMCLK is on [1]SMCLK is off
   * XT2OFF: [0]XT2 is on [1]XT2 is off if it is not used for MCLK or SMCLK
   * SELMx[4-3]: These bits select the MCLK source. [00 01]DCOCLK [10]XT2CLK [11]LFXT1CLK
   * SELS: This bit selects the SMCLK source [0]DCOCLK [1]XT2CLK
   * FLL_DIVx[1-0]: ACLK divider [00] ACLK/1 [01]ACLK/2 [10]ACLK/4 [11]ACLK/8
   */
  //FLL_CTL1 &= ~SMCLKOFF;

  /**SCFI0 040h
   * FLLDx[7-6]: FLL+ loop divider. These bits divide fDCOCLK in the FLL+ feedback loop.
   * This results in an additional multiplier for the multiplier bits. See also
   * multiplier bits. [00]/1 [01]/2 [10]/4 [11]/8
   * FN_x[5-2]: DCO range control.These bits select the fDCO operating range
   * [0000]0.65 to 6.1 MHz
   * FN2[0001]1.3 to 12.1 MHz
   * FN3[001x]2 to 17.9 MHz
   * FN4[01xx]2.8 to 26.6 MHz
   * FN8[1xxx]4.2 to 46 MHz
   */
  SCFI0 |= FN_4;
  SCFQCTL = 121;          // 2 x (121+1) x 32768 = 7.99 MHz
  //P1DIR |= BIT1 + BIT5;   // P1.1 & P1.5 to output direction
  //P1SEL |= BIT1 + BIT5;   // P1.1 & P1.5 to output MCLK & ACLK

  /**
   *OFIE: Oscillator fault interrupt enable [0]Interrupt not enabled
   */
  IE1 &= ~OFIE;
}


/**
 * @brief  Sets System clock frequency to External clock 4MHz.
 * @param  None
 * @retval None
 */
static void SetSysClockToExt4(void)
{
  /**
   *           -----------------
   *       /|\|              XIN|- 4Mhz
   *        | |                 |
   *        --|RST          XOUT|-
   *          |                 |
   *          |                 |
   */


  /**FLL_CTL0 003h
   * DCOPLUS: [0]DCO output is divided fDCOCLK = (N+1)xfcrystal [1]DCO output is not divided fDCOCLK = FLLDx(N+1)xfcrystal
   * DCO output pre-divider. This bit selects if the DCO output is pre-divided
   * before sourcing MCLK or SMCLK. The division rate is selected with the FLL_D bits
   *
   * XTS_FLL: [0]Low frequency mode [1]High frequency mode or external clock
   *
   * XCAPxPF: Oscillator capacitor selection. These bits select the effective capacitance
   * seen by the LFXT1 crystal or resonator. Should be set to 00 if the
   * high-frequency mode is selected for LFXT1 with XTS_FLL = 1.
   *
   * XT2OF: XT2 oscillator fault [0] No fault condition present [1] Fault condition present
   * XT1OF: LFXT1 high-frequency oscillator fault
   * LFOF: LFXT1 low-frequency oscillator fault
   * DCOF: DCO oscillator fault
   */
  FLL_CTL0 |=  XTS_FLL + XCAP10PF;
  //FLL_CTL0 &= ~(XT2OF + XT1OF + LFOF + DCOF);

  /**FLL_CTL1 00h
   * LFXT1DIG
   * SMCLKOFF: [0]SMCLK is on [1]SMCLK is off
   * XT2OFF: [0]XT2 is on [1]XT2 is off if it is not used for MCLK or SMCLK
   * SELMx[4-3]: These bits select the MCLK source. [00 01]DCOCLK [10]XT2CLK [11]LFXT1CLK
   * SELS: This bit selects the SMCLK source [0]DCOCLK [1]XT2CLK
   * FLL_DIVx[1-0]: ACLK divider [00] ACLK/1 [01]ACLK/2 [10]ACLK/4 [11]ACLK/8
   */
  //FLL_CTL1 &= ~SMCLKOFF;

  /**SCFI0 040h
   * FLLDx[7-6]: FLL+ loop divider. These bits divide fDCOCLK in the FLL+ feedback loop.
   * This results in an additional multiplier for the multiplier bits. See also
   * multiplier bits. [00]/1 [01]/2 [10]/4 [11]/8
   * FN_x[5-2]: DCO range control.These bits select the fDCO operating range
   * [0000]0.65 to 6.1 MHz
   * FN2[0001]1.3 to 12.1 MHz
   * FN3[001x]2 to 17.9 MHz
   * FN4[01xx]2.8 to 26.6 MHz
   * FN8[1xxx]4.2 to 46 MHz
   */
  SCFI0 |= FN_2;
  SCFQCTL = SCFQ_4M;        // (127+1) x 32768 = 4194304Hz
  //P1DIR |= BIT1 + BIT5;   // P1.1 & P1.5 to output direction
  //P1SEL |= BIT1 + BIT5;   // P1.1 & P1.5 to output MCLK & ACLK

  /**
   *OFIE: Oscillator fault interrupt enable [0]Interrupt not enabled
   */
  IE1 &= ~OFIE;
}


/**
 * @}
 */

