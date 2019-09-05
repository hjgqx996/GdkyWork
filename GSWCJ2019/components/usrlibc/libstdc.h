/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY  All Rights Reserved
  *
  * @file     libstdc.h
  * @author   ZouZH
  * @version  V1.01
  * @date     03-Feb-2016
  * @brief    self-defining standard c library function .
  ******************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __LIBSTDC_H
#define __LIBSTDC_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include "stdint.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

void  *lib_memset(void *dst, uint8_t val, uint16_t count);

void  *lib_memcpy(void *dst, const void *src, uint16_t count);
void  *lib_memcpy_invert(void *dst, const void *src, uint16_t count);

const void *lib_memchr(const void *buf, uint8_t chr, uint16_t cnt);

int8_t lib_memcmp(const void * buf1, const void * buf2, uint16_t count);
int8_t lib_memcmp_invert(const void *buf1, const void *buf2, uint16_t count);
int8_t lib_memicmp(const void *first, const void *last, uint16_t count);
int8_t lib_memvalcmp(const void *buf, uint16_t count, uint8_t val);

char * lib_strncpy(char *dest, const char *source, uint32_t count);
char * lib_strncpybrk(char *dest, const char *source, uint32_t count, char c);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __LIBSTDC_H */

/***************************** END OF FILE ************************************/

