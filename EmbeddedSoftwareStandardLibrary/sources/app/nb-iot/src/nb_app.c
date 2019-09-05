/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_app.c
  * @author   ZouZH
  * @version  V1.00
  * @date     07-08-2018
  * @brief    NB network maintenance.
  ******************************************************************************
  * @history
  */
#define LOG_TAG    "NB"

/* INCLUDES ----------------------------------------------------------------- */
#include "nb_app.h"

#include "includes.h"

/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @defgroup NB-App
 * @brief NB-IoT应用层(网络状态维护, 数据组包队列发送接收)
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

// Get tick count
#define NB_GetCurTick() (SYSGetTickCount() + NBAPP_MAX_TIMEOUT)


/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

// NBApp Run FSM Param
static NBAppRunObj_t s_tNBAppObj;

// Data buffer
static uint8_t s_ucNBAppSendBuf[NB_CFG_PKG_SIZE];
static uint8_t s_ucNBAppRecvBuf[NB_CFG_PKG_SIZE];

// NB Driver
static const NB_t *s_ptNBObj;

/* LOCAL FUNCTIONS ---------------------------------------------------------- */
static NB_Error_t nbAppEventCB(NB_CB_Param_t* cbParam);


/**
 * @brief  网络初始化
 * @param  无
 * @retval 0成功, 其他失败
 */
NB_Error_t nbAppInit(void)
{
  s_tNBAppObj.error = NB_OK;
  s_tNBAppObj.state = NB_State_Idle;

  s_tNBAppObj.retryCnt = 0;
  s_tNBAppObj.restartCnt = 0;
  
  s_ptNBObj = nbGetObj();
  s_tNBAppObj.error = nbInit(NB_CFG_BAUDRATE, nbAppEventCB);

  NB_Print("Module Init Result: %d", s_tNBAppObj.error);

  s_tNBAppObj.exeTick = NB_GetCurTick() - NBAPP_MAX_TIMEOUT;
  s_tNBAppObj.sendTime = SYSGetTimeSec();
  s_tNBAppObj.sendFlag = 0;

#if NB_CFG_OS  
  if (s_tNBAppObj.queueSend == NULL)
    s_tNBAppObj.queueSend = xQueueCreate(NBAPP_QUEUE_DATA_SEND, sizeof(NBDataBuff_t));

  if (s_tNBAppObj.queueRecv == NULL)
    s_tNBAppObj.queueRecv = xQueueCreate(NBAPP_QUEUE_DATA_RECV, sizeof(NBDataBuff_t));

  if ((s_tNBAppObj.queueSend == NULL) || (s_tNBAppObj.queueRecv == NULL))
    s_tNBAppObj.error = NB_ERRMEM;
#endif

  return s_tNBAppObj.error;
}

/**
 * @brief  NBApp轮询处理
 * @param  无
 * @retval 0成功, 其他失败
 */
NB_Error_t nbAppPoll(void)
{
  // Network FSM Poll
  nbAppNetPoll();

  return s_tNBAppObj.error;
}


/**
 * @brief  网络事件轮询处理
 * @param  无
 * @return 返回当前运行状态
 */
