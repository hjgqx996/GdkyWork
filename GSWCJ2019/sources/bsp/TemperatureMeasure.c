/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     TemperatureMeasure.c
  * @author   Tian Ruidong
  * @version  V1.00
  * @date     2017.8.22
  * @brief    温湿度测量程序.
  ******************************************************************************
  */

/* INCLUDES ------------------------------------------------------------------- */

/** 
 * @defgroup Temperature Measure
 * @brief 温湿度测量程序.
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

unsigned char T_able,H_able;  //读取温、湿度故障标志

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */



/**
 * @brief   传感器初始化
 *
 * @param   无
 *
 * @retval  无
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
 * @brief   启动一次温度转换
 *
 * @param   无
 *
 * @retval  无
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
 * @brief   更新温度
 *
 * @param   无
 *
 * @retval  无
 */
void UpdateTemperature(void)
{
#if(TEMPERATURE_SENSOR_TYPE == 1)  //18B20
  {
    Ds18b20UpdateTemperature();  //读取温度
  }
#elif(TEMPERATURE_SENSOR_TYPE == 2)  //HDC1080
  {
    Hdc1080_UpdateTemperature();
  }
#else
  {
////    Ds18b20UpdateTemperature();  //读取温度
  }
#endif
}


/**
 * @brief   更新湿度
 *
 * @param   无
 *
 * @retval  无
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
 * @brief   周期温度存储
 *
 * @param   无
 *
 * @retval  无
 */
void SensorSamplePoll(void)
{
  static uint32_t s_u32TimeSample  = 0xFFFF;//采样时间
  static uint32_t s_u32TimeCollect = 0xFFFF;//每  记录一次数据//初始化为较大值，为了第一次运行时，采集一次数据
  static uint32_t s_u32TimeSend    = 0xFFFF;//每  发送一帧数据 
  
  if(SysTickGetSecDif(s_u32TimeSample, SysTickGetSec()) > 60)//每1分钟运行一次
  {
    s_u32TimeSample = SysTickGetSec(); 
    UpdateTemperature();//采集温度值
    UpdateHumidity();   //采集湿度
      
    if(SysTickGetSecDif(s_u32TimeCollect, SysTickGetSec()) > 60*g_u16RecordInterval)//每  记录一次数据
    {
      s_u32TimeCollect = SysTickGetSec(); 
      RecordApp_DevData();//暂存一组数据
      
      if(SysTickGetSecDif(s_u32TimeSend, SysTickGetSec()) > 60*g_u8SendInterval)//每  发送一帧数据
      {
        s_u32TimeSend = SysTickGetSec();
        
        if(REC_OK == RecordApp_GenerateData()) {//生成数据。
          RecordApp_PackData();//打包数据。
        }
           
      }
      
    }
  }

}

////#if(USER_AGREEMENT == 2)  //安阳MODBUS协议
////  {
////    ayModbusCycleTemperatureStoragePoll();
////  }
////#elif(USER_AGREEMENT == 3)  //整合后工大科雅协议
////  {
////    gdkyV2CycleTemperatureStoragePoll();
////  }
////#elif(USER_AGREEMENT == 4)  //整合后工大科雅CoAP协议
////  {
////    gdkyV2CoapCycleTemperatureStoragePoll();
////  }
////#elif(USER_AGREEMENT == 5)  //整合后工大科雅OneNet协议
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



