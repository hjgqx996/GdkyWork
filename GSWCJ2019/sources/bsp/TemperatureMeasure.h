/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     TemperatureMeasure.h
  * @author   Tian Ruidong
  * @version  V1.00
  * @date     2017.8.22
  * @brief    ��ʪ�Ȳ�������.
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
 * @brief ��ʪ�Ȳ�������.
 * @{
 */

#include "includes.h"
#include "HDC1080.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern unsigned char T_able,H_able;  //��ȡ�¡�ʪ�ȹ��ϱ�־

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

void TemperatureHumiditySensorInit(void);    //��������ʼ��
void StartTemperatureHumidityConversion(void);   //����һ���¶�ת��
void UpdateTemperature(void);   //�����¶�
void UpdateHumidity(void);   //����ʪ��
void SensorSamplePoll(void);  //�洢�¶���ѯ


/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __TEMPERATURE_MEASURE_H */

/**
 * @}
 */

