/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_typedef.h
  * @author   ZouZH
  * @version  V1.00
  * @date     01-Aug-2018
  * @brief    NB-IoT typedef.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NB_TYPEDEF_H
#define __NB_TYPEDEF_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @defgroup NB-Config
 * @brief NB-IoT类型定义
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * @brief  Result enumeration used across application functions
 */
typedef enum {
    NB_OK = 0,                         /*!< Returned OK */
    NB_ERR,
    NB_BUSY,                           /*!< Device is busy right now */
    NB_TIMEOUT,                        /*!< Timeout occurred on command */

    NB_NETNOTRDY,                      /*!< Network is not ready to operate */
    NB_SENDING,                        /*!< Operation is in progress */
    NB_SENDOK,                         /*!< Send data success on connection */
    NB_SENDFAIL,                       /*!< Send data failed on connection */
    
    NB_ERRPAR,                         /*!< Wrong parameters on function call */
    NB_ERRMEM,                         /*!< Memory error occurred */
    NB_ERRHW,                          /*!< Hardware error occurred */
} NB_Error_t;


/**
 * @brief  List of possible connection types
 */
typedef enum {
    NB_CONN_TYPE_CoAP,                 /*!< Connection type is CoAP */
    NB_CONN_TYPE_UDP,                  /*!< Connection type is UDP */
} NB_ConnType_t;


/**
 * @brief  List of possible Send mode
 */
typedef enum {
    NB_SEND_MODE_NON,                 /*!< No reply data */
    NB_SEND_MODE_CON,                 /*!< Reply data */
    NB_SEND_MODE_NON_R,               /*!< No reply data, release now */
    NB_SEND_MODE_CON_R,               /*!< Reply data once */
} NB_SendMode_t;


/**
 * @brief  Network status
 */
typedef enum {
    NB_NET_NotReg     = 0x00,          /*!< Not registered and not searching */
    NB_NET_RegHome    = 0x01,          /*!< Registered in home network */
    NB_NET_Searching  = 0x02,          /*!< Searching for network */
    NB_NET_RegDenied  = 0x03,          /*!< Registration has been denied */
    NB_NET_Unknown    = 0x04,          /*!< Unknown status */
    NB_NET_RegRoaming = 0x05           /*!< Registered and roaming */
} NB_NetStatus_t;

/**
 * @brief  List of possible event callback types received to user
 */
typedef enum {
    NB_EVT_IDLE,                       /*!< Device reset detected */

    NB_EVT_Attach,                     /*!< Attached */
    NB_EVT_AttachError,                /*!< Error trying to attached */
    NB_EVT_Detached,                   /*!< Connection was just closed */
    
    NB_EVT_Connected,                  /*!< Connected */

    NB_EVT_Received,                   /*!< Connection data received */
    NB_EVT_Sended,                     /*!< Data were successfully sent */
    NB_EVT_SendError,                  /*!< Error trying to send data */
} NB_CB_EvtType_t;

/**
 * @brief   Global callback structure to pass as parameter to callback function
 */
typedef struct {
    NB_CB_EvtType_t type;              /*!< Event type */
    union {
      
        struct {
            uint16_t sentlen;
        } send;

        struct {
            uint16_t recvlen;
        } recv;
    }UN;
} NB_CB_Param_t;


/**
 * @brief        Event function prototype
 * @param[in]    Callback event
 * @return       NB_Error_t
 */
typedef NB_Error_t (*NB_EventCallback_t)(NB_CB_Param_t* cbParam);

/**
 * @brief  Date and time structure
 */
typedef struct {
    uint8_t year;                      /*!< Year */
    uint8_t month;                     /*!< Month 1~12 */
    uint8_t day;                       /*!< Day 1~31 */
    uint8_t hour;                      /*!< Hour 0~23 */
    uint8_t minute;                    /*!< Minute 0~59 */
    uint8_t second;                    /*!< Second 0~59 */
    int8_t  zone;                      /*!< TimeZone hour + zone/4 */
} NB_Datetime_t;


/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __NB_CONFIG_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

