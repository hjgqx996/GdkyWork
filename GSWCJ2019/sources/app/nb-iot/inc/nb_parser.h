/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_parser.h
  * @author   ZouZH
  * @version  V1.00
  * @date     02-Aug-2018
  * @brief    Parse incoming data from AT port.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NB_PARSER_H
#define __NB_PARSER_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @addtogroup NB-Parser
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */
int32_t  nb_parse_number(const char** str);
uint32_t nb_parse_hexnumber(const char** str);
uint8_t  nb_parse_string(const char **src, char *dst, size_t dst_len, uint8_t trim);


uint8_t  nb_parse_cme(const char *str);
uint8_t  nb_parse_csq(const char *str);
uint8_t  nb_parse_cfun(const char *str);
uint8_t  nb_parse_creg(const char* str);
uint8_t  nb_parse_ip(const char* str);
uint8_t  nb_parse_cscon(const char* str);

uint8_t  nb_parse_datetime(const char* str);

uint8_t  nb_parse_recvdata(const char* str);



/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __NB_CONFIG_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

