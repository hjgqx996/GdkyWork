/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     cmdproc.h
  * @author   ZouZH
  * @version  V1.00
  * @date     25-July-2018
  * @brief    command process routine for NB-IoT GCJJZQ 2017.
  ******************************************************************************
  */
#define LOG_TAG    "CMD"

/* 命令帧格式(V2.5)
 *------------------------------------------------------------------------------
 * 帧头|控制码|命令ID|设备编号|数据体|通信状态|校验码|帧尾|
 *  1  |   2  |  4   |  4     |  var |  1     |  1   | 1  |
 *------------------------------------------------------------------------------
 * 帧    头: 0xFE
 * 校 验 码: 命令ID-数据体 8位和校验(BCD)
 * 帧    尾: 0xED
 */

/* INCLUDES ------------------------------------------------------------------- */
#include "cmdproc.h"
#include "includes.h"

#include "combus.h"
#include "nb_app.h"
#include "pwrmgr.h"

#include "record_app.h"
#include "record_tbl.h"
#include "heatmeter.h"
#include "usermeter.h"


/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

// General response
const char RSP_OK[]    = "\r\nOK\r\n";
const char RSP_ERROR[] = "\r\nERROR\r\n";
const char RSP_CRLF[] =  "\r\n";


// AT+LIST
const char RSP_AT_LIST[]  ="\r\n\
AT+LIST                                      List all AT Command\r\n\
AT+SHOW                                      Check all the current arguments\r\n\
AT+IDNT                                      Get Device ID Number\r\n\
AT+VERS                                      Get SoftWare Version.\r\n\
AT+PHON                                      Get IMSI Number\r\n\
AT+IMEI                                      Get IMEI Number\r\n\
AT+APN=<apn>                                 Set Access Point Name\r\n\
AT+USER=<user id>                            Set PPP Authentication User Id\r\n\
AT+PWD=<password>                            Set PPP Authentication Password\r\n\
AT+IPAD=<Server's Ip Address>                Set Server's Ip or Domain\r\n\
AT+PORT=< Server's Port>                     Set Main Server's Port\r\n\
AT+IPSEC=<Secondary Server's Ip Addr.>       Set Secondary Server's Ip\r\n\
AT+PTSEC=<Secondary Server's Port>           Set Secondary Server's Port\r\n\
AT+QUIT                                      Quit the at command config program.\r\n\
AT+RESET                                     Reset the system.\r\n\
";

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* 接收队列 */
static QueueHandle_t shQueueRecvCmdBuf = NULL;


/* 命令处理变量 */
#define CMD_SEND_BUF_LEN   300
static uint8_t s_ucCmdSendBuf[CMD_SEND_BUF_LEN];
static CmdQueue_t s_tCmdRecv = {0, 0, NULL};

/* 命令执行缓存标志 */
#define CMD_FLAG_INIT_USER_START  0x03
#define CMD_FLAG_INIT_USER_END    0x04
static uint8_t s_ucCmdInitFlag;

/* LOCAL FUNCTIONS ------------------------------------------------------------ */
uint8_t CmdSendStr(const char *str);
uint8_t CmdParseAT(char *str, uint16_t len);


/**
 * @brief  命令处理初始化
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
CMD_Error_t CmdInit(void)
{
  if (shQueueRecvCmdBuf == NULL)
    shQueueRecvCmdBuf = xQueueCreate(CMD_RECV_BUF_MAX_NUM, sizeof(CmdQueue_t));

  if (shQueueRecvCmdBuf == NULL)
    return CMD_ERR_Malloc;

  vQueueAddToRegistry(shQueueRecvCmdBuf, "CmdQ");

  return CMD_OK;
}


/**
 * @brief  存储MBUS帧,包含帧头帧尾
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
CMD_Error_t CmdRecvQueueWriteMBUS(void)
{
  CMD_Error_t ret = CMD_OK;
  CmdQueue_t cmdQueue;

  uint16_t len = 0;
  uint8_t *pbuf = NULL;

  if (!BIT_READ(AppGetMode(), BIT(APP_MODE_CONFIG)))
    return ret;

  if (!BusCheckRecv(BUS_MBUS_0))
    return ret;

  do
  {
    if (shQueueRecvCmdBuf == NULL)
    {
      CmdInit();
      ret = CMD_ERR_Malloc;
      continue;
    }

    pbuf = BusGetRecvBuff(BUS_MBUS_0);
    len = BusGetRecvLen(BUS_MBUS_0);

    if (strncasecmp((char *)pbuf, "AT", 2) && strncasecmp((char *)pbuf, "CO", 2))
    {
      pbuf = BusGetRecvBuff(BUS_MBUS_0);
      len = lib_findframegdkyhex(&pbuf, BusGetRecvLen(BUS_MBUS_0));

      if (len < 14)
      {
        ret = CMD_ERR_Recv;
        break;
      }

      if (chk_sum8_dec(&pbuf[3], len - 5) != pbuf[len - 2])
      {
        ret = CMD_ERR_Recv;
        break;
      }
    }
    else
    {
      pbuf[len] = '\0';
      len += 1;
    }
    

    cmdQueue.port = CMD_PORT_MBUS;
    cmdQueue.len = len;
    if (cmdQueue.len > BusGetRecvLenMax(BUS_MBUS_0))
    {
      ret = CMD_ERR_Recv;
      break;
    }

    cmdQueue.pbuf = (uint8_t *)SYSMalloc(cmdQueue.len);
    if (NULL == cmdQueue.pbuf)
    {
      ret = CMD_ERR_Malloc;
      break;
    }

    lib_memcpy(cmdQueue.pbuf, pbuf, cmdQueue.len);

    if (pdTRUE != xQueueSend(shQueueRecvCmdBuf, &cmdQueue, 0))
    {
      ret = CMD_ERR_Full;
      SYSFree(cmdQueue.pbuf);
      cmdQueue.pbuf = NULL;
    }

    if (!uxQueueSpacesAvailable(shQueueRecvCmdBuf))
      log_w("Receive queue is full!!!");
  }while(0);

  BusEnableRx(BUS_MBUS_0);

  return ret;
}

/**
 * @brief  存储RS485帧,包含帧头帧尾
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
CMD_Error_t CmdRecvQueueWriteRS485(void)
{
  CMD_Error_t ret = CMD_OK;
  CmdQueue_t cmdQueue;

  uint16_t len = 0;
  uint8_t *pbuf = NULL;

  if (!BIT_READ(AppGetMode(), BIT(APP_MODE_CONFIG)))
    return ret;

  if (!BusCheckRecv(BUS_RS485_0))
    return ret;

  do
  {
    if (shQueueRecvCmdBuf == NULL)
    {
      CmdInit();
      ret = CMD_ERR_Malloc;
      continue;
    }

    pbuf = BusGetRecvBuff(BUS_RS485_0);
    len = BusGetRecvLen(BUS_RS485_0);

    if (strncasecmp((char *)pbuf, "AT", 2) && strncasecmp((char *)pbuf, "CO", 2))
    {
      pbuf = BusGetRecvBuff(BUS_RS485_0);
      len = lib_findframegdkyhex(&pbuf, BusGetRecvLen(BUS_RS485_0));

      if (len < 14)
      {
        ret = CMD_ERR_Recv;
        break;
      }

      if (chk_sum8_dec(&pbuf[3], len - 5) != pbuf[len - 2])
      {
        ret = CMD_ERR_Recv;
        break;
      }
    }
    else
    {
      pbuf[len] = '\0';
      len += 1;
    }
    

    cmdQueue.port = CMD_PORT_RS485;
    cmdQueue.len = len;
    if (cmdQueue.len > BusGetRecvLenMax(BUS_RS485_0))
    {
      ret = CMD_ERR_Recv;
      break;
    }

    cmdQueue.pbuf = (uint8_t *)SYSMalloc(cmdQueue.len);
    if (NULL == cmdQueue.pbuf)
    {
      ret = CMD_ERR_Malloc;
      break;
    }

    lib_memcpy(cmdQueue.pbuf, pbuf, cmdQueue.len);

    if (pdTRUE != xQueueSend(shQueueRecvCmdBuf, &cmdQueue, 0))
    {
      ret = CMD_ERR_Full;
      SYSFree(cmdQueue.pbuf);
      cmdQueue.pbuf = NULL;
    }

    if (!uxQueueSpacesAvailable(shQueueRecvCmdBuf))
      log_w("Receive queue is full!!!");
  }while(0);

  BusEnableRx(BUS_RS485_0);

  return ret;
}



/**
 * @brief  存储NB网络帧,包含帧头帧尾
 *
 * @param  数据缓存和长度
 *
 * @retval 0 成功，其他失败
 */
