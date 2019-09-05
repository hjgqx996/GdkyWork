/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     record.c
  * @author   ZouZH
  * @version  V1.03
  * @date     17-March-2016
  * @brief    线性数据存储应用层接口.
  ******************************************************************************
  * @history
  * 2015-11-04 V1.00 ZouZH 初次创建
  * 2015-12-07 V1.01 ZouZH 修正读取和写入数据项地址计算错误
  * 2015-12-24 V1.02 ZouZH 修正读取和写入数据项地址计算错误
  * 2016-03-17 V1.03 ZouZH 增加Record_Erase()
  */


/* INCLUDES ----------------------------------------------------------------- */
#include "record.h"

#include "includes.h"
#include "record_hw.h"

/** 
 * @defgroup Record
 * @brief 线性数据存储模块
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/**
 * 系统参数,网络参数,历史参数
 */
SysParam_t g_tSysParam;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


/**
 * @brief  存储初始化
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t Record_Init(void)
{
  RecordErr_t recErr = REC_OK;

  // 硬件初始化
  recErr = Record_HWInit(NULL);
  if (recErr != REC_OK)
    return recErr;

  // 读系统参数
  recErr = Record_Read(REC_DT_SysParam, REC_DI_NONE, 0, &g_tSysParam);
  if (recErr != REC_OK)
    recErr = Record_Read(REC_DT_SysParam, REC_DI_NONE, 1, &g_tSysParam);

  if ((recErr != REC_OK) || (g_tSysParam.version != RECORD_SYSPARAM_VER))
  {
    Record_SysParamDeInit();
    recErr = Record_Write(REC_DT_SysParam, REC_DI_NONE, 0, &g_tSysParam);
    recErr = Record_Write(REC_DT_SysParam, REC_DI_NONE, 1, &g_tSysParam);
  }

  return recErr;
}


/**
 * @brief  读取数据
 *
 * @param  REC_DT_x 数据类型(1-REC_DT_MAX)
 * @param  REC_DI_x 数据条项(0-REC_DI_MAX)
 * @param  itemIdx  数据条项索引(0-65535)
 * @param  pvBuf    数据项缓存
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t Record_Read(RecordDataType_t REC_DT_x, RecordDataItem_t REC_DI_x, uint16_t itemIdx, void *pvBuf)
{
  RecordErr_t recErr = REC_OK;

  uint8_t  i = 0;
  uint8_t  tbl_idx = 0;
  uint32_t addr = 0;

  ASSERT(REC_DT_x && (REC_DT_x < REC_DT_MAX) && (REC_DI_x < REC_DI_MAX) && pvBuf);

  // 搜索存储分配表中对应数据项
  while (RecordTBL[i].dataType && RecordTBL[i].devType)
  {
    if ((REC_DT_x == RecordTBL[i].dataType) && (REC_DI_x == RecordTBL[i].dataItem))
      break;
    else
      i += 1;
  }
  // 未搜索到有效条项
  if (!RecordTBL[i].dataType || !RecordTBL[i].devType)
  {
    return REC_ERR_PARAM;
  }

  // 计算存储地址
  tbl_idx = i;
  addr = 0;

  // 从同数据类型的最后一个数据项开始计算
  while (RecordTBL[tbl_idx].dataType == RecordTBL[i + 1].dataType)
  {
    i += 1;
  }

  do
  {
    addr += RecordTBL[i].dataItemSize;
    if (i > 0)
    {
      i -= 1;
    }
    else
    {
      break;
    }
  }while (RecordTBL[tbl_idx].dataType == RecordTBL[i].dataType);
  addr *= itemIdx;
  addr += RecordTBL[tbl_idx].dataStartAddr;

  // 判断地址范围
  if ((addr + RecordTBL[tbl_idx].dataItemSize > (RecordTBL[tbl_idx].dataStartAddr + RecordTBL[tbl_idx].dataTotalSize)) || \
      (addr + RecordTBL[tbl_idx].dataItemSize > RecordTBL[tbl_idx].devTotalSize))
  {
    return REC_ERR_OVER;
  }

  for (i = 0; i < RECORD_ERR_RETRY_CNT; i++)
  {
    if (recErr != REC_OK)
    {
      recErr = Record_HWInit(&RecordTBL[tbl_idx]);
    }

    if (Record_HWRead(&RecordTBL[tbl_idx], addr, pvBuf, RecordTBL[tbl_idx].dataItemSize) != REC_OK)
    {
      recErr = REC_ERR_DRIVE;
      continue;
    }

    if (chk_crc16_MB((const uint8_t *)pvBuf,  RecordTBL[tbl_idx].dataItemSize))
    {
      recErr = REC_ERR_CHECK;
      break;
    }

    if (chk_bcc((const uint8_t *)pvBuf, RecordTBL[tbl_idx].dataItemSize - 2))
    {
      recErr = REC_ERR_CHECK;
      break;
    }
    else
    {
      i = 0x55;
    }
  }

  return recErr;
}


/**
 * @brief  写入数据
 *
 * @param  REC_DT_x 数据类型(1-REC_DT_MAX)
 * @param  REC_DI_x 数据条项(0-REC_DI_MAX)
 * @param  itemIdx  数据条项索引(0-65535)
 * @param  pvBuf    数据项缓存
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t Record_Write(RecordDataType_t REC_DT_x, RecordDataItem_t REC_DI_x, uint16_t itemIdx, void *pvBuf)
{
  RecordErr_t recErr = REC_OK;

  uint8_t  i = 0;
  uint8_t  tbl_idx = 0;
  uint32_t addr = 0;

  ASSERT(REC_DT_x && (REC_DT_x < REC_DT_MAX) && (REC_DI_x < REC_DI_MAX) && pvBuf);

  // 搜索存储分配表中对应数据项
  while (RecordTBL[i].dataType && RecordTBL[i].devType)
  {
    if ((REC_DT_x == RecordTBL[i].dataType) && (REC_DI_x == RecordTBL[i].dataItem))
      break;
    else
      i += 1;
  }
  // 未搜索到有效条项
  if (!RecordTBL[i].dataType || !RecordTBL[i].devType)
  {
    return REC_ERR_PARAM;
  }

  // 计算存储地址
  tbl_idx = i;
  addr = 0;

  // 从同数据类型的最后一个数据项开始计算
  while (RecordTBL[tbl_idx].dataType == RecordTBL[i + 1].dataType)
  {
    i += 1;
  }

  do
  {
    addr += RecordTBL[i].dataItemSize;
    if (i > 0)
    {
      i -= 1;
    }
    else
    {
      break;
    }
  }while (RecordTBL[tbl_idx].dataType == RecordTBL[i].dataType);
  addr *= itemIdx;
  addr += RecordTBL[tbl_idx].dataStartAddr;

  // 判断地址范围
  if ((addr + RecordTBL[tbl_idx].dataItemSize > (RecordTBL[tbl_idx].dataStartAddr + RecordTBL[tbl_idx].dataTotalSize)) || \
      (addr + RecordTBL[tbl_idx].dataItemSize > RecordTBL[tbl_idx].devTotalSize))
  {
    return REC_ERR_OVER;
  }

  *(uint8_t *)(((uint8_t *)pvBuf) + RecordTBL[tbl_idx].dataItemSize - 3) =
    chk_bcc((const uint8_t *)pvBuf, RecordTBL[tbl_idx].dataItemSize - 3);

  *(uint16_t *)(((uint8_t *)pvBuf) + RecordTBL[tbl_idx].dataItemSize - 2) =
    chk_crc16_MB((const uint8_t *)pvBuf, RecordTBL[tbl_idx].dataItemSize - 2);

  for (i = 0; i < RECORD_ERR_RETRY_CNT; i++)
  {
    if (recErr != REC_OK)
    {
      recErr = Record_HWInit(&RecordTBL[tbl_idx]);
    }

    if (Record_HWWrite(&RecordTBL[tbl_idx], addr, pvBuf, RecordTBL[tbl_idx].dataItemSize) != REC_OK)
    {
      recErr = REC_ERR_DRIVE;
      continue;
    }
    else
    {
      i = 0x55;
    }
  }

  return recErr;
}

/**
 * @brief  擦除数据
 *
 * @param  REC_DT_x 数据类型(1-REC_DT_MAX)
 * @param  REC_DI_x 数据条项(0-REC_DI_MAX)
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t Record_Erase(RecordDataType_t REC_DT_x, RecordDataItem_t REC_DI_x)
{
  RecordErr_t recErr = REC_OK;

  uint8_t  i = 0;
  uint8_t  tbl_idx = 0;
  uint32_t addr = 0;

  ASSERT(REC_DT_x && (REC_DT_x < REC_DT_MAX) && (REC_DI_x < REC_DI_MAX));

  // 搜索存储分配表中对应数据项
  while (RecordTBL[i].dataType && RecordTBL[i].devType)
  {
    if (((REC_DT_x == RecordTBL[i].dataType) && (REC_DI_x == RecordTBL[i].dataItem)) \
     || ((REC_DT_x == RecordTBL[i].dataType) && (REC_DI_x == REC_DI_NONE)))
      break;
    else
      i += 1;
  }
  // 未搜索到有效条项
  if (!RecordTBL[i].dataType || !RecordTBL[i].devType)
  {
    return REC_ERR_PARAM;
  }

  // 计算存储地址
  tbl_idx = i;
  addr = RecordTBL[tbl_idx].dataStartAddr;

  // 判断地址范围
  if ((addr + RecordTBL[tbl_idx].dataItemSize > (RecordTBL[tbl_idx].dataStartAddr + RecordTBL[tbl_idx].dataTotalSize)) \
   || (addr + RecordTBL[tbl_idx].dataItemSize > RecordTBL[tbl_idx].devTotalSize))
  {
    return REC_ERR_OVER;
  }

  // 删除整个dataType
  if (REC_DI_x == REC_DI_NONE)
  {
    for (i = 0; i < RECORD_ERR_RETRY_CNT; i++)
    {
      if (recErr != REC_OK)
      {
        recErr = Record_HWInit(&RecordTBL[tbl_idx]);
      }

      if (Record_HWErase(&RecordTBL[tbl_idx], addr, RecordTBL[tbl_idx].dataTotalSize) != REC_OK)
      {
        recErr = REC_ERR_DRIVE;
        continue;
      }
      else
      {
        i = 0x55;
      }
    }
  }
  else
  {
    while (REC_DT_x == RecordTBL[tbl_idx].dataType)
    {
      if (REC_DI_x == RecordTBL[tbl_idx].dataItem)
      {
        for (i = 0; i < RECORD_ERR_RETRY_CNT; i++)
        {
          if (recErr != REC_OK)
          {
            recErr = Record_HWInit(&RecordTBL[tbl_idx]);
          }

          if (Record_HWErase(&RecordTBL[tbl_idx], addr, RecordTBL[tbl_idx].dataItemSize) != REC_OK)
          {
            recErr = REC_ERR_DRIVE;
            continue;
          }
          else
          {
            i = 0x55;
          }
        }
      }

      addr += RecordTBL[tbl_idx].dataItemSize;
      tbl_idx += 1;
    }
  }

  return recErr;
}


/* ============================ 用户函数 ===================================== */

/**
 * @brief  设置系统参数默认值
 *
 * @param  无
 *
 * @retval 无
 */
void Record_SysParamDeInit(void)
{
  lib_memset(&g_tSysParam, 0x00, sizeof(SysParam_t));

  g_tSysParam.version         = RECORD_SYSPARAM_VER;
}

/**
 * @}
 */

