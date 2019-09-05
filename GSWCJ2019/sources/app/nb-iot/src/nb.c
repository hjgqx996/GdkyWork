/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb.c
  * @author   ZouZH
  * @version  V1.00
  * @date     02-Aug-2018
  * @brief    NB-IoT core.
  ******************************************************************************
  * @history
  */


/* INCLUDES ----------------------------------------------------------------- */
#include "nb.h"

#include "includes.h"
#include "systick.h"

/**
 * @defgroup NB-IoT
 * @brief NB-IoT物联网模块驱动
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */
//AT command init sequence, state machine.
#define INIT_CMD_START      1
#define INIT_CMD_AT         2   
#define INIT_CMD_AT_WAIT    3      
#define INIT_CMD_ATE0       4   /* Disable ECHO */
#define INIT_CMD_ATE0_WAIT  5
#define INIT_CMD_CGMR       6   /* GET CGMR Version */
#define INIT_CMD_CGMR_WAIT  7
#define INIT_CMD_CMEE       8   /* Enable numeric error codes */
#define INIT_CMD_CMEE_WAIT  9
#define INIT_CMD_CIMI       10   
#define INIT_CMD_CIMI_WAIT  11  /* Get IMSI for SIM Card status */
#define INIT_CMD_CGSN       12   
#define INIT_CMD_CGSN_WAIT  13  /* Get IMEI */
#define INIT_CMD_OK         14   
#define INIT_NET_DETACH_RUN 15  /* Network Command Run Detach GSM base station */
   
/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */
NB_t nbobj;
//SIM卡20位串号 ICCID
uint8_t SimCardNum[20] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
                          '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'}; 
////uint8_t pu8PhoneNum[12]= {'1', '3', '7', '0', '0', '0', 
////                          '0', '0', '0', '0', '0', 0}; //电话号码
/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */
static uint8_t  s_uCmdRunState    = 0;//0:set cmd; 1:wait ultil finish.
static uint8_t  s_uCmdBlockState  = 0;//0:set nbobj.Flags.F.isBlocking; 1:detect busy.
static uint8_t  s_uCmdPrepare     = 0;//0:prepare work; 1:other.  

/* LOCAL FUNCTIONS ---------------------------------------------------------- */
NB_Error_t nb_default_evt_cb(NB_CB_Param_t* cbParam);


/**
 * @brief  Init NB-IoT Driver
 * @param[in]  baud: uart baudrate
 * @param[in]  cb:   nb driver event callback function
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbInit(uint32_t baud, NB_EventCallback_t cb) {

    memset(&nbobj, 0x00, sizeof(NB_t));


    /* Set callback */
    nbobj.cbFunc = cb;
    if (NULL == nbobj.cbFunc) {
        nbobj.cbFunc = nb_default_evt_cb;
    }

    /* Set start values */
    nbobj.timeMs = 0;

    nb_buff_free(&nbobj.buff);
    nb_buff_init(&nbobj.buff, NB_CFG_BUFF_SIZE, nbobj.bufDat);

    /* Low-Level initialization */
    if (nbHWInit(baud)) {
        NB_RETURN(NB_ERRHW);
    }

    /* RTOS support */
  #if NB_CFG_OS
    if (nbOsMutexInit()) {
        NB_RETURN(NB_ERRMEM);
    }
  #endif

    /* Init all threads */
    NB_RST_THREADS();

    /* Initialization commands */
    nbobj.Flags.F.isBlocking = 0;
    nbobj.cmdTmout = 1000;

    NB_SET_IDLE();
    nbobj.Flags.F.callIdle = 0;

    NB_RETURN(nbobj.cmdRetVal);
}

/**
 * @brief  wait process when Module AT command init sequence
 * @param[out] pu8State: machine state of AT command init sequence
 * @param[out] pu8Retry: retry times.
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbInitCmdWait(uint8_t * pu8State, uint8_t * pu8Retry) {
  
  NB_Error_t      retVal = NB_CMD_WAIT;    /*!< Return value */
  
  if(NB_IS_BUSY())//busy, please wait.
  {}
  else if((--(*pu8Retry)) && (nbobj.cmdRetVal != NB_OK))//failure , retry
  {
    (*pu8State)--;
  }
  else if((nbobj.cmdRetVal != NB_OK)&&(*pu8State == INIT_CMD_AT_WAIT))//failure
  {
    retVal = nbobj.cmdRetVal;
    *pu8State = INIT_CMD_START;
  }
  else//OK
  {
    *pu8Retry = 2;//retry times
    (*pu8State)++;
  }
  
  if((*pu8State < INIT_CMD_START) || (*pu8State > INIT_CMD_OK))
  {
    *pu8State = INIT_CMD_START;
  }
  
  NB_RETURN(retVal);
}

