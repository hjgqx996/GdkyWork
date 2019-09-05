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
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @defgroup NB-Threads
 * @brief NB-IoT AT指令PT事件处理线程
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

/**
 * @brief      Create string from number
 * @param[out] Pointer to string to save result to
 * @param[in]  Number to convert to string
 */
void nb_number2str(char* str, uint32_t num) {
    if (NULL != str) {
        sprintf(str, "%u", num);
    }
}


PT_THREAD(PT_Thread_GEN(struct pt* pt)) {
    char str[6];
    PT_BEGIN(pt);

    if (nbobj.cmd == NB_CMD_AT) {
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_AT, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_ATE0) {
        NB_RST_EVT_RESP();
        NB_SEND_STR(("ATE0"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_ATE0, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_CGMR) {
            NB_RST_EVT_RESP();
            NB_SEND_STR(("AT+CGMR"));
            NB_SEND_STR(NB_ATCRLF);
            NB_START_CMD(NB_CMD_CGMR, NULL);
            PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);
    
            nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
            NB_SET_IDLE();

    } else if (nbobj.cmd == NB_CMD_CMEE) {
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CMEE=1"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_CMEE, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_CFUN_SET) {
        NB_RST_EVT_RESP();
        nb_number2str(str, nbobj.cmdParam.func.cfun);
        NB_SEND_STR(("AT+CFUN="));
        NB_SEND_STR((str));
        NB_SEND_STR((NB_ATCRLF));
        NB_START_CMD(NB_CMD_CFUN, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;

        if (nbobj.cmdRetVal == NB_OK) {
            nbobj.cfun = nbobj.cmdParam.func.cfun;
        }

        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_CFUN_GET) {
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CFUN?"));
        NB_SEND_STR((NB_ATCRLF));
        NB_START_CMD(NB_CMD_CFUN, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;

        NB_SET_IDLE();
    }
    PT_END(pt);
}


PT_THREAD(PT_Thread_INFO(struct pt* pt)) {
    PT_BEGIN(pt);

    if (nbobj.cmd == NB_CMD_CIMI) {
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CIMI"));
        NB_SEND_STR((NB_ATCRLF));
        NB_START_CMD(NB_CMD_CIMI, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;

        NB_SET_IDLE();
    }  else if (nbobj.cmd == NB_CMD_CGSN) {
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CGSN=1"));
        NB_SEND_STR((NB_ATCRLF));
        NB_START_CMD(NB_CMD_CGSN, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_CSQ) {
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CSQ"));
        NB_SEND_STR((NB_ATCRLF));
        NB_START_CMD(NB_CMD_CSQ, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        NB_SET_IDLE();
    }
    PT_END(pt);
}


PT_THREAD(PT_Thread_DATETIME(struct pt* pt)) {
    PT_BEGIN(pt);

    if (nbobj.cmd == NB_CMD_DATETIME) {
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CCLK?"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_DATETIME, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        NB_SET_IDLE();
    }

    PT_END(pt);
}


PT_THREAD(PT_Thread_NETWORK(struct pt* pt)) {
    char str[8];
    static uint16_t start, btw;
    static uint8_t tries;
    static uint8_t hdrlen;

    PT_BEGIN(pt);

    if (nbobj.cmd == NB_CMD_CREG) {
        NB_CMD_SAVE();

        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CEREG?"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_CREG, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if (nbobj.cmdRetVal != NB_OK) {
            nbobj.netStat = NB_NET_NotReg;
        }

        NB_CMD_RESTORE();
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_IP_GET) {
        NB_CMD_SAVE();

        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CGPADDR=0"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_IP_GET, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if (nbobj.cmdRetVal != NB_OK) {
            memset(nbobj.ipAddr, 0x00, sizeof(nbobj.ipAddr));
        }

        NB_CMD_RESTORE();
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_CSCON) {
        NB_CMD_SAVE();

        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CSCON?"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_CSCON, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;

        NB_CMD_RESTORE();
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_SETAPN) {
        NB_CMD_SAVE();

        // Define socket PDP context
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CGDCONT=1,\"IP\",\""));
        NB_SEND_STR((nbobj.cmdParam.pdp.apn));
        NB_SEND_STR(("\""));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_SETAPN, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if (nbobj.cmdRetVal != NB_OK) {
            goto cmd_gprs_setapn_clean; 
        }

cmd_gprs_setapn_clean:
        NB_CMD_RESTORE();
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_SETCONN) {
        NB_CMD_SAVE();

        /**** Set remote IP and Port ****/
        nb_number2str(str, nbobj.cmdParam.host.port);
        
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+NCDP="));
        /* Domain/IP */
        NB_SEND_STR((nbobj.cmdParam.host.ip));
        NB_SEND_STR((","));
        /* Port number */
        NB_SEND_STR((str));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_SETCONN, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);
        
        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;

        NB_CMD_RESTORE();
        NB_SET_IDLE();

    } else if (nbobj.cmd == NB_CMD_ATTACH) {
        NB_CMD_SAVE();

        nbobj.cfun = 0;
        nbobj.netStat = NB_NET_NotReg;
        memset(nbobj.ipAddr, 0x00, sizeof(nbobj.ipAddr));

        /**** Set Netowrk Config(Auto connect) ****/
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+NCONFIG=AUTOCONNECT,TRUE"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_ATTACH, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if (nbobj.cmdRetVal != NB_OK) {
            goto cmd_gprs_attach_clean;
        }

        /**** Set Netowrk Config(Cell reselection) ****/
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+NCONFIG=CELL_RESELECTION,TRUE"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_ATTACH, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if (nbobj.cmdRetVal != NB_OK) {
            goto cmd_gprs_attach_clean;
        }

        /**** Enable Phone Functionality ****/
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CFUN=1"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_ATTACH, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if (nbobj.cmdRetVal != NB_OK) {
            goto cmd_gprs_attach_clean;
        }

        nbobj.cfun = 1;

        /**** Attach Network ****/
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CGATT=1"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_ATTACH, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if (nbobj.cmdRetVal != NB_OK) {
            goto cmd_gprs_attach_clean;
        }

        /**** Set New Message Indications: 0=No indications ****/
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+NNMI=0"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_ATTACH, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if (nbobj.cmdRetVal != NB_OK) {
            goto cmd_gprs_attach_clean;
        }

