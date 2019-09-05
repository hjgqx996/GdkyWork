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
  REC_DT_TempCollect       = 3,  /* 温度采集        - Temperature collection */

  REC_DT_HistDataParam     = 4,  /* 历史数据参数    - System Parameter */

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
typedef struct
{
  uint32_t version;             /* 版本号: xx.xx.xx.xx(BCD Code) */

  uint8_t  heatSeason[4];       /* BCD码, 采暖季,开始月日结束月日 */
  uint8_t  isHeatSeason;        /* 00 非采暖季 01 采暖季 */

  uint16_t collPeriod;          /* 采集周期(单位:分钟)2B: 2-1440 */

  uint8_t  hardwareVer[5];      /* BCD码, 硬件版本号 */
  uint8_t  softwareVer[4];      /* BCD码, 软件版本号 */
  uint8_t  protocolVer[4];      /* BCD码, 协议版本号 */
  uint32_t deviceNum     ;      /* device number */
    
  uint8_t  reserved[29];        /* 保留字节 */////????

  uint8_t  bcc;                 /* BCC校验 */
  uint16_t crc;                 /* CRC校验 */
}SysParam_t;

/**
 *  网络参数 64Bytes
 */
typedef struct
{
  uint8_t  reserved[57];        /* 保留字节 */

  uint8_t  bcc;                 /* BCC校验 */
  uint16_t crc;                 /* CRC校验 */
}NetParam_t;

/**
 *  设备数据
 */

#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为4字节对齐
typedef struct
{
  uint8_t  devStatus;    /* 设备状态 Bit0：0 电池没报低电；1 电池低电；
                                     Bit1：0 没有移位；    1 移位 
                                     Bit7:有效数据指示，0：有效。*/
  uint8_t  tempStatus;   /* 室温状态 Bit0 = 0 室温2不修正  Bit0 = 1 室温2修正
                                     Bit1 = 0 室温2正常    Bit1 = 1 室温2故障 */
  int16_t  i16TempVal;   /* 温度值 有符号16位整形数据（实际温度*100） */
  uint8_t  humidityVal;  /* 湿度值 数据范围：1-100,数据错误或无湿度传感器发送0*/
////  uint8_t  humidityVal2; //补对齐
}DevData_t;
#pragma pack(pop) //保存对齐状态


#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为4字节对齐
/**
 *  温度采集器  Bytes
 */
typedef struct
{
//  uint8_t  frameLen;          /* 数据帧长度 0:无数据；> :*/
//  //frame start
//  uint8_t  startCode;         /* 帧起始符 68H */
//  uint8_t  devType;           /* 设备类型 0：无线自组网室温采集器; 1：万年历外壳;
//                                 2：插座式外壳; 3：笔筒状外壳；4：碱性电池外壳.*/
//  uint8_t  addr[7];           /* 地址域 由7个字节BCD码组成高位字节在前，
//                                      高3字节固定0，低4字节数据范围0~99999999*/
//  uint8_t  ctrlCode;          /* 控制码 */
//  uint8_t  dataLen;           /* 数据长度域 */
//  uint16_t u16dataID;         /* 数据标识 0x901F：主动上报实时数据；0x911F:主动
//                        上报实时数据，在901F标识码指令基出上增加上传SIM卡串号 */
//  uint8_t  sequenceNum;       /* 包序号 以自然数累加，（上位机返回的序列号 需和
//                                        室温采集器发送的数据序列号一致） */
  uint8_t  packageTime[6];    /* 设备数据包生成时间 依次为：年（1B）、月（1B）、
                                       日（1B）、时（1B）、分（1B）、秒（1B）*/
  uint8_t  softwareVer[2];    /* 软件版本号 BCD码*/
  uint8_t  protocolVer[6];    /* 协议版本 BCD码 该协议版本170922为固定值 */
  uint8_t  WlFactory;         /* 无线厂商代码 2利尔达 */
  uint16_t u16sendInterval;   /* 定时发送间隔 单位：分钟;范围:30-1440*/
  uint8_t  tempDispEn;        /* 是否显示温度 Bit0 = 0 显示  Bit0 = 1 不显示*/
////  uint8_t  tempDispEn2;////补对齐
  uint16_t u16recordInterval; /* 数据存储间隔 单位：分钟;范围:30-1440*/
  uint8_t  RSSI;              /* 信号强度 数据范围0-31*/
  uint8_t  lastError;         /* 上次网络故障代码 */
  uint8_t  tempOffset;        /* 室温偏移修正值（实际修正值*10）*/
  uint8_t  devDataCnt;        /* 发送设备数据次数（1-6）
                                （下边设备数据1-6根据此设置值决定是否发送） */
  DevData_t devData[6];         /* 需上传的设备数据 (最多6组) */
  uint8_t  lastDialCnt;       /* 上次拨号次数 */
  uint8_t  lastDialTime;      /* 上次拨号时间 单位：秒*/
  uint8_t  lastTranceiveTime; /* 上次数据收发时间 单位：秒*/
  uint8_t  dataType;          /*  0 实时数据; 1 历史数据 */
  uint16_t u16DownDataTime;   /* 数据下发时间 单位：分钟;范围:30-1440 */
  uint8_t  RSSI_Min;          /* 信号强度阈值0-31 
                                 信号强度小于等于阈值时，有数据重发功能*/
  uint8_t  simNumber[10];     /* SIM卡串号 高位字节在前，共10个字节 20位数据*/
//  uint8_t  checkCode;         /* 校验码 从帧起始符68H开始 到校验码前
//                                 （不包含校验码）所有数据的二进制累加和后8位*/
//  uint8_t  endCode;           /* 结束符 16H*/
  //frame end
  
  uint8_t  bcc;           /* BCC校验 */
  uint16_t crc;           /* CRC校验 */
} TempCollect_t;
#pragma pack(pop) //保存对齐状态

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
#define RECORD_SYSPARAM_DEV_NUM   0x12345678
   
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

