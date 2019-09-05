/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_config.h
  * @author   ZouZH
  * @version  V1.00
  * @date     01-Aug-2018
  * @brief    Configuration for NB-IoT.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NB_CONFIG_H
#define __NB_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */

/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @defgroup NB-Config
 * @brief NB-IoT参数配置项
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */


/**
 * @brief  Enables (1) or disables (0) RTOS support (re-entrancy).
 *
 * @note   Enabled only when processing function is called from separate thread.
 *
 * @note   When this mode is enabled, RTOS dependant locking system is required for thread synchronization.
 */
#define NB_CFG_OS                      0
#define NB_CFG_ASYNC                   1


/**
 * @brief  Enables (1) or disables (0) Command execution function blocking.
 *
 * @note   When this mode is enabled, User need call @ref nbGetLastError() to check result of execution .
 */
#define NB_CFG_BLOCKING                1

/**
 * @brief  Enables (1) or disables (0) Profile Header(3Bytes).
 */
#define NB_CFG_USE_PROFILE             1
#define NB_CFG_LEN_PROFILE             3


/**
 * @brief  NB-IoT receive one AT command  in units of char.
 */
#define NB_CFG_AT_LINE_MAX             128


/**
 * @brief  NB-IoT receive buffer size in units of bytes.
 *
 * @note   Use as much as possible, but not less than @ref NB_CFG_AT_LINE_MAX bytes.
 *         For faster CPU you may be allowed to use smaller buffer.
 */
#define NB_CFG_BUFF_SIZE               256


/**
 * @brief  Timeout in milliseconds for mutex
 */
#define NB_CFG_OS_WAIT_TIME            180000


/**
 * @brief  Set data packet max size.
 */
#define NB_CFG_PKG_SIZE                256


/**
 * @brief  Default baudrate used for AT port
 */
#define NB_CFG_BAUDRATE                4800////9600


/**
 * @brief  Set ip and port for remote center.
 */
//#define NB_CFG_CONN_IP                 "180.101.147.115"  //测试平台
//#define NB_CFG_CONN_PORT               5683
#define NB_CFG_CONN_IP                 "117.60.157.137"  //正式平台
#define NB_CFG_CONN_PORT               5683


/**
 * @brief  Set number of retries for send data.
 */
#define NB_CFG_SEND_RETRIES            3


/**
 * @brief  Set time of enter to PSM mode(S).
 */
#define NB_CFG_TIME_PSM                30


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