CMD_Error_t CmdRecvQueueWriteNB(void *pdat, uint8_t datlen)
{
  CMD_Error_t ret = CMD_OK;
  CmdQueue_t cmdQueue;

  uint16_t len = 0;
  uint8_t *pbuf = NULL;

  do
  {
    if (shQueueRecvCmdBuf == NULL)
    {
      CmdInit();
      ret = CMD_ERR_Malloc;
      continue;
    }

    pbuf = pdat;
    len = lib_findframegdkyhex(&pbuf, datlen);

    if (len < 14)
    {
      ret = CMD_ERR_Recv;
      break;
    }

    if (chk_sum8_dec(&pbuf[3], len - 5) != pbuf[len - 2])
    {
      ret = CMD_ERR_Recv;
      break;
    }

    cmdQueue.port = CMD_PORT_NB;
    cmdQueue.len = len;

    cmdQueue.pbuf = (uint8_t *)SYSMalloc(cmdQueue.len);
    if (NULL == cmdQueue.pbuf)
    {
      ret = CMD_ERR_Malloc;
      break;
    }

    lib_memcpy(cmdQueue.pbuf, pbuf, cmdQueue.len);

    if (pdTRUE != xQueueSend(shQueueRecvCmdBuf, &cmdQueue, 0))
    {
      ret = CMD_ERR_Full;
      SYSFree(cmdQueue.pbuf);
      cmdQueue.pbuf = NULL;
    }

    if (!uxQueueSpacesAvailable(shQueueRecvCmdBuf))
      log_w("Receive queue is full!!!");
  }while(0);

  return ret;
}



/**
 * @brief  存储通过NB发送全部户表周期数据指令
 *
 * @param  cmd 命令字
 *
 * @retval 0 成功，其他失败
 */
CMD_Error_t CmdRecvQueueWriteCmdToNB(uint16_t cmd)
{
  static uint32_t id = 0;
  
  CMD_Error_t ret = CMD_OK;
  CmdQueue_t cmdQueue;

  if (BIT_READ(AppGetMode(), BIT(APP_MODE_CONFIG)))
    return ret;

  do
  {
    if (shQueueRecvCmdBuf == NULL)
    {
      CmdInit();
      ret = CMD_ERR_Malloc;
      continue;
    }

    cmdQueue.port = CMD_PORT_NB;
    cmdQueue.len = 14;
    cmdQueue.pbuf = (uint8_t *)SYSMalloc(cmdQueue.len);
    if (NULL == cmdQueue.pbuf)
    {
      ret = CMD_ERR_Malloc;
      break;
    }

    if (cmd != CMD_GET_NB_INFO)
      id += 1;
      
    cmdQueue.pbuf[0] = 0xFE;
    cmdQueue.pbuf[1] = UINT16_HI(cmd);
    cmdQueue.pbuf[2] = UINT16_LO(cmd);
    dec2bcds(id, &cmdQueue.pbuf[3], 4);
    lib_memcpy(&cmdQueue.pbuf[7], APP_DEV_ADDR, 4);
    cmdQueue.pbuf[11] = 0x00;
    cmdQueue.pbuf[12] = chk_sum8_dec(&cmdQueue.pbuf[3], cmdQueue.len - 5);
    cmdQueue.pbuf[13] = 0xED;

    if (pdTRUE != xQueueSend(shQueueRecvCmdBuf, &cmdQueue, 0))
    {
      ret = CMD_ERR_Full;
      SYSFree(cmdQueue.pbuf);
      cmdQueue.pbuf = NULL;
    }

    if (!uxQueueSpacesAvailable(shQueueRecvCmdBuf))
      log_w("Receive queue is full!!!");
  }while(0);

  return ret;
}


/**
 * @brief  发送数据包
 *
 * @param  datalen  数据长度:设备号之后到通信状态之前
 * @param  recverr  接收命令处理错误状态
 * @param  senderr  发送命令处理错误状态
 *
 * @retval 实际发送数据长度
 */
uint16_t CmdSendToHost(uint16_t datalen, uint8_t recverr, uint8_t senderr)
{
  // 通信状态
  s_ucCmdSendBuf[CMD_OFS_DATA + datalen] = 0;
  if (CMD_ERR_Param == recverr)
    s_ucCmdSendBuf[CMD_OFS_DATA + datalen] |= CMD_ERR_BIT_PARAM;
  else if (CMD_ERR_Command == recverr)
    s_ucCmdSendBuf[CMD_OFS_DATA + datalen] |= CMD_ERR_BIT_COMMAND;
  else if (CMD_ERR_Timeout == recverr)
    s_ucCmdSendBuf[CMD_OFS_DATA + datalen] |= CMD_ERR_BIT_TIMEOUT;
  else
  {
    if (recverr || senderr)
      s_ucCmdSendBuf[CMD_OFS_DATA + datalen] |= CMD_ERR_BIT_RUN;
  }

  s_ucCmdSendBuf[CMD_OFS_DATA + datalen + 1] = chk_sum8_dec(&s_ucCmdSendBuf[CMD_OFS_ID], datalen + 9);
  s_ucCmdSendBuf[CMD_OFS_DATA + datalen + 2] = 0xED;

  if (CMD_PORT_MBUS == s_tCmdRecv.port)
    return BusSend(BUS_MBUS_0 ,s_ucCmdSendBuf, datalen + 14);

  if (CMD_PORT_RS485 == s_tCmdRecv.port)
    return BusSend(BUS_RS485_0 ,s_ucCmdSendBuf, datalen + 14);

  if (CMD_PORT_NB == s_tCmdRecv.port)
    return nbAppWriteSendBuff(s_ucCmdSendBuf, datalen + 14);

  return 0;
}

/**
 * @brief  查询接收命令并处理
 *
 * @param  无
 *
 * @retval 1处理一条命令, 0无命令.
 */
uint8_t CmdRecvQueueRead(void)
{

	if (xQueueReceive(shQueueRecvCmdBuf, &s_tCmdRecv, 0) != pdTRUE)
    return 0;
  
  if (!strncasecmp((char *)s_tCmdRecv.pbuf, "AT", 2) || !strncasecmp((char *)s_tCmdRecv.pbuf, "CO", 2))
  {
    log_i("[ATCmd Recv] %s", s_tCmdRecv.pbuf);
    CmdParseAT((char *)s_tCmdRecv.pbuf, s_tCmdRecv.len);

    // 释放内存
    SYSFree(s_tCmdRecv.pbuf);
    s_tCmdRecv.pbuf = NULL;
    s_tCmdRecv.len = 0;

  	return 1;
  }

#ifdef USE_FULL_ASSERT
  uint8_t i = 0;
  log_i("[Cmd Recv]port=%d, len=%d", s_tCmdRecv.port, s_tCmdRecv.len);
  for (i = 0; i < s_tCmdRecv.len; i++)
  {
    xprintf("%02X ", s_tCmdRecv.pbuf[i]);
    if (!((i + 1) % 32))
      xprintf(RSP_CRLF);
  }
  xprintf(RSP_CRLF);
#endif

  CmdRecvMsgParse();

  // 释放内存
  SYSFree(s_tCmdRecv.pbuf);
  s_tCmdRecv.pbuf = NULL;
  s_tCmdRecv.len = 0;

	return 1;
}

/**
 * @brief  命令是否空闲
 *
 * @param  无
 *
 * @retval 1空闲, 0忙
 */
uint8_t CmdIsIdle(void)
{
  return (uxQueueSpacesAvailable(shQueueRecvCmdBuf) >= CMD_RECV_BUF_MAX_NUM);
}


/**
 * @brief  接收命令处理
 *
 * @param  无
 *
 * @retval 无
 */
