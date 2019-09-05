/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 ZouZH  All Rights Reserved
  *
  * @file    app.h
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application for nb-iot collector
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

/**
 * 应用程序错误 < 8
 */
enum
{
  APP_ERR_Record          = 0,  /* 存储错误 */
  APP_ERR_RAM             = 1,  /* 内存错误 */
  APP_ERR_CLOCK           = 2,  /* 时钟错误 */
  APP_ERR_BATLOW          = 3,  /* 电池低电 */
  APP_ERR_NB              = 4,  /* NB-IoT */
};

/**
 * 应用运行程序对象
 */
typedef struct
{
  uint8_t  error;               /* 错误 */
}AppRunObj_t;

/* MACROS  ------------------------------------------------------------------ */

#define LED_RUN  LED_1
#define LED_NET  LED_2

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern const uint8_t APP_DEV_ADDR[APP_DEV_ADDR_SIZE];
extern const uint8_t APP_SOFT_VER[APP_SOFT_VER_SIZE];


/* GLOBAL FUNCTIONS --------------------------------------------------------- */

void AppDispInfo(void);

void vAppTaskInit(void *pvParameters);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __APP_H */

/***************************** END OF FILE ************************************/

