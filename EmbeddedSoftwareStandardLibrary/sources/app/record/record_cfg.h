/**
  ******************************************************************************
  *               Copyright(C) 2015-2025 GDKY  All Rights Reserved
  *
  * @file     record_cfg.h
  * @author   ZouZH
  * @version  V1.00
  * @date     27-Nov-2015
  * @brief    数据存储配置和定义文件.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __RECORD_CFG_H
#define __RECORD_CFG_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/** 
 * @addtogroup Record
 * @{
 */

/** 
 * @defgroup RecordApp
 * @brief 数据存储配置项
 * @{
 */


/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * 存储错误代码
 */
typedef enum
{
  REC_OK            = 0,  /* 执行正常 */
  REC_ERR_PARAM     = 1,  /* 参数错误 */
  REC_ERR_CHECK     = 2,  /* 校验错误 */
  REC_ERR_DRIVE     = 3,  /* 底层驱动错误 */
  REC_ERR_PASSWD    = 4,  /* 系统密码错误 */
  REC_ERR_CFG       = 5,  /* 配置参数错误 */
  REC_ERR_OVER      = 6,  /* 地址溢出 */

}RecordErr_t;


/*
*****************************************************************************
* 以下定义需要与实际使用的 RecordTBL 内存分配表对应
*****************************************************************************
*/

/**
 * 数据类型(DataType)定义
 */
typedef enum
{
  REC_DT_NONE              = 0,  /* 无效类型 */
  REC_DT_SysParam          = 1,  /* 系统参数        - System Parameter */
  REC_DT_NetParam          = 2,  /* 网络参数        - Network Parameter */
  REC_DT_SysPasswd         = 3,  /* 系统密码        - System Password */
  REC_DT_PowerMeter        = 4,  /* 电能表          - PowerMeter */
  REC_DT_TempCtrl          = 5,  /* 温控            - TempCtrl */

  REC_DT_HistDataParam     = 6,  /* 历史数据参数    - System Parameter */

  REC_DT_MAX,                    /* 最大数据类型 */
}RecordDataType_t;

/**
 * 数据条项(DataItem)定义
 */
typedef enum
{
  REC_DI_NONE              = 0,  /* 无效条项 */
  REC_DI_InitInfo          = 1,  /* 初始化信息     - Initial Information */
  REC_DI_RTimeData         = 2,  /* 实时数据       - Real Time Data */
  REC_DI_CycleData         = 3,  /* 周期数据       - Cycle Data */
  REC_DI_CycleDataBak      = 4,  /* 周期数据备份   - Cycle Data backup */
  REC_DI_HistoryData       = 5,  /* 历史数据       - History Data */
  REC_DI_Event             = 6,  /* 事件记录       - Event */
  REC_DI_Alarm             = 7,  /* 报警记录       - Alarm */
  REC_DI_Param             = 8,  /* 参数信息       - Parameter Information */
  REC_DI_MAX,                    /* 最大数据项 */

}RecordDataItem_t;

/**
 * 存储设备(Device)定义
 */
typedef enum
{
  REC_DEV_NONE        = 0,  /* 未指定设备 */
  REC_DEV_FM24CL64    = 1,  /* FM24CL64B-G，8KByte */
  REC_DEV_FM24W256    = 2,  /* FM24W256 32KB */
  REC_DEV_SDCARD      = 3,  /* SD Card */
  REC_DEV_S25FL132K   = 4,  /* 4MB Flash */
  REC_DEV_S25FL164K   = 5,  /* 8MB Flash */
}RecordDevType_t;


/*
*****************************************************************************
* 以下为用户存储的数据条项定义,必须按照以下标准存储结构(即最后3字节必须为校验)
* __packed typedef struct
* {
*   uint8_t  xxx;
*   uint16_t xxx;
*   uint32_t xxx;
*
*   uint8_t  bcc;              // BCC校验
*   uint16_t crc;              // CRC校验
*
* }UserDataItem_t
*****************************************************************************
*/

/**
 *  系统参数 64Bytes
 */
__packed typedef struct
{
  uint32_t version;             /* 版本号: xx.xx.xx.xx(BCD Code) */

  uint8_t  heatSeason[4];       /* BCD码, 采暖季,开始月日结束月日 */
  uint8_t  isHeatSeason;        /* 00 非采暖季 01 采暖季 */

  uint16_t collPeriod;          /* 采集周期(单位:分钟)2B: 2-1440 */
  uint8_t  cmdInterval;         /* 命令发送间隔1B, 100ms */

  uint8_t  powerMeterNbr;       /* 电表表数量 */
  uint16_t tempctrlNbr;         /* 温控数量 */

  uint8_t  hardwareVer[5];      /* BCD码, 硬件版本号 */
  uint8_t  softwareVer[4];      /* BCD码, 软件版本号 */
  uint8_t  protocolVer[4];      /* BCD码, 协议版本号 */

  uint8_t  reserved[33];        /* 保留字节 */

  uint8_t  bcc;                 /* BCC校验 */
  uint16_t crc;                 /* CRC校验 */
}SysParam_t;

/* MACROS  ------------------------------------------------------------------ */

/**
 *  系统参数
 */

/* 采集周期(2-1440分钟) */
//#define REC_CollPeriod_Def       10
//#define REC_CollPeriod           g_tSysParam.collPeriod

/* CONSTANTS  --------------------------------------------------------------- */

/**
 * 系统参数版本
 */
#define RECORD_SYSPARAM_VER       0x20180428

/**
 * 存储运行配置参数项
 */
#define RECORD_HW_MAX_NBR         3  /* 最多设备数, 0-255 */

#define RECORD_ERR_RETRY_CNT      3  /* 存储发生错误重试次数 */

#define RECORD_HIST_MAX_YEAR      2  /* 历史数据存储最大年份 */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RECORD_CFG_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