/**
 * @brief  Module AT command init sequence
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbInitCmd(void) {
    NB_Error_t      retVal = NB_CMD_WAIT;    /*!< Return value */
    static uint8_t  s_u8Retry = 0;//retry times
    static uint8_t  s_u8State = INIT_CMD_START;//
    
    switch(s_u8State)
    {
    case INIT_CMD_START:
      {
        /* Send initialization commands */
        nbobj.Flags.F.isBlocking = 1;
        nbobj.cmdTmout = 1000;
        nbobj.cmdRetVal = NB_TIMEOUT;
        memset((void *)&nbobj.cmdParam, 0x00, sizeof(NB_CmdParam_t));
        
        s_u8Retry = 10;//retry times
        s_u8State = INIT_CMD_AT;
      }//break;
    case INIT_CMD_AT:
      {
        NB_SET_CMD(NB_CMD_AT);
        nbobj.cmdTmout = 1000;
        s_u8State = INIT_CMD_AT_WAIT;
      }break;
    case INIT_CMD_AT_WAIT:
      {
        retVal = nbInitCmdWait(&s_u8State, &s_u8Retry);
      }break;
    case INIT_CMD_ATE0:/* Disable ECHO */
      {
        NB_SET_CMD(NB_CMD_ATE0);
        nbobj.cmdTmout = 1000;
        s_u8State = INIT_CMD_ATE0_WAIT;
      }break;
    case INIT_CMD_ATE0_WAIT:
      {
        retVal = nbInitCmdWait(&s_u8State, &s_u8Retry);
      }break;
    case INIT_CMD_CGMR:/* GET CGMR Version */
      {
        NB_SET_CMD(NB_CMD_CGMR);
        nbobj.cmdTmout = 1000;
        s_u8State = INIT_CMD_CGMR_WAIT;
      }break;
    case INIT_CMD_CGMR_WAIT:
      {
        retVal = nbInitCmdWait(&s_u8State, &s_u8Retry);
      }break;    
    case INIT_CMD_CMEE:/* Enable numeric error codes */
      {
        NB_SET_CMD(NB_CMD_CMEE);
        nbobj.cmdTmout = 1000;
        s_u8State = INIT_CMD_CMEE_WAIT; 
      }break;
    case INIT_CMD_CMEE_WAIT:
      {
        retVal = nbInitCmdWait(&s_u8State, &s_u8Retry);
      }break;
    case INIT_CMD_CIMI:/* Get IMSI for SIM Card status */
      {
        NB_SET_CMD(NB_CMD_CIMI);
        nbobj.cmdTmout = 1000;
        s_u8State = INIT_CMD_CIMI_WAIT;
      }break;
    case INIT_CMD_CIMI_WAIT:
      {
        retVal = nbInitCmdWait(&s_u8State, &s_u8Retry);
      }break;
    case INIT_CMD_CGSN:/* Get IMEI */
      {
        NB_SET_CMD(NB_CMD_CGSN);
        nbobj.cmdTmout = 1000;
        s_u8State = INIT_CMD_CGSN_WAIT;
      }break;
    case INIT_CMD_CGSN_WAIT:
      {
        retVal = nbInitCmdWait(&s_u8State, &s_u8Retry);
      }break; 
    case INIT_CMD_OK:
      {
        nbobj.Flags.F.isBlocking = 0;
        NB_SET_IDLE();
        nbobj.Flags.F.callIdle = 0;
        
        //retVal = nbobj.cmdRetVal;
        s_u8State = INIT_NET_DETACH_RUN; 
      }break; 
    case INIT_NET_DETACH_RUN:/* Network Command Run Detach GSM base station */
      {
        retVal = nbNETDetachRun();
        if((NB_CMD_WAIT != retVal))
        {
          s_u8State = INIT_CMD_START;//INIT_OK; 
        }
      }break;
    default:
      {s_u8State = INIT_CMD_START;}
      break;
    }
    
    NB_RETURN(retVal);
}

/**
 * @brief  Get @ref NB_t driver const object point
 * @param  void
 * @return @ref NB_t object point
 */
const NB_t *nbGetObj(void) {
    NB_t *pnb = &nbobj;

    return pnb;
}

