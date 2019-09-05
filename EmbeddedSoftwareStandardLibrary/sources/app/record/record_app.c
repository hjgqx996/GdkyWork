/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     record_app.c
  * @author   ZouZH
  * @version  V1.00
  * @date     16-June-2016
  * @brief    数据存储应用程序
  ******************************************************************************
  * @history
  */


/* INCLUDES ------------------------------------------------------------------- */
#include "record_app.h"
#include "record_tbl.h"

#include "includes.h"

/** 
 * @addtogroup Record
 * @{
 */

/** 
 * @defgroup RecordApp
 * @brief 数据存储应用程序
 * @{
 */


/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* 系统参数临时变量 */
static SysParam_t s_tSysParam;

/* LOCAL FUNCTIONS ------------------------------------------------------------ */

/**
 * @brief  存储初始化
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t RecordApp_Init(void)
{
  RecordErr_t retErr;

  retErr = Record_Init();

  SYSEnterCritical();

  g_tSysParam.hardwareVer[0] = 0x05;
  g_tSysParam.hardwareVer[1] = 0x03;
  g_tSysParam.hardwareVer[2] = 0x03;
  g_tSysParam.hardwareVer[3] = 0x03;
  g_tSysParam.hardwareVer[4] = 0x04;
  lib_memcpy(g_tSysParam.softwareVer, APP_SOFT_VER, 4);
  lib_memset(g_tSysParam.protocolVer, 0x00, 4);

  SYSExitCritical();

  return retErr;
}

/**
 * @brief  保存系统参数
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t RecordApp_SaveSysParam(void)
{
  RecordErr_t err = REC_OK;

  SYSEnterCritical();
  lib_memcpy(&s_tSysParam, &g_tSysParam, sizeof(SysParam_t));
  SYSExitCritical();

  err = Record_Write(REC_DT_SysParam, REC_DI_NONE, 0, &s_tSysParam);
  err = Record_Write(REC_DT_SysParam, REC_DI_NONE, 1, &s_tSysParam);

  return err;
}

/**
 * @brief  读取系统参数
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t RecordApp_ReadSysParam(void)
{
  RecordErr_t err = REC_OK;

  err = Record_Read(REC_DT_SysParam, REC_DI_NONE, 0, &s_tSysParam);
  if (err != REC_OK)
    err = Record_Read(REC_DT_SysParam, REC_DI_NONE, 1, &s_tSysParam);

  SYSEnterCritical();
  if (err == REC_OK)
    lib_memcpy(&g_tSysParam, &s_tSysParam, sizeof(SysParam_t));
  SYSExitCritical();

  return err;
}

/**
 * @}
 */

/**
 * @}
 */

