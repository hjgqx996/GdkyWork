/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_private.h
  * @author   ZouZH
  * @version  V1.00
  * @date     01-Aug-2018
  * @brief    Private structures and enumerations for NB-IoT.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NB_PRIVATE_H
#define __NB_PRIVATE_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

#include "nb.h"
#include "nb_buffer.h"

#include "pt/pt.h"
   
/**
 * @addtogroup NB-IoT
 * @{
 */

/**
 * @defgroup NB-Private
 * @brief NB-IoT驱动私有对象类型和宏定义
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * @brief List of possible command
 */
typedef enum {
    NB_CMD_IDLE = 0,                  /*!< IDLE mode */

    /*******  General commands *******/
    NB_CMD_GEN_START,
    NB_CMD_AT,
    NB_CMD_ATE0,                      /*!< Echo mode off */
    NB_CMD_SET_BAUD,                  /*!< Configure UART Port Baud Rate */
    NB_CMD_CGMR,                      /*!< Request Manufacturer Revision */
    NB_CMD_CMEE,                      /*!< Result  code  and  use  numeric when set=1 */
    NB_CMD_CFUN,                      /*!< Phone Functionality */
    NB_CMD_CFUN_SET,                  /*!< Set Phone Functionality */
    NB_CMD_CFUN_GET,                  /*!< Get Phone Functionality */
    NB_CMD_NRB,                       /*!< Reboot device */
    NB_CMD_GEN_END,

    /*******  Infomation commands *******/
    NB_CMD_INFO_START,
    NB_CMD_CIMI,                      /*!< Return 15 Numbers <IMSI> */
    NB_CMD_CGSN,                      /*!< Return 15 Numbers <IMEI> */
    NB_CMD_CSQ,                       /*!< Get Signal Strength Indicator */
    NB_CMD_INFO_END,

    /*******  Datetime commands *******/
    NB_CMD_DATE_START,
    NB_CMD_DATETIME,                  /*!< Get Date and Time */
    NB_CMD_DATE_END,

    /*******  Network commands *******/
    NB_CMD_NETWORK_START,
    NB_CMD_NCONFIG,                   /*!< AT+NCONFIG Configure UE Behaviour  */
    NB_CMD_NNMI,                      /*!< New Message Indications: 0=No indications */
    NB_CMD_SETAPN,                    /*!< Set PDP context  */
    NB_CMD_SETCONN,                   /*!< Configure and Query CDP Server Settings */
    NB_CMD_CGATT,                     /*!< PS Attach or Detach */
    NB_CMD_CREG,                      /*!< Get Network Registration Status */
    NB_CMD_IP_GET,                    /*!< Get Device IP Address */
    NB_CMD_CSCON,                     /*!< Signalling Connection Status */

    NB_CMD_ATTACH,                    /*!< Attach network */
    NB_CMD_DETACH,                    /*!< Detach network */

    NB_CMD_DATA_SEND,                 /*!< Send one buffer data */
    NB_CMD_DATA_READ,                 /*!< Read one buffer data */
    NB_CMD_NETWORK_END,

    NB_CMD_END,                       /*!< Last CMD entry */
} NB_Cmd_t;


/**
 * @brief   Global structure to pass as parameter to exe function
 */
typedef union {

    struct {
        uint8_t cfun;
    }func;

    struct {
        const char *apn;
        const char *usr;
        const char *pwd;
    }pdp;

    struct {
        const char *ip;
        uint16_t port;
    }host;

    struct {
        const uint8_t *data;
        uint16_t btw;
        uint16_t *bw;
        NB_SendMode_t mode;
    }send;

    struct {
        uint8_t *data;
        uint16_t btr;
        uint16_t *br;
    }read;

} NB_CmdParam_t;


/**
 * @brief  Receive character structure to handle full line terminated with `\n` character
 */
typedef struct {
    char data[NB_CFG_AT_LINE_MAX];
    uint8_t len;
} NB_RecvAT_t;


/**
 * @brief NB-IoT run structure
 */
