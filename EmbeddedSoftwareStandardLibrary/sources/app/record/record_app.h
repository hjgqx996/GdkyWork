/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     record_app.h
  * @author   ZouZH
  * @version  V1.00
  * @date     16-June-2016
  * @brief    数据存储应用程序
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __RECORD_APP_H
#define __RECORD_APP_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>
#include "record.h"

/** 
 * @addtogroup Record
 * @{
 */

/** 
 * @addtogroup RecordApp
 * @{
 */

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
RecordErr_t RecordApp_Init(void);
RecordErr_t RecordApp_SaveSysParam(void);
RecordErr_t RecordApp_ReadSysParam(void);


/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RECORD_APP_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE *************************************/

