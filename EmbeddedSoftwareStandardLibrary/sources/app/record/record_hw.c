/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY  All Rights Reserved
  *
  * @file     record_hw.c
  * @author   ZouZH
  * @version  V1.03
  * @date     26-May-2016
  * @brief    数据存储底层硬件驱动.
  ******************************************************************************
  * @history
  * 2015-11-04 V1.00 ZouZH Create this file
  * 2016-03-17 V1.01 ZouZH Add Record_HWErase()
  * 2016-04-11 V1.02 ZouZH Add FM24V10 -U5=11
  * 2016-05-26 V1.03 ZouZH Add FM24V10 1,2,3
  */


/* INCLUDES ----------------------------------------------------------------- */
#include "record_hw.h"

#include "includes.h"
#include "fm24clxx.h"

/** 
 * @addtogroup Record
 * @{
 */

/** 
 * @defgroup RecordHardware
 * @brief 数据存储硬件驱动接口
 * @{
 */


/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */



/**
 * @brief  初始化存储分配表(RecordTBL)内所有硬件
 *
 * @param  ptbl 指定初始化分配表中设备,为NULL时初始化所有设备
 *
 * @retval 0成功,其他失败
 */
RecordErr_t Record_HWInit(const RecordTBL_t *ptbl)
{
  uint8_t err = 0;

  if (ptbl != NULL)
  {
    if ((ptbl->devType == REC_DEV_FM24CL64) || (ptbl->devType == REC_DEV_FM24W256))
    {
      FRAM_Init();
      xprintf("FRAM Init...\r\n");
      return REC_OK;
    }
    return REC_ERR_PARAM;
  }

  xprintf("FRAM Init...\r\n");
  FRAM_Init();

  if (err)
    return REC_ERR_DRIVE;
  else
    return REC_OK;
}


/**
 * @brief  从底层硬件中读取数据
 *
 * @param  ptbl   存储表项
 * @param  addr   存储地址
 * @param  pvbuf  数据缓存
 * @param  len    数据长度
 *
 * @retval 0成功,其他失败
 */
RecordErr_t Record_HWRead(const RecordTBL_t *ptbl, uint32_t addr, void *pvbuf, uint32_t len)
{
  ASSERT((ptbl != NULL) && (pvbuf != NULL));

  if ((ptbl->devType == REC_DEV_FM24W256) && (1 == ptbl->devAddr))
  {
    if (FRAM_Read(FM24W256_2, addr, pvbuf, len))
      return REC_OK;
    else
      return REC_ERR_DRIVE;
  }

  return REC_ERR_CFG;
}


/**
 * @brief  向底层硬件中写入数据
 *
 * @param  ptbl   存储表项
 * @param  addr   存储地址
 * @param  pvbuf  数据缓存
 * @param  len    数据长度
 *
 * @retval 0成功,其他失败
 */
RecordErr_t Record_HWWrite(const RecordTBL_t *ptbl, uint32_t addr, const void *pvbuf, uint32_t len)
{
  ASSERT((ptbl != NULL) && (pvbuf != NULL));

  if ((ptbl->devType == REC_DEV_FM24W256) && (1 == ptbl->devAddr))
  {
    if (FRAM_Write(FM24W256_2, addr, pvbuf, len))
      return REC_OK;
    else
      return REC_ERR_DRIVE;
  }

  return REC_ERR_CFG;
}

/**
 * @brief  从底层硬件中擦除数据
 *
 * @param  ptbl   存储表项
 * @param  addr   存储地址
 * @param  len    数据长度
 *
 * @retval 0成功,其他失败
 */
RecordErr_t Record_HWErase(const RecordTBL_t *ptbl, uint32_t addr, uint32_t len)
{
  ASSERT(ptbl != NULL);

  if ((ptbl->devType == REC_DEV_FM24W256) && (1 == ptbl->devAddr))
  {
    if (FRAM_Erase(FM24W256_2, addr, len))
      return REC_OK;
    else
      return REC_ERR_DRIVE;
  }

  return REC_ERR_CFG;
}

/**
 * @}
 */

/**
 * @}
 */