CMD_Error_t CmdRecvMsgParse(void)
{
  CMD_Error_t recvErr = CMD_OK;
  uint8_t chk = 0;
  uint16_t datalenrd = 0;
  uint8_t *pdatard = NULL;
  uint16_t cmd = 0;

  if ((NULL == s_tCmdRecv.pbuf) || !s_tCmdRecv.len)
    return CMD_ERR_Param;

  do
  {
    // 帧头正确?
    if ((0xFE != s_tCmdRecv.pbuf[CMD_OFS_HDR]) && \
        (0xED != s_tCmdRecv.pbuf[s_tCmdRecv.len - 1]))
    {
      recvErr = CMD_ERR_Recv;
      continue;
    }

    // 校验正确?
    chk = chk_sum8_dec(&s_tCmdRecv.pbuf[CMD_OFS_ID], s_tCmdRecv.len - 5);
    if (s_tCmdRecv.pbuf[s_tCmdRecv.len - 2] != chk)
    {
      recvErr = CMD_ERR_Recv;
      continue;
    }

    // 设备号正确?
    if (lib_memcmp(&s_tCmdRecv.pbuf[CMD_OFS_ADDR], APP_DEV_ADDR, APP_DEV_ADDR_SIZE))
    {
      if (lib_memvalcmp(&s_tCmdRecv.pbuf[CMD_OFS_ADDR], APP_DEV_ADDR_SIZE, 0xAA))
      {
         recvErr = CMD_ERR_Recv;
         continue;
      }
    }

    // 转换接收数据和命令字
    cmd = UINT16_BUILD(s_tCmdRecv.pbuf[CMD_OFS_FUNC + 1], s_tCmdRecv.pbuf[CMD_OFS_FUNC]);
    pdatard = &s_tCmdRecv.pbuf[CMD_OFS_DATA];
    datalenrd = s_tCmdRecv.len - 14;

    // 命令处理
    switch (cmd)
    {
      // 升级指令
#ifdef USE_IAP_UPDATE
      case CMD_IAP_UPDATE:                               /*!< 设备程序更新 */
      {
        if (Record_SaveAppUpFlag() == REC_OK)
        {
          CmdSendMsg(CMD_IAP_UPDATE, recvErr, NULL, 0);

          // 设备重启(软复位)
          SYSDelayMs(1000);
          taskDISABLE_INTERRUPTS();
          NVIC_SystemReset();
          break;
        }

        recvErr = CMD_ERR_Record;
        CmdSendMsg(CMD_IAP_UPDATE, recvErr, NULL, 0);
      }break;
#endif

      case CMD_GET_HAMS_PARAM:                           /*!< 读取采集计算器参数 */
      case CMD_GET_NB_INFO:                              /*!< 读取NB模块网络信息 */
      {
        CmdSendMsg(cmd, recvErr, NULL, 0);
      }break;

      case CMD_SET_HAMS_ALLOC_FACTOR:                    /*!< 设置分摊热量系数 */
      {
        // 数据长度
        if ((datalenrd != 2) || !isbcds(&pdatard[0], datalenrd))
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, 0);
          break;
        }

        g_tSysParam.allocPeriod = bcd2dec(pdatard[1]);
        if (g_tSysParam.allocPeriod > 24)
          g_tSysParam.allocPeriod = 24;

        g_tSysParam.allocPeriod -= (g_tSysParam.allocPeriod % 2);
        if (RecordApp_SaveSysParam() != REC_OK)
          recvErr = CMD_ERR_Record;

        CmdSendMsg(cmd, recvErr, NULL, 0);
      }break;

      case CMD_GET_HAMS_ALLOC_FACTOR:                    /*!< 读取分摊热量系数 */
      {
        CmdSendMsg(cmd, recvErr, NULL, 0);
      }break;

      case CMD_SET_HAMS_CMD_INTERVAL:                    /*!< 设置每条指令的时间间隔 */
      {
        if ((datalenrd != 2) || !isbcds(&pdatard[0], datalenrd))
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, 0);
          break;
        }

        REC_CMDInterval = bcds2dec(&pdatard[0], 2) * 10;
        if (RecordApp_SaveSysParam() != REC_OK)
          recvErr = CMD_ERR_Record;

        CmdSendMsg(cmd, recvErr, NULL, 0);
      }break;

      case CMD_SET_HAMS_PROTOCOL_VER:                    /*!< 设置采集计算器协议版本号 */
      {
        // 数据长度
        if ((datalenrd != 27) || !isbcds(&pdatard[0], datalenrd))
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, 0);
          break;
        }

        lib_memcpy(g_tSysParam.protocolVer, &pdatard[0], 4);
        g_tSysParam.protocolVer[0] = 0x00;
        g_tSysParam.softType = pdatard[4];
				g_tSysParam.collectStat = pdatard[10];
        g_tSysParam.protocolTime[0] = g_tSysTime.year;
        g_tSysParam.protocolTime[1] = g_tSysTime.month;
        g_tSysParam.protocolTime[2] = g_tSysTime.day;
        g_tSysParam.protocolTime[3] = g_tSysTime.hour;
        g_tSysParam.protocolTime[4] = g_tSysTime.minute;
        g_tSysParam.protocolTime[5] = g_tSysTime.second;

        if (++g_tSysParam.protocolCnt > 999999)
          g_tSysParam.protocolCnt = 1;

        if (RecordApp_SaveSysParam() != REC_OK)
          recvErr = CMD_ERR_Record;

        CmdSendMsg(cmd, recvErr, NULL, 0);
      }break;

      case CMD_GET_HAMS_PROTOCOL_VER:                    /*!< 读取采集计算器协议版本号 */
      {
        CmdSendMsg(cmd, recvErr, NULL, 0);
      }break;

      case CMD_SET_ALL_USER_HEAT_SEASON:                 /*!< 设置群表采暖季 */
      {
        if ((datalenrd != 8) || !isbcds(&pdatard[0], datalenrd))
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(CMD_REPLY_SET_ALL_USER_HEAT_SEASON_END, recvErr, NULL, 0);
          break;
        }

        lib_memcpy(g_tSysParam.heatSeason, &pdatard[4], 4);
        if (RecordApp_SaveSysParam() != REC_OK)
          recvErr = CMD_ERR_Record;

        // 设置完成
        CmdSendMsg(CMD_REPLY_SET_ALL_USER_HEAT_SEASON_END, 0, NULL, 0);
      }break;

      // 校时命令
      case CMD_SET_TIME_BROADCAST:                       /*!< 广播校时 */
      {
        // 数据长度
        if ((datalenrd != 6) || !isbcds(&pdatard[0], datalenrd))
        {
          recvErr = CMD_ERR_Param;
          break;
        }
        SysTimeSet(UINT16_BUILD(pdatard[0], 0x20), pdatard[1], pdatard[2], \
                       pdatard[3], pdatard[4], pdatard[5]);
        SysTimeUpdate();
      }break;

      case CMD_SET_TIME_ALL:                             /*!< 群表校时 */
      {
        if ((datalenrd != 6) || !isbcds(&pdatard[0], datalenrd))
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, 0);
          break;
        }

        SysTimeSet(UINT16_BUILD(pdatard[0], 0x20), pdatard[1], pdatard[2], \
                       pdatard[3], pdatard[4], pdatard[5]);
        SysTimeUpdate();

        CmdSendMsg(cmd, recvErr, NULL, 0);

        SYSDelayMs(REC_CMDInterval);

        // 校时完成
        CmdSendMsg(CMD_REPLY_SET_TIME_ALL_END, 0, NULL, 0);
      }break;

      case CMD_GET_TIME_ALL:                             /*!< 群表读时间 */
      {
        if ((datalenrd != 2) || !isbcds(&pdatard[0], datalenrd))
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, 0);
          break;
        }

        // 回复集中器时间
        CmdSendMsg(CMD_REPLY_GET_TIME_HAMS, recvErr, NULL, 0);
        SYSDelayMs(REC_CMDInterval);

        // 回复结束
        CmdSendMsg(CMD_REPLY_GET_TIME_ALL_END, 0, NULL, 0);
      }break;

      // 初始化命令
      case CMD_INIT_ALL_USER_INFO_START:                 /*!< 初始化所有住户信息开始 */
      {
        if (datalenrd != 0)
        {
          recvErr = CMD_ERR_Param;
          BIT_CLEAR(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_START));
        }
        else
        {
          BIT_SET(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_START));
        }
        CmdSendMsg(cmd, recvErr, NULL, 0);
      }break;

      case CMD_INIT_ALL_USER_INFO_EVERY:                 /*!< 初始化每个住户信息 */
      {
        uint16_t cmdTmp = 0;
        UM_Info_t *pinfo = NULL;

        // 检测是否发送了开始命令
        if ((datalenrd != 36) /* || !isbcds(&pdatard[0], datalenrd) */ \
            || !BIT_READ(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_START)))
        {
          recvErr = CMD_ERR_Param;
          break;
        }

        // 简码是否超限
        cmdTmp = bcds2dec(&pdatard[0], 2) - 1;
        if (cmdTmp >= REC_UserNbr_MAX)
        {
          recvErr = CMD_ERR_Param;
          break;
        }

        // 分配内存
        if (NULL == (pinfo = (UM_Info_t *)SYSMalloc(sizeof(UM_Info_t))))
        {
          recvErr = CMD_ERR_Malloc;
          break;
        }

        if (Record_Read(REC_DT_User, REC_DI_InitInfo, cmdTmp, pinfo) != REC_OK)
          lib_memset(pinfo, 0x00, sizeof(UM_Info_t));

        pinfo->index[0] = pdatard[0];
        pinfo->index[1] = pdatard[1];
        lib_memcpy(pinfo->valveAddr, &pdatard[2], 4);
        pinfo->heatArea = bcds2dec(&pdatard[6], 3);
        lib_memcpy(pinfo->roomID, &pdatard[9], 4);
        lib_memcpy(pinfo->icID, &pdatard[13], 4);
        pinfo->onoffFactor = bcd2dec(pdatard[17]);
        lib_memcpy(pinfo->valveAddrH, &pdatard[18], 4);
        pinfo->commType = pdatard[22];
        pinfo->systemID = pdatard[23];

        // 表号不相等, 换表, 置表底
        if (lib_memcmp(pinfo->meterAddr, &pdatard[25], 8))
          pinfo->rstBaseHeat = 1;

        pinfo->meterAccount = pdatard[24];
        lib_memcpy(pinfo->meterAddr, &pdatard[25], 8);
        pinfo->devType = pdatard[33];
        pinfo->devVendor = pdatard[34];
        pinfo->devConfig = pdatard[35];

        if (Record_Write(REC_DT_User, REC_DI_InitInfo, cmdTmp, pinfo) != REC_OK)
          recvErr = CMD_ERR_Record;

        // 释放内存
        SYSFree(pinfo);
        pinfo = NULL;
      }break;

      case CMD_INIT_ALL_USER_INFO_END:                   /*!< 初始化所有住户信息结束 */
      {
        if (((datalenrd != 2) && (datalenrd != 4)) || !isbcds(&pdatard[0], datalenrd))
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, datalenrd);
          break;
        }
        else if (BIT_READ(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_START)))
        {
          REC_UserNbr = bcds2dec(&pdatard[0], 2);
          if (REC_UserNbr > REC_UserNbr_MAX)
            REC_UserNbr = REC_UserNbr_MAX;

          if (datalenrd > 2)
            REC_UserMeterNbr = bcds2dec(&pdatard[2], 2);
          else
            REC_UserMeterNbr = 0;

          if (REC_UserMeterNbr > REC_UserNbr_MAX)
            REC_UserMeterNbr = REC_UserNbr_MAX;

          if (RecordApp_SaveSysParam() != REC_OK)
          {
            recvErr = CMD_ERR_Record;
            CmdSendMsg(cmd, recvErr, NULL, datalenrd);
            break;
          }

          // 清零住户
          if (!REC_UserNbr)
          {
            Record_Erase(REC_DT_User, REC_DI_NONE);
          }
        }

        // 设置初始化完成标志
        BIT_CLEAR(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_START));
        BIT_SET(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_END));

        CmdSendMsg(CMD_INIT_ALL_USER_INFO_EVERY, recvErr, NULL, datalenrd);
      }break;

      case CMD_INIT_ONE_USER_INFO:                       /*!< 初始化单个住户信息 */
      {
        uint16_t cmdTmp1 = 0;
        uint16_t cmdTmp2 = 0;
        UM_Info_t *pinfo = NULL;

        if ((datalenrd != 40)/* || !isbcds(&pdatard[0], datalenrd)*/)
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, cmdTmp1);
          break;
        }

        // 简码是否超限
        cmdTmp1 = bcds2dec(&pdatard[0], 2) - 1;
        cmdTmp2 = bcds2dec(&pdatard[22], 2);
        if ((cmdTmp1 >= REC_UserNbr_MAX) || (cmdTmp2 > REC_UserNbr_MAX))
          recvErr = CMD_ERR_Param;
        // 分配内存
        else if (NULL == (pinfo = (UM_Info_t *)SYSMalloc(sizeof(UM_Info_t))))
          recvErr = CMD_ERR_Malloc;
        else
        {
          if (Record_Read(REC_DT_User, REC_DI_InitInfo, cmdTmp1, pinfo) != REC_OK)
            lib_memset(pinfo, 0x00, sizeof(UM_Info_t));

          pinfo->index[0] = pdatard[0];
          pinfo->index[1] = pdatard[1];
          lib_memcpy(pinfo->valveAddr, &pdatard[2], 4);
          pinfo->heatArea = bcds2dec(&pdatard[6], 3);
          lib_memcpy(pinfo->roomID, &pdatard[9], 4);
          lib_memcpy(pinfo->icID, &pdatard[13], 4);
          pinfo->onoffFactor = bcd2dec(pdatard[17]);
          lib_memcpy(pinfo->valveAddrH, &pdatard[18], 4);

          pinfo->commType = pdatard[24];
          pinfo->systemID = pdatard[25];

          // 表号不相等, 换表, 置表底
          if (lib_memcmp(pinfo->meterAddr, &pdatard[25], 8))
            pinfo->rstBaseHeat = 1;

          pinfo->meterAccount = pdatard[26];
          lib_memcpy(pinfo->meterAddr, &pdatard[27], 8);
          pinfo->devType = pdatard[35];
          pinfo->devVendor = pdatard[36];
          pinfo->devConfig = pdatard[37];
          REC_UserMeterNbr = bcds2dec(&pdatard[38], 2);
          if (REC_UserMeterNbr > REC_UserNbr_MAX)
            REC_UserMeterNbr = REC_UserNbr_MAX;

          REC_UserNbr = cmdTmp2;
          if (Record_Write(REC_DT_User, REC_DI_InitInfo, cmdTmp1, pinfo) \
           || RecordApp_SaveSysParam())
          {
            recvErr = CMD_ERR_Record;
          }
        }

        // 释放内存
        SYSFree(pinfo);
        pinfo = NULL;
        CmdSendMsg(cmd, recvErr, NULL, cmdTmp1);
      }break;

      case CMD_GET_INIT_ONE_USER_INFO:                   /*!< 读取初始化单个住户信息 */
      {
        uint16_t cmdTmp1 = 0;

        if ((datalenrd != 2) || !isbcds(&pdatard[0], datalenrd))
          recvErr = CMD_ERR_Param;

        // 房间简码是否超限
        cmdTmp1 = bcds2dec(&pdatard[0], 2) - 1;
        if (cmdTmp1 >= REC_UserNbr_MAX)
          recvErr = CMD_ERR_Param;

        CmdSendMsg(cmd, recvErr, NULL, cmdTmp1);
      }break;

      // 读取周期数据
      case CMD_GET_ALL_USER_METER_CYCLE_DATA:            /*!< 读取所有住户热表周期数据 */
      {
        // 数据长度
        if (datalenrd != 0)
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(CMD_GET_ALL_USER_METER_CYCLE_DATA_END, recvErr, NULL, 0);
          break;
        }

        if (0 == REC_UserMeterNbr)
        {
          CmdSendMsg(CMD_GET_ALL_USER_METER_CYCLE_DATA_END, recvErr, NULL, 0);
          break;
        }

        // 回复上位机
        CmdSendMsg(CMD_REPLY_GET_ALL_USER_METER_CYCLE_DATA, recvErr, NULL, 0);
      }break;

      case CMD_GET_ONE_USER_METER_CYCLE_DATA:            /*!< 采集单个住户热表周期数据 */
      {
        uint16_t cmdTmp = 0;

        // 数据长度
        if ((datalenrd != 10) /*|| !isbcds(&pdatard[0], datalenrd)*/)
          recvErr = CMD_ERR_Param;

        // 房间简码判断
        cmdTmp = bcds2dec(&pdatard[0], 2) - 1;
        if (cmdTmp >= REC_UserNbr)
          recvErr = CMD_ERR_Param;

        // 回复上位机
        CmdSendMsg(cmd, recvErr, NULL, cmdTmp);
      }break;

      // 采集实时(Real Time)数据命令
      case CMD_GET_USER_METER_RT_DATA:                   /*!< 采集户用热表的即时数据 */
      {
        uint16_t cmdTmp = 0;
        UM_Info_t *pinfo = NULL;

        if ((datalenrd != 10) /*|| !isbcds(&pdatard[0], datalenrd)*/)
        {
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, cmdTmp);
          break;
        }

        // 简码
        cmdTmp = bcds2dec(&pdatard[0], 2) - 1;
        if (cmdTmp >= REC_UserNbr)
        {
          cmdTmp= 0;
          recvErr = CMD_ERR_Param;
          CmdSendMsg(cmd, recvErr, NULL, cmdTmp);
          break;
        }

        // 分配内存
        if (NULL == (pinfo = (UM_Info_t *)SYSMalloc(sizeof(UM_Info_t))))
        {
          recvErr = CMD_ERR_Malloc;
          CmdSendMsg(cmd, recvErr, NULL, cmdTmp);
          break;
        }

        if (Record_Read(REC_DT_User, REC_DI_InitInfo, cmdTmp, pinfo) != REC_OK)
          recvErr = CMD_ERR_Malloc;
        else if (!lib_memvalcmp(&pinfo->meterAddr[1], 7, 0x00))
          recvErr = CMD_ERR_Param;
        else
        {
          /*
          UM_Error_t umErr = UM_CollRealTimeData(cmdTmp, pinfo);
          if (umErr)
            recvErr = CMD_ERR_Param;

          if ((UM_ERR_Timeout == umErr) || (UM_ERR_Busy == umErr))
            recvErr = CMD_ERR_Timeout;
          */
          recvErr = CMD_OK;
        }
        CmdSendMsg(cmd, recvErr, NULL, cmdTmp);

        // 释放内存
        SYSFree(pinfo);
        pinfo = NULL;
      }break;

      default:
      break;
    }
  }while(0);

  return recvErr;
}


