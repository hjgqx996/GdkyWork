/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     PowerManage.c
  * @author   scf
  * @version  V1.00
  * @date     03-June-2019
  * @brief    Power management, low power process..
  ******************************************************************************
  * @history
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "PowerManage.h"
#include "app_cfg.h"
#include "analogio.h"
#include "digitio.h"

/** 
 * @defgroup PowerManage
 * @brief Power management, low power process.
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */
static uint8_t  s_u8FlagLowVolt = 0;    //电源电压低标志。 1：电源电压低; 其他：正常
static uint16_t s_u16OriginVolt = 0;    //其实电压 用于判断电压下降过快异常。

/* LOCAL FUNCTIONS ---------------------------------------------------------- */



/**
 * @brief   电源管理初始化
 * @param   无
 * @retval  无
 */
void PM_Init(void)
{
#if(DEVICE_TYPE == 2)  //如果为锂电池型
  {
////    LOW_VOLTAGE_IN();     //端口置为输入
////    SET_LOW_VOLTAGE();    //P2.3口输出高    //低电检测
////    
////    CHARGE_INPUT_IN();    //P1.4输入
////    SET_CHARGE_INPUT();   //1.4口输出高  //充电检测
////    
////    CHRG_IN();    //充电检测口设置为输入
////    SET_CHRG();
////      
////    DONE_IN();   //充满电口设置为输入
////    SET_DONE();
  }
#elif(DEVICE_TYPE == 3)  //如果为开关型
  {
//    AIO_Init();//初始化电源电压AD采集
//    
//    LAMP_SWITCH_IN();     //LOS1,灯开关
//    SET_LAMP_SWITCH();    //
//    
//    LOW_VOLTAGE_IN();     //端口置为输入
//    SET_LOW_VOLTAGE();    //P2.3口输出高    //低电检测
  }
#endif
  
}

/**
 * @brief  Get flag of low voltage. 
 * @param  None
* @retval  Flag of low voltage. 1:valid.
 */
uint8_t PM_GetFlagLowVolt(void)
{
  return s_u8FlagLowVolt;
}

/**
 * @brief  Get power Voltage Level 
 * @param[in] u16OriginVolt: voltage to be compared.
 * @param[in] u32StartTime:  start time.
 * @retval 0: normal; 1:power abnormal
 */
uint8_t PM_PowerAbnormalDetect(uint32_t u32StartTime)
{
  uint8_t  u8Ret = 0;
  uint16_t u16CurrVolt = 0; //电源电压值
  uint32_t u32CurrTime = 0;
  
  u16CurrVolt = AIO_GetInVoltage(AIO_TYPE_IN1);
  
  if(u16CurrVolt < VOLT_LEVEL_POWER_OFF) {
    u8Ret = 1;
  }
  
  u32CurrTime = SysTickGetSec();
  if(SysTickGetSecDif(u32StartTime, u32CurrTime) > 5)//如果超时5S
  {  
    if(SysTickGetSecDif(u32StartTime, u32CurrTime) < 600)//如果没有10分钟超时
    {
      //如果法拉电容的电压比数据通信完成时下降了0.2V，则断电
      if((s_u16OriginVolt > u16CurrVolt) && ((s_u16OriginVolt - u16CurrVolt) >= 200))   
      {
        u8Ret = 1;
      }
    }
    else
    {
      if(SysTickGetSecDif(u32StartTime, u32CurrTime) < 1200)//如果没有20分钟超时
      {
        //如果法拉电容的电压比数据通信完成时下降了0.5V，则断电
        if((s_u16OriginVolt > u16CurrVolt) && ((s_u16OriginVolt - u16CurrVolt) >= 500))   
        {
          u8Ret = 1;
        }
      }
    }
  }
  else
  {
    s_u16OriginVolt = AIO_GetInVoltage(AIO_TYPE_IN1);//记录数据通信完成时的法拉电容电压  
  }
  
  return u8Ret;
}

/**
 * @brief   电源管理
 * @param   无
 * @retval  无
 */
void PM_Poll(void)
{
#if(DEVICE_TYPE == 3)  //如果为开关型
  static uint32_t s_u32TimeStart = 0;  
#endif
  
#if(DEVICE_TYPE == 2)  //如果为锂电池型
  {
  }
#elif(DEVICE_TYPE == 3)  //如果为开关型
  {
    if(DIO_GetInStatus(DIO_IN_DYD) == 0) {
      s_u8FlagLowVolt = 1;  //法拉电容低电
    }
    else {
      s_u8FlagLowVolt = 0;  //法拉电容电量正常
    }
    
    //电源电压周期采集
    if(SysTickGetMsDif(s_u32TimeStart, SysTickGetMs()) > CAP_VOLTAGE_ADC_CYCLE)
    {
      s_u32TimeStart = SysTickGetMs();
      AIO_InOnOff(1);//start AD sample
    }
  }
#endif
  
} 
  
  



/**
 * @}
 */

