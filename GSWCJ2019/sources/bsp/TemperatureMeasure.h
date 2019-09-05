/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     TemperatureMeasure.h
  * @author   Tian Ruidong
  * @version  V1.00
  * @date     2017.8.22
  * @brief    温湿度测量程序.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __TEMPERATURE_MEASURE_H
#define __TEMPERATURE_MEASURE_H


#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

   
/* INCLUDES ------------------------------------------------------------------- */

/** 
 * @defgroup Temperature Measure
 * @brief 温湿度测量程序.
 * @{
 */

#include "includes.h"
#include "HDC1080.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern unsigned char T_able,H_able;  //读取温、湿度故障标志

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

void TemperatureHumiditySensorInit(void);    //传感器初始化
void StartTemperatureHumidityConversion(void);   //启动一次温度转换
void UpdateTemperature(void);   //更新温度
void UpdateHumidity(void);   //更新湿度
void SensorSamplePoll(void);  //存储温度轮询


/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __TEMPERATURE_MEASURE_H */

/**
 * @}
 */