/**
 * @brief  发送响应命令处理
 *
 * @param  cmd  命令码
 * @param  err  接收上位机命令错误标志,0代表无错误
 * @param  pbuf 参数
 * @param  dat  上位机命令参数,例如房间简码等
 *
 * @retval 0无错误
 */
CMD_Error_t CmdSendMsg(uint16_t cmd, uint8_t recvErr, void *pbuf, uint16_t dat)
{
  CMD_Error_t sendErr = CMD_OK;
  uint8_t datalen = 0;  /* 只包含数据长度 */
  uint8_t *pdatasd = NULL;
  uint8_t *pdatard = NULL;

  // 固定信息
  s_ucCmdSendBuf[CMD_OFS_HDR] = 0xFE;
  s_ucCmdSendBuf[CMD_OFS_FUNC + 0] = UINT16_HI(cmd);
  s_ucCmdSendBuf[CMD_OFS_FUNC + 1] = UINT16_LO(cmd);
  lib_memcpy(&s_ucCmdSendBuf[CMD_OFS_ID], &s_tCmdRecv.pbuf[CMD_OFS_ID], 4);
  lib_memcpy(&s_ucCmdSendBuf[CMD_OFS_ADDR], APP_DEV_ADDR, 4);

  pdatasd = &s_ucCmdSendBuf[CMD_OFS_DATA];
  pdatard = &s_tCmdRecv.pbuf[CMD_OFS_DATA];

  switch (cmd)
  {
    case CMD_IAP_UPDATE:                               /* IAP 升级指令 */
    {
      datalen = 0;
    }break;

    case CMD_IAP_UP_DEV_INFO:                          /* IAP之后第一次启动时上传版本号 */
    {
      datalen = 5;
      lib_memcpy(&pdatasd[0], &APP_SOFT_VER[0], APP_SOFT_VER_SIZE);
      pdatasd[4] = APP_VER_DISP;
    }break;

    // 设置命令

    case CMD_GET_HAMS_PARAM:                           /*!< 读取采集计算器参数 */
    {
      if (RecordApp_ReadSysParam() != REC_OK)
        sendErr = CMD_ERR_Record;

      datalen = 17;
      pdatasd[0] = dec2bcd(g_tSysParam.allocPeriod);
      lib_memcpy(&pdatasd[1], (uint8_t *)APP_SOFT_VER, 4);
      pdatasd[5] = APP_VER_DISP;
      dec2bcds(REC_UserNbr, &pdatasd[6], 2);
      dec2bcds(0, &pdatasd[8], 2);
      pdatasd[10] = 0x10;
      pdatasd[11] = 0x00;
      pdatasd[12] = 0x00;
      lib_memset(&pdatasd[13], 0x00, 4);
    }break;

    case CMD_GET_NB_INFO:                              /*!< 读取NB模块网络信息 */
    {
      const NB_t *pnb = nbGetObj();
      uint8_t i = 0;
      
      for (i = 0; i < 8; i++) 
      {
        pdatasd[i] = UINT8_BUILD(NB_CHARHEXTONUM(pnb->imei[i * 2]), NB_CHARHEXTONUM(pnb->imei[(i * 2) + 1]));
      }

      for (i = 0; i < 10; i++) 
      {
        pdatasd[8 + i] = UINT8_BUILD(NB_CHARHEXTONUM(pnb->iccid[i * 2]), NB_CHARHEXTONUM(pnb->iccid[(i * 2) + 1]));
      }

      lib_memcpy(&pdatasd[18], pnb->ipAddr, 4);

      pdatasd[22] = pnb->rssi;

      // 电池状态
      pdatasd[23] = 0;
      if (PMIsBatLowVol())
      {
        BIT_SET(pdatasd[23], BIT(0));
      }

      lib_memset(&pdatasd[24], 0x00, 10);

      datalen = 34;
    }break;

    case CMD_SET_HAMS_ALLOC_FACTOR:                    /*!< 设置分摊热量系数 */
    {
      if (RecordApp_ReadSysParam() != REC_OK)
        sendErr = CMD_ERR_Record;

      datalen = 2;
      pdatasd[0] = 0x00;
      pdatasd[1] = dec2bcd(g_tSysParam.allocPeriod);
    }break;

    case CMD_GET_HAMS_ALLOC_FACTOR:                    /*!< 读取分摊热量系数 */
    {
      if (RecordApp_ReadSysParam() != REC_OK)
        sendErr = CMD_ERR_Record;

      datalen = 2;
      pdatasd[0] = 0x00;
      pdatasd[1] = dec2bcd(g_tSysParam.allocPeriod);
    }break;

    case CMD_SET_HAMS_CMD_INTERVAL:                    /*!< 设置每条指令的时间间隔 */
    {
      if (RecordApp_ReadSysParam() != REC_OK)
        sendErr = CMD_ERR_Record;

      datalen = 2;
      dec2bcds(REC_CMDInterval / 10, &pdatasd[0], 2);
    }break;

    case CMD_SET_HAMS_PROTOCOL_VER:                    /*!< 设置采集计算器协议版本号 */
    case CMD_GET_HAMS_PROTOCOL_VER:                    /*!< 读取采集计算器协议版本号 */
    {
      if (RecordApp_ReadSysParam() != REC_OK)
        sendErr = CMD_ERR_Record;

      lib_memcpy(&pdatasd[0], g_tSysParam.protocolVer, 4);
      pdatasd[0] = 0x00;
      pdatasd[4] = g_tSysParam.softType;
			lib_memset(&pdatasd[5], 0x00, 5);
			pdatasd[10] = g_tSysParam.collectStat;
      lib_memset(&pdatasd[11], 0x00, 16);

      dec2bcds(g_tSysParam.protocolCnt, &pdatasd[27], 3);
      lib_memcpy(&pdatasd[30], g_tSysParam.protocolTime, 6);

      datalen = 36;
    }break;

    case CMD_REPLY_SET_ALL_USER_HEAT_SEASON_END:       /*!< 回复设置群表采暖季完毕 */
    {
      datalen = 0;
    }break;

    case CMD_SET_TIME_ALL:                             /*!< 群表校时 */
    {
      datalen = 6;
      pdatasd[0] = g_tSysTime.year;
      pdatasd[1] = g_tSysTime.month;
      pdatasd[2] = g_tSysTime.day;
      pdatasd[3] = g_tSysTime.hour;
      pdatasd[4] = g_tSysTime.minute;
      pdatasd[5] = g_tSysTime.second;
    }break;

    case CMD_REPLY_SET_TIME_ALL_END:                   /*!< 回复群表校时完成 */
    {
      datalen = 0;
    }break;

    case CMD_REPLY_GET_TIME_HAMS:                      /*!< 回复集中器时间 */
    {
      datalen = 6;
      pdatasd[0] = UINT16_LO(g_tSysTime.year);
      pdatasd[1] = g_tSysTime.month;
      pdatasd[2] = g_tSysTime.day;
      pdatasd[3] = g_tSysTime.hour;
      pdatasd[4] = g_tSysTime.minute;
      pdatasd[5] = g_tSysTime.second;
    }break;

    case CMD_REPLY_GET_TIME_ALL_END:                   /*!< 回复群表读时间完成 */
    {
      datalen = 2;
      dec2bcds(REC_UserNbr, &pdatasd[0], 2);
    }break;

    // 初始化命令
    case CMD_INIT_ALL_USER_INFO_START:                 /*!< 初始化所有住户信息开始 */
    {
      datalen = 0;
    }break;

    case CMD_INIT_ALL_USER_INFO_EVERY:                 /*!< 初始化每个住户信息 */
    {
      uint16_t cmdTmp = 0;
      UM_Info_t *pinfo = NULL;

      if (NULL == (pinfo = (UM_Info_t *)SYSMalloc(sizeof(UM_Info_t))))
      {
        sendErr = CMD_ERR_Malloc;
      }
      else
      {
        // 返回所有地址
        for (cmdTmp = 0; cmdTmp < REC_UserNbr; cmdTmp++)
        {
          if (Record_Read(REC_DT_User, REC_DI_InitInfo, cmdTmp, pinfo) != REC_OK)
            continue;

          pdatasd[0] = pinfo->index[0];
          pdatasd[1] = pinfo->index[1];
          lib_memcpy(&pdatasd[2], pinfo->valveAddr, 4);
          dec2bcds(pinfo->heatArea, &pdatasd[6], 3);
          lib_memcpy(&pdatasd[9], pinfo->roomID, 4);
          lib_memcpy(&pdatasd[13], pinfo->icID, 4);
          pdatasd[17] = dec2bcd(pinfo->onoffFactor);
          lib_memcpy(&pdatasd[18], pinfo->valveAddrH, 4);

          pdatasd[22] = pinfo->commType;
          pdatasd[23] = pinfo->systemID;
          pdatasd[24] = pinfo->meterAccount;
          lib_memcpy(&pdatasd[25], pinfo->meterAddr, 8);
          pdatasd[33] = pinfo->devType;
          pdatasd[34] = pinfo->devVendor;
          pdatasd[35] = pinfo->devConfig;
          datalen = 36;

          // 发送数据
          CmdSendToHost(datalen, recvErr, sendErr);

          if (CMD_PORT_NB != s_tCmdRecv.port)
            SYSDelayMs(REC_CMDInterval);
        }
      }

      // 释放内存
      SYSFree(pinfo);
      pinfo = NULL;

      // 发送返回初始化结束
      s_ucCmdSendBuf[CMD_OFS_FUNC + 0] = UINT16_HI(CMD_INIT_ALL_USER_INFO_END);
      s_ucCmdSendBuf[CMD_OFS_FUNC + 1] = UINT16_LO(CMD_INIT_ALL_USER_INFO_END);

      datalen = 4;
      pdatasd[0] = dec2bcd(REC_UserNbr / 100);
      pdatasd[1] = dec2bcd(REC_UserNbr % 100);
      pdatasd[2] = dec2bcd(REC_UserMeterNbr / 100);
      pdatasd[3] = dec2bcd(REC_UserMeterNbr % 100);

      BIT_CLEAR(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_START));
      BIT_CLEAR(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_END));
    }break;

    case CMD_INIT_ALL_USER_INFO_END:                   /*!< 初始化所有住户信息结束 */
    {
      if (RecordApp_ReadSysParam() != REC_OK)
        sendErr = CMD_ERR_Record;

      datalen = 4;
      pdatasd[0] = dec2bcd(REC_UserNbr / 100);
      pdatasd[1] = dec2bcd(REC_UserNbr % 100);
      pdatasd[2] = dec2bcd(REC_UserMeterNbr / 100);
      pdatasd[3] = dec2bcd(REC_UserMeterNbr % 100);

      BIT_CLEAR(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_START));
      BIT_CLEAR(s_ucCmdInitFlag, BIT(CMD_FLAG_INIT_USER_END));
    }break;

    case CMD_INIT_ONE_USER_INFO:                       /*!< 初始化单个住户信息 */
    case CMD_GET_INIT_ONE_USER_INFO:                   /*!< 读取初始化单个住户信息 */
    {
      UM_Info_t *pinfo = NULL;

      if (recvErr != CMD_OK)
        break;

      if (RecordApp_ReadSysParam() != REC_OK)
        sendErr = CMD_ERR_Record;
      else if (dat >= REC_UserNbr_MAX)
        sendErr = CMD_ERR_Param;
      else if (NULL == (pinfo = (UM_Info_t *)SYSMalloc(sizeof(UM_Info_t))))
        sendErr = CMD_ERR_Malloc;
      // 返回地址
      else if (Record_Read(REC_DT_User, REC_DI_InitInfo, dat, pinfo) != REC_OK)
        sendErr = CMD_ERR_Record;
      else
      {
        pdatasd[0] = pinfo->index[0];
        pdatasd[1] = pinfo->index[1];
        lib_memcpy(&pdatasd[2], pinfo->valveAddr, 4);
        dec2bcds(pinfo->heatArea, &pdatasd[6], 3);
        lib_memcpy(&pdatasd[9], pinfo->roomID, 4);
        lib_memcpy(&pdatasd[13], pinfo->icID, 4);
        pdatasd[17] = dec2bcd(pinfo->onoffFactor);
        lib_memcpy(&pdatasd[18], pinfo->valveAddrH, 4);
        pdatasd[22] = dec2bcd(REC_UserNbr / 100);
        pdatasd[23] = dec2bcd(REC_UserNbr % 100);

        pdatasd[24] = pinfo->commType;
        pdatasd[25] = pinfo->systemID;
        pdatasd[26] = pinfo->meterAccount;
        lib_memcpy(&pdatasd[27], pinfo->meterAddr, 8);
        pdatasd[35] = pinfo->devType;
        pdatasd[36] = pinfo->devVendor;
        pdatasd[37] = pinfo->devConfig;
        pdatasd[38] = dec2bcd(REC_UserMeterNbr / 100);
        pdatasd[39] = dec2bcd(REC_UserMeterNbr % 100);
        datalen = 40;
      }
      // 释放内存
      SYSFree(pinfo);
      pinfo = NULL;
    }break;

    // 读取周期数据命令
    case CMD_REPLY_GET_ALL_USER_METER_CYCLE_DATA:      /*!< 回复读取所有住户热表周期数据 */
    {
      UM_Info_t *pinfo = NULL;
      UM_Cycle_t *pcycle = NULL;
      uint16_t cmdTmp = 0;

      datalen = 35 + 2;
      pinfo = (UM_Info_t *)SYSMalloc(sizeof(UM_Info_t));
      pcycle = (UM_Cycle_t *)SYSMalloc(sizeof(UM_Cycle_t));
      if ((NULL == pinfo) || (NULL == pcycle))
      {
        sendErr = CMD_ERR_Malloc;
      }
      else
      {
        // 返回所有周期数据
        for (cmdTmp = 0; cmdTmp < REC_UserNbr; cmdTmp++)
        {
          if (Record_Read(REC_DT_User, REC_DI_InitInfo, cmdTmp, pinfo) != REC_OK)
            continue;

          if (!lib_memvalcmp(&pinfo->meterAddr[1], 7, 0x00))
            continue;

          if (Record_Read(REC_DT_User, REC_DI_Cycle, cmdTmp, pcycle) != REC_OK)
            lib_memset(pcycle, 0x00, sizeof(UM_Cycle_t));

          pdatasd = &s_ucCmdSendBuf[CMD_OFS_DATA];
          lib_memcpy(&pdatasd[0], pinfo->index, 2);
          pdatasd += 2;

          lib_memcpy(&pdatasd[0], pinfo->meterAddr, 8);
          dec2bcds_ex(pcycle->totalHeat, &pdatasd[8], 5);
          dec2bcds(pcycle->totalFlow, &pdatasd[13], 4);
          dec2bcds(pcycle->supplyTemp, &pdatasd[17], 3);
          dec2bcds(pcycle->backTemp, &pdatasd[20], 3);
          dec2bcds(pcycle->instFlow, &pdatasd[23], 4);
          if (pcycle->online & 0x01)
          {
            if (pcycle->online & 0x02)
              pdatasd[27] = 0x75;
            else
              pdatasd[27] = 0x65;
          }
          else
            pdatasd[27] = 0x00;
          pdatasd[28] = pcycle->status;
          lib_memcpy(&pdatasd[29], pcycle->time, 6);
          datalen = 35 + 2;

          // 发送数据
          CmdSendToHost(datalen, recvErr, sendErr);

          if (CMD_PORT_NB != s_tCmdRecv.port)
            SYSDelayMs(REC_CMDInterval);
        }
      }

      // 释放内存
      SYSFree(pinfo);
      SYSFree(pcycle);
      pinfo = NULL;
      pcycle = NULL;

      datalen = 0;
      s_ucCmdSendBuf[CMD_OFS_FUNC + 0] = UINT16_HI(CMD_GET_ALL_USER_METER_CYCLE_DATA_END);
      s_ucCmdSendBuf[CMD_OFS_FUNC + 1] = UINT16_LO(CMD_GET_ALL_USER_METER_CYCLE_DATA_END);
    }break;

    case CMD_GET_ALL_USER_METER_CYCLE_DATA_END:        /*!< 读取所有住户热表周期数据结束 */
    {
      datalen = 0;
    }break;

    case CMD_GET_ONE_USER_METER_CYCLE_DATA:            /*!< 采集单个住户热表周期数据 */
    {
      UM_Info_t *pinfo = NULL;
      UM_Cycle_t *pcycle = NULL;

      datalen = 35 + 2;
      lib_memset(&pdatasd[0], 0x00, datalen);
      lib_memcpy(&pdatasd[0], &pdatard[0], 10);
      if (dat >= REC_UserNbr || recvErr)
        sendErr = CMD_ERR_Param;
      else if (NULL == (pinfo = (UM_Info_t *)SYSMalloc(sizeof(UM_Info_t))))
        sendErr = CMD_ERR_Malloc;
      else if (NULL == (pcycle = (UM_Cycle_t *)SYSMalloc(sizeof(UM_Cycle_t))))
        sendErr = CMD_ERR_Malloc;
      else if (Record_Read(REC_DT_User, REC_DI_InitInfo, dat, pinfo) != REC_OK)
        sendErr = CMD_ERR_Record;
      else
      {
        if (!lib_memvalcmp(&pinfo->meterAddr[1], 7, 0x00))
          sendErr = CMD_ERR_Param;

        if (Record_Read(REC_DT_User, REC_DI_Cycle, dat, pcycle) != REC_OK)
          lib_memset(pcycle, 0x00, sizeof(UM_Cycle_t));

        pdatasd = &s_ucCmdSendBuf[CMD_OFS_DATA];
        lib_memcpy(&pdatasd[0], pinfo->index, 2);
        pdatasd += 2;

        lib_memcpy(&pdatasd[0], pinfo->meterAddr, 8);
        dec2bcds_ex(pcycle->totalHeat, &pdatasd[8], 5);
        dec2bcds(pcycle->totalFlow, &pdatasd[13], 4);
        dec2bcds(pcycle->supplyTemp, &pdatasd[17], 3);
        dec2bcds(pcycle->backTemp, &pdatasd[20], 3);
        dec2bcds(pcycle->instFlow, &pdatasd[23], 4);
        if (pcycle->online & 0x01)
        {
          if (pcycle->online & 0x02)
            pdatasd[27] = 0x75;
          else
            pdatasd[27] = 0x65;
        }
        else
          pdatasd[27] = 0x00;
        pdatasd[28] = pcycle->status;
        lib_memcpy(&pdatasd[29], pcycle->time, 6);
        datalen = 35 + 2;
      }

      // 释放内存
      SYSFree(pinfo);
      SYSFree(pcycle);
      pinfo = NULL;
      pcycle = NULL;
    }break;

    // 采集实时(Real Time)数据命令
    case CMD_GET_USER_METER_RT_DATA:                   /*!< 采集户用热表的实时数据 */
    {
      UM_Info_t *pinfo = NULL;
      UM_Inst_t *pcycle = NULL;

      datalen = 35 + 2;
      lib_memset(&pdatasd[0], 0x00, datalen);
      lib_memcpy(&pdatasd[0], &pdatard[0], 10);
      if (dat >= REC_UserNbr/* || recvErr*/)
        sendErr = CMD_ERR_Param;
      else if (NULL == (pinfo = (UM_Info_t *)SYSMalloc(sizeof(UM_Info_t))))
        sendErr = CMD_ERR_Malloc;
      else if (NULL == (pcycle = (UM_Inst_t *)SYSMalloc(sizeof(UM_Inst_t))))
        sendErr = CMD_ERR_Malloc;
      else if (Record_Read(REC_DT_User, REC_DI_InitInfo, dat, pinfo) != REC_OK)
        sendErr = CMD_ERR_Record;
      else
      {
        if (Record_Read(REC_DT_User, REC_DI_RTime, dat, pcycle) != REC_OK)
          lib_memset(pcycle, 0x00, sizeof(UM_Inst_t));

        pdatasd = &s_ucCmdSendBuf[CMD_OFS_DATA];
        lib_memcpy(&pdatasd[0], pinfo->index, 8);
        pdatasd += 2;

        lib_memcpy(&pdatasd[0], pinfo->meterAddr, 8);

        dec2bcds_ex(pcycle->totalHeat, &pdatasd[8], 5);
        dec2bcds(pcycle->totalFlow, &pdatasd[13], 4);
        dec2bcds(pcycle->supplyTemp, &pdatasd[17], 3);
        dec2bcds(pcycle->backTemp, &pdatasd[20], 3);
        dec2bcds(pcycle->instFlow, &pdatasd[23], 4);
        if(pcycle->online & 0x01)
          pdatasd[27] = 0x55;
        else
          pdatasd[27] = 0x00;
        pdatasd[28] = pcycle->status;
        lib_memcpy(&pdatasd[29], pcycle->time, 6);
        datalen = 35 + 2;
      }
      // 释放内存
      SYSFree(pinfo);
      SYSFree(pcycle);
      pinfo = NULL;
      pcycle = NULL;
    }break;

    default:
      datalen = 0;
      break;
  }

  // 发送数据
  CmdSendToHost(datalen, recvErr, sendErr);

  // 发送到NB-IoT
  if (CMD_PORT_NB == s_tCmdRecv.port)
    nbAppStartSend();

  return sendErr;
}


