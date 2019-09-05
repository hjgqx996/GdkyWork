/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     libmisc.h
  * @author   ZouZH
  * @version  V1.08
  * @date     12-July-2018
  * @brief    .
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __LIBMISC_H
#define __LIBMISC_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

uint32_t lib_pow(uint8_t base, uint8_t times);
uint64_t lib_pow64(uint8_t base, uint8_t times);

uint8_t  isbcds(const void *pbuf, uint8_t len);
uint8_t  bcd2dec(uint8_t bcd);
uint8_t  dec2bcd(uint8_t dec);
uint32_t bcds2dec(const void *pbuf, uint8_t len);
uint64_t bcds2dec_ex(const void *pbuf, uint8_t len);
uint32_t bcds2dec_invert(const void *pbuf, uint8_t len);
uint64_t bcds2dec_ex_invert(const void *pbuf, uint8_t len);

void     dec2bcds(uint32_t dec, void *pbuf, uint8_t len);
void     dec2bcds_invert(uint32_t dec, void *pbuf, uint8_t len);
void     dec2bcds_ex(uint64_t dec, void *pbuf, uint8_t len);
void     dec2bcds_ex_invert(uint64_t dec, void *pbuf, uint8_t len);

int      lib_atob(uint32_t *vp, char *p, int base);
int32_t  lib_atoi(const char *p);
long     lib_atol(const char *p);
double   lib_atof(char *p);

uint16_t lib_findframe188(uint8_t **ppbuf, uint16_t len);
uint16_t lib_findframegdky(uint8_t **ppbuf, uint16_t len);
uint16_t lib_findframegdkyhex(uint8_t **ppbuf, uint16_t len);
uint16_t lib_findframejgsh(uint8_t **ppbuf, uint16_t len);

uint16_t lib_findframedlsd(uint8_t **ppbuf, uint16_t len);
uint8_t  lib_encrypt_frame_dlsd(uint8_t *pbuf, uint16_t len);
uint8_t  lib_decrypt_frame_dlsd(uint8_t *pbuf, uint16_t len);
/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __LIBMISC_H */

