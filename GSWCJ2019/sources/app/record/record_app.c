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
#include "HDC1080.h" 
#include "nb_app.h"
#include "flash.h"
#include "PowerManage.h"
#include "TemperatureMeasure.h"

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
uint8_t  g_u8SeqNum = 254;

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* 系统参数临时变量 */
static SysParam_t s_tSysParam;
static TempCollect_t s_tTempCollect;

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
 * @brief  暂存一组数据
 * @param  无
 * @retval 0 成功，其他失败
 */
RecordErr_t RecordApp_DevData(void)
{
  int16_t iTemp;
  
  if(g_u8DevDateIndex < g_u8DevDataCnt)
  {
    //设备状态//Bit0 = 1 电池低电; Bit1 = 1 移位
    g_tDevData[g_u8DevDateIndex].devStatus   = (PM_GetFlagLowVolt()); 
    //室温状态//Bit0 = 1 室温修正; Bit1 = 1 室温故障
    g_tDevData[g_u8DevDateIndex].tempStatus  = (T_able<<1) | (g_u8TempCalibrateEn & 0x01); 
    iTemp = Hdc1080_GetTempVal(); //温度
    g_tDevData[g_u8DevDateIndex].i16TempVal  = ((iTemp & 0x00FF)<<8) + ((iTemp & 0xFF00)>>8);//温度
    g_tDevData[g_u8DevDateIndex].humidityVal = Hdc1080_GetHumidityVal(); //湿度
    
    g_u8DevDateIndex++;
    return REC_OK;
  }
  return REC_ERR_OVER;
}

/**
 * @brief  生成数据
 * @param  无
 * @retval 0 成功，其他失败
 */
RecordErr_t RecordApp_GenerateData(void)
{
  uint8_t i;
  uint8_t u8Tmp1, u8Tmp2;

  // 设备数据包生成时间 依次为：年（1B）、月（1B） 日（1B）、时（1B）、分（1B）、秒（1B）
  if(0)//已经校时过了////????
  {
    g_tTempCollect.packageTime[0] = (g_tSysTime.year % 100);
    g_tTempCollect.packageTime[1] = g_tSysTime.month;
    g_tTempCollect.packageTime[2] = g_tSysTime.day;
    g_tTempCollect.packageTime[3] = g_tSysTime.hour;
    g_tTempCollect.packageTime[4] = g_tSysTime.minute;
    g_tTempCollect.packageTime[5] = g_tSysTime.second; 
  }
  else
  {
    g_tTempCollect.packageTime[0] = 0;
    g_tTempCollect.packageTime[1] = 0;
    g_tTempCollect.packageTime[2] = 0;
    g_tTempCollect.packageTime[3] = 0;
    g_tTempCollect.packageTime[4] = 0;
    g_tTempCollect.packageTime[5] = 0; 
  }

  //BCD码	设备软件版本号,高位字节在前
  g_tTempCollect.softwareVer[0] = g_cSoftVer[0];////可在定义时初始化，就不用每次赋值了
  g_tTempCollect.softwareVer[1] = g_cSoftVer[1];
  
  //协议版本 BCD码 该协议版本170922为固定值
  lib_memcpy(g_tTempCollect.protocolVer, ProtocolVerNum, 6);
  
  g_tTempCollect.WlFactory  = g_u8ModemType;/* 无线厂商代码 2利尔达 */
  
  g_tTempCollect.u16sendInterval    = g_u8SendInterval;   //定时发送间隔 单位：分钟;范围:30-1440
  g_tTempCollect.tempDispEn         = g_u8TempDispEn;     //是否显示温度 Bit0 = 0 显示  Bit0 = 1 不显示
  g_tTempCollect.u16recordInterval  = g_u16RecordInterval;//数据存储间隔 单位：分钟;范围:30-1440
  g_tTempCollect.RSSI               = nbAppGetRSSI();     //信号强度 数据范围0-31
  g_tTempCollect.lastError          = nbAppGetLastError();//上次网络故障代码 
  g_tTempCollect.tempOffset         = g_i8TempCalibrate;  //室温偏移修正值（实际修正值*10）
  
  //发送设备数据次数（1-6）（下边设备数据1-6根据此设置值决定是否发送）
  g_tTempCollect.devDataCnt = g_u8DevDateIndex;
                                
  //需上传的设备数据
  for(i=0; (i < g_u8DevDateIndex); i++) {
    lib_memcpy( (g_tTempCollect.devData + i), &(g_tDevData[i]), sizeof(DevData_t));
  }
//  //设备数据的无效标志置位。
//  for(i=g_u8DevDateIndex; i < 6; i++) {
//    g_tTempCollect.devData[i].devStatus |= 0x80;//设备状态的最高位作为有效数据指示，0：有效。
//  }
  g_tTempCollect.lastDialCnt = 1;       /* 上次拨号次数 */
  g_tTempCollect.lastDialTime = 1;      /* 上次拨号时间 单位：秒*/
  g_tTempCollect.lastTranceiveTime = 5; /* 上次数据收发时间 单位：秒*/
  g_tTempCollect.dataType = 0;          /*  0 实时数据; 1 历史数据 */
  g_tTempCollect.u16DownDataTime = 1160;////710;////????  g_u16DownDataTime;//数据下发时间 单位：分钟;范围:30-1440
  //信号强度阈值0-31 信号强度小于等于阈值时，有数据重发功能
  u8Tmp1 = (*(uint8_t *)SignalThreshold_add);
  if(u8Tmp1 > 31) {
    u8Tmp1 = 10;
  }
  g_tTempCollect.RSSI_Min = (*(uint8_t *)SignalThreshold_add);
  
  //SIM卡串号 高位字节在前，共10个字节 20位数据    
  for(i=0; i<10; i++)
  {
    u8Tmp1 = SimCardNum[i*2];
    if(u8Tmp1 > '9'){         ////????
      u8Tmp1 += 9;
    }
    
    u8Tmp2 = SimCardNum[(i*2)+1];
    if(u8Tmp2 > '9'){
      u8Tmp2 += 9;
    }
    
    g_tTempCollect.simNumber[i] = ((u8Tmp1&0x0f)<<4) | (u8Tmp2&0x0f);
  }
  
  g_u8DevDateIndex = 0;//暂存的数据已拷贝走，索引清零。
  
  return REC_OK;
}


