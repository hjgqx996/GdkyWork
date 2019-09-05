/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 ZouZH  All Rights Reserved
  *
  * @file     pwrmgr.h
  * @author   ZouZH
  * @version  V1.00
  * @date     13-Aug-2018
  * @brief    Power Management.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __PWRMGR_H
#define __PWRMGR_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */


/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

void     PMInit(void);
void     PMWaitWakeupApp(void);
void     PMGiveWakeupApp(void);
void     PMWaitWakeupCom(void);
void     PMGiveWakeupCom(void);
uint8_t  PMIsBatLowVol(void);


/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __PWRMGR_H */

/***************************** END OF FILE *************************************/

