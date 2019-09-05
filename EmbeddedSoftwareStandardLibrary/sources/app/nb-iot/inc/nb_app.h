/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_app.h
  * @author   ZouZH
  * @version  V1.00
  * @date     07-08-2018
  * @brief    NB-IoT network maintenance.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __NB_APP_H
#define __NB_APP_H 100

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>
   
#include "nb.h"

/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @addtogroup NB-App
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 *  运行状态
 */
typedef enum
{
  NB_State_Idle  = 0,
  NB_State_Restart,
  NB_State_Startup,
  NB_State_WaitStart,
  NB_State_ModInit,
  NB_State_NetConfig,
  NB_State_NetAttach,
  NB_State_WaitAttach,
  
  NB_State_SendData,
  NB_State_Running,
  NB_State_Suspend,
  NB_State_Error,
}NB_State_t;


/**
 * 数据缓存队列
 */
__packed typedef struct
{
  uint16_t len;    /* 数据长度 */
  uint8_t *pbuf;   /* 缓存数据 */
}NBDataBuff_t;


/**
 * 运行参数
 */
typedef struct
{
  NB_Error_t     error;        /* 错误代码 */
  NB_State_t     state;        /* 运行状态 */
  uint8_t        retryCnt;     /* 重试计数 */
  uint8_t        restartCnt;   /* 重启计数 */
  uint8_t        curConn;      /* 当前中心, 1主, 2备 */

  uint32_t       exeTick;      /* 执行时间戳(单位:ms) */
  uint32_t       psmTick;      /* 执行时间戳(单位:ms) */ 

  uint32_t       sendTime;     /* 成功发送数据时间戳(单位:S) */
  uint8_t        sendFlag;     /* 0空闲, 1准备发送, 2正在发送 */

#if NB_CFG_OS
  #include "FreeRTOS.h"
  #include "queue.h"

  QueueHandle_t  queueSend;    /* 发送数据队列 */
  QueueHandle_t  queueRecv;    /* 接收数据队列 */
#endif
}NBAppRunObj_t;


/* MACROS  ------------------------------------------------------------------ */

// Print
#define NB_Print log_d

#define NB_MS_TO_TICKS(xTimeInMs)  SYS_MS_TO_TICKS(xTimeInMs)

/* CONSTANTS  --------------------------------------------------------------- */

/**
 * 最大超时 60S
 */
#define NBAPP_MAX_TIMEOUT NB_MS_TO_TICKS(60000UL)


/**
 * 最多模块错误重试次数
 */
#define NBAPP_MAX_RETRY_CNT 0


/**
 * 数据缓存队列数据条数
 */
#define NBAPP_QUEUE_DATA_SEND 48
#define NBAPP_QUEUE_DATA_RECV 1


/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */
NB_Error_t nbAppInit(void);
NB_Error_t nbAppPoll(void);

NB_State_t nbAppNetPoll(void);
NB_State_t nbAppSetRunState(NB_State_t state);
NB_State_t nbAppGetRunState(void);
NB_Error_t nbAppGetLastError(void);
const NBAppRunObj_t *nbAppGetObj(void);

uint8_t    nbAppIsIdle(void);

uint16_t   nbAppWriteSendBuff(const void *pbuf, uint16_t len);
uint16_t   nbAppWriteSendBuffHist(const void *pbuf, uint16_t len);
NB_Error_t nbAppStartSend(void);

uint16_t   nbAppSendNetPkt(const void *pbuf, uint16_t len);

uint16_t   nbAppWriteRecvBuff(const void *pbuf, uint16_t len);


/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __NB_NB_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

