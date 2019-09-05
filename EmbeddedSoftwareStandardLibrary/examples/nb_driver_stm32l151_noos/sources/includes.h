/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY  All Rights Reserved
  *
  * @file    includes.h
  * @author  ZouZH
  * @version V1.01
  * @date    12-July-2018
  * @brief   Projects include the header file.
  ******************************************************************************
  */
// Verision History
// 2017-11-13 V1.00 Create the header file

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __INCLUDES_H
#define __INCLUDES_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */

/* Standard includes. */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* Scheduler includes. */

/* Library includes. */
#include "libstdc.h"
#include "libmisc.h"
#include "libcheck.h"

//#include "stm32f10x.h"
#include "stm32l1xx.h"

/* Drives includes */
#include "bsp.h"
#include "systime.h"

/* Application includes */
#include "app.h"
#include "bsp.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/**
 * Commonly used macro function definitions
 */
#define SYSGetTickCount()            (0)
#define SYSGetTickCountISR()         (0)
#define SYS_MS_TO_TICKS(x)           (x) 

#define SYSGetTimeSec()              SysTimeGetSecond()

#define SYSDelayMs(ms)               

#define SYSMakeMs(min, sec, ms)      (uint32_t)(((min) * 60000UL) + ((sec) * 1000UL) + (ms))
#define SYSMakeSec(hour, min, sec)   (uint32_t)(((hour) * 3600UL) + ((min) * 60UL) + (sec))

#define SYSMalloc(size)              ((void *)0)    
#define SYSFree(ptr)                 

#define SYSEnterCritical()           
#define SYSExitCritical()            

#define SYSTaskSuspendAll()          
#define SYSTaskResumeAll()           

/**
 * Debug mode macro definition
 */
#include "elog.h"
#ifdef USE_FULL_ASSERT
  #define xprintf   elog_raw
  #define ASSERT(x) assert_param(x)
#else
  #define xprintf(...)
  #define ASSERT(x)
#endif /* USE_FULL_ASSERT */

#define DBG_Print(...) do {\
    xprintf("[%02X:%02X:%02X]", g_tSysTime.hour, g_tSysTime.minute, g_tSysTime.second);\
    xprintf(__VA_ARGS__);\
}while(0)

#ifndef BIT
#define BIT(n)              (1 << (n))
#define BIT_SET(REG, BIT)   ((REG) |= (BIT))
#define BIT_CLEAR(REG, BIT) ((REG) &= ~(BIT))
#define BIT_READ(REG, BIT)  ((REG) & (BIT))
#define REG_CLEAR(REG)      ((REG) = (0x0))
#define REG_WRITE(REG, VAL) ((REG) = (VAL))
#define REG_READ(REG)       ((REG))
#define REG_MODIFY(REG, CLEARMASK, SETMASK)  REG_WRITE((REG), (((REG_READ(REG)) & (~(CLEARMASK))) | (SETMASK)))
#endif

#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

/*
 *  This macro is for use by other macros to form a fully valid C statement.
 *  Without this, the if/else conditionals could show unexpected behavior.
 *
 *  For example, use...
 *    #define SET_REGS()  st( ioreg1 = 0; ioreg2 = 0; )
 *  instead of ...
 *    #define SET_REGS()  { ioreg1 = 0; ioreg2 = 0; }
 *  or
 *    #define  SET_REGS()    ioreg1 = 0; ioreg2 = 0;
 *  The last macro would not behave as expected in the if/else construct.
 *  The second to last macro will cause a compiler error in certain uses
 *  of if/else construct
 *
 *  It is not necessary, or recommended, to use this macro where there is
 *  already a valid C statement.  For example, the following is redundant...
 *    #define CALL_FUNC()   st(  func();  )
 *  This should simply be...
 *    #define CALL_FUNC()   func()
 *
 * (The while condition below evaluates false without generating a
 *  constant-controlling-loop type of warning on most compilers.)
 */
#define ST(x)      do { x } while (__LINE__ == -1)

#define UINT64_BUILD(Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7) \
          ((uint64_t)((uint64_t)((Byte0) & 0x00FF) \
          + ((uint64_t)((Byte1) & 0x00FF) << 8) \
          + ((uint64_t)((Byte2) & 0x00FF) << 16) \
          + ((uint64_t)((Byte3) & 0x00FF) << 24) \
          + ((uint64_t)((Byte4) & 0x00FF) << 32) \
          + ((uint64_t)((Byte5) & 0x00FF) << 40) \
          + ((uint64_t)((Byte6) & 0x00FF) << 48) \
          + ((uint64_t)((Byte7) & 0x00FF) << 56)))