NB_State_t nbAppNetPoll(void)
{
  uint32_t curTick = 0;
  NB_State_t retState;

  curTick = NB_GetCurTick();
  retState = s_tNBAppObj.state;

  // Wait Timeout
  if ((curTick - s_tNBAppObj.exeTick) < NBAPP_MAX_TIMEOUT)
    return retState;

  switch (s_tNBAppObj.state)
  {
    case NB_State_Idle:
    {
      // Reset run parameters
      s_tNBAppObj.error = NB_OK;
      s_tNBAppObj.retryCnt = 0;
      //s_tNBAppObj.restartCnt = 0;

      s_tNBAppObj.sendTime = SYSGetTimeSec();

      s_tNBAppObj.state = NB_State_Startup;
      s_tNBAppObj.exeTick = NB_GetCurTick() - NBAPP_MAX_TIMEOUT;
    }break;

    case NB_State_Restart:
    {
      NB_Print("Restart %d times", s_tNBAppObj.restartCnt);

      if (s_tNBAppObj.sendFlag)
      {
        s_tNBAppObj.error = NB_SENDFAIL;
        s_tNBAppObj.sendFlag = 0;
      }

      nbHWSetReset(1);
      nbHWSetPower(0);
      
      s_tNBAppObj.restartCnt += 1;
      s_tNBAppObj.state = NB_State_Error;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
    }break;

    case NB_State_Startup:
    {
      // Power On
      s_tNBAppObj.error = nbInit(NB_CFG_BAUDRATE, nbAppEventCB);
      NB_Print("Startup Error= %d, PowerOn, Reset 2S...", s_tNBAppObj.error);
      
      nbHWSetReset(1);
      nbHWSetPower(1);
      
      s_tNBAppObj.retryCnt = 0;
      s_tNBAppObj.state = NB_State_WaitStart;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(2000);
    }break;

    case NB_State_WaitStart:
    {
      nbHWSetReset(0);
      NB_Print("Reset end!, Wait Start 5S...");
      
      s_tNBAppObj.retryCnt = 0;
      s_tNBAppObj.state = NB_State_ModInit;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(5000);
    }break;

    case NB_State_ModInit:
    {
      // Wait Init 1min
      if (++s_tNBAppObj.retryCnt > 6)
      {
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Restart;
        break;
      }

      // Module Init and Detach Network
      if ((NB_OK != (s_tNBAppObj.error = nbInitCmd())) || (NB_OK != (s_tNBAppObj.error = nbNETDetach())))
      {
        NB_Print("ModInit Fail! %dtimes, Error=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_tNBAppObj.error);

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);
        break;
      }

      NB_Print("ModInit OK! , Wait 3S...");
      s_tNBAppObj.retryCnt = 0;
      s_tNBAppObj.state = NB_State_NetConfig;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(3000);
    }break;

    case NB_State_NetConfig:
    {
      // Retry 3 times
      if (++s_tNBAppObj.retryCnt > 3)
      {
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Restart;
        break;
      }

      if (NB_OK != (s_tNBAppObj.error = nbNETSetConn(NB_CFG_CONN_IP, NB_CFG_CONN_PORT)))
      {
        NB_Print("NetConfig Fail! %dtimes, Error=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_tNBAppObj.error);
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);
        break;
      }

      NB_Print("NetConfig OK!, Wait 1S...");
      
      s_tNBAppObj.retryCnt = 0;
      s_tNBAppObj.state = NB_State_NetAttach;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(1000);
    }break;

    case NB_State_NetAttach:
    {
      // Retry 3 times
      if (++s_tNBAppObj.retryCnt > 3)
      {
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Restart;

        nbNETDetach();
        break;
      }

      if (NB_OK != (s_tNBAppObj.error = nbNETAttach()))
      {
        NB_Print("NetAttach Fail! %dtimes, Error=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_tNBAppObj.error);

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);
        break;
      }

      NB_Print("NetAttach OK!, Wait 3S...");
      s_tNBAppObj.retryCnt = 0;
      s_tNBAppObj.state = NB_State_WaitAttach;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(3000);
    }break;

    case NB_State_WaitAttach:
    {
      // Each timeout is 10sec, total 3minutes
      if (++s_tNBAppObj.retryCnt > 18)
      {
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Restart;
        break;
      }

      if (NB_OK != (s_tNBAppObj.error = nbINFOGetIMSI(NULL, 0)))
      {
        NB_Print("IMSI Get Fail! %dtimes, Error=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_tNBAppObj.error);
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10 * 1000);
        break;
      }

      NB_Print("IMSI=%s", s_ptNBObj->imsi);

      if ((NB_OK != (s_tNBAppObj.error = nbNETGetStaus())) || \
          ((s_ptNBObj->netStat != NB_NET_RegHome) && (s_ptNBObj->netStat != NB_NET_RegRoaming)))
      {
        NB_Print("NetRegSat Fail! %dtimes, Stat=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_ptNBObj->netStat);
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10 * 1000);
        break;
      }

      NB_Print("NetRegSat=%d", s_ptNBObj->netStat);

      nbINFOGetRSSI(NULL);
      NB_Print("NetCSQ=%d", s_ptNBObj->rssi);

      
      if ((NB_OK != (s_tNBAppObj.error = nbNETGetIPAddr())) || \
          !lib_memvalcmp(s_ptNBObj->ipAddr, sizeof(s_ptNBObj->ipAddr), 0x00))
      {
        NB_Print("NetIP Get Fail! %dtimes, Wait 10S...", s_tNBAppObj.retryCnt);
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10 * 1000);
        break;
      }

      NB_Print("NetIP=%d.%d.%d.%d", s_ptNBObj->ipAddr[0], s_ptNBObj->ipAddr[1], \
                                    s_ptNBObj->ipAddr[2], s_ptNBObj->ipAddr[3]);

      nbNETGetCSCON(NULL);
      NB_Print("NetCSCON=%d", s_ptNBObj->connStat);

      
      NB_Print("Net Connect OK!");
      s_tNBAppObj.retryCnt = 0;
      s_tNBAppObj.state = NB_State_Running;
        
      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(1000);
      s_tNBAppObj.psmTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(NB_CFG_TIME_PSM * 1000);
    }break;

    case NB_State_SendData:
    {
#if NB_CFG_OS
      uint16_t sendlen = 0;
      NBDataBuff_t databuff;

      // 读CSQ
      nbINFOGetRSSI(NULL);
      NB_Print("NetCSQ=%d", s_ptNBObj->rssi);
      
      if (!uxQueueMessagesWaiting(s_tNBAppObj.queueSend))
      {
        NB_Print("No Send Data...");
        s_tNBAppObj.sendFlag = 0;

        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Suspend;
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
        break; 
      }

      sendlen = 0;
      s_tNBAppObj.error = NB_OK;
      while (NB_SENDFAIL != s_tNBAppObj.error)
      {
        if (xQueuePeek(s_tNBAppObj.queueSend, &databuff, 0) != pdTRUE)
          break;
          
        // 组包发送
        if (sendlen + databuff.len <= NB_CFG_PKG_SIZE - NB_CFG_LEN_PROFILE)
        {
          if (xQueueReceive(s_tNBAppObj.queueSend, &databuff, 0) != pdTRUE)
            break;
          
          ASSERT(sendlen + databuff.len <= sizeof(s_ucNBAppSendBuf));
          lib_memcpy(&s_ucNBAppSendBuf[sendlen], databuff.pbuf, databuff.len);
          sendlen += databuff.len;
          SYSFree(databuff.pbuf);
          continue;
        }

        // 发送失败
        if (sendlen != nbAppSendNetPkt(s_ucNBAppSendBuf, sendlen))
        {
          s_tNBAppObj.error = NB_SENDFAIL;
          continue;
        }

        sendlen = 0;
      }

      // 最后一包数据
      if ((s_tNBAppObj.error != NB_SENDFAIL) && sendlen)
      {
        if (sendlen != nbAppSendNetPkt(s_ucNBAppSendBuf, sendlen))
          s_tNBAppObj.error = NB_SENDFAIL;
        else
          sendlen = 0;
      }

      // 发送失败
      if (s_tNBAppObj.error == NB_SENDFAIL)
      {
        // 保存未成功数据
        if (sendlen)
          nbAppWriteSendBuffHist(s_ucNBAppSendBuf, sendlen);
          
        NB_Print("Send Data Failed!, Error=%d", s_ptNBObj->cme);
        s_tNBAppObj.error = NB_SENDFAIL;
        s_tNBAppObj.sendFlag = 0;

        // 超时1天未成功发数,断电下次重新开始
        s_tNBAppObj.retryCnt = 0;
        if (SYSGetTimeSec() - s_tNBAppObj.sendTime >= SYSMakeSec(24, 0, 0))
          s_tNBAppObj.state = NB_State_Restart;
        else
          s_tNBAppObj.state = NB_State_Suspend;
       
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
        break; 
      }
#endif

      // 发送成功
      NB_Print("Send Data OK!");
      s_tNBAppObj.error = NB_OK;
      s_tNBAppObj.sendFlag = 0;
      s_tNBAppObj.sendTime = SYSGetTimeSec();
      
      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(1000);
      s_tNBAppObj.psmTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(NB_CFG_TIME_PSM * 1000);

      // 获取时间并校时
      if ((NB_OK == nbDATEGetTime(NULL)) && (s_ptNBObj->datetime.year >= 18) && (s_ptNBObj->datetime.year <= 99))
      {
        uint32_t tmsec = 0;
        SysTime_t tm;
        tmsec = SysMakeSecond(2000 + s_ptNBObj->datetime.year, s_ptNBObj->datetime.month, s_ptNBObj->datetime.day, \
                              s_ptNBObj->datetime.hour, s_ptNBObj->datetime.minute, s_ptNBObj->datetime.second);

        tmsec += ((int32_t)(s_ptNBObj->datetime.zone / 4)) * 3600;
        
        if (!SysMakeDateTime(tmsec, &tm) && (tm.year >= 0x2018) && (tm.year <= 0x2099))
        {
          NB_Print("NetSetTime=%04X-%02X-%02X %02X:%02X:%02X", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
          SysTimeSet(tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
        }
      }
      
      s_tNBAppObj.retryCnt = 0;
      s_tNBAppObj.state = NB_State_Running;
    }break;

    case NB_State_Running:
    {
      uint16_t br = 0;

      if (1 == s_tNBAppObj.sendFlag)
      {
        s_tNBAppObj.sendFlag = 2;
        
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_SendData;
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
        break;
      }

      s_tNBAppObj.error = nbNETRead(s_ucNBAppRecvBuf, sizeof(s_ucNBAppRecvBuf), &br);
      if ((s_tNBAppObj.error == NB_OK) && (br > 0))
      {
        //CmdRecvQueueWriteNB(s_ucNBAppRecvBuf, br);
      }

      //if ((curTick - s_tNBAppObj.psmTick) > NBAPP_MAX_TIMEOUT)
      {
        NB_Print("Enter PSM Mode, Suspend...");

        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Suspend;
        //break;
      }
      
      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(3 * 1000);
    }break;

    case NB_State_Suspend:
    {
      if (1 == s_tNBAppObj.sendFlag)
      {
        s_tNBAppObj.sendFlag = 2;

        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_SendData;
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
        break;
      }
    }break;

    case NB_State_Error:
    {
      if (1 == s_tNBAppObj.sendFlag)
      {
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Idle;
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
      }
    }break;

    default:
    {
      s_tNBAppObj.state = NB_State_Idle;
    }break;
  }

  /* Process callback checks */
  if (s_tNBAppObj.state > NB_State_ModInit)
    nbProcessCallbacks();

  return retState;
}

/**
 * @brief  设置运行状态
 * @param  state: 设置运行状态
 * @return 设置前状态
 */
NB_State_t nbAppSetRunState(NB_State_t state)
{
  NB_State_t retstat = s_tNBAppObj.state;

  s_tNBAppObj.retryCnt = 0;
  s_tNBAppObj.state = state;

  return retstat;
}


/**
 * @brief  获取运行状态
 * @param  无
 * @return 运行状态
 */
NB_State_t nbAppGetRunState(void)
{
  return s_tNBAppObj.state;
}


/**
 * @brief  获取最后运行错误代码
 * @param  无
 * @retval 0无错误
 */
NB_Error_t nbAppGetLastError(void)
{
  return s_tNBAppObj.error;
}


/**
 * @brief  获取NB应用对象指针
 * @param  无
 * @return 对象指针 @ref NBAppRunObj_t
 */
const NBAppRunObj_t *nbAppGetObj(void) {
    NBAppRunObj_t *pnb = &s_tNBAppObj;

    return pnb;
}


/**
 * @brief  是否空闲状态
 * @param  无
 * @retval 1空闲
 */
uint8_t nbAppIsIdle(void)
{
  return ((s_tNBAppObj.state >= NB_State_Suspend) && !s_tNBAppObj.sendFlag);
}

/**
 * @brief  组包方式数据包写入发送队列
 * @param[in]  pbuf: 数据缓存
 * @param[in]  len:  数据长度
 * @return 已写入字节数
 */
uint16_t nbAppWriteSendBuff(const void *pbuf, uint16_t len)
{
#if NB_CFG_OS
  NBDataBuff_t databuff;
  
  if ((NULL == pbuf) || !len || (len > NB_CFG_PKG_SIZE - NB_CFG_LEN_PROFILE))
  {
    NB_Print("Send Packet param error!!!");
    return 0;
  }

  // 队列满, 移除最早指令
  if (!uxQueueSpacesAvailable(s_tNBAppObj.queueSend))
  {
    if (xQueueReceive(s_tNBAppObj.queueSend, &databuff, 0) != pdTRUE)
      log_e("Send Packet xQueueReceive Failed!");
    else
      SYSFree(databuff.pbuf);
  }

  // 空间不足,移除最早指令
  do
  {
    databuff.len = len;
    databuff.pbuf = (uint8_t *)SYSMalloc(databuff.len);
    if (NULL != databuff.pbuf)
      break;

    if (xQueueReceive(s_tNBAppObj.queueSend, &databuff, 0) == pdTRUE)
    {
      SYSFree(databuff.pbuf);
      databuff.pbuf = NULL;
    }
  }while (uxQueueSpacesAvailable(s_tNBAppObj.queueSend));

  if (NULL == databuff.pbuf)
  {
    log_w("Send Malloc Failed!!!");
    return 0;
  }

  lib_memcpy(databuff.pbuf, pbuf, databuff.len);
  if (pdTRUE != xQueueSend(s_tNBAppObj.queueSend, &databuff, 0))
  {
    databuff.len = 0;
    SYSFree(databuff.pbuf);
    log_e("Send write queue is Failed!!!");
  }

  return databuff.len;
#else
  return 0;
#endif
}

/**
 * @brief  组包方式未发送成功的数据包重新写入发送队列
 * @param[in]  pbuf: 数据缓存
 * @param[in]  len:  数据长度
 * @return 已写入字节数
 */
uint16_t nbAppWriteSendBuffHist(const void *pbuf, uint16_t len)
{
#if NB_CFG_OS
  
  NBDataBuff_t databuff;
  
  if ((NULL == pbuf) || !len || (len > NB_CFG_PKG_SIZE - NB_CFG_LEN_PROFILE))
  {
    NB_Print("Send Packet param error!!!");
    return 0;
  }

  if (!uxQueueSpacesAvailable(s_tNBAppObj.queueSend))
    return 0;

  databuff.len = len;
  databuff.pbuf = (uint8_t *)SYSMalloc(databuff.len);
  if (NULL == databuff.pbuf)
  {
    log_w("Send Hist Malloc Failed!!!");
    return 0;
  }

  lib_memcpy(databuff.pbuf, pbuf, databuff.len);
  if (pdTRUE != xQueueSendToFront(s_tNBAppObj.queueSend, &databuff, 0))
  {
    databuff.len = 0;
    SYSFree(databuff.pbuf);
    log_e("Send Hist write queue is Failed!!!");
  }

  return databuff.len;
#else
  return 0;
#endif
}


/**
 * @brief  组包方式执行发送所有数据
 * @param  无
 * @retval 0成功, 其他失败
 */
NB_Error_t nbAppStartSend(void)
{
#if NB_CFG_OS
  
  if (!uxQueueMessagesWaiting(s_tNBAppObj.queueSend))
    return NB_ERRPAR;
    
  if (s_tNBAppObj.sendFlag == 2)
    return NB_SENDING;
  
  s_tNBAppObj.sendFlag = 1;
#endif
  return NB_OK;
}


/**
 * @brief  直接发送数据包
 * @param[in]  pbuf: 数据缓存
 * @param[in]  len:  数据长度
 * @return 已发送字节数
 */
uint16_t nbAppSendNetPkt(const void *pbuf, uint16_t len)
{
  uint16_t bw = 0;

  if ((NULL == pbuf) || !len)
  {
    NB_Print("Send Net Packet param error!!!");
    return 0;
  }

  s_tNBAppObj.error = nbNETSend(pbuf, len, &bw, NB_SEND_MODE_NON_R);
  if (NB_OK != s_tNBAppObj.error)
    bw = 0;

  return bw;
}


/**
 * @brief  组包方式将接收数据写入接收队列
 * @param[in]  pbuf: 数据缓存
 * @param[in]  len:  数据长度
 * @return 已写入字节数
 */
uint16_t nbAppWriteRecvBuff(const void *pbuf, uint16_t len)
{
#if NB_CFG_OS
  
  NBDataBuff_t databuff;
  
  if ((NULL == pbuf) || !len || (len > NB_CFG_PKG_SIZE))
  {
    NB_Print("Receive Net Packet param error!!!");
    return 0;
  }

  databuff.len = len;
  databuff.pbuf = (uint8_t *)SYSMalloc(databuff.len);
  if (NULL == databuff.pbuf)
  {
    log_w("Recv Malloc Failed!!!");
    return 0;
  }

  lib_memcpy(databuff.pbuf, pbuf, databuff.len);

  if (pdTRUE != xQueueSend(s_tNBAppObj.queueRecv, &databuff, 0))
  {
    databuff.len = 0;
    SYSFree(databuff.pbuf);
    log_w("Recv queue is full!!!");
  }

  return databuff.len;
#else
  return 0;
#endif
}



/**
 * @brief  NB-IoT驱动执行回调函数
 * @param[in]  cbParam: 回调参数 @ref NB_CB_Param_t
 * @return 0无错误
 */
NB_Error_t nbAppEventCB(NB_CB_Param_t* cbParam)
{
  switch (cbParam->type)
  {
    case NB_EVT_IDLE:
      NB_Print("NB_EVT_IDLE");
      break;

    case NB_EVT_Attach:
      NB_Print("NB_EVT_Attach");
      break;

    case NB_EVT_AttachError:
      NB_Print("NB_EVT_AttachError");
      break;

    case NB_EVT_Detached:
      NB_Print("NB_EVT_Detached");
      break;

    case NB_EVT_Connected:
      NB_Print("NB_EVT_Connected");
      break;

    case NB_EVT_Received:
      NB_Print("NB_EVT_Received %d Bytes", cbParam->UN.recv.recvlen);
      break;

    case NB_EVT_Sended:
      NB_Print("NB_EVT_Sended %d Bytes", cbParam->UN.send.sentlen);
      break;

    case NB_EVT_SendError:
      NB_Print("NB_EVT_SendError");
      break;
      
    default:
      NB_Print("Unknowns Event");
      break;
  }

  return NB_OK;
}

/**
 * @}
 */

/**
 * @}
 */

