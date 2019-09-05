/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     combus.c
  * @author   ZouZH
  * @version  V1.01
  * @date     29-Aug-2018
  * @brief    Multi-communication bus management.
  ******************************************************************************
  * @history
  */
#define LOG_TAG    "BUS"

/* INCLUDES ----------------------------------------------------------------- */
#include "combus.h"
#include "includes.h"

#include "mbus.h"
#include "rs485.h"

/** 
 * @defgroup ComBus
 * @brief 总线通讯通用接口
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */
static BusObj_t s_tBusObj[BUS_NUM];

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


/**
 * @brief  Init bus
 *
 * @param  BUS_x: Bus Type
 *
 * @retval 0 Success
 */
BusError_t BusInit(BusType_t BUS_x)
{
  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      SYSEnterCritical();
      lib_memset(&s_tBusObj[BUS_MBUS_0], 0, sizeof(BusObj_t));
      SYSExitCritical();

      MBUSInit();
    }break;

    case BUS_RS485_0:
    {
      SYSEnterCritical();
      lib_memset(&s_tBusObj[BUS_RS485_0], 0, sizeof(BusObj_t));
      SYSExitCritical();

      RS485Init();
    }break;

    default:
    {
      SYSEnterCritical();
      lib_memset(&s_tBusObj[0], 0, sizeof(BusObj_t) * 2);
      SYSExitCritical();

      MBUSInit();
      RS485Init();
    }break;
  }

  return BUS_OK;
}

/**
 * @brief  Bus time division multiplex access poll
 *
 * @param  None
 *
 * @retval None
 */
void BusPoll(void)
{
  uint8_t i = 0;
  uint8_t j = 0;

  for (i = 0; i < BUS_NUM; i++)
  {
    if (s_tBusObj[i].runBit)
      continue;

    for (j = 0; j < BUS_FLAG_BIT_NUM; j++)
    {
      if (BIT_READ(s_tBusObj[i].pendingFlag, BIT(j)))
      {
        s_tBusObj[i].runBit = j + 1;
        BIT_CLEAR(s_tBusObj[i].pendingFlag, BIT(j));
      }
    }
  }

  MBUSProtectPoll();
}

/**
 * @brief  Set bus wait flag bit
 *
 * @param  BUS_x:    Bus Type(<BUS_NUM)
 * @param  bitIndex: Bus bit index(0-sizeof(BUS_FLAG_TYPE_t))
 *
 * @retval 0 Success
 */
BusError_t BusSetWaitFlag(BusType_t BUS_x, uint8_t bitIndex)
{
  if ((BUS_x >= BUS_NUM) || (bitIndex >= BUS_FLAG_BIT_NUM))
    return BUS_ERR_Param;

  SYSEnterCritical();
  if (!(s_tBusObj[BUS_x].pendingFlag & ~((BUS_FLAG_TYPE_t)(1 << bitIndex))) && !s_tBusObj[BUS_x].runBit)
  {
    s_tBusObj[BUS_x].runBit = bitIndex + 1;
    s_tBusObj[BUS_x].pendingFlag &= ~((BUS_FLAG_TYPE_t)(1 << bitIndex));
  }

  if (s_tBusObj[BUS_x].runBit != (bitIndex + 1))
    s_tBusObj[BUS_x].pendingFlag |= ((BUS_FLAG_TYPE_t)(1 << bitIndex));
  SYSExitCritical();

  return BUS_OK;
}

/**
 * @brief  Clear bus wait flag bit
 *
 * @param  BUS_x:    Bus Type(<BUS_NUM)
 * @param  bitIndex: Bus bit index(0-sizeof(BUS_FLAG_TYPE_t))
 *
 * @retval 0 Success
 */
BusError_t BusClearWaitFlag(BusType_t BUS_x, uint8_t bitIndex)
{
  if ((BUS_x >= BUS_NUM) || (bitIndex >= BUS_FLAG_BIT_NUM))
    return BUS_ERR_Param;

  SYSEnterCritical();
  s_tBusObj[BUS_x].pendingFlag &= ~((BUS_FLAG_TYPE_t)(1 << bitIndex));
  if (s_tBusObj[BUS_x].runBit == (bitIndex + 1))
    s_tBusObj[BUS_x].runBit = 0;
  SYSExitCritical();

  return BUS_OK;
}

