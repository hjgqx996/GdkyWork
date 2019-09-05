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

#define MAX_DEV_DATA_CNT    6  //每帧待发送数据中，包含温度个数最大值
#define MAX_DATA_ITEM_MAX   50 //存储数据次数，最大值，
   
/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern SysParam_t g_tSysParam;

extern uint16_t g_u8SendInterval;//定时发送间隔 单位：分钟;范围:30-1440
extern uint8_t  g_u8TempDispEn  ;//是否显示温度 Bit0 = 0 显示  Bit0 = 1 不显示
extern uint16_t g_u16RecordInterval;//数据存储间隔 单位：分钟;范围:30-1440
extern uint16_t g_u16DownDataTime  ;//上位机数据下发时间。30-1440(必须为定时发送间隔的整数倍)

extern uint8_t  g_u8DevDataCnt ;//每帧待发送数据中，包含温度个数
extern uint8_t   g_u8DevDateIndex;//设备数据缓冲区索引。
extern DevData_t g_tDevData[MAX_DEV_DATA_CNT];//设备数据(设备状态1+室温1状态+温度1+湿度1)

extern TempCollect_t g_tTempCollect;
extern uint8_t        g_DataItemHead;    //数据条项索引 头
extern uint8_t        g_DataItemTail;    //数据条项索引 尾

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

