﻿/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY  All Rights Reserved
  *
  * @file    app_cfg.h
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application configuration for user
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

/* CONSTANTS  --------------------------------------------------------------- */

/**
 * device address and software version bytes
 */
#define APP_DEV_ADDR_SIZE         4
#define APP_SOFT_VER_SIZE         4

/**
 * define device address
 */
#define APP_DEV_ADDR_0            0x16  /* highest */
#define APP_DEV_ADDR_1            0x00
#define APP_DEV_ADDR_2            0x00
#define APP_DEV_ADDR_3            0x01  /* lowest */


/**
 * define software version
 */
#define APP_VER_TYPE              0x01  /* product type */
#define APP_VER_FUNC              0x01  /* product function */
#define APP_VER_DEV               0x01
#define APP_VER_CODE              0x03  /* software update */


/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __APP_CFG_H */

/***************************** END OF FILE ************************************/