/**
 * @brief  General Command Set "AT+CFUN"
 * @param[in]  func: 0 or 1
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbGENSetCFUN(uint8_t func) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_CFUN_SET);

    nbobj.cmdParam.func.cfun = func;

    NB_RETURN_BLOCK(NB_CFG_BLOCKING, 10000);
}

/**
 * @brief  General Command Get "AT+CFUN"
 * @param[out] func: output CFUN state 0 or 1
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbGENGetCFUN(uint8_t *func) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_CFUN_GET);
    NB_WAIT_BLOCK(NB_CFG_BLOCKING, 1000);

    if (func) {
      *func = nbobj.cfun;
    }

    return nbobj.retVal;
}

/**
 * @brief  Information Command Get IMEI
 * @param[out] str: output IMEI string buffer
 * @param[in]  length: string buffer length(>=16)
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbINFOGetIMEI(char* str, uint8_t length) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_CGSN);
    NB_WAIT_BLOCK(NB_CFG_BLOCKING, 1000);

    if (str) {
      strncpy(str, nbobj.imei, length);
    }

    return nbobj.retVal;
}

/**
 * @brief  Information Command Get IMSI
 * @param[out] str: output IMEI string buffer
 * @param[in]  length: string buffer length(>=16)
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbINFOGetIMSI(char* str, uint8_t length) {
  NB_Error_t res = NB_CMD_WAIT;  
  
  res = nbCmdRun(NB_CMD_CIMI, NB_CFG_BLOCKING, 1000);
  
  if(res != NB_CMD_WAIT) {
    if (str) {
      strncpy(str, nbobj.imsi, length);
    }
  }
  
  return nbobj.retVal;
}

/**
 * @brief  General Command Get "AT+CSQ"
 * @param[out] rssi: output RSSI
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbINFOGetRSSI(uint8_t* rssi) {
  NB_Error_t res = NB_CMD_WAIT;  
  
  res = nbCmdRun(NB_CMD_CSQ, NB_CFG_BLOCKING, 1000);
  
  if(res != NB_CMD_WAIT) {
    if (rssi) {
      *rssi = nbobj.rssi;
    }
  }
  
  return nbobj.retVal;
}

/**
 * @brief  Datetime Command Get module time
 * @param[out] datetime: output @ref NB_Datetime_t datetime
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbDATEGetTime(NB_Datetime_t* datetime) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_DATETIME);

    NB_WAIT_BLOCK(NB_CFG_BLOCKING, 1000);

    if (datetime) {
      const NB_Datetime_t *p = (const NB_Datetime_t *)&nbobj.datetime;
      memcpy(datetime, p, sizeof(NB_Datetime_t));
    }

    return nbobj.retVal;
}

/**
 * @brief  Network Command Get "AT+CREG"
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETGetStaus(void) {
  NB_Error_t res = NB_CMD_WAIT;  
  
  res = nbCmdRun(NB_CMD_CREG, NB_CFG_BLOCKING, 1000);
  
  return res;
}

/**
 * @brief  Network Command Get module ip address
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETGetIPAddr(void) {
  NB_Error_t res = NB_CMD_WAIT;  
  
  res = nbCmdRun(NB_CMD_IP_GET, NB_CFG_BLOCKING, 1000);
  
  return res;
}

/**
 * @brief  Network Command Get "AT+CSCON"
 * @param[out] stat: output CSCON state
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETGetCSCON(uint8_t *stat) {
  NB_Error_t res = NB_CMD_WAIT;  
  
  res = nbCmdRun(NB_CMD_CSCON, NB_CFG_BLOCKING, 1000);
  
  if(res != NB_CMD_WAIT) {
    if (stat) {
      *stat = nbobj.connStat;
    }
  }
  
  return nbobj.retVal;
}

/**
 * @brief  Network Command Set apn parameter
 * @param[in]  apn:  apn string end with '\0'
 * @param[in]  user: user string end with '\0'
 * @param[in]  pwd:  password string end with '\0'
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETSetAPN(const char* apn, const char* user, const char* pwd) {
    NB_CHECK_PARAM(apn);
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_SETAPN);

    nbobj.cmdParam.pdp.apn = apn;
    nbobj.cmdParam.pdp.usr = NULL;
    nbobj.cmdParam.pdp.pwd = NULL;
    if (user && strlen(user)) {
        nbobj.cmdParam.pdp.usr = user;
    }
    if (pwd && strlen(pwd)) {
        nbobj.cmdParam.pdp.pwd = pwd;
    }
    NB_RETURN_BLOCK(NB_CFG_BLOCKING, 3000);
}

/**
 * @brief  Network Command Set remote host address
 * @param[in]  host: remote host ip string end with '\0'
 * @param[in]  port: remote host port
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETSetConn(const char* host, uint16_t port) {
    NB_Error_t res = NB_CMD_WAIT; 
    
    if (!(host)) {
      res = NB_ERRPAR;      
    }    
    else if(s_uCmdPrepare == 0)
    {
      nbobj.cmdParam.host.ip = host;
      nbobj.cmdParam.host.port = port;
      s_uCmdPrepare = 1;
    }
    else {
      res = nbCmdRun(NB_CMD_SETCONN, NB_CFG_BLOCKING, 3000);
    }
    
    NB_RETURN(res);
}

/**
 * @brief  Network Command Run Attach GSM base station
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETAttach(void) {
    NB_Error_t res = NB_CMD_WAIT;  
    
    res = nbCmdRun(NB_CMD_ATTACH, NB_CFG_BLOCKING, 10000);
    NB_RETURN(res);
}

/**
 * @brief  Network Command Run Detach GSM base station
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETDetachRun(void) {
    NB_Error_t res = NB_CMD_WAIT;  
    
    res = nbCmdRun(NB_CMD_DETACH, NB_CFG_BLOCKING, 10000);
    NB_RETURN(res);
}

/**
 * @brief  Network Command Send Data buffer
 * @param[in]  data: data buffer
 * @param[in]  btw:  bytes to write
 * @param[out] bw:   bytes wrote
 * @param[in]  mode: send mode @ref NB_SendMode_t
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETSend(const void* data, uint16_t btw, uint16_t* bw, NB_SendMode_t mode) {
    NB_Error_t res = NB_CMD_WAIT; 
    
    if ((NULL == data) || !btw)
    {
//      NB_Print("Send Net Packet param error!!!");
      res = NB_ERRPAR;   
    }
    else if(s_uCmdPrepare == 0)
    {
      nbobj.cmdParam.send.data = data;
      nbobj.cmdParam.send.btw = btw;
      nbobj.cmdParam.send.bw = bw;
      nbobj.cmdParam.send.mode = mode;
      if (bw) {
          *bw = 0;
      }
      s_uCmdPrepare = 1;
    }
    else {
      res = nbCmdRun(NB_CMD_DATA_SEND, NB_CFG_BLOCKING, NB_CFG_TIME_PSM * 1000);
    }
    
    NB_RETURN(res);
}

/**
 * @brief  Network Command Read Data
 * @param[out] data: data buffer
 * @param[in]  btr:  bytes to read
 * @param[out] br:   bytes readed
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETRead(void* data, uint16_t btr, uint16_t* br) {
  NB_Error_t res = NB_CMD_WAIT; 
  
  if ((NULL == data) || !btr)
  {
    res = NB_ERRPAR;   
  }
  else if(s_uCmdPrepare == 0){    
    nbobj.cmdParam.read.data = data;
    nbobj.cmdParam.read.btr = btr;
    nbobj.cmdParam.read.br = br;
    nbobj.cbParam.UN.recv.recvlen = 0;
    if (br != NULL) {
        *br = 0;
    }
    s_uCmdPrepare = 1;
  }
  else {
    res = nbCmdRun(NB_CMD_DATA_READ, NB_CFG_BLOCKING, 10000);
  }
  
  NB_RETURN(res);
}

/**
 * @brief  NB dirver blocking delay some ms
 * @param[in]  timeout:  ms to delay
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbDelay(uint32_t timeout) {
    uint32_t start = SysTickGetMs();////nbobj.timeMs;
    do {
      #if !NB_CFG_OS && !NB_CFG_ASYNC
        nbUpdate();
      #endif
    } while (SysTickGetMsDif(start, SysTickGetMs()) < timeout);
    NB_RETURN(NB_OK);
}


/**
 * @brief  Get NB dirver is idle state
 * @param  void
 * @retval NB_OK: Ilde state NB_ERR: busy state
 */