/**
 * @brief  打包一帧数据
 * @param  无
 * @retval 0 成功，其他失败
 */
RecordErr_t RecordApp_PackData(void)
{
  uint16_t u16LenSent = 0;
  uint8_t  pu8SendBuf[NB_CFG_PKG_SIZE] = {0};
  
  //frame start
  pu8SendBuf[u16LenSent++] = 0x68;    /* 帧起始符 68H */
  pu8SendBuf[u16LenSent++] = DEVICE_TYPE;/* 设备类型 0：无线自组网室温采集器; 1：万年历外壳;
                                 2：插座式外壳; 3：笔筒状外壳；4：碱性电池外壳.*/
  // 地址域 由7个字节BCD码组成高位字节在前，
  //高3字节固定0，低4字节数据范围0~99999999
  pu8SendBuf[u16LenSent++] = 0;
  pu8SendBuf[u16LenSent++] = 0;
  pu8SendBuf[u16LenSent++] = 0;
  pu8SendBuf[u16LenSent++] = 0x00;//(*( (uint8_t *)DeviceNum_add));
  pu8SendBuf[u16LenSent++] = 0x00;//(*( (uint8_t *)DeviceNum_add + 1));
  pu8SendBuf[u16LenSent++] = 0x45;//(*( (uint8_t *)DeviceNum_add + 2));
  pu8SendBuf[u16LenSent++] = 0x76;//(*( (uint8_t *)DeviceNum_add + 3));
  
  pu8SendBuf[u16LenSent++]  = 0x81;    /* 控制码 81	室温采集器主动发送数据*/
  pu8SendBuf[u16LenSent++]  = 0;       //数据长度域，下标10。
  
  /* 数据标识 0x901F：主动上报实时数据；s_u8SeqNum
              0x911F: 主动上报实时数据 + 增加上传SIM卡串号 */
  pu8SendBuf[u16LenSent++]  = 0x1F;////????
  pu8SendBuf[u16LenSent++]  = 0x91;//下标12  
  //包序号 以自然数累加,(上位机返回的序列号 需和 室温采集器发送的数据序列号一致)                
  pu8SendBuf[u16LenSent++]  = (++g_u8SeqNum); ////????   
                                        
  // 设备数据包生成时间 依次为：年（1B）、月（1B）、日（1B）、时（1B）、分（1B）、秒（1B）
  lib_memcpy(pu8SendBuf+u16LenSent, g_tTempCollect.packageTime, 6); 
  u16LenSent += 6;
  
  //BCD码	设备软件版本号,高位字节在前
  pu8SendBuf[u16LenSent++] = g_tTempCollect.softwareVer[0];
  pu8SendBuf[u16LenSent++] = g_tTempCollect.softwareVer[1];
  
  //协议版本 BCD码 该协议版本170922为固定值
  lib_memcpy(pu8SendBuf+u16LenSent, g_tTempCollect.protocolVer, 6); 
  u16LenSent += 6;
  
  //无线厂商代码 2利尔达
  pu8SendBuf[u16LenSent++] = g_tTempCollect.WlFactory;
  //定时发送间隔 单位：分钟;范围:30-1440
  pu8SendBuf[u16LenSent++] = (g_tTempCollect.u16sendInterval>>8)&0x00FF; 
  pu8SendBuf[u16LenSent++] = g_tTempCollect.u16sendInterval&0x00FF;
  //是否显示温度 Bit0 = 0 显示  Bit0 = 1 不显示
  pu8SendBuf[u16LenSent++] = g_tTempCollect.tempDispEn;
  //数据存储间隔 单位：分钟;范围:30-1440
  pu8SendBuf[u16LenSent++] = (g_tTempCollect.u16recordInterval>>8)&0x00FF;
  pu8SendBuf[u16LenSent++] = g_tTempCollect.u16recordInterval&0x00FF;
  
  pu8SendBuf[u16LenSent++] = g_tTempCollect.RSSI;//信号强度 数据范围0-31
  pu8SendBuf[u16LenSent++] = g_tTempCollect.lastError;//上次网络故障代码 
  pu8SendBuf[u16LenSent++] = g_tTempCollect.tempOffset;//室温偏移修正值（实际修正值*10）
  
  //下标38，发送设备数据次数（1-6）（下边设备数据1-6根据此设置值决定是否发送） 
  pu8SendBuf[u16LenSent++] = g_tTempCollect.devDataCnt;
                                
  //需上传的设备数据
  lib_memcpy( pu8SendBuf+u16LenSent, 
             (const uint8_t *)g_tTempCollect.devData, 
             (sizeof(DevData_t)*g_tTempCollect.devDataCnt));
  u16LenSent += (sizeof(DevData_t)*g_tTempCollect.devDataCnt);
  
  pu8SendBuf[u16LenSent++] = g_tTempCollect.lastDialCnt ;      //上次拨号次数 
  pu8SendBuf[u16LenSent++] = g_tTempCollect.lastDialTime ;     //上次拨号时间 单位：秒
  pu8SendBuf[u16LenSent++] = g_tTempCollect.lastTranceiveTime; //上次数据收发时间 单位：秒
  pu8SendBuf[u16LenSent++] = g_tTempCollect.dataType;          // 0 实时数据; 1 历史数据 
  //数据下发时间 单位：分钟;范围:30-1440
  pu8SendBuf[u16LenSent++] = (g_tTempCollect.u16DownDataTime>>8)&0x00FF;  
  pu8SendBuf[u16LenSent++] = g_tTempCollect.u16DownDataTime&0x00FF;   
  //信号强度阈值0-31 信号强度小于等于阈值时，有数据重发功能
  pu8SendBuf[u16LenSent++] = g_tTempCollect.RSSI_Min;//// = 20;
  
  //SIM卡串号 高位字节在前，共10个字节 20位数据 
  if(0x91 == pu8SendBuf[12]) {
    lib_memcpy(pu8SendBuf+u16LenSent, g_tTempCollect.simNumber, 10);     
    u16LenSent += 10;
  }
  
  pu8SendBuf[10]  = u16LenSent-11;//数据长度域，下标10。
  
  //校验码 从帧起始符68H开始 到校验码前（不包含校验码）所有数据的二进制累加和后8位
  pu8SendBuf[u16LenSent] = chk_sum8_hex(pu8SendBuf, u16LenSent);
  u16LenSent++ ;
  pu8SendBuf[u16LenSent++] = 0x16;//结束符 16H
  
  //拷贝数据到nb发送缓冲区
  nbAppWriteSendBuff(pu8SendBuf, u16LenSent);

  
  return REC_OK;
}