typedef struct {

    /*! Processing Threads */
    struct pt               ptGen;     /*!< General commands processing thread */
    struct pt               ptInfo;    /*!< Information commands processing thread */
    struct pt               ptDate;    /*!< Datetime commands processing thread */
    struct pt               ptNet;     /*!< Network commands processing thread */

    uint32_t                timeMs;    /*!< Current time in units of milliseconds */
    NB_Error_t              retVal;    /*!< Return value */

    /*! Hardware management */

    /*!< Active command informations */
    NB_CmdParam_t           cmdParam;  /*!< Command exe param */
    NB_Error_t              cmdRetVal; /*!< Result to return from function */
    NB_Cmd_t                cmd;       /*!< Current active Cmd */
    NB_Cmd_t                cmdSaved;  /*!< Saved active Cmd */
    const char*             cmdRspStr; /*!< Active command response buffer */
    uint32_t                cmdTime;   /*!< Command started time */
    uint32_t                cmdTmout;  /*!< Timeout in units of MS for active command to finish */

    char                    imsi[16];  /*!< Device IMSI number */
    char                    imei[16];  /*!< Device IMEI number */
    uint8_t                 rssi;      /*!< Signal strength */
    uint8_t                 cfun;      /*!< Phone Functionality */
    uint16_t                cme;       /*!< CME Error Code */
    NB_NetStatus_t          netStat;   /*!< Network status */
    NB_ConnType_t           connType;  /*!< Network connect type */
    uint8_t                 ipAddr[4]; /*!< Device IP address */
    uint8_t                 connStat;  /*!< 1 Connected, 0 Idle */


    NB_Datetime_t           datetime;  /*!< Network date time */

    nb_buff_t               buff;      /*!< Input data cache buffer manage */
    uint8_t                 bufDat[NB_CFG_BUFF_SIZE];

    NB_RecvAT_t             recv_at;   /*!< Line terminated with `\n` buffer */

    NB_CB_Param_t           cbParam;   /*!< Event callback parameters */
    NB_EventCallback_t      cbFunc;    /*!< Event callback function */

    /*!< Flags management */
    union {
        struct {
            uint8_t isRdy:1;           /*!< 1 Module is ready */
            uint8_t isBlocking:1;      /*!< 1 In blocking execution */

            uint8_t callIdle:1;        /*!< 1 We have to call IDLE callback */
            uint8_t callAttached:1;    /*!< 1 Attached sucessfully */
            uint8_t callAttachError:1; /*!< 1 Attached failed */
            uint8_t callDetached:1;    /*!< 1 Detached */
            uint8_t callConnected:1;   /*!< 1 Connected */
            uint8_t callReceived:1;    /*!< 1 Received data */
            uint8_t callSent:1;        /*!< 1 Send data success */
            uint8_t callSendError:1;   /*!< 1 Send data success */

            uint8_t lastOper:1;
        } F;
        uint16_t value;
    } Flags;

    /*!< Command respond flags */
    union {
        struct {
            uint8_t rspOk:1;
            uint8_t rspError:1;
            uint8_t rspTimeout:1;

            uint8_t rspSendOk:1;
            uint8_t rspSendFail:1;
        } F;
        uint8_t value;
    } Evts;
} NB_t;

/* MACROS  ------------------------------------------------------------------ */

#define NB_ATOK                        "OK\r\n"
#define NB_ATCRLF                      "\r\n"


#define NB_CHARISNUM(x)                ((x) >= '0' && (x) <= '9')
#define NB_CHARISHEXNUM(x)             (((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F'))

#define NB_CHARTONUM(x)                ((x) - '0')
#define NB_CHARHEXTONUM(x)             (((x) >= '0' && (x) <= '9') ? ((x) - '0') : (((x) >= 'a' && (x) <= 'f') ? ((x) - 'a' + 10) : (((x) >= 'A' && (x) <= 'F') ? ((x) - 'A' + 10) : 0)))
#define NB_ISVALIDASCII(x)             (((x) >= 32 && (x) <= 126) || (x) == '\r' || (x) == '\n')

#define NB_RECV_CHECK()                do { if (nbobj.recv_at.len >= NB_CFG_AT_LINE_MAX - 1) nbobj.recv_at.len = 0;} while (0)
#define NB_RECV_ADD(ch)                do { NB_RECV_CHECK(); nbobj.recv_at.data[nbobj.recv_at.len++] = ch; nbobj.recv_at.data[nbobj.recv_at.len] = 0; } while (0)
#define NB_RECV_RESET()                do { nbobj.recv_at.len = 0; nbobj.recv_at.data[0] = 0; } while (0)
#define NB_RECV_LEN()                  nbobj.recv_at.len
#define NB_RECV_IDX(index)             nbobj.recv_at.data[index]

#define NB_SEND_BEGIN()                do { NB_SEND_STR("AT"); } while (0)
#define NB_SEND_END()                  do { NB_SEND_STR(NB_ATCRLF); } while (0)

#define NB_SEND_STR(str)               nbHWSend((const uint8_t *)(str), (uint16_t)strlen(str))
#define NB_SEND_CHR(ch)                nbHWSend((const uint8_t *)(ch), (uint16_t)1)
#define NB_SEND(d, l)                  nbHWSend((const uint8_t *)(d), (uint16_t)(l))


#define NB_CMD_IS_CUR(c)               (nbobj.cmd == (c))
#define NB_CMD_GET_CUR(c)              nbobj.cmd