/**
 * @brief  Parse AT command string
 *
 * @param  str strings
 * @param  len length
 *
 * @retval 0 Success, Other is error
 */
uint8_t CmdParseAT(char *str, uint16_t len)
{
  int32_t itmp = 0;
  uint8_t ret = 0;

  char *pCmdStr = NULL;
  char *pRspStr = NULL;
  char *pTmpStr = NULL;
  const NB_t *pnb = nbGetObj();

  // COMMIT CONFIG
  if (!strncasecmp(str, "COMMIT CONFIG", 13))
  {
    CmdSendStr("+Enter Config: OK!\r\n");
    return 0;
  }

  // AT+CONFIGEND
  if (!strncasecmp(str, "AT+CONFIGEND", 12))
  {
    if (RecordApp_SaveNetParam())
      CmdSendStr("+Exit Config:ERROR!\r\n");
    else
      CmdSendStr("+Exit Config:OK!\r\n");

    return 0;
  }

  // 调试指令
  
  // AT
  if (!strncasecmp(str, "AT\r", 3))
    CmdSendStr(RSP_OK);

  // AT+LIST
  else if (!strncasecmp(str, "AT+LIST", 7))
  {
    CmdSendStr(RSP_AT_LIST);
    CmdSendStr(RSP_OK);
  }

  // AT+SHOW
  else if (!strncasecmp(str, "AT+SHOW", 7))
  {
    extern void CmdATShowReply(void);
    CmdATShowReply();
    CmdSendStr(RSP_OK);
  }
  // AT+QUIT
  else if (!strncasecmp(str, "AT+QUIT", 7))
  {
    if (RecordApp_SaveNetParam())
      CmdSendStr(RSP_ERROR);
    else
      CmdSendStr(RSP_OK);
  }
  // AT+RESET
  else if (!strncasecmp(str, "AT+RESET", 8))
  {
    if (RecordApp_SaveNetParam())
      CmdSendStr(RSP_ERROR);
    else
      CmdSendStr(RSP_OK);

    NVIC_SystemReset();
  }

  // 组合指令
  pCmdStr = strtok_r(str, "&", &pTmpStr);
  if (NULL == pCmdStr)
    pCmdStr = strstr(str, "AT+");

  pRspStr = (char *)s_ucCmdSendBuf;
  if (NULL == pCmdStr)
    return 1;

  lib_memset(s_ucCmdSendBuf, 0x00, sizeof(s_ucCmdSendBuf));

AT_SEARACH:

  // AT+IDNT
  if (!strncasecmp(pCmdStr, "AT+IDNT", 7))
  {
    sprintf(pRspStr, "+IDNT:%02X%02X%02X%02X\r\n",
            APP_DEV_ADDR[0], APP_DEV_ADDR[1], APP_DEV_ADDR[2], APP_DEV_ADDR[3]);
  }

  // AT+VERS
  else if (!strncasecmp(pCmdStr, "AT+VERS", 7))
  {
    sprintf(pRspStr, "+VERS:%X.%X.%X.%X DATE %s\r\n",
            APP_SOFT_VER[0], APP_SOFT_VER[1], APP_SOFT_VER[2], APP_SOFT_VER[3],
            __DATE__);
  }

  // AT+TEMPNUM
  else if (!strncasecmp(pCmdStr, "AT+TEMPNUM", 10))
  {
    sprintf(pRspStr, "+TEMPNUM:%d\r\n", 0);
  }
  
  // AT+TEMP
  else if (!strncasecmp(pCmdStr, "AT+TEMP", 7))
  {
    sprintf(pRspStr, "+TEMP:%d\r\n", pnb->rssi);
  }

  // AT+TXTIME
  else if (!strncasecmp(pCmdStr, "AT+TXTIME", 9))
  {
    sprintf(pRspStr, "+TXTIME:%d\r\n", REC_AllocPeriod);
  }

  // AT+PHON
  else if (!strncasecmp(pCmdStr, "AT+PHON", 7))
  {
    sprintf(pRspStr, "+PHON:%s\r\n", pnb->iccid);
  }

  // AT+IMEI
  else if (!strncasecmp(pCmdStr, "AT+IMEI", 7))
  {
    sprintf(pRspStr, "+IMEI:%s\r\n", pnb->imei);
  }

  // AT+IPAD
  else if (!strncasecmp(pCmdStr, "AT+IPAD", 7))
  {
    pCmdStr += 7;
    if (('=' == *pCmdStr) && strncasecmp(pCmdStr, "=?", 2))
      lib_strncpybrk(g_tNetParam.mainAddr, pCmdStr + 1, sizeof(g_tNetParam.mainAddr), '\r');

    sprintf(pRspStr, "+IPAD:%s\r\n", g_tNetParam.mainAddr);
  }

  // AT+PORT
  else if (!strncasecmp(pCmdStr, "AT+PORT", 7))
  {
    pCmdStr += 7;
    if (('=' == *pCmdStr) && strncasecmp(pCmdStr, "=?", 2))
    {
      if (0 != (itmp = lib_atoi(pCmdStr + 1)))
        g_tNetParam.mainPort = itmp;
    }

    sprintf(pRspStr, "+PORT:%d\r\n", g_tNetParam.mainPort);
  }

  // AT+IPSEC
  else if (!strncasecmp(pCmdStr, "AT+IPSEC", 8))
  {
    pCmdStr += 8;
    if (('=' == *pCmdStr) && strncasecmp(pCmdStr, "=?", 2))
      lib_strncpybrk(g_tNetParam.bakAddr, pCmdStr + 1, sizeof(g_tNetParam.bakAddr), '\r');
    
    sprintf(pRspStr, "+IPSEC:%s\r\n", g_tNetParam.bakAddr);
  }

  // AT+PTSEC
  else if (!strncasecmp(pCmdStr, "AT+PTSEC", 8))
  {
    pCmdStr += 8;
    if (('=' == *pCmdStr) && strncasecmp(pCmdStr, "=?", 2))
    {
      if (0 != (itmp = lib_atoi(pCmdStr + 1)))
        g_tNetParam.bakPort = itmp;
    }

    sprintf(pRspStr, "+PTSEC:%d\r\n", g_tNetParam.bakPort);
  }

  // AT+APN
  else if (!strncasecmp(pCmdStr, "AT+APN", 6))
  {
    pCmdStr += 6;

    if (('=' == *pCmdStr) && strncasecmp(pCmdStr, "=?", 2))
      lib_strncpybrk(g_tNetParam.apn, pCmdStr + 1, sizeof(g_tNetParam.apn), '\r');

    sprintf(pRspStr, "+APN:%s\r\n", g_tNetParam.apn);
  }

  // AT+USER
  else if (!strncasecmp(pCmdStr, "AT+USER", 7))
  {
    pCmdStr += 7;
    if (('=' == *pCmdStr) && strncasecmp(pCmdStr, "=?", 2))
      lib_strncpybrk(g_tNetParam.apnUser, pCmdStr + 1, sizeof(g_tNetParam.apnUser), '\r');

    sprintf(pRspStr, "+USER:%s\r\n", g_tNetParam.apnUser);
  }

  // AT+PWD
  else if (!strncasecmp(pCmdStr, "AT+PWD", 6))
  {
    pCmdStr += 6;
    if (('=' == *pCmdStr) && strncasecmp(pCmdStr, "=?", 2))
      lib_strncpybrk(g_tNetParam.apnPasswd, pCmdStr + 1, sizeof(g_tNetParam.apnPasswd), '\r');
      
    sprintf(pRspStr, "+PWD:%s\r\n", g_tNetParam.apnPasswd);
  }


  if (strlen((char *)s_ucCmdSendBuf) >= sizeof(s_ucCmdSendBuf))
  { 
    s_ucCmdSendBuf[sizeof(s_ucCmdSendBuf) - 1] = '\0';
    CmdSendStr((char *)s_ucCmdSendBuf);
    return 1;
  }
    
  pCmdStr = strtok_r(NULL, "&", &pTmpStr);
  pRspStr = (char *)s_ucCmdSendBuf + strlen((char *)s_ucCmdSendBuf);
  if (pCmdStr)
    goto AT_SEARACH;

  
  RecordApp_SaveNetParam();
  
  s_ucCmdSendBuf[sizeof(s_ucCmdSendBuf) - 1] = '\0';
  CmdSendStr((char *)s_ucCmdSendBuf);
  
  return ret;
}