/**
 * @brief  读取存储的数据参数
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t RecordApp_ReadTempCollect(void)
{
  RecordErr_t err = REC_OK;
  
  if(g_DataItemTail == g_DataItemHead) {
    return REC_ERR_OVER;
  }

  err = Record_Read(REC_DT_TempCollect, REC_DI_CycleData, g_DataItemHead++, &s_tTempCollect);
  if(g_DataItemHead >= MAX_DATA_ITEM_MAX) {
    g_DataItemHead = 0;
  }
  if (err != REC_OK) {
    return REC_ERR_PARAM;
  }
    
  return err;
}

/**
 * @brief  写入存储的数据参数
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
RecordErr_t RecordApp_WriteTempCollect(void)
{
  RecordErr_t err = REC_OK;
  
  g_tTempCollect.dataType = 1;/*  0 实时数据; 1 历史数据 */
  err = Record_Write(REC_DT_TempCollect, REC_DI_CycleData, g_DataItemTail++, &g_tTempCollect);
  if(g_DataItemTail >= MAX_DATA_ITEM_MAX) {
    g_DataItemTail = 0;
  }
  
  if (err != REC_OK) {
    return REC_ERR_PARAM;
  }
  
  return err;
}


/**
 * @}
 */

/**
 * @}
 */

