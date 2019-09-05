/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY  All Rights Reserved
  *
  * @file     record.h
  * @author   ZouZH
  * @version  V1.03
  * @date     17-March-2016
  * @brief    线性数据存储应用层接口.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __RECORD_H
#define __RECORD_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include "record_cfg.h"

/** 
 * @addtogroup Record
 * @{
 */ 

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern SysParam_t g_tSysParam;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/**
 * 系统应用接口
 */
RecordErr_t Record_Init(void);
RecordErr_t Record_Read(RecordDataType_t REC_DT_x, RecordDataItem_t REC_DI_x, uint16_t itemIdx, void *pvBuf);
RecordErr_t Record_Write(RecordDataType_t REC_DT_x, RecordDataItem_t REC_DI_x, uint16_t itemIdx, void *pvBuf);
RecordErr_t Record_Erase(RecordDataType_t REC_DT_x, RecordDataItem_t REC_DI_x);

/**
 * 用户接口函数
 */
void Record_SysParamDeInit(void);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RECORD_H */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