/**
 * @brief  Check bus wait flag bit
 *
 * @param  BUS_x:    Bus Type(<BUS_NUM)
 * @param  bitIndex: Bus bit index(0-sizeof(BUS_FLAG_TYPE_t))
 *
 * @retval 0 Success
 */
BusError_t BusCheckWaitFlag(BusType_t BUS_x, uint8_t bitIndex)
{
  BusError_t retErr = BUS_ERR_Busy;

  if ((BUS_x >= BUS_NUM) || (bitIndex >= BUS_FLAG_BIT_NUM))
    return BUS_ERR_Param;

  SYSEnterCritical();
  if (s_tBusObj[BUS_x].runBit == (bitIndex + 1))
    retErr = BUS_OK;
  SYSExitCritical();

  return retErr;
}

/**
 * @brief  Check whether the bus is idle
 *
 * @param  BUS_x: Bus Type(<BUS_NUM)
 *
 * @retval 0 Success
 */
BusError_t BusIsIdle(BusType_t BUS_x)
{
  if (BUS_x >= BUS_NUM)
    return BUS_ERR_Param;

  if (s_tBusObj[BUS_x].pendingFlag || s_tBusObj[BUS_x].runBit)
    return BUS_ERR_Busy;

  return BUS_OK;
}

/**
 * @brief  Wait bus idle
 *
 * @param  BUS_x:    Bus Type(<BUS_NUM)
 * @param  bitIndex: Bus bit index(0-sizeof(BUS_FLAG_TYPE_t))
 * @param  tmout:    Maxium wait time
 *
 * @retval 0 Success
 */
BusError_t BusWaitIdle(BusType_t BUS_x, uint8_t bitIndex, uint32_t tmout)
{
  BusError_t retErr = BUS_ERR_Busy;

  uint32_t curtick = SYSGetTickCount() + SYS_MS_TO_TICKS(tmout);

  if ((BUS_x >= BUS_NUM) || (bitIndex >= BUS_FLAG_BIT_NUM))
    return BUS_ERR_Param;

  BusSetWaitFlag(BUS_x, bitIndex);

  while ((SYSGetTickCount() < curtick) && (BUS_OK != retErr))
  {
    retErr = BusCheckWaitFlag(BUS_x, bitIndex);
    SYSDelayMs(10);
  }

  if (BUS_OK != retErr)
    BusClearWaitFlag(BUS_x, bitIndex);

  return BusCheckWaitFlag(BUS_x, bitIndex);
}


/**
 * @brief  Set communication parameter
 *
 * @param  BUS_x:  Bus Type(<=BUS_NUM)
 * @param  baud:   baudrate
 * @param  parity: parity bit(0-None 1-Odd 2-Even)
 *
 * @retval 0 Success
 */
uint8_t BusSetParam(BusType_t BUS_x, uint32_t baud, uint8_t parity)
{
  uint8_t ret = 0;

  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      ret = MBUSSetParam(baud, parity);
    }break;

    case BUS_RS485_0:
    {
      ret = RS485SetParam(baud, parity);
    }break;

    default:
    {
      ret = MBUSSetParam(baud, parity);
      ret = RS485SetParam(baud, parity);
    }break;
  }
  return ret;
}


/**
 * @brief  Send data
 *
 * @param  BUS_x: Bus Type(<=BUS_NUM)
 * @param  pbuf:  data buffer
 * @param  len:   data length
 *
 * @return Actual send length
 */
uint16_t BusSend(BusType_t BUS_x, const uint8_t *pbuf, uint16_t len)
{
  uint16_t ret = 0;

#ifdef USE_FULL_ASSERT
  uint16_t i = 0;
  log_i("[Bus Send]port=%d, len=%d", BUS_x, len);
  for (i = 0; i < len; i++)
  {
    xprintf("%02X ", pbuf[i]);
    if (!((i + 1) % 32))
      xprintf("\r\n");
  }
  xprintf("\r\n");
#endif

  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      ret = MBUSSend(pbuf, len);
    }break;

    case BUS_RS485_0:
    {
      ret = RS485Send(pbuf, len);
    }break;

    default:
    {
      ret = MBUSSend(pbuf, len);
      ret = RS485Send(pbuf, len);
    }break;
  }
  return ret;
}

