/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY  All Rights Reserved
  *
  * @file    app.c
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application for user
  ******************************************************************************
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "includes.h"
#include "lcd.h"
#include "TemperatureMeasure.h"
#include "analogio.h"
#include "usart0.h"
#include "flash.h"
#include "nb_app.h"
#include "record_app.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/**
 * device address and software version
 */
//#pragma location = DeviceNum_add
//__root const uint8_t DeviceNum[APP_DEV_ADDR_SIZE] = { APP_DEV_ADDR_0, 
//                                                      APP_DEV_ADDR_1, 
//                                                      APP_DEV_ADDR_2, 
//                                                      APP_DEV_ADDR_3};//设备号

const uint8_t g_cSoftVer[APP_SOFT_VER_SIZE] = {APP_VER_TYPE, APP_VER_FUNC};////, APP_VER_DEV, APP_VER_CODE};

//协议版本号
const uint8_t ProtocolVerNum[APP_PROTOCOL_VER_SIZE] = { PROTOCOL_VER_NUM_ADDR0,
                                                        PROTOCOL_VER_NUM_ADDR1,
                                                        PROTOCOL_VER_NUM_ADDR2,
                                                        PROTOCOL_VER_NUM_ADDR3,
                                                        PROTOCOL_VER_NUM_ADDR4,
                                                        PROTOCOL_VER_NUM_ADDR5}; 
uint8_t   g_u8ModemType     = GPRS_MODEM_TYPE;//模块型号

uint8_t   g_u8TempCalibrateEn = 0; //温度修正标志  0不修正   1修正
int8_t    g_i8TempCalibrate = 0;//温度修正值（8位有符号二进制*10）范围 ±3.9℃

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

/**
 * @brief  hardware initialise
 *
 * @param  none
 *
 * @retval none
 */
static void AppInitHard(void)
{
}

/**
 * @brief  Globle variable initialise
 * @param  none
 * @retval none
 */
void AppInitVariable(void)
{
  
}


/**
 * @brief Init task function
 *
 * @param[in] task parameter pointer
 *
 * @retval none
 */
void vAppTaskInit(void *pvParameters)
{

  (void)pvParameters;

  AppInitHard();
  AppInitVariable();
  
  nbAppInit();      //网络初始化
  RecordApp_Init();
////
////  while (1)
////  {
////  }
}
