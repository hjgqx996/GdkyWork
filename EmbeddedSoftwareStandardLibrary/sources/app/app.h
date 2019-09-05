/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY  All Rights Reserved
  *
  * @file    app.h
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application for user
  ******************************************************************************
  */


/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>
#include "app_cfg.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */


/* CONSTANTS  --------------------------------------------------------------- */


/* Task priorities. */
#define APP_TASK_PRIO_INIT        (tskIDLE_PRIORITY + 2)


/* Task Stack Size */
#define APP_TASK_STK_INIT         (configMINIMAL_STACK_SIZE * 4)  /* 2KB */


/* GLOBAL VARIABLES --------------------------------------------------------- */

extern const uint8_t APP_DEV_ADDR[APP_DEV_ADDR_SIZE];
extern const uint8_t APP_SOFT_VER[APP_SOFT_VER_SIZE];

extern TaskHandle_t g_hTaskInit;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

void vAppTaskInit(void *pvParameters);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __APP_H */

/***************************** END OF FILE ************************************/

