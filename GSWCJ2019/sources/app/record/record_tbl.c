/**
  ******************************************************************************
  *               Copyright(C) 2015-2025 GDKY  All Rights Reserved
  *
  * @file     record_tbl.c
  * @author   ZouZH
  * @version  V1.01
  * @date     07-Dec-2015
  * @brief    存储分配表,包含数据类型，设备类型，地址范围等定义.
  ******************************************************************************
  * @attention
  * 2015-11-04 V1.00 ZouZH 初次创建
  * 2015-12-07 V1.01 ZouZH 修改数据项(dataItem)起始地址定义
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "record_tbl.h"

#include "fm24clxx.h"

/** 
 * @addtogroup Record
 * @{
 */

/** 
 * @defgroup RecordTable
 * @brief 数据存储地址分配表项
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/**
 * @note 存储分配表定义
 * 1)相同dataType必须相邻,必须按照dataItem从小到大排序,dataItem为0时,代表只有一种条项;
 * 2)一个dataType具有多个dataItem时，dataItem序号必须从大于0开始递增
 * 3)devType和devAddr必须大于0
 * 4)dataStartAddr为每个dataType内dataItem相对于首个dataItem偏移的存储起始地址
 * 5)dataItemSize为数据条项大小
 * 6)dataTotalSize为每个dataType总存储大小
 * 7)devTotalSize为devType总空间大小
 */
const RecordTBL_t RecordTBL[] =
{
  // 系统参数
  { .dataType      = REC_DT_SysParam,
    .dataItem      = REC_DI_NONE,
    .devType       = REC_DEV_FM24CL64,
    .devAddr       = 0x00,
    .dataStartAddr = REC_Addr_SysParam,
    .dataItemSize  = sizeof(SysParam_t),
    .dataTotalSize = REC_Size_SysParam,
    .devTotalSize  = FM24CL64_MAX_ADDR
  },
  
  // 网络参数
  { .dataType      = REC_DT_NetParam,
    .dataItem      = REC_DI_NONE,
    .devType       = REC_DEV_FM24CL64,
    .devAddr       = 0x00,
    .dataStartAddr = REC_Addr_NetParam,
    .dataItemSize  = sizeof(NetParam_t),
    .dataTotalSize = REC_Size_NetParam,
    .devTotalSize  = FM24CL64_MAX_ADDR
  },
  
  // 温度采集
  { .dataType      = REC_DT_TempCollect,
    .dataItem      = REC_DI_CycleData,
    .devType       = REC_DEV_FM24CL64,
    .devAddr       = 0x00,
    .dataStartAddr = REC_Addr_TempCollect,
    .dataItemSize  = sizeof(TempCollect_t),
    .dataTotalSize = REC_Size_TempCollect,
    .devTotalSize  = FM24CL64_MAX_ADDR
  },

  // 存储分配表-结束标志
  { .dataType      = REC_DT_NONE,
    .dataItem      = REC_DI_NONE,
    .devType       = REC_DEV_NONE,
    .devAddr       = 0x00,
    .dataStartAddr = 0x00000000,
    .dataItemSize  = 0x00000000,
    .dataTotalSize = 0x00000000,
    .devTotalSize  = 0x00000000
  },
};

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */



/**
 * @brief
 *
 * @param
 *
 * @retval
 */


/**
 * @}
 */

/**
 * @}
 */

