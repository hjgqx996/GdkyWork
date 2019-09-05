/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY  All Rights Reserved
  *
  * @file     record_hw.h
  * @author   ZouZH
  * @version  V1.03
  * @date     26-May-2016
  * @brief    数据存储底层硬件驱动.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __RECORD_HW_H
#define __RECORD_HW_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include "record_cfg.h"
#include "record_tbl.h"

/** 
 * @addtogroup Record
 * @{
 */

/** 
 * @addtogroup RecordHardware
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

RecordErr_t Record_HWInit(const RecordTBL_t *ptbl);
RecordErr_t Record_HWRead(const RecordTBL_t *ptbl, uint32_t addr, void *pvbuf, uint32_t len);
RecordErr_t Record_HWWrite(const RecordTBL_t *ptbl, uint32_t addr, const void *pvbuf, uint32_t len);
RecordErr_t Record_HWErase(const RecordTBL_t *ptbl, uint32_t addr, uint32_t len);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RECORD_HW_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

