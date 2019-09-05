/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_parser.c
  * @author   ZouZH
  * @version  V1.00
  * @date     02-Aug-2018
  * @brief    Parse incoming data from AT port.
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
 * @defgroup NB-Parser
 * @brief NB-IoT AT回复指令解析
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
 * @brief      Parse number from string
 * @note       Input string pointer is changed and number is skipped
 * @param[in]  str: Pointer to pointer to string to parse
 * @return     Parsed number
 */
int32_t nb_parse_number(const char** str) {
    int32_t val = 0;
    uint8_t minus = 0;
    const char *p = *str;

    ASSERT(p != NULL);
  
    if (*p == ' ') {                            /* Skip leading space */
        p++;
    }
    if (*p == '"') {                            /* Skip leading quotes */
        p++;
    }
    if (*p == ',') {                            /* Skip leading comma */
        p++;
    }
    if (*p == '"') {                            /* Skip leading quotes */
        p++;
    }
    if (*p == '/') {                            /* Skip '/' character, used in datetime */
        p++;
    }
    if (*p == ':') {                            /* Skip ':' character, used in datetime */
        p++;
    }
    if (*p == '+') {                            /* Skip '+' character, used in datetime */
        p++;
    }
    if (*p == '-') {                            /* Check negative number */
        minus = 1;
        p++;
    }
    while (NB_CHARISNUM(*p)) {                  /* Parse until character is valid number */
        val = val * 10 + NB_CHARTONUM(*p);
        p++;
    }
    if (*p == ',') {                            /* Go to next entry if possible */
        p++;
    }
    *str = p;                                   /* Save new pointer with new offset */

    return minus ? -val : val;
}

/**
 * @brief      Parse number from string as hex
 * @note       Input string pointer is changed and number is skipped
 * @param[in]  str: Pointer to pointer to string to parse
 * @return     Parsed number
 */
uint32_t nb_parse_hexnumber(const char** str) {
    int32_t val = 0;
    const char *p = *str;

    ASSERT(p != NULL);

    if (*p == '"') {                            /* Skip leading quotes */
        p++;
    }
    if (*p == ',') {                            /* Skip leading comma */
        p++;
    }
    if (*p == '"') {                            /* Skip leading quotes */
        p++;
    }
    while (NB_CHARISHEXNUM(*p)) {               /* Parse until character is valid number */
        val = val * 16 + NB_CHARHEXTONUM(*p);
        p++;
    }
    if (*p == ',') {                            /* Go to next entry if possible */
        p++;
    }
    *str = p;                                   /* Save new pointer with new offset */
    return val;
}

/**
 * @brief           Parse input string as string part of AT command
 * @param[in,out]   src: Pointer to pointer to string to parse from
 * @param[in]       dst: Destination pointer. Use NULL in case you want only skip string in source
 * @param[in]       dst_len: Length of distance buffer, including memory for NULL termination
 * @param[in]       trim: Set to 1 to process entire string, even if no memory anymore
 * @return          1 on success, 0 otherwise
 */
uint8_t nb_parse_string(const char **src, char *dst, size_t dst_len, uint8_t trim) {
    const char *p = *src;
    size_t i;

    ASSERT(p != NULL);

    if (*p == ',') {
        p++;
    }
    if (*p == '"') {
        p++;
    }
    i = 0;
    if (dst_len) {
        dst_len--;
    }
    while (*p) {
        if (*p == '"' && (p[1] == ',' || p[1] == '\r' || p[1] == '\n')) {
            p++;
            break;
        }
        if (dst != NULL) {
            if (i < dst_len) {
                *dst++ = *p;
                i++;
            } else if (!trim) {
                break;
            }
        }
        p++;
    }
    if (dst != NULL) {
        *dst = 0;
    }
    *src = p;
    return 1;
}

/**
 * @brief      Check current string position and trim to the next entry
 * @param[in]  src: Pointer to pointer to input string
 * @return     None
 */
void nb_check_and_trim(const char **src) {
    const char *t = *src;

    ASSERT(t != NULL);

    if (*t != '"' && *t != '\r' && *t != ',') { /* Check if trim required */
        nb_parse_string(src, NULL, 0, 1);     /* Trim to the end */
    }
}