/**
 * @brief  print app info
 *
 * @param  none
 *
 * @retval none
 */
void CmdATDispInfo(void)
{
  extern AppRunObj_t s_tAppObj;
  
  CmdSendStr(RSP_CRLF);
  sprintf((char *)s_ucCmdSendBuf, "Device   %02X%02X%02X%02X\r\n", APP_DEV_ADDR[0], APP_DEV_ADDR[1], APP_DEV_ADDR[2], APP_DEV_ADDR[3]);
  CmdSendStr((char *)s_ucCmdSendBuf);
  
  sprintf((char *)s_ucCmdSendBuf, "Version  %X.%X.%X.%X\r\n", APP_SOFT_VER[0], APP_SOFT_VER[1], APP_SOFT_VER[2], APP_SOFT_VER[3]);
  CmdSendStr((char *)s_ucCmdSendBuf);
  
  sprintf((char *)s_ucCmdSendBuf, "Time     %04X-%02X-%02X %02X:%02X:%02X Week-%d\r\n", g_tSysTime.year, g_tSysTime.month, g_tSysTime.day, \
                                                                g_tSysTime.hour, g_tSysTime.minute, g_tSysTime.second, \
                                                                g_tSysTime.week);
  CmdSendStr((char *)s_ucCmdSendBuf);
  
  sprintf((char *)s_ucCmdSendBuf, "Season   %02X-%02X %02X-%02X\r\n", g_tSysParam.heatSeason[0], g_tSysParam.heatSeason[1], \
                                              g_tSysParam.heatSeason[2], g_tSysParam.heatSeason[3]);
  CmdSendStr((char *)s_ucCmdSendBuf);
  
  sprintf((char *)s_ucCmdSendBuf, "Period   %d\r\n", REC_AllocPeriod);
  CmdSendStr((char *)s_ucCmdSendBuf);
  
  sprintf((char *)s_ucCmdSendBuf, "User     %d\r\n", REC_UserNbr);
  CmdSendStr((char *)s_ucCmdSendBuf);
  
  sprintf((char *)s_ucCmdSendBuf, "APPErr   %02X\r\n", s_tAppObj.error);
  CmdSendStr((char *)s_ucCmdSendBuf);

  if (nbAppGetRunState() > NB_State_Idle)
  {
    const NB_t *pnb = nbGetObj();
    const NBAppRunObj_t *pnbapp = nbAppGetObj();
    sprintf((char *)s_ucCmdSendBuf, "NBErr    %02X\r\n", pnb->cmdRetVal);
    CmdSendStr((char *)s_ucCmdSendBuf);
    
    sprintf((char *)s_ucCmdSendBuf, "IMEI     %s\r\n", pnb->imei);
    CmdSendStr((char *)s_ucCmdSendBuf);
    
    sprintf((char *)s_ucCmdSendBuf, "ICCID    %s\r\n", pnb->iccid);
    CmdSendStr((char *)s_ucCmdSendBuf);
    
    sprintf((char *)s_ucCmdSendBuf, "RSSI     %d\r\n", pnb->rssi);
    CmdSendStr((char *)s_ucCmdSendBuf);
    
    sprintf((char *)s_ucCmdSendBuf, "NETSTAT  %d\r\n", pnb->netStat);
    CmdSendStr((char *)s_ucCmdSendBuf);
    
    sprintf((char *)s_ucCmdSendBuf, "IP       %d.%d.%d.%d\r\n", pnb->ipAddr[0], pnb->ipAddr[1], pnb->ipAddr[2], pnb->ipAddr[3]);
    CmdSendStr((char *)s_ucCmdSendBuf);
    
    sprintf((char *)s_ucCmdSendBuf, "CMEErr   %d\r\n", pnb->cme);
    CmdSendStr((char *)s_ucCmdSendBuf);

    sprintf((char *)s_ucCmdSendBuf, "NBAPPErr %02X\r\n", pnbapp->error);
    CmdSendStr((char *)s_ucCmdSendBuf);

    sprintf((char *)s_ucCmdSendBuf, "NBStat   %d\r\n", pnbapp->state);
    CmdSendStr((char *)s_ucCmdSendBuf);

    sprintf((char *)s_ucCmdSendBuf, "NBRstCnt %d\r\n", pnbapp->restartCnt);
    CmdSendStr((char *)s_ucCmdSendBuf);

    sprintf((char *)s_ucCmdSendBuf, "NBSndUse %d/%d\r\n", uxQueueMessagesWaiting(pnbapp->queueSend), NBAPP_QUEUE_DATA_SEND);
    CmdSendStr((char *)s_ucCmdSendBuf);

    sprintf((char *)s_ucCmdSendBuf, "RAM      %d\r\n", xPortGetFreeHeapSize());
    CmdSendStr((char *)s_ucCmdSendBuf);

    sprintf((char *)s_ucCmdSendBuf, "TaskInit %d\r\n", uxTaskGetStackHighWaterMark(g_hTaskInit));
    CmdSendStr((char *)s_ucCmdSendBuf);

    sprintf((char *)s_ucCmdSendBuf, "TaskCom  %d\r\n", uxTaskGetStackHighWaterMark(g_hTaskCom));
    CmdSendStr((char *)s_ucCmdSendBuf);
  }
  
  CmdSendStr(RSP_CRLF);
}


