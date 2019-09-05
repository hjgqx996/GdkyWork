/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     TemperatureMeasure.c
  * @author   Tian Ruidong
  * @version  V1.00
  * @date     2017.8.22
  * @brief    ��ʪ�Ȳ�������.
  ******************************************************************************
  */

/* INCLUDES ------------------------------------------------------------------- */

/** 
 * @defgroup Temperature Measure
 * @brief ��ʪ�Ȳ�������.
 * @{
 */

#include "includes.h"
#include "TemperatureMeasure.h"
////#include "18B20.h"
#include "HDC1080.h"
#include "record_app.h"
////#include "ayModbusAgreement.h"
////#include "gdkyV2Agreement.h"
////#include "gdkyV2CoapAgreement.h"
////#include "gdkyV2OneNetAgreement.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

unsigned char T_able,H_able;  //��ȡ�¡�ʪ�ȹ��ϱ�־

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */



/**
 * @brief   ��������ʼ��
 *
 * @param   ��
 *
 * @retval  ��
 */
void TemperatureHumiditySensorInit(void)
{
#if(TEMPERATURE_SENSOR_TYPE == 1)  //18B20
  {
    
  }
#elif(TEMPERATURE_SENSOR_TYPE == 2)  //HDC1080
  {
    HDC1080_Init();
  }
#else
  {
    
  }
#endif
}


/**
 * @brief   ����һ���¶�ת��
 *
 * @param   ��
 *
 * @retval  ��
 */
void StartTemperatureHumidityConversion(void)
{
#if(TEMPERATURE_SENSOR_TYPE == 1)  //18B20
  {
    Ds18b20StartTemperatureHumidityConversion();
  }
#elif(TEMPERATURE_SENSOR_TYPE == 2)  //HDC1080
  {
    
  }
#else
  {
    
  }
#endif
}


/**
 * @brief   �����¶�
 *
 * @param   ��
 *
 * @retval  ��
 */
void UpdateTemperature(void)
{
#if(TEMPERATURE_SENSOR_TYPE == 1)  //18B20
  {
    Ds18b20UpdateTemperature();  //��ȡ�¶�
  }
#elif(TEMPERATURE_SENSOR_TYPE == 2)  //HDC1080
  {
    Hdc1080_UpdateTemperature();
  }
#else
  {
////    Ds18b20UpdateTemperature();  //��ȡ�¶�
  }
#endif
}


/**
 * @brief   ����ʪ��
 *
 * @param   ��
 *
 * @retval  ��
 */
void UpdateHumidity(void)
{
#if(TEMPERATURE_SENSOR_TYPE == 1)  //18B20
  {
  
  }
#elif(TEMPERATURE_SENSOR_TYPE == 2)  //HDC1080
  {
    Hdc1080_UpdateHumidity();
  }
#else
  {
    
  }
#endif
}



/**
 * @brief   �����¶ȴ洢
 *
 * @param   ��
 *
 * @retval  ��
 */
void SensorSamplePoll(void)
{
  static uint32_t s_u32TimeSample  = 0xFFFF;//����ʱ��
  static uint32_t s_u32TimeCollect = 0xFFFF;//ÿ  ��¼һ������//��ʼ��Ϊ�ϴ�ֵ��Ϊ�˵�һ������ʱ���ɼ�һ������
  static uint32_t s_u32TimeSend    = 0xFFFF;//ÿ  ����һ֡���� 
  
  if(SysTickGetSecDif(s_u32TimeSample, SysTickGetSec()) > 60)//ÿ1��������һ��
  {
    s_u32TimeSample = SysTickGetSec(); 
    UpdateTemperature();//�ɼ��¶�ֵ
    UpdateHumidity();   //�ɼ�ʪ��
      
    if(SysTickGetSecDif(s_u32TimeCollect, SysTickGetSec()) > 60*g_u16RecordInterval)//ÿ  ��¼һ������
    {
      s_u32TimeCollect = SysTickGetSec(); 
      RecordApp_DevData();//�ݴ�һ������
      
      if(SysTickGetSecDif(s_u32TimeSend, SysTickGetSec()) > 60*g_u8SendInterval)//ÿ  ����һ֡����
      {
        s_u32TimeSend = SysTickGetSec();
        
        if(REC_OK == RecordApp_GenerateData()) {//�������ݡ�
          RecordApp_PackData();//������ݡ�
        }
           
      }
      
    }
  }

}

////#if(USER_AGREEMENT == 2)  //����MODBUSЭ��
////  {
////    ayModbusCycleTemperatureStoragePoll();
////  }
////#elif(USER_AGREEMENT == 3)  //���Ϻ󹤴����Э��
////  {
////    gdkyV2CycleTemperatureStoragePoll();
////  }
////#elif(USER_AGREEMENT == 4)  //���Ϻ󹤴����CoAPЭ��
////  {
////    gdkyV2CoapCycleTemperatureStoragePoll();
////  }
////#elif(USER_AGREEMENT == 5)  //���Ϻ󹤴����OneNetЭ��
////  {
////    gdkyV2OneNetCycleTemperatureStoragePoll();
////  }
////#else
////  {
////    
////  }
////#endif

/**
 * @}
 */