/**
 * @brief      Parse received +CME ERROR value
 * @param[in]  str: Input string
 * @retval     1 on success, 0 otherwise
 */
uint8_t nb_parse_cme(const char *str) {
    ASSERT(str != NULL);

    nbobj.cme = nb_parse_number(&str);

    return 1;
}



/**
 * @brief      Parse received +CSQ signal value
 * @param[in]  str: Input string
 * @retval     1 on success, 0 otherwise
 */
uint8_t nb_parse_csq(const char *str) {
    ASSERT(str != NULL);

    nbobj.rssi = nb_parse_number(&str);

    return 1;
}


/**
 * @brief      Parse received +CFUN phone function value
 * @param[in]  str: Input string
 * @retval     1 on success, 0 otherwise
 */
uint8_t nb_parse_cfun(const char *str) {
    ASSERT(str != NULL);

    nbobj.cfun = nb_parse_number(&str);

    return 1;
}


/**
 * @brief      Parse received +CREG message
 * @param[in]  str: Input string
 * @retval     1 on success, 0 otherwise
 */
uint8_t nb_parse_creg(const char* str) {
    ASSERT(str != NULL);
    
    str += 2;
    nbobj.netStat = (NB_NetStatus_t)nb_parse_number(&str);

    return 1;
}


/**
 * @brief      Parse string as IP address
 * @param[in]  str: Pointer to pointer to string to parse from
 * @retval     1 on success, 0 otherwise
 */
uint8_t nb_parse_ip(const char* str) {

    uint8_t ip0 = 0;
    
    ASSERT(str != NULL);

    str += 2;

    ip0 = nb_parse_number(&str); str++;

    if (ip0 && !nbobj.ipAddr[0]) {
        nbobj.Flags.F.callConnected = 1;

        nbobj.ipAddr[0] = ip0;
        nbobj.ipAddr[1] = nb_parse_number(&str); str++;
        nbobj.ipAddr[2] = nb_parse_number(&str); str++;
        nbobj.ipAddr[3] = nb_parse_number(&str);
        return 1;
    }
    
    return 0;
}

/**
 * @brief      Parse received +CSCON message
 * @param[in]  str: Input string
 * @retval     1 on success, 0 otherwise
 */
uint8_t nb_parse_cscon(const char* str) {
    ASSERT(str != NULL);

    str += 2;
    nbobj.connStat = nb_parse_number(&str);

    return 1;
}


/**
 * @brief      Parse datetime in format dd/mm/yy,hh:mm:ss
 * @param[in]  str: Pointer to pointer to input string
 * @retval     1 on success, 0 otherwise
 */
uint8_t nb_parse_datetime(const char* str) {

    ASSERT(str != NULL);
    
    nbobj.datetime.year = nb_parse_number(&str);
    nbobj.datetime.month = nb_parse_number(&str);
    nbobj.datetime.day = nb_parse_number(&str);
    nbobj.datetime.hour = nb_parse_number(&str);
    nbobj.datetime.minute = nb_parse_number(&str);
    nbobj.datetime.second = nb_parse_number(&str);
    nbobj.datetime.zone = nb_parse_number(&str);

    return 1;
}


/**
 * @brief      Parse string as receive data
 * @param[in]  str: Pointer to pointer to string to parse from
 * @retval     1 on success, 0 otherwise
 */
uint8_t nb_parse_recvdata(const char* str) {

    uint16_t recvlen = 0;
    
    ASSERT(str != NULL);

    recvlen = nb_parse_number(&str);

#if NB_CFG_USE_PROFILE
    if (recvlen < NB_CFG_LEN_PROFILE) {
        return 0;
    }

    if (*str != '0' || *(str + 1) != '1') {
        return 0;
    }

    recvlen -= NB_CFG_LEN_PROFILE;
    str += 6;
    
#endif /* NB_CFG_USE_PROFILE */

    if ((NULL == nbobj.cmdParam.read.br) || (NULL == nbobj.cmdParam.read.data) || !nbobj.cmdParam.read.btr) {
        return 0;
    }

    *nbobj.cmdParam.read.br = 0;

    while (NB_CHARISHEXNUM(*str) && NB_CHARISHEXNUM(*(str + 1)) && (*nbobj.cmdParam.read.br < recvlen)) {
        if (*nbobj.cmdParam.read.br >= nbobj.cmdParam.read.btr) {
            break;
        }
        *nbobj.cmdParam.read.data = UINT8_BUILD(NB_CHARHEXTONUM(*str), NB_CHARHEXTONUM(*(str + 1)));
        nbobj.cmdParam.read.data += 1;
        *nbobj.cmdParam.read.br += 1;
        str += 2;
    }

    nbobj.cbParam.UN.recv.recvlen = *nbobj.cmdParam.read.br;
    
    return 1;
}