/**
 * @brief : AT+SHOW return
 *
 * @param : None
 *
 * @retval: None
 */
void CmdATShowReply(void)
{
  CmdSendStr("Access Point Name:                              ");
  CmdSendStr(g_tNetParam.apn);
  CmdSendStr(RSP_CRLF);

  CmdSendStr("PPP Authentication User Id:                     ");
  CmdSendStr(g_tNetParam.apnUser);
  CmdSendStr(RSP_CRLF);

  CmdSendStr("PPP Authentication Password:                    ");
  CmdSendStr(g_tNetParam.apnPasswd);
  CmdSendStr(RSP_CRLF);

  CmdSendStr("Server's Ip:                                    ");
  CmdSendStr(g_tNetParam.mainAddr);
  CmdSendStr(RSP_CRLF);

  CmdSendStr("Server's Port                                   ");
  sprintf((char *)s_ucCmdSendBuf, "%d", g_tNetParam.mainPort);
  CmdSendStr((char *)s_ucCmdSendBuf);
  CmdSendStr(RSP_CRLF);

  CmdSendStr("Secondary Server's Ip                           ");
  CmdSendStr(g_tNetParam.bakAddr);
  CmdSendStr(RSP_CRLF);

  CmdSendStr("Secondary Server's Port                         ");
  sprintf((char *)s_ucCmdSendBuf, "%d", g_tNetParam.bakPort);
  CmdSendStr((char *)s_ucCmdSendBuf);
  CmdSendStr(RSP_CRLF);

  CmdATDispInfo();
}


/**
 * @brief  Send String
 *
 * @param  str strings
 *
 * @retval 0Success
 */
uint8_t CmdSendStr(const char *str)
{
  uint16_t len = strlen(str);
  uint16_t sndlen = 0;

  if (CMD_PORT_MBUS == s_tCmdRecv.port)
    sndlen = BusSend(BUS_MBUS_0 ,(const uint8_t *)str, len);
    
  if (CMD_PORT_RS485 == s_tCmdRecv.port)
    sndlen = BusSend(BUS_RS485_0 ,(const uint8_t *)str, len);

  if (CMD_PORT_NB == s_tCmdRecv.port)
  {
    sndlen = nbAppWriteSendBuff((const uint8_t *)str, len);
    nbAppStartSend();
  }

  return (len == sndlen)? 0 : 1;
}

