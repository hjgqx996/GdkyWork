/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_config.h
  * @author   ZouZH
  * @version  V1.00
  * @date     01-Aug-2018
  * @brief    NB-IoT.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NB_H
#define __NB_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include "nb_config.h"
#include "nb_typedef.h"
#include "nb_private.h"
#include "nb_parser.h"
#include "nb_buffer.h"

#include "nb_hw.h"
#include "nb_os.h"

/** 
 * @addtogroup NB-IoT
 * @{
 */ 

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

NB_Error_t nbInit(uint32_t baud, NB_EventCallback_t cb);
NB_Error_t nbInitCmd(void);
const NB_t *nbGetObj(void);


NB_Error_t nbGENSetCFUN(uint8_t func);
NB_Error_t nbGENGetCFUN(uint8_t *func);

NB_Error_t nbINFOGetIMEI(char* str, uint8_t length);
NB_Error_t nbINFOGetIMSI(char* str, uint8_t length);
NB_Error_t nbINFOGetRSSI(uint8_t* rssi);

NB_Error_t nbDATEGetTime(NB_Datetime_t* datetime);

NB_Error_t nbNETGetStaus(void);
NB_Error_t nbNETGetIPAddr(void);
NB_Error_t nbNETGetCSCON(uint8_t *stat);

NB_Error_t nbNETSetAPN(const char* apn, const char* user, const char* pwd);
NB_Error_t nbNETSetConn(const char* host, uint16_t port);

NB_Error_t nbNETAttach(void);
NB_Error_t nbNETDetach(void);

NB_Error_t nbNETSend(const void* data, uint16_t btw, uint16_t* bw, NB_SendMode_t mode);
NB_Error_t nbNETRead(void* data, uint16_t btr, uint16_t* br);

NB_Error_t nbDelay(uint32_t timeout);
NB_Error_t nbIsIdle(void);
NB_Error_t nbWaitIdle(uint32_t timeout);
NB_Error_t nbGetLastError(void);


NB_Error_t nbUpdate(void);
NB_Error_t nbUpdateTime(uint32_t millis);
NB_Error_t nbDataReceived(uint8_t* ch, uint16_t count);
NB_Error_t nbProcessCallbacks(void);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __NB_H */

/**
 * @}
 */ 

/***************************** END OF FILE ************************************/

