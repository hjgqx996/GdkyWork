/**
  ******************************************************************************
  *               Copyright(C) 2015-2025 GDKY  All Rights Reserved
  *
  * @file     record_tbl.h
  * @author   ZouZH
  * @version  V1.01
  * @date     07-Dec-2015
  * @brief    存储分配表,包含数据类型，设备类型，地址范围等定义.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __RECORD_TBL_H
#define __RECORD_TBL_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include "record_cfg.h"

/** 
 * @addtogroup Record
 * @{
 */

/** 
 * @addtogroup RecordTable
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * Record Table
 */
typedef struct
{
  RecordDataType_t  dataType;
  RecordDataItem_t  dataItem;
  RecordDevType_t   devType;
  uint8_t           devAddr;
  uint32_t          dataStartAddr;
  uint32_t          dataItemSize;
  uint32_t          dataTotalSize;
  uint32_t          devTotalSize;

}RecordTBL_t;

/* MACROS  ------------------------------------------------------------------ */

/* 定义各个数据类型占用空间总大小 */

/* 系统参数 */
#define REC_Size_SysParam         (2 * sizeof(SysParam_t))


/* 定义各个数据类型存储起始地址 */

/* FM24W256_2 U3=001 */
#define REC_Addr_SysParam         0x00000000

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern const RecordTBL_t RecordTBL[];

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RECORD_TBL_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

