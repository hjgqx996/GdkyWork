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
 * 应用程序模式定义 < 8
 */
enum
{
  APP_MODE_CONFIG         = 0,  /* 配置模式 */
  APP_MODE_READ_UM        = 1,  /* 读户表User    Meter */
  APP_MODE_SLEEP          = 6,  /* 睡眠模式 */
  APP_MODE_WAIT           = 7,  /* 等待时间超出采集间隔 */
};


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
  uint8_t  mode;                /* 模式 */
  uint8_t  powerOn;             /* 1已上电 */
  uint8_t  rdyAlloc;            /* 1准备分摊 */
  uint8_t  allocHour;           /* 采集周期小时 00-22 */
  uint8_t  allocMinute;         /* 采集周期分钟 00-59 */
  uint8_t  configTime;          /* 配置持续时间 S */
}AppRunObj_t;

/* MACROS  ------------------------------------------------------------------ */

#define LED_RUN  LED_1
#define LED_NET  LED_2

/* CONSTANTS  --------------------------------------------------------------- */


/* Task priorities. */
#define APP_TASK_PRIO_INIT        (tskIDLE_PRIORITY + 1)
#define APP_TASK_PRIO_COM         (tskIDLE_PRIORITY + 2)


/* Task Stack Size */
#define APP_TASK_STK_INIT         (configMINIMAL_STACK_SIZE * 4)  /* 2KB */
#define APP_TASK_STK_COM          (configMINIMAL_STACK_SIZE * 4)  /* 2KB */


/* GLOBAL VARIABLES --------------------------------------------------------- */

extern const uint8_t APP_DEV_ADDR[APP_DEV_ADDR_SIZE];
extern const uint8_t APP_SOFT_VER[APP_SOFT_VER_SIZE];

extern TaskHandle_t g_hTaskInit;
extern TaskHandle_t g_hTaskCom;


/* GLOBAL FUNCTIONS --------------------------------------------------------- */

void    AppDispInfo(void);
uint8_t AppIsHeatSeason(void);
void    AppModePoll(void);
uint8_t AppGetMode(void);


void vAppTaskInit(void *pvParameters);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __APP_H */

/***************************** END OF FILE ************************************/