cmd_gprs_attach_clean:
        if (nbobj.cmdRetVal == NB_OK) {
            nbobj.Flags.F.callAttached = 1;
        } else {
            nbobj.Flags.F.callAttachError = 1;
        }
        NB_CMD_RESTORE();
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_DETACH) {
        NB_CMD_SAVE();

        /**** Detach Network ****/
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CGATT=0"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_DETACH, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        /**** Disable Phone Functionality ****/
        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+CFUN=0"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_DETACH, NULL);
        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;

        nbobj.cfun = 0;
        nbobj.netStat = NB_NET_NotReg;
        memset(nbobj.ipAddr, 0x00, sizeof(nbobj.ipAddr));

        nbobj.Flags.F.callDetached = 1;
        NB_CMD_RESTORE();
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_DATA_SEND) {
        NB_CMD_SAVE();
        
        tries = NB_CFG_SEND_RETRIES;
        nbobj.cbParam.UN.send.sentlen = nbobj.cmdParam.send.btw;
        
        do {
        #if NB_CFG_USE_PROFILE
            hdrlen = 3;
        #else
            hdrlen = 0;
        #endif /* NB_CFG_USE_PROFILE */
        
            btw = nbobj.cmdParam.send.btw + hdrlen > \
            NB_CFG_PKG_SIZE ? NB_CFG_PKG_SIZE : nbobj.cmdParam.send.btw + hdrlen;
        
            nb_number2str(str, btw);
            
            NB_RST_EVT_RESP();
            NB_SEND_STR(("AT+QLWULDATAEX="));
            NB_SEND_STR(str);
            NB_SEND_STR((","));

            /* Send Profile Header */
        #if NB_CFG_USE_PROFILE
            sprintf(str, "00%04X", btw - hdrlen);
            NB_SEND_STR(str);
        #endif /* NB_CFG_USE_PROFILE */
        
            /* Send data */
            for (start = 0; start < btw - hdrlen; start++) {
                sprintf(str, "%02X", *(nbobj.cmdParam.send.data + start));
                NB_SEND(str, 2);
            }
            NB_SEND_STR((","));

            if (nbobj.cmdParam.send.mode == NB_SEND_MODE_NON) {
                NB_SEND_STR(("0x0000"));
            } else if (nbobj.cmdParam.send.mode == NB_SEND_MODE_CON) {
                NB_SEND_STR(("0x0100"));
            } else if (nbobj.cmdParam.send.mode == NB_SEND_MODE_NON_R) {
                NB_SEND_STR(("0x0001"));
            } else {
                NB_SEND_STR(("0x0101"));
            }

            NB_SEND_STR(NB_ATCRLF);
            NB_START_CMD(NB_CMD_DATA_SEND, NULL);

            if (nbobj.cmdParam.send.mode == NB_SEND_MODE_NON || \
                nbobj.cmdParam.send.mode == NB_SEND_MODE_NON_R) {
                PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);
                nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
            } else {
                PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspSendOk || nbobj.Evts.F.rspSendFail || nbobj.Evts.F.rspError);
                nbobj.cmdRetVal = nbobj.Evts.F.rspSendOk ? NB_OK : NB_ERR;
            }
            
            if (nbobj.cmdRetVal == NB_OK) {
            
                /* Increase number of sent bytes */
                if (nbobj.cmdParam.send.bw != NULL) {
                    *nbobj.cmdParam.send.bw += (btw - hdrlen);
                }
                
                /* Decrease number of sent bytes */
                nbobj.cmdParam.send.btw -= (btw - hdrlen);
                nbobj.cmdParam.send.data += (btw - hdrlen);

                tries = NB_CFG_SEND_RETRIES;
            } else {
                nbDelay(3000);
                tries--;
            }
        } while (nbobj.cmdParam.send.btw && tries);

        if (!nbobj.cmdParam.send.btw) {
            nbobj.Flags.F.callSent = 1;
        } else {
            nbobj.cbParam.UN.send.sentlen -= nbobj.cmdParam.send.btw;
            nbobj.Flags.F.callSendError = 1;
        }

        NB_CMD_RESTORE();
        NB_SET_IDLE();
    } else if (nbobj.cmd == NB_CMD_DATA_READ) {
        NB_CMD_SAVE();

        NB_RST_EVT_RESP();
        NB_SEND_STR(("AT+NMGR"));
        NB_SEND_STR(NB_ATCRLF);
        NB_START_CMD(NB_CMD_DATA_READ, NULL);

        PT_WAIT_UNTIL(pt, nbobj.Evts.F.rspOk || nbobj.Evts.F.rspError);

        nbobj.cmdRetVal = nbobj.Evts.F.rspOk ? NB_OK : NB_ERR;
        if ((nbobj.cmdRetVal == NB_OK) && *nbobj.cmdParam.read.br) {
            nbobj.Flags.F.callReceived = 1;
        }
        NB_CMD_RESTORE();
        NB_SET_IDLE();
    }

    PT_END(pt);
}