#define NB_CMD_SAVE()                  nbobj.cmdSaved = nbobj.cmd
#define NB_CMD_RESTORE()               nbobj.cmd = nbobj.cmdSaved

#define NB_IS_BUSY()                   (nbobj.cmd != NB_CMD_IDLE || nbobj.Flags.F.callIdle != 0)
#define NB_IS_READY()                  (!NB_IS_BUSY())

#define NB_CHECK_BUSY()                do { if (NB_IS_BUSY()) { NB_RETURN(NB_BUSY); } } while (0)
#define NB_CHECK_PARAM(c)              do { if (!(c)) { NB_RETURN(NB_ERRPAR); } } while (0)


#if NB_CFG_OS

#define NB_SET_IDLE()                  do {      \
    nbOsMutexRelease();                          \
    nbobj.cmd = NB_CMD_IDLE;                     \
    NB_RST_THREADS();                            \
    if (!nbobj.Flags.F.isBlocking) {             \
        nbobj.Flags.F.callIdle = 1;              \
    }                                            \
    memset((void *)&nbobj.cmdParam, 0x00, sizeof(NB_CmdParam_t));  \
} while (0)


#define NB_SET_CMD(cmdType)            do {      \
    if (nbOsMutexAcquire(NB_CFG_OS_WAIT_TIME)) { \
        return NB_TIMEOUT;                       \
    }                                            \
    if (nbobj.cmd == NB_CMD_IDLE) {              \
        nbobj.cmdTime = nbobj.timeMs;            \
    }                                            \
    nbobj.cmd = cmdType;                         \
} while (0)

#else

#define NB_SET_IDLE()                  do {      \
    nbobj.cmd = NB_CMD_IDLE;                     \
    NB_RST_THREADS();                            \
    if (!nbobj.Flags.F.isBlocking) {             \
        nbobj.Flags.F.callIdle = 1;              \
    }                                            \
    memset((void *)&nbobj.cmdParam, 0x00, sizeof(NB_CmdParam_t));  \
} while (0)

#define NB_SET_CMD(cmdType)                do {  \
    if (nbobj.cmd == NB_CMD_IDLE) {              \
        nbobj.cmdTime = SysTickGetMs();          \
    }                                            \
    nbobj.cmd = (cmdType);                       \
} while (0)

#endif /* NB_CFG_OS */


#define NB_START_CMD(cmdType, rspStr)  do {      \
    nbobj.cmd = cmdType;                         \
    nbobj.cmdRspStr = rspStr;                    \
    nbobj.cmdTime = SysTickGetMs();              \
    nbobj.cmdRetVal = NB_OK;                     \
} while (0)


#define NB_RETURN(val)                 do { nbobj.retVal = (val); return (val); } while (0)
#define NB_RETURN_BLOCK(b, tmout)      do {      \
    NB_Error_t res;                              \
    nbobj.cmdTmout = tmout;                      \
    if (!(b)) {                                  \
        nbobj.Flags.F.isBlocking = 0;            \
        NB_RETURN(NB_OK);                        \
    }                                            \
    nbobj.Flags.F.isBlocking = 1;                \
    res = nbWaitIdle(tmout);                     \
    if (res == NB_TIMEOUT) {                     \
        return NB_TIMEOUT;                       \
    }                                            \
    res = nbobj.cmdRetVal;                       \
    nbobj.cmdRetVal = NB_OK;                     \
    return res;                                  \
} while (0)

#define NB_WAIT_BLOCK(b, tmout)        do {      \
    NB_Error_t res;                              \
    nbobj.cmdTmout = tmout;                      \
    if (!(b)) {                                  \
        nbobj.Flags.F.isBlocking = 0;            \
        nbobj.retVal = NB_OK;                    \
        break;                                   \
    }                                            \
    nbobj.Flags.F.isBlocking = 1;                \
    res = nbWaitIdle(tmout);                     \
    if (res == NB_TIMEOUT) {                     \
        nbobj.retVal = NB_TIMEOUT;               \
        break;                                   \
    }                                            \
    nbobj.retVal = nbobj.cmdRetVal;              \
    nbobj.cmdRetVal = NB_OK;                     \
} while (0)

#define NB_RST_EVT_RESP()              do { nbobj.Evts.value = 0; } while (0)
#define NB_CALL_CB_FUNC()              nbobj.cbFunc(&(nbobj.cbParam))


#define NB_RST_THREADS()               do {      \
    PT_INIT(&(nbobj.ptGen));                     \
    PT_INIT(&(nbobj.ptInfo));                    \
    PT_INIT(&(nbobj.ptDate));                    \
    PT_INIT(&(nbobj.ptNet));                     \
} while (0)


/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern NB_t nbobj;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __NB_PRIVATE_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