/**
 * @brief      Process received string from module
 * @param[in]  prcv: Pointer to @ref NB_Recv_t structure with input string
 * @return     None
 */
void nb_parse_received(NB_RecvAT_t* prcv) {

    uint8_t is_ok = 0;
    uint8_t is_error = 0;

    if (NULL == prcv) {
        return;
    }

    /* Try to remove non-parsable strings */
    if (prcv->len == 2 && prcv->data[0] == '\r' && prcv->data[1] == '\n') {
        return;
    }

    /* Check OK response */
    is_ok = (prcv->len == 4 && !strcmp(prcv->data, NB_ATOK));

    /* Check error response */
	  if (!is_ok) {
        is_error = (prcv->data[0] == '+' && !strncmp(prcv->data, "+CME ERROR:", 11));
        if (!is_error) {
            is_error = (prcv->data[0] == '+' && !strncmp(prcv->data, "+CMS ERROR:", 11));
        } else {
            nb_parse_cme(&prcv->data[11]);
        }
    }

	  /* Scan received strings which start with '+' */
    if (prcv->data[0] == '+'  && !is_error) {
        if (!strncmp(prcv->data, "+CSQ:", 5)) {
            nb_parse_csq(&prcv->data[5]);
        } else if (!strncmp(prcv->data, "+CEREG:", 7)) {
            nb_parse_creg(&prcv->data[7]);
        } else if (!strncmp(prcv->data, "+CGPADDR:", 9) && (prcv->len >= 21)) {
            nb_parse_ip(&prcv->data[9]);
        } else if (!strncmp(prcv->data, "+CSCON:", 7)) {
            nb_parse_cscon(&prcv->data[7]);
        } else if (!strncmp(prcv->data, "+QLWULDATASTATUS:", 17) && (prcv->len >= 18)) {
            if (prcv->data[17] == '4') {
                nbobj.Evts.F.rspSendOk = 1;
            } else {
                nbobj.Evts.F.rspSendFail = 1;
            }
        } else if (!strncmp(prcv->data, "+CGSN:", 6)) {
            if (NB_CHARISNUM(prcv->data[6]) && (prcv->len >= 21)) {
                memcpy(nbobj.imei, &prcv->data[6], 15);
                nbobj.imei[15] = '\0';
            }
        } else if (!strncmp(prcv->data, "+CFUN:", 6)) {
            nb_parse_cfun(&prcv->data[6]);
        } else if (!strncmp(prcv->data, "+CCLK:", 6) && (prcv->len >= 26)) {
            nb_parse_datetime(&prcv->data[6]);
        }
    }

    /*
     * Check messages which do not start with '+' sign
     */
    if (prcv->data[0] != '+') {
        if (!is_ok && !is_error && strncmp(prcv->data, "AT+", 3)) {
            const char* pdat = prcv->data;
            if (NB_CMD_IS_CUR(NB_CMD_CIMI)) {
                nb_parse_string(&pdat, nbobj.imsi, sizeof(nbobj.imsi), 1);
            } else if (NB_CMD_IS_CUR(NB_CMD_DATA_READ) && NB_CHARISNUM(pdat[0])) {
                nb_parse_recvdata(pdat);
            }
        }
    }

    if (is_ok) {
        nbobj.Evts.F.rspOk = 1;
        nbobj.Evts.F.rspError = 0;
        nbobj.Flags.F.lastOper = 1;
    } else if (is_error) {
        nbobj.Evts.F.rspOk = 0;
        nbobj.Evts.F.rspError = 1;
        nbobj.Flags.F.lastOper = 0;
    }
}

/**
 * @}
 */

/**
 * @}
 */

