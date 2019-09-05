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

/** 
 * @defgroup NB-IoT
 * @brief NB-IoT物联网模块驱动
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */
NB_t nbobj;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

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
 * @brief  Module AT command init sequence
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbInitCmd(void) {
    uint32_t i = 0;
    
    /* Send initialization commands */
    nbobj.Flags.F.isBlocking = 1;
    nbobj.cmdTmout = 1000;
    nbobj.cmdRetVal = NB_TIMEOUT;
    memset((void *)&nbobj.cmdParam, 0x00, sizeof(NB_CmdParam_t));

    i = 10;
    do {
        NB_SET_CMD(NB_CMD_AT);
        nbWaitIdle(1000);
    } while (--i && (nbobj.cmdRetVal != NB_OK));

    if (nbobj.cmdRetVal != NB_OK) {
        NB_RETURN(nbobj.cmdRetVal);
    }

    i = 2;
    do {
        /* Disable ECHO */
        NB_SET_CMD(NB_CMD_ATE0);
        nbWaitIdle(1000);
    } while (--i && (nbobj.cmdRetVal != NB_OK));

    i = 2;
    do {
        /* GET CGMR Version */
        NB_SET_CMD(NB_CMD_CGMR);
        nbWaitIdle(1000);
    } while (--i && (nbobj.cmdRetVal != NB_OK));

    i = 2;
    do {
        /* Enable numeric error codes */
        NB_SET_CMD(NB_CMD_CMEE);
        nbWaitIdle(1000);
    } while (--i && (nbobj.cmdRetVal != NB_OK));

    
    i = 2;
    do {
        /* Get IMSI for SIM Card status */
        NB_SET_CMD(NB_CMD_CIMI);
        nbWaitIdle(1000);
    } while (--i && (nbobj.cmdRetVal != NB_OK));

    i = 2;
    do {
        /* Get IMEI */
        NB_SET_CMD(NB_CMD_CGSN);
        nbWaitIdle(1000);
    } while (--i && (nbobj.cmdRetVal != NB_OK));

    nbobj.Flags.F.isBlocking = 0;
    NB_SET_IDLE();
    nbobj.Flags.F.callIdle = 0;

    NB_RETURN(nbobj.cmdRetVal);
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
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_CIMI);
    NB_WAIT_BLOCK(NB_CFG_BLOCKING, 1000);

    if (str) {
      strncpy(str, nbobj.imsi, length);
    }

    return nbobj.retVal;
}

/**
 * @brief  General Command Get "AT+CSQ" 
 * @param[out] rssi: output RSSI
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbINFOGetRSSI(uint8_t* rssi) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_CSQ);
    NB_WAIT_BLOCK(NB_CFG_BLOCKING, 1000);

    if (rssi) {
      *rssi = nbobj.rssi;
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
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_CREG);

    NB_RETURN_BLOCK(NB_CFG_BLOCKING, 1000);
}

/**
 * @brief  Network Command Get module ip address 
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETGetIPAddr(void) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_IP_GET);

    NB_RETURN_BLOCK(NB_CFG_BLOCKING, 1000);
}

/**
 * @brief  Network Command Get "AT+CSCON" 
 * @param[out] stat: output CSCON state
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETGetCSCON(uint8_t *stat) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_CSCON);
    NB_WAIT_BLOCK(NB_CFG_BLOCKING, 1000);

    if (stat) {
      *stat = nbobj.connStat;
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
    NB_CHECK_PARAM(host);
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_SETCONN);

    nbobj.cmdParam.host.ip = host;
    nbobj.cmdParam.host.port = port;
    NB_RETURN_BLOCK(NB_CFG_BLOCKING, 3000);
}

/**
 * @brief  Network Command Run Attach GSM base station
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETAttach(void) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_ATTACH);

    NB_RETURN_BLOCK(NB_CFG_BLOCKING, 10000);
}

/**
 * @brief  Network Command Run Detach GSM base station
 * @param  void
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETDetach(void) {
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_DETACH);

    NB_RETURN_BLOCK(NB_CFG_BLOCKING, 10000);
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
    NB_CHECK_PARAM(data && btw);
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_DATA_SEND);

    nbobj.cmdParam.send.data = data;
    nbobj.cmdParam.send.btw = btw;
    nbobj.cmdParam.send.bw = bw;
    nbobj.cmdParam.send.mode = mode;

    if (bw) {
        *bw = 0;
    }

    NB_RETURN_BLOCK(NB_CFG_BLOCKING, NB_CFG_TIME_PSM * 1000);
}

/**
 * @brief  Network Command Read Data
 * @param[out] data: data buffer
 * @param[in]  btr:  bytes to read
 * @param[out] br:   bytes readed
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbNETRead(void* data, uint16_t btr, uint16_t* br) {
    NB_CHECK_PARAM(data && btr);
    NB_CHECK_BUSY();
    NB_SET_CMD(NB_CMD_DATA_READ);

    nbobj.cmdParam.read.data = data;
    nbobj.cmdParam.read.btr = btr;
    nbobj.cmdParam.read.br = br;
    if (br != NULL) {
        *br = 0;
    }

    NB_RETURN_BLOCK(NB_CFG_BLOCKING, 10000);
}


/**
 * @brief  NB dirver blocking delay some ms
 * @param[in]  timeout:  ms to delay
 * @retval NB_OK: No error, other @ref NB_Error_t
 */
NB_Error_t nbDelay(uint32_t timeout) {
    volatile uint32_t start = nbobj.timeMs;
    do {
      #if !NB_CFG_OS && !NB_CFG_ASYNC
        nbUpdate();
      #endif
    } while (nbobj.timeMs - start < timeout);
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
    if ((nbobj.cmd != NB_CMD_IDLE) && (nbobj.timeMs - nbobj.cmdTime) > nbobj.cmdTmout) {
        nbobj.Evts.F.rspTimeout = 1;
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
 * @}
 */ 

