/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY  All Rights Reserved
  *
  * @file    bsp_cfg.h
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   This file contains the interface to the bsp config.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __BSP_CFG_H
#define __BSP_CFG_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */


/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/*
 * Drive Configuration
 */

/* Set to TRUE enable LED usage, FALSE disable it */
#ifndef BSP_LED
  #define BSP_LED TRUE
#endif /* BSP_LED */
#if (!defined LED_BLINK) && (BSP_LED == TRUE)
  //#define LED_BLINK
#endif /* LED_BLINK */

/* Set to TRUE enable KEY usage, FALSE disable it */
#ifndef BSP_KEY
  #define BSP_KEY TRUE
#endif /* BSP_KEY */

/* Set to TRUE enable RTC usage, FALSE disable it */
#ifndef BSP_RTC
  #define BSP_RTC TRUE
#endif /* BSP_RTC */

/* Set to TRUE enable USART1 usage, FALSE disable it */
#ifndef BSP_USART1
  #define BSP_USART1 FALSE
#endif /* BSP_USART1 */


/* Set to TRUE enable SDIO usage, FALSE disable it */
#ifndef BSP_SDIO
  #define BSP_SDIO FALSE
#endif /* BSP_SDIO */



/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __BSP_CFG_H */

/***************************** END OF FILE ************************************/