NB_Error_t nbIsIdle(void) {
    return (nbobj.cmd != NB_CMD_IDLE)? NB_ERR : NB_OK;
}


/**
 * @brief  Wait NB dirver idle state some ms
 * @param[in]  timeout:  ms to delay
 * @retval NB_OK: Ilde state
 */
NB_Error_t nbWaitIdle(uint32_t timeout) {
    nbobj.cmdTmout = timeout;
    do {
      #if !NB_CFG_OS && !NB_CFG_ASYNC
        nbUpdate();
      #endif
    } while (NB_IS_BUSY());
    NB_RETURN(nbobj.cmdRetVal);
}


/**
 * @brief  Get NB dirver Last return error state
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbGetLastError(void) {
    NB_Error_t tmp = nbobj.cmdRetVal;
    nbobj.cmdRetVal = NB_OK;

    return tmp;
}

/**
 * @brief  Poll update process module receive char and AT command
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbUpdate(void) {
    char ch;

    uint16_t processedCount = 512;

    /* Check for timeout */
    if( (nbobj.cmd != NB_CMD_IDLE) 
      &&(SysTickGetMsDif(nbobj.cmdTime, SysTickGetMs()) > nbobj.cmdTmout)) {
        nbobj.Evts.F.rspTimeout = 1;////NB_TIMEOUT;
        nbobj.Evts.F.rspError = 1;
    }

    while (
      #if !NB_CFG_OS && NB_CFG_ASYNC
        processedCount-- &&
      #else
        processedCount &&
      #endif
        nb_buff_read(&nbobj.buff, &ch, 1)
    ) {
        xprintf("%c", ch);
        switch (ch) {
            case '\n':
                NB_RECV_ADD(ch);
                extern void nb_parse_received(NB_RecvAT_t* prcv);
                nb_parse_received(&nbobj.recv_at);
                NB_RECV_RESET();
                break;
            default:
                NB_RECV_ADD(ch);
                break;
        }
    }

    extern NB_Error_t nbProcessThreads(void);
    return nbProcessThreads();
}


