/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 ZouZH  All Rights Reserved
  *
  * @file    app_cfg.h
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application configuration for nb-iot collector
  ******************************************************************************
  */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __APP_CFG_H
#define __APP_CFG_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

#ifndef APP_TASK_COM
  #define APP_TASK_COM           0
#endif /* APP_TASK_COM */


/* CONSTANTS  --------------------------------------------------------------- */

/**
 * device address and software version bytes
 */
#define APP_DEV_ADDR_SIZE         4
#define APP_SOFT_VER_SIZE         4

/**
 * define device address
 */
#define APP_DEV_ADDR_0            0x18  /* highest */
#define APP_DEV_ADDR_1            0x11
#define APP_DEV_ADDR_2            0x00
#define APP_DEV_ADDR_3            0x05  /* lowest */


/**
 * define software version
 */
#define APP_VER_TYPE              0x01  /* product type */
#define APP_VER_FUNC              0x01  /* product function */
#define APP_VER_DEV               0x01
#define APP_VER_CODE              0x01  /* software update */

#define APP_VER_DISP              0x00


/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __APP_CFG_H */

/***************************** END OF FILE ************************************/


