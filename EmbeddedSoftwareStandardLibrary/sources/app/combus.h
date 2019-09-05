/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     combus.h
  * @author   ZouZH
  * @version  V1.01
  * @date     29-Aug-2018
  * @brief    Multi-communication bus management.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __COMBUS_H
#define __COMBUS_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/** 
 * @addtogroup ComBus
 * @{
 */ 

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * Bus error type
 */
typedef enum
{
  BUS_OK = 0x00,
  BUS_ERR_Param,
  BUS_ERR_Busy,
  BUS_ERR_Timeout,
}BusError_t;

/**
 * bus type
 */
typedef enum
{
  BUS_MBUS_0 = 0,
  BUS_RS485_0,
  BUS_NUM,
}BusType_t;

/**
 * bus wait (Flag Bit) type
 */
typedef enum
{
  BUS_FB_UM_RT = 0,      /* 户表实时 */
  BUS_FB_UM_CYCLE,       /* 户表周期 */

}BusFlagBit_t;

/* bus wait flag type */
typedef uint16_t BUS_FLAG_TYPE_t;

/**
 * bus manager object
 */
typedef struct
{
  BUS_FLAG_TYPE_t pendingFlag;  /* Waiting to use the bus bit flag, Bit0 first use */
  uint8_t         runBit;       /* The bit index currently in use(1-sizeof(BUS_FLAG_TYPE_t)), 0-Bus Idle */
}BusObj_t;


/* MACROS  ------------------------------------------------------------------ */

#define BUS_FLAG_BIT_NUM  (8 * sizeof(BUS_FLAG_TYPE_t))

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */
BusError_t BusInit(BusType_t BUS_x);
void BusPoll(void);
BusError_t BusSetWaitFlag(BusType_t BUS_x, uint8_t bitIndex);
BusError_t BusClearWaitFlag(BusType_t BUS_x, uint8_t bitIndex);
BusError_t BusCheckWaitFlag(BusType_t BUS_x, uint8_t bitIndex);
BusError_t BusIsIdle(BusType_t BUS_x);
BusError_t BusWaitIdle(BusType_t BUS_x, uint8_t bitIndex, uint32_t tmout);


/* Bus hardware interface */
uint8_t    BusSetParam(BusType_t BUS_x, uint32_t baud, uint8_t parity);
uint16_t   BusSend(BusType_t BUS_x, const uint8_t *pbuf, uint16_t len);
uint8_t    BusCheckRecv(BusType_t BUS_x);
uint16_t   BusGetRecvLenMax(BusType_t BUS_x);
uint16_t   BusGetRecvLen(BusType_t BUS_x);
void      *BusGetRecvBuff(BusType_t BUS_x);
uint32_t   BusGetLastRecvTick(BusType_t BUS_x);
void       BusEnableRx(BusType_t BUS_x);
void       BusDisableRx(BusType_t BUS_x);


/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __COMBUS_H */

/**
 * @}
 */ 

/***************************** END OF FILE ************************************/