/**
 * @brief  Checks whether a data frame has been received.
 *
 * @param  BUS_x: Bus Type(<BUS_NUM)
 *
 * @retval 1 Have one 0 Nothing
 */
uint8_t BusCheckRecv(BusType_t BUS_x)
{
  uint8_t ret = 0;

  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      ret = MBUSCheckReceive();
    }break;

    case BUS_RS485_0:
    {
      ret = RS485CheckReceive();
    }break;

    default:
    {
      ret = 0;
    }break;
  }

#ifdef USE_FULL_ASSERT
if (ret)
{
  uint16_t i = 0;
  uint16_t len = BusGetRecvLen(BUS_x);
  const uint8_t *pbuf = BusGetRecvBuff(BUS_x);
  log_i("[Bus Recv]port=%d, len=%d", BUS_x, len);
  for (i = 0; i < len; i++)
  {
    xprintf("%02X ", pbuf[i]);
    if (!((i + 1) % 32))
      xprintf("\r\n");
  }
  xprintf("\r\n");
}
#endif

  return ret;
}

/**
 * @brief  Get receive data max length.
 *
 * @param  BUS_x: Bus Type(<BUS_NUM)
 *
 * @return Receive data length
 */
uint16_t BusGetRecvLenMax(BusType_t BUS_x)
{
  uint16_t ret = 0;

  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      ret = MBUS_MAX_RLEN;
    }break;

    case BUS_RS485_0:
    {
      ret = RS485_MAX_RLEN;
    }break;

    default:
    {
      ret = 0;
    }break;
  }
  return ret;
}


/**
 * @brief  Get receive data length.
 *
 * @param  BUS_x: Bus Type(<BUS_NUM)
 *
 * @return Receive data length
 */
uint16_t BusGetRecvLen(BusType_t BUS_x)
{
  uint16_t ret = 0;

  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      ret = MBUSGetReceiveLength();
    }break;

    case BUS_RS485_0:
    {
      ret = RS485GetReceiveLength();
    }break;

    default:
    {
      ret = 0;
    }break;
  }
  return ret;
}

/**
 * @brief  Get receive data buffer point.
 *
 * @param  BUS_x: Bus Type(<BUS_NUM)
 *
 * @return Receive data buffer point
 */
void *BusGetRecvBuff(BusType_t BUS_x)
{
  switch (BUS_x)
  {
    case BUS_MBUS_0:
      return g_ucMBUSRxBuf;
    case BUS_RS485_0:
      return g_ucRS485RxBuf;

    default:
      return (void*)0;
   }
}

/**
 * @brief  Get last receive char timestamp.
 *
 * @param  BUS_x: Bus Type(<BUS_NUM)
 *
 * @return Last receive char timestamp
 */
uint32_t BusGetLastRecvTick(BusType_t BUS_x)
{
  uint32_t ret = 0;

  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      ret = MBUSGetLastRecvCharTick();
    }break;

    case BUS_RS485_0:
    {
      ret = RS485GetLastRecvCharTick();
    }break;

    default:
    {
      ret = 0;
    }break;
  }
  return ret;
}

/**
 * @brief  Clear rx buff and enable receive
 *
 * @param  BUS_x: Bus Type(<BUS_NUM)
 *
 * @return None
 */
void BusEnableRx(BusType_t BUS_x)
{
  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      MBUSPower(1);
      MBUSEnableRx();
    }break;

    case BUS_RS485_0:
    {
      RS485Power(1);
      RS485EnableRx();
    }break;

    default:
    {
      MBUSPower(1);
      MBUSEnableRx();
    }break;
  }
}

/**
 * @brief  Stop receive
 *
 * @param  BUS_x: Bus Type(<BUS_NUM)
 *
 * @return None
 */
void BusDisableRx(BusType_t BUS_x)
{
  switch (BUS_x)
  {
    case BUS_MBUS_0:
    {
      MBUSDisableRx();
      MBUSPower(0);
    }break;

    case BUS_RS485_0:
    {
      RS485DisableRx();
      RS485Power(0);
    }break;

    default:
    {
      MBUSPower(0);
      RS485Power(0);
      MBUSDisableRx();
      RS485DisableRx();
    }break;
  }
}

/**
 * @}
 */ 

