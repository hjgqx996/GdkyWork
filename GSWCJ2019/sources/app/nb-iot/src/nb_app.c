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
#include "includes.h"
#include "nb_app.h"
#include "record_app.h"
#include "PowerManage.h"
#include "paramConfig.h"

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
NB_SendState_t s_tSendState = NB_SendState_Idle;

// Data buffer
static uint8_t  s_ucNBAppSendBuf[NB_CFG_PKG_SIZE];
static uint8_t  s_ucNBAppRecvBuf[NB_CFG_PKG_SIZE];
NBDataBuff_t    s_tdatabuff;
NBDataBuff_t    s_tdatabuffSend;

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
  nbUpdate();     //Poll update process module receive char and parse AT command
  nbAppNetPoll(); //网络事件轮询处理// Network FSM Poll

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
  uint32_t u32Tmp = 0;

  curTick = NB_GetCurTick();
  retState = s_tNBAppObj.state;
  static uint16_t s_u16br = 0;

  // Wait Timeout
  if ((curTick - s_tNBAppObj.exeTick) < NBAPP_MAX_TIMEOUT)
    return retState;

  switch (s_tNBAppObj.state)
  {
    case NB_State_Idle:
    {
      #if(DEVICE_TYPE == 3)  //如果为开关型
      u32Tmp = AIO_GetInVoltage(AIO_TYPE_IN1);
      if(u32Tmp < VOLT_LEVEL_MODULE_INIT)//法拉电容电压 < nb初始化电压
      {
        if (s_tNBAppObj.sendFlag)//发送标志置位，则存储一条历史数据
        {
          s_tNBAppObj.error = NB_SENDFAIL;
          s_tNBAppObj.sendFlag = 0;
          
          RecordApp_WriteTempCollect();//存储一条历史数据
        }
        nbHWSetReset(1);
        nbHWSetPower(0);
        s_tNBAppObj.state = NB_State_Error;
      }
      else
      #endif
      {
        // Reset run parameters
        s_tNBAppObj.error = NB_OK;
        s_tNBAppObj.retryCnt = 0;
        //s_tNBAppObj.restartCnt = 0;

        s_tNBAppObj.sendTime = SYSGetTimeSec();
        s_tNBAppObj.state = NB_State_Startup;
      }
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
      s_tNBAppObj.state = NB_State_ModInitWait;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
    }break;
    
    case NB_State_ModInitWait:// Module Init and Detach Network
    {
      s_tNBAppObj.error = nbInitCmd();
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//init running
      {}
      else if(NB_OK != s_tNBAppObj.error)//fail，retry.
      {
        NB_Print("ModInit Fail! %dtimes, Error=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_tNBAppObj.error);
        s_tNBAppObj.state = NB_State_ModInit;
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);    
      }
      else//Init OK
      {
        NB_Print("ModInit OK! , Wait 3S...");
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_NetConfig;

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(3000);
      }
    }break;
  
    case NB_State_NetConfig://Set remote IP and Port 
    {
      // Retry 3 times
      if (++s_tNBAppObj.retryCnt > 3)
      {
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Restart;
        break;
      }
      nbCmdRunStateClr();
      s_tNBAppObj.state = NB_State_NetConfigWait;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
    }break;
    
    case NB_State_NetConfigWait:
    {
      s_tNBAppObj.error = nbNETSetConn(NB_CFG_CONN_IP, NB_CFG_CONN_PORT);
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//NetConfig running
      {}
      else if(NB_OK != s_tNBAppObj.error)//fail，retry.
      {
        NB_Print("NetConfig Fail! %dtimes, Error=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_tNBAppObj.error);
        s_tNBAppObj.state = NB_State_NetConfig;
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);
      }
      else//NetConfig OK
      {
        NB_Print("NetConfig OK!, Wait 1S...");
        
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_NetAttach;
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(1000);
      }
    }break;
    
    case NB_State_NetAttach://Attach GSM base station
    {
      // Retry 3 times
      if (++s_tNBAppObj.retryCnt > 3)
      {
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_Restart;

        nbNETDetachRun();////nbNETDetach();
        
        ////???? delay wait?
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(3000);
        break;
      }
      nbCmdRunStateClr();
      s_tNBAppObj.state = NB_State_NetAttachWait;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
    }break;
    
    case NB_State_NetAttachWait://
    {
      s_tNBAppObj.error = nbNETAttach();
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//NetAttach running
      {}
      else if(NB_OK != s_tNBAppObj.error)//fail，retry.
      {
        NB_Print("NetAttach Fail! %dtimes, Error=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_tNBAppObj.error);
        s_tNBAppObj.state = NB_State_NetAttach;
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);
      }
      else//NetAttach OK
      {
        NB_Print("NetAttach OK!, Wait 3S...");
        s_tNBAppObj.retryCnt = 0;
        s_tNBAppObj.state = NB_State_WaitAttach;

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(3000);
      }
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
      nbCmdRunStateClr();
      s_tNBAppObj.state = NB_State_WaitAttachIMSI;

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
    }break;
    
    case NB_State_WaitAttachIMSI://Get IMSI
    {
      s_tNBAppObj.error = nbINFOGetIMSI(NULL, 0);
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//
      {}
      else if(NB_OK != s_tNBAppObj.error)//fail，retry.
      {
        NB_Print("IMSI Get Fail! %dtimes, Error=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_tNBAppObj.error);
        s_tNBAppObj.state = NB_State_WaitAttach;
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);
      }
      else// OK
      {
        NB_Print("IMSI=%s", s_ptNBObj->imsi);
        nbCmdRunStateClr();
        s_tNBAppObj.state = NB_State_WaitAttachGetStatus;

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
      }
    }break;
    
    case NB_State_WaitAttachGetStatus://Get "AT+CREG"
    {
      s_tNBAppObj.error = nbNETGetStaus();
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//
      {}
      else if((NB_OK != s_tNBAppObj.error) || \
          ((s_ptNBObj->netStat != NB_NET_RegHome) && (s_ptNBObj->netStat != NB_NET_RegRoaming)) )
      {
        NB_Print("NetRegSat Fail! %dtimes, Stat=%d, Wait 10S...", s_tNBAppObj.retryCnt, s_ptNBObj->netStat);
        s_tNBAppObj.state = NB_State_WaitAttach;
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);
      }
      else// OK
      {
        NB_Print("NetRegSat=%d", s_ptNBObj->netStat);
        nbCmdRunStateClr();
        s_tNBAppObj.state = NB_State_WaitAttachGetRSSI;//

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
      }
    }break;
    
    case NB_State_WaitAttachGetRSSI://Get RSSI ,"AT+CSQ"
    {
      s_tNBAppObj.error = nbINFOGetRSSI(NULL);
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//
      {}
      else// OK
      {
        NB_Print("NetCSQ=%d", s_ptNBObj->rssi);
        nbCmdRunStateClr();
        s_tNBAppObj.state = NB_State_WaitAttachGetIP;//

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
      }
    }break;
    
    case NB_State_WaitAttachGetIP://Get module ip address
    {
      s_tNBAppObj.error = nbNETGetIPAddr();
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//
      {}
      else if((NB_OK != s_tNBAppObj.error) || \
          !lib_memvalcmp(s_ptNBObj->ipAddr, sizeof(s_ptNBObj->ipAddr), 0x00) )
      {
        NB_Print("NetIP Get Fail! %dtimes, Wait 10S...", s_tNBAppObj.retryCnt);
        s_tNBAppObj.state = NB_State_WaitAttach;
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(10000);
      }
      else// OK
      {
        NB_Print("NetIP=%d.%d.%d.%d", s_ptNBObj->ipAddr[0], s_ptNBObj->ipAddr[1], \
                                    s_ptNBObj->ipAddr[2], s_ptNBObj->ipAddr[3]);
        nbCmdRunStateClr();
        s_tNBAppObj.state = NB_State_WaitAttachGetCSCON;//

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
      }
    }break;
    
    case NB_State_WaitAttachGetCSCON: //Get "AT+CSCON"
    {
      s_tNBAppObj.error = nbNETGetCSCON(NULL);
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//
      {}
      else// OK
      {
        NB_Print("NetCSCON=%d", s_ptNBObj->connStat);
        NB_Print("Net Connect OK!");
        s_tNBAppObj.retryCnt = 0;
        nbCmdRunStateClr();
        s_tNBAppObj.state = NB_State_Running;

        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(1000);
        s_tNBAppObj.psmTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(NB_CFG_TIME_PSM * 1000);
      }
    }break;

    case NB_State_SendData:
    {
      s_tNBAppObj.state = nbAppSendRun();
    }break;

    case NB_State_Running:
    {
      if (1 == s_tNBAppObj.sendFlag)
      {
        #if(DEVICE_TYPE == 3)  //如果为开关型
        u32Tmp = AIO_GetInVoltage(AIO_TYPE_IN1);
        if(u32Tmp < VOLT_LEVEL_SEND)//法拉电容电压 < nb发送数据电压
        {
          s_tNBAppObj.error = NB_SENDFAIL;
          s_tNBAppObj.sendFlag = 0;
          RecordApp_WriteTempCollect();//存储一条历史数据
            
          if(u32Tmp < VOLT_LEVEL_POWER_OFF)//法拉电容电压 < nb工作电压
          {
            nbHWSetReset(1);
            nbHWSetPower(0);
          }
          s_tNBAppObj.state = NB_State_Error;
        }
        else
        #endif
        {
          s_tNBAppObj.sendFlag = 2;

          s_tNBAppObj.retryCnt = 0;
          s_tSendState = NB_SendState_Idle;
          s_tNBAppObj.state = NB_State_SendData;
        }
        
        curTick = NB_GetCurTick();
        s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
        break;
      }
      else
      {
        if(1 == PM_PowerAbnormalDetect(s_tNBAppObj.sendTime)) { //电压下降过快 异常检测
          nbHWSetReset(1);
          nbHWSetPower(0);
          s_tNBAppObj.state = NB_State_Error;
          
          curTick = NB_GetCurTick();
          s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
          break;
        }
      }

      s_tNBAppObj.error = nbNETRead(s_ucNBAppRecvBuf, sizeof(s_ucNBAppRecvBuf), &s_u16br);

      if ((s_tNBAppObj.error == NB_OK) && (s_u16br > 0))
      {
        ParaParseFromNB(s_ucNBAppRecvBuf, s_u16br);
      }

////      if ((curTick - s_tNBAppObj.psmTick) > NBAPP_MAX_TIMEOUT)
////      {
////        NB_Print("Enter PSM Mode, Suspend...");
////
////        s_tNBAppObj.retryCnt = 0;
////        s_tNBAppObj.state = NB_State_Suspend;
////        //break;
////      }

      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;//// + NB_MS_TO_TICKS(3 * 1000);
    }break;

    case NB_State_Suspend:
    {
      if (1 == s_tNBAppObj.sendFlag)
      {
        #if(DEVICE_TYPE == 3)  //如果为开关型
        u32Tmp = AIO_GetInVoltage(AIO_TYPE_IN1);
        if(u32Tmp < VOLT_LEVEL_SEND)//法拉电容电压 < nb发送数据电压
        {
          s_tNBAppObj.error = NB_SENDFAIL;
          s_tNBAppObj.sendFlag = 0;
          RecordApp_WriteTempCollect();//存储一条历史数据
            
          if(u32Tmp < VOLT_LEVEL_POWER_OFF)//法拉电容电压 < nb工作电压
          {
            nbHWSetReset(1);
            nbHWSetPower(0);
          }
          s_tNBAppObj.state = NB_State_Error;
        }
        else
        #endif
        {
          s_tNBAppObj.sendFlag = 2;

          s_tNBAppObj.retryCnt = 0;
          s_tSendState = NB_SendState_Idle;
          s_tNBAppObj.state = NB_State_SendData;
        }

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
 * @brief  是否在发送中
 * @param  无
 * @retval 1:正在发送
 */
uint8_t nbAppIsSending(void)
{
  if(2 == s_tNBAppObj.sendFlag) {
    return 1;
  }
  else {
    return 0;
  }
}


/**
 * @brief  Get Signal strength
 * @param  None
 * @return rssi value
 */
uint8_t nbAppGetRSSI(void)
{
  return s_ptNBObj->rssi;
}

/**
 * @brief  Get Net State
 * @param  None
  * @return 1:registered; 0:Not registered
 */
uint8_t nbAppGetNetStat(void)
{
  if(   (s_ptNBObj->netStat == NB_NET_RegHome) 
     || (s_ptNBObj->netStat == NB_NET_RegRoaming))
  {
    return 1;
  }
  else {
    return 0;
  }
  
}

/**
 * @brief  组包方式数据包写入发送队列
 * @param[in]  pbuf: 数据缓存
 * @param[in]  len:  数据长度
 * @return 已写入字节数
 */
uint16_t nbAppWriteSendBuff(const void *pbuf, uint16_t len)
{
  if ((NULL == pbuf) || !len || ((len) > (NB_CFG_PKG_SIZE - NB_CFG_LEN_PROFILE)))
  {
    NB_Print("Send Packet param error!!!");
    return 0;
  }
  lib_memcpy(s_ucNBAppSendBuf, pbuf, len);
  s_tdatabuffSend.len = len;
  
  s_tNBAppObj.sendFlag = 1;//0空闲, 1准备发送, 2正在发送
    
  return 0;
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
  if ((NULL == pbuf) || !len || (len > NB_CFG_PKG_SIZE - NB_CFG_LEN_PROFILE))
  {
    NB_Print("Send Packet param error!!!");
    return 0;
  }

  if (!uxQueueSpacesAvailable(s_tNBAppObj.queueSend))
    return 0;

  s_tdatabuff.len = len;
  s_tdatabuff.pbuf = (uint8_t *)SYSMalloc(s_tdatabuff.len);
  if (NULL == s_tdatabuff.pbuf)
  {
    log_w("Send Hist Malloc Failed!!!");
    return 0;
  }

  lib_memcpy(s_tdatabuff.pbuf, pbuf, s_tdatabuff.len);
  if (pdTRUE != xQueueSendToFront(s_tNBAppObj.queueSend, &s_tdatabuff, 0))
  {
    s_tdatabuff.len = 0;
    SYSFree(s_tdatabuff.pbuf);
    log_e("Send Hist write queue is Failed!!!");
  }

  return s_tdatabuff.len;
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
 * @brief  组包方式将接收数据写入接收队列
 * @param[in]  pbuf: 数据缓存
 * @param[in]  len:  数据长度
 * @return 已写入字节数
 */
uint16_t nbAppWriteRecvBuff(const void *pbuf, uint16_t len)
{
#if NB_CFG_OS
  if ((NULL == pbuf) || !len || (len > NB_CFG_PKG_SIZE))
  {
    NB_Print("Receive Net Packet param error!!!");
    return 0;
  }

  s_tdatabuff.len = len;
  s_tdatabuff.pbuf = (uint8_t *)SYSMalloc(s_tdatabuff.len);
  if (NULL == s_tdatabuff.pbuf)
  {
    log_w("Recv Malloc Failed!!!");
    return 0;
  }

  lib_memcpy(s_tdatabuff.pbuf, pbuf, s_tdatabuff.len);

  if (pdTRUE != xQueueSend(s_tNBAppObj.queueRecv, &s_tdatabuff, 0))
  {
    s_tdatabuff.len = 0;
    SYSFree(s_tdatabuff.pbuf);
    log_w("Recv queue is full!!!");
  }

  return s_tdatabuff.len;
#else
  return 0;
#endif
}

/**
 * @brief  发送数据运行接口
 * @param  无
 * @retval NB net poll state machine's state.
 */
NB_State_t nbAppSendRun(void)
{
  NB_State_t  retState = NB_State_SendData;/* 运行状态 */
  NB_Error_t  resError = NB_CMD_WAIT;
  uint32_t    curTick = 0;
  uint16_t    bw = 0;
  
  switch(s_tSendState)
  {
    case NB_SendState_Idle:
      s_tSendState = NB_SendState_GetRSSI;
      break;
    case NB_SendState_GetRSSI:// 读CSQ
      s_tNBAppObj.error = nbINFOGetRSSI(NULL);
      
      if(NB_CMD_WAIT == s_tNBAppObj.error)//
      {}
      else// OK
      {
        NB_Print("NetCSQ=%d", s_ptNBObj->rssi);
        s_tSendState = NB_SendState_Start;
      }
      break;

    case NB_SendState_Start:
      if(  (s_tdatabuffSend.len > NB_CFG_PKG_SIZE - NB_CFG_LEN_PROFILE)
         ||(s_tdatabuffSend.len == 0))
      {
        NB_Print("Send Data size error...");
        s_tNBAppObj.sendFlag = 0;
        s_tNBAppObj.retryCnt = 0;
        s_tdatabuffSend.len      = 0;
        
        retState = NB_State_Suspend;
        s_tSendState = NB_SendState_Idle;
        s_tNBAppObj.exeTick = NB_GetCurTick() - NBAPP_MAX_TIMEOUT;
        break;
      }
      s_tNBAppObj.error = NB_OK;
      nbCmdRunStateClr();
      s_tSendState = NB_SendState_Wait;
      break;

    case NB_SendState_Wait:                                        ////NB_SEND_MODE_CON_R);
      resError = nbNETSend(s_ucNBAppSendBuf, s_tdatabuffSend.len, &bw, NB_SEND_MODE_CON);////NB_SEND_MODE_NON_R);////
      if (NB_CMD_WAIT == resError) {
        break;
      }
      s_tdatabuffSend.len = 0;
      if (NB_OK != resError) {  // 发送失败
        s_tSendState = NB_SendState_fail;
      }
      else {
        s_tSendState = NB_SendState_OK;
      }
      break;
      
    case NB_SendState_fail:
      // 保存未成功数据
      RecordApp_WriteTempCollect();
      
      NB_Print("Send Data Failed!, Error=%d", s_ptNBObj->cme);
      s_tNBAppObj.error = NB_SENDFAIL;
      s_tNBAppObj.sendFlag = 0;
      
      // 超时1天未成功发数,断电下次重新开始
      s_tNBAppObj.retryCnt = 0;
      if (SYSGetTimeSec() - s_tNBAppObj.sendTime >= SYSMakeSec(24, 0, 0))
        retState = NB_State_Restart;
      else
        retState = NB_State_Suspend;
      
      nbCmdRunStateClr();
      
      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT;
      break;
      
    case NB_SendState_OK:
      // 发送成功
      NB_Print("Send Data OK!");
      s_tNBAppObj.error = NB_OK;
      s_tNBAppObj.sendFlag = 0;
      s_tNBAppObj.sendTime = SYSGetTimeSec();
      
      if(REC_OK == RecordApp_ReadTempCollect()) { //如果有历史数据，则打包发送。
        RecordApp_PackData();
      }
      
      curTick = NB_GetCurTick();
      s_tNBAppObj.exeTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(1000);
      s_tNBAppObj.psmTick = curTick - NBAPP_MAX_TIMEOUT + NB_MS_TO_TICKS(NB_CFG_TIME_PSM * 1000);
      
      // 获取时间并校时
      
      s_tNBAppObj.retryCnt = 0;
      nbCmdRunStateClr();
      retState = NB_State_Running;
      break;
    default:
      break;
  }
//  s_tNBAppObj.state = retState;
  return retState;
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