NB_Error_t nbProcessCallbacks(void) {

    /* Process callbacks */
    if ((nbobj.cmd == NB_CMD_IDLE) && nbobj.Flags.F.callIdle) {
        nbobj.Flags.F.callIdle = 0;
        nbobj.cbParam.type = NB_EVT_IDLE;
        nbobj.cbFunc(&nbobj.cbParam);
    }

    if (NB_IS_READY() && nbobj.Flags.F.callAttached) {
        nbobj.Flags.F.callAttached = 0;
        nbobj.cbParam.type = NB_EVT_Attach;
        nbobj.cbFunc(&nbobj.cbParam);
    }
    
    if (NB_IS_READY() && nbobj.Flags.F.callAttachError) {
        nbobj.Flags.F.callAttachError = 0;
        nbobj.cbParam.type = NB_EVT_AttachError;
        nbobj.cbFunc(&nbobj.cbParam);
    }
    
    if (NB_IS_READY() && nbobj.Flags.F.callDetached) {
        nbobj.Flags.F.callDetached = 0;
        nbobj.cbParam.type = NB_EVT_Detached;
        nbobj.cbFunc(&nbobj.cbParam);
    }

    if (NB_IS_READY() && nbobj.Flags.F.callConnected) {
        nbobj.Flags.F.callConnected = 0;
        nbobj.cbParam.type = NB_EVT_Connected;
        nbobj.cbFunc(&nbobj.cbParam);
    }

    if (NB_IS_READY() && nbobj.Flags.F.callReceived) {
        nbobj.Flags.F.callReceived = 0;
        nbobj.cbParam.type = NB_EVT_Received;
        nbobj.cbFunc(&nbobj.cbParam);
    }

    if (NB_IS_READY() && nbobj.Flags.F.callSent) {
        nbobj.Flags.F.callSent = 0;
        nbobj.cbParam.type = NB_EVT_Sended;
        nbobj.cbFunc(&nbobj.cbParam);
    }

    if (NB_IS_READY() && nbobj.Flags.F.callSendError) {
        nbobj.Flags.F.callSendError = 0;
        nbobj.cbParam.type = NB_EVT_SendError;
        nbobj.cbFunc(&nbobj.cbParam);
    }

    NB_RETURN(NB_OK);
}


NB_Error_t nbProcessThreads(void) {
    if (nbobj.cmd > NB_CMD_GEN_START && nbobj.cmd < NB_CMD_GEN_END) {
        PT_Thread_GEN(&nbobj.ptGen);
    }
    if (nbobj.cmd > NB_CMD_INFO_START && nbobj.cmd < NB_CMD_INFO_END) {
        PT_Thread_INFO(&nbobj.ptInfo);
    }
    
    if (nbobj.cmd > NB_CMD_DATE_START && nbobj.cmd < NB_CMD_DATE_END) {
        PT_Thread_DATETIME(&nbobj.ptDate);
    }
    if (nbobj.cmd > NB_CMD_NETWORK_START && nbobj.cmd < NB_CMD_NETWORK_END) {
        PT_Thread_NETWORK(&nbobj.ptNet);
    }

#if !NB_CFG_OS && !NB_CFG_ASYNC
    nbProcessCallbacks();
#endif /* !NB_CFG_OS && !NB_CFG_ASYNC */

    NB_RETURN(NB_OK);
}

/**
 * @}
 */

/**
 * @}
 */