/* takes a byte out of a uint32_t : var - uint32_t,  ByteNum - byte to take out (0 - 3) */
#define UINT32_BREAK( var, ByteNum ) \
          (uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define UINT32_BUILD(Byte0, Byte1, Byte2, Byte3) \
          ((uint32_t)((uint32_t)((Byte0) & 0x00FF) \
          + ((uint32_t)((Byte1) & 0x00FF) << 8) \
          + ((uint32_t)((Byte2) & 0x00FF) << 16) \
          + ((uint32_t)((Byte3) & 0x00FF) << 24)))

#define UINT16_BUILD(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define UINT16_HI(a) (uint8_t)(((a) >> 8) & 0xFF)
#define UINT16_LO(a) (uint8_t)((a) & 0xFF)

#define UINT8_BUILD(hiByte, loByte) \
          ((uint8_t)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define UINT8_HI(a) (((a) >> 4) & 0x0F)
#define UINT8_LO(a) ((a) & 0x0F)

/*High and low byte exchange, mainly for the Little and Big Endian conversion */
#define BSWAP_16(x)    ((uint16_t)((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8)))
#define BSWAP_32(x)    ((uint32_t)((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) |    \
                       (((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24)))
#define BSWAP_64(x)    ((uint64_t)((((x) & 0xff00000000000000ull) >> 56)                      \
                       | (((x) & 0x00ff000000000000ull) >> 40)                      \
                       | (((x) & 0x0000ff0000000000ull) >> 24)                      \
                       | (((x) & 0x000000ff00000000ull) >> 8)                      \
                       | (((x) & 0x00000000ff000000ull) << 8)                      \
                       | (((x) & 0x0000000000ff0000ull) << 24)                      \
                       | (((x) & 0x000000000000ff00ull) << 40)                      \
                       | (((x) & 0x00000000000000ffull) << 56)))



/* Get an unsigned 8bit(BYTE), 16bit(WORD), 32bit(DWORD) number from the specified address */
#define UINT8_GETFROMADDR(addr)      (*((uint8_t*)(addr)))
#define UINT16_GETFROMADDR(addr)     (*((uint16_t*)(addr)))
#define UINT32_GETFROMADDR(addr)     (*((uint32_t*)(addr)))

/* SET RESET GET  specified data bits an of array  */
#define ARRAY8_SETBIT(array, pos)    (array[(pos) >> 3] |= (1 << ((pos) & 0x07)))
#define ARRAY8_RESETBIT(array, pos)  (array[(pos) >> 3] &= (~(1 << ((pos) & 0x07))))
#define ARRAY8_GETBIT(array, pos)    ((array[(pos) >> 3] >> ((pos) & 0x07)) & 0x01)

#define ARRAY16_SETBIT(array, pos)   (array[(pos) >> 4] |= (1 << ((pos) & 0x0F)))
#define ARRAY16_RESETBIT(array, pos) (array[(pos) >> 4] &= (~(1 << ((pos) & 0x0F))))
#define ARRAY16_GETBIT(array, pos)   ((array[(pos) >> 4]>>((pos) & 0x0F)) & 0x01)

#define ARRAY32_SETBIT(array, pos)   (array[(pos) >> 5] |= (1 << ((pos) & 0x1F)))
#define ARRAY32_RESETBIT(array, pos) (array[(pos) >> 5] &= (~(1 << ((pos) & 0x1F))))
#define ARRAY32_GETBIT(array, pos)   ((array[(pos) >> 5]>>((pos) & 0x1F)) & 0x01)

/* To obtain an item in the structure of the offset and size */
#define ST_FO(st_type, field)    ((uint32_t)&(((st_type*)0)->field))
#define ST_FS(st_type, field)    (sizeof(((st_type*)0)->field))

/* upper and lower case conversion */
#define UPPERCASE(c) (((c)>='a'&&(c)<='z')? ((c)-0x20):(c))
#define LOWERCASE(c) (((c)>='A'&&(c)<='Z')? ((c)+0x20):(c))

/* To determine whether character decimal Numbers */
#define ISDEC(c) ((c)>='0'&&(c)<='9')

/* To determine whether character Hexadecimal numbers */
#define ISHEX(c) (((c)>='0'&&(c)<='9')||((c)>='A'&&(c)<='F')||((c)>='a'&&(c)<='f'))

/* Get the number of array elements */
#define ARR_ITEM_NUM(array)  (sizeof((array))/sizeof((array[0])))

/* Definitions for byte order, according to significance of bytes,
   from low addresses to high addresses.  The value is what you get by
   putting '4' in the most significant byte, '3' in the second most
   significant byte, '2' in the second least significant byte, and '1'
   in the least significant byte, and then writing down one digit for
   each byte, starting with the byte at the lowest address at the left,
   and proceeding to the byte with the highest address at the right. */

#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321
#define PDP_ENDIAN    3412

/* Microcontroller Byte Order */
#define SYS_ENDIAN    LITTLE_ENDIAN

/* Struct Pack */
#define PACK          __packed

/* Inline function macros */
#define _INLINE

/* Set MDK variable address */
#define VAR_AT(addr)  __attribute__((at(addr)))


/* CONSTANTS  --------------------------------------------------------------- */

/* BCD Code Max Value */
#define BCD_MAX_7B    ((uint64_t)99999999999999)
#define BCD_MAX_6B    ((uint64_t)999999999999)
#define BCD_MAX_5B    ((uint64_t)9999999999)
#define BCD_MAX_4B    ((uint32_t)99999999)
#define BCD_MAX_3B    ((uint32_t)999999)
#define BCD_MAX_2B    ((uint16_t)9999)
#define BCD_MAX_1B    ((uint8_t)99)

/**
 * Macros define symbols using small tricks
 *
 * ## is Connector
 * #@ Add single quotes to the parameters
 * #  Double quotation marks for parameters are converted to strings
 */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __INCLUDES_H */

