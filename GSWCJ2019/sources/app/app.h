/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY  All Rights Reserved
  *
  * @file    app.h
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application for user
  ******************************************************************************
  */


/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>
#include "app_cfg.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

//extern const uint8_t DeviceNum[APP_DEV_ADDR_SIZE];
extern const uint8_t g_cSoftVer[APP_SOFT_VER_SIZE];
//协议版本号
extern const uint8_t ProtocolVerNum[APP_PROTOCOL_VER_SIZE];
extern uint8_t  g_u8ModemType;//模块型号

extern uint8_t  g_u8TempCalibrateEn; //温度修正标志  0不修正   1修正
extern int8_t   g_i8TempCalibrate  ;//温度修正值（8位有符号二进制*10）范围 ±3.9℃

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

void vAppTaskInit(void *pvParameters);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __APP_H */

/***************************** END OF FILE ************************************/

