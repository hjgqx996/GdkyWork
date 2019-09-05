/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_buffer.h
  * @author   ZouZH
  * @version  V1.00
  * @date     01-Aug-2018
  * @brief    Ring buffer for NB-IoT.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NB_BUFFER_H
#define __NB_BUFFER_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>


/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @addtogroup NB-Buffer
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/**
 * @brief  Buffer structure
 */
typedef struct {
    uint16_t size;                     /*!< Size of buffer in units of bytes */
    uint16_t in;                       /*!< Input pointer to save next value */
    uint16_t out;                      /*!< Output pointer to read next value */
    uint8_t* buff;                     /*!< Pointer to buffer data array */
    uint8_t  flags;                    /*!< Flags for buffer */
} nb_buff_t;


/* MACROS  ------------------------------------------------------------------ */

#define NB_BUFF_FLAG_STATIC    0x01    /*!< Buffer uses static for memory */
#define NB_BUFF_FLAG_MALLOC    0x02    /*!< Buffer uses malloc for memory */


/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

uint8_t     nb_buff_init(nb_buff_t* buff, uint16_t size, void *buff_ptr);
void        nb_buff_free(nb_buff_t* buff);
uint16_t    nb_buff_write(nb_buff_t* buff, const void* data, uint16_t count);
uint16_t    nb_buff_read(nb_buff_t* buff, void* data, uint16_t count);
uint16_t    nb_buff_get_free(nb_buff_t* buff);
uint16_t    nb_buff_get_full(const nb_buff_t* buff);
void        nb_buff_reset(nb_buff_t* buff);
uint16_t    nb_buff_peek(nb_buff_t* buff, uint16_t skip_count, void* data, uint16_t count);
void *      nb_buff_get_linear_block_address(nb_buff_t* buff);
uint16_t    nb_buff_get_linear_block_length(nb_buff_t* buff);
uint16_t    nb_buff_skip(nb_buff_t* buff, uint16_t len);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __NB_BUFFER_H */

/**
 * @}
 */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

