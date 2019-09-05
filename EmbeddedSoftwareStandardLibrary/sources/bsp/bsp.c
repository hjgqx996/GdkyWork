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


