/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY All Rights Reserved
  *
  * @file    bsp.h
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   board support packet.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include "bsp_cfg.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

extern void BspInitHard(void);

extern void BspStartIWDG(void);
extern void BspReStartIWDG(void);

extern void BspEnterStopMode(void);
extern void BspExitStopMode(void);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __BSP_H */

/***************************** END OF FILE ************************************/

