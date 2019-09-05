/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_buffer.c
  * @author   ZouZH
  * @version  V1.00
  * @date     01-Aug-2018
  * @brief    Ring buffer manager.
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
 * @defgroup NB-Buffer
 * @brief NB-IoT FIFO 缓存维护
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
 * @brief      Initialize buffer
 * @param[in]  buff: Pointer to nb_buff_t structure
 * @param[in]  size: Size of buffer in units of bytes
 * @param[in]  buff_ptr: Pointer to array for buffer storage.
 *             Its length should be equal to Size parameter.
 *             If NULL is passed as parameter, malloc will be used to allocate memory on heap.
 * @return     0 success, failed otherwise
 */
uint8_t nb_buff_init(nb_buff_t* buff, uint16_t size, void *buff_ptr) {
    if (buff == NULL) {
        return 0;
    }
    memset(buff, 0, sizeof(*buff));

    buff->size = size;
    buff->buff = buff_ptr;
    buff->flags = 0;
#if NB_CFG_OS
    if (buff->buff == NULL) {
        buff->buff = nbOsMemAlloc(size);
        buff->flags |= NB_BUFF_FLAG_MALLOC;
    } else {
#else
    {
#endif
        buff->flags |= NB_BUFF_FLAG_STATIC;
    }

    if (buff->buff == NULL) {
        buff->size = 0;
        return 1;
    }

    return 0;
}


/**
 * @brief      Free dynamic allocation if used on memory
 * @param[in]  buff: Pointer to buffer structure
 */
void nb_buff_free(nb_buff_t* buff) {
    if (buff == NULL) {
        return;
    }
    
#if NB_CFG_OS
    if (buff->flags & NB_BUFF_FLAG_MALLOC) {
        nbOsMemFree(buff->buff);
    }
#endif

    buff->buff = NULL;
    buff->size = 0;
    buff->flags = 0;
}

/**
 * @brief      Write data to buffer
 * @param[in]  buff: Pointer to buffer structure
 * @param[in]  data: Pointer to data to copy memory from
 * @param[in]  count: Number of bytes we want to write
 * @return     Number of bytes actually written to buffer
 */
uint16_t nb_buff_write(nb_buff_t* buff, const void* data, uint16_t count) {
  	uint16_t i = 0;
  	uint16_t free;
    const uint8_t* d = data;
    uint16_t tocopy;

    if (buff == NULL || count == 0) {
        return 0;
    }
    if (buff->in >= buff->size) {
        buff->in = 0;
    }
    free = nb_buff_get_free(buff);
    if (free < count) {
        if (free == 0) {
            return 0;
        }
        count = free;
    }

    /* We have calculated memory for write */
    tocopy = buff->size - buff->in;
    if (tocopy > count) {
        tocopy = count;
    }
    memcpy(&buff->buff[buff->in], d, tocopy);
    i += tocopy;
    buff->in += tocopy;
    count -= tocopy;
    if (count > 0) {
        memcpy(buff->buff, (void *)&d[i], count);
        buff->in = count;
    }
    if (buff->in >= buff->size) {
        buff->in = 0;
    }
    return (i + count);
}

/**
 * @brief      Read data from buffer
 * @param[in]  buff: Pointer to buffer structure
 * @param[out] data: Pointer to data to copy memory to
 * @param[in]  count: Number of bytes we want to read
 * @return     Number of bytes actually read and saved to data variable
 */
uint16_t nb_buff_read(nb_buff_t* buff, void* data, uint16_t count) {
    uint8_t *d = data;
    uint16_t i = 0, full;
    uint16_t tocopy;

    if (buff == NULL || count == 0) {
        return 0;
    }
    if (buff->out >= buff->size) {
        buff->out = 0;
    }
    full = nb_buff_get_full(buff);
    if (full < count) {
        if (full == 0) {
            return 0;
        }
        count = full;
    }

    tocopy = buff->size - buff->out;
    if (tocopy > count) {
        tocopy = count;
    }
    memcpy(d, &buff->buff[buff->out], tocopy);
    i += tocopy;
    buff->out += tocopy;
    count -= tocopy;
    if (count > 0) {
        memcpy(&d[i], buff->buff, count);
        buff->out = count;
    }
    if (buff->out >= buff->size) {
        buff->out = 0;
    }
    return (i + count);
}

/**
 * @brief      Read from buffer but do not change read and write pointers
 * @param[in]  buff: Pointer to buffer structure
 * @param[in]  skip_count: Number of bytes to skip before reading peek data
 * @param[out] data: Pointer to data to save read memory
 * @param[in]  count: Number of bytes to peek
 * @return     Number of bytes written to data array
 */
uint16_t nb_buff_peek(nb_buff_t* buff, uint16_t skip_count, void* data, uint16_t count) {
    uint8_t *d = data;
    uint16_t i = 0, full, tocopy, out;

    if (buff == NULL || count == 0) {
        return 0;
    }
    out = buff->out;
    if (buff->out >= buff->size) {
        buff->out = 0;
    }
    full = nb_buff_get_full(buff);
    if (skip_count >= full) {
        return 0;
    }
    out += skip_count;
    full -= skip_count;
    if (out >= buff->size) {
        out -= buff->size;
    }
    if (full < count) {
        if (full == 0) {
            return 0;
        }
        count = full;
    }

    tocopy = buff->size - out;
    if (tocopy > count) {
        tocopy = count;
    }
    memcpy(d, &buff->buff[out], tocopy);
    i += tocopy;
    count -= tocopy;
    if (count > 0) {
        memcpy(&d[i], buff->buff, count);
    }
    return (i + count);
}

/**
 * @brief      Get length of free space
 * @param[in]  buff: Pointer to buffer structure
 * @return     Number of free bytes in memory
 */
uint16_t nb_buff_get_free(nb_buff_t* buff) {
    uint16_t size, in, out;

    if (buff == NULL) {
        return 0;
    }
    in = buff->in;
    out = buff->out;
    if (in == out) {
        size = buff->size;
    } else if (out > in) {
        size = out - in;
    } else {
        size = buff->size - (in - out);
    }
    return (size - 1);
}

/**
 * @brief      Get length of buffer currently being used
 * @param[in]  buff: Pointer to buffer structure
 * @return     Number of bytes ready to be read
 */
uint16_t nb_buff_get_full(const nb_buff_t* buff) {
    uint16_t in, out, size;

    if (buff == NULL) {
        return 0;
    }
    in = buff->in;
    out = buff->out;
    if (in == out) {
        size = 0;
    } else if (in > out) {
        size = in - out;
    } else {
        size = buff->size - (out - in);
    }
    return size;
}

/**
 * @brief      Resets and clears buffer
 * @param[in]  buff: Pointer to buffer structure
 */
void nb_buff_reset(nb_buff_t* buff) {
    if (buff == NULL) {
    	  return;
    }
    buff->in = 0;
    buff->out = 0;
}

/**
 * @brief      Get linear address for buffer for fast read
 * @param[in]  buff: Pointer to buffer
 * @return     Pointer to start of linear address
 */
void *
nb_buff_get_linear_block_address(nb_buff_t* buff) {
    return &buff->buff[buff->out];
}

/**
 * @brief      Get length of linear block address before it overflows
 * @param[in]  buff: Pointer to buffer
 * @return     Length of linear address
 */
uint16_t nb_buff_get_linear_block_length(nb_buff_t* buff) {
    uint16_t len;
    if (buff->in > buff->out) {
        len = buff->in - buff->out;
    } else if (buff->out > buff->in) {
        len = buff->size - buff->out;
    } else {
        len = 0;
    }
    return len;
}

/**
 * @brief      Skip (ignore) buffer data.
 * @note       Useful at the end of streaming transfer such as DMA
 * @param[in]  buff: Pointer to buffer structure
 * @param[in]  len: Length of bytes we want to skip
 * @return     Number of bytes skipped
 */
uint16_t nb_buff_skip(nb_buff_t* buff, uint16_t len) {
    uint16_t full;
    full = nb_buff_get_full(buff);
    if (len > full) {
        len = full;
    }
    buff->out += len;
    if (buff->out >= buff->size) {
        buff->out -= buff->size;
    }
    return len;
}

/**
 * @}
 */

/**
 * @}
 */

