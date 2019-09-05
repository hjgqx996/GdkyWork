/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY  All Rights Reserved
  *
  * @file    app.c
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application for user
  ******************************************************************************
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "includes.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/**
 * device address and software version
 */
VAR_AT(0x8008000) const uint8_t APP_DEV_ADDR[APP_DEV_ADDR_SIZE] =
                  {APP_DEV_ADDR_0, APP_DEV_ADDR_1, APP_DEV_ADDR_2, APP_DEV_ADDR_3};
VAR_AT(0x8008004) const uint8_t APP_SOFT_VER[APP_SOFT_VER_SIZE] =
                  {APP_VER_TYPE, APP_VER_FUNC, APP_VER_DEV, APP_VER_CODE};

/* GLOBAL VARIABLES --------------------------------------------------------- */


/**
 * task handle
 */
TaskHandle_t g_hTaskInit;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

/**
 * @brief  hardware initialise
 *
 * @param  none
 *
 * @retval none
 */
static void AppInitHard(void)
{
  // Power on delay
  SYSDelayMs(100);

  BspInitHard();
}

/**
 * @brief Init task function
 *
 * @param[in] task parameter pointer
 *
 * @retval none
 */
void vAppTaskInit(void *pvParameters)
{

  (void)pvParameters;

  AppInitHard();

  while (1)
  {
  }
}