/**
 * @brief  Poll update add driver tick for delay
 * @param  millis: The interval ms between two calls
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbUpdateTime(uint32_t millis) {
    nbobj.timeMs += millis;
    NB_RETURN(NB_OK);
}


/**
 * @brief  Poll call add driver receive char from module
 * @param[in]  ch: receive char buffer
 * @param[in]  ch: receive buffer length
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbDataReceived(uint8_t* ch, uint16_t count) {

    return (nb_buff_write(&nbobj.buff, ch, count) == count) ? NB_OK : NB_ERR;
}


/**
 * @brief  Default callback function for events
 * @param[in]  cbParam: Pointer to callback data structure @ref NB_CB_Param_t
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nb_default_evt_cb(NB_CB_Param_t* cbParam) {
    return NB_OK;
}

/**
 * @brief  Network Command Run Detach GSM base station
 * @param[in]  u8BlockEn: Enables (1) or disables (0) Command execution function blocking.
 * @param[in]  u32Timeout: timeout value
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbCmdBlock(uint8_t u8BlockEn, uint32_t u32Timeout) {
  NB_Error_t res = NB_CMD_WAIT;                
  
  nbobj.cmdTmout = u32Timeout;                      
  if (!(u8BlockEn)) {                                  
    nbobj.Flags.F.isBlocking = 0;            
    NB_RETURN(NB_OK);                        
  }                         
  else {
    if(0 == s_uCmdBlockState) {
      nbobj.Flags.F.isBlocking = 1; 
      s_uCmdBlockState = 1; 
    }
    else {
      if(!NB_IS_BUSY())
      {
        if (nbobj.Evts.F.rspTimeout == 1) {
          res = NB_TIMEOUT;   
          nbobj.cmdRetVal = NB_TIMEOUT; 
        } 
        else{
          res = nbobj.cmdRetVal;                       
          nbobj.cmdRetVal = NB_OK;
        }
        s_uCmdBlockState = 0; 
      }
    }
  }
  NB_RETURN(res);////????
}

/**
 * @brief  set cmd and wait ultil finish.
 * @param[in]  cmd: command
 * @param[in]  u8BlockEn: Enables (1) or disables (0) Command execution function blocking.
 * @param[in]  u32Timeout: timeout value
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbCmdRun(NB_Cmd_t cmd, uint8_t u8BlockEn, uint32_t u32Timeout) {
    NB_Error_t res = NB_CMD_WAIT;  
    
    if(0 == s_uCmdRunState) {
      if(NB_IS_BUSY()) {
        res = NB_BUSY;
      }
      else {
        NB_SET_CMD(cmd);
        s_uCmdRunState = 1;
      }
    }
    else {
      res = nbCmdBlock(u8BlockEn, u32Timeout);
      if(NB_CMD_WAIT != res){
        s_uCmdRunState = 0; 
      }
    }
    
    NB_RETURN(res);
}

/**
 * @brief  Clear cmd run state and block state varibles.
 */
void nbCmdRunStateClr(void) {
  s_uCmdRunState = 0; 
  s_uCmdBlockState = 0;
  s_uCmdPrepare     = 0;//0:prepare work;
}


/**
 * @}
 */

