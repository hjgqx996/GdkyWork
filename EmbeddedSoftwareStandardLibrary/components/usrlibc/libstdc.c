/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY  All Rights Reserved
  *
  * @file     libstdc.c
  * @author   ZouZH
  * @version  V1.01
  * @date     03-Feb-2016
  * @brief    self-defining standard c library function .
  ******************************************************************************
  */
// Verision History
// 2015-05-06 V1.00 Create this file
// 2016-02-03 V1.01 Add lib_strncpy


/* INCLUDES ----------------------------------------------------------------- */
#include "libstdc.h"
#include <stddef.h>

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

#define _TOLOWER(c) ( ((c) >= 'A') && ((c) <= 'Z') ? ((c) - 'A' + 'a') : (c) )

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */



/**
 * @brief  Sets the first "count" bytes of the memory starting
 *          at "dst" to the character value "val".
 *
 * @param  dst   - pointer to memory to fill with val
 * @param  val   - value to put in dst bytes
 * @param  count - number of bytes of dst to fill
 *
 * @retval returns dst, with filled bytes
 */
void *lib_memset(void *dst, uint8_t val, uint16_t count)
{
  void *start = dst;
  
  if (!count || (NULL == dst))
  {
    return (start);
  }

  while(count)
  {
    count -= 1;
    *(uint8_t *)dst = val;
    dst = (uint8_t *)dst + 1;
  }

  return (start);
}


/**
 * @brief   Copies a source memory buffer to a destination memory buffer.
 *          This routine does NOT recognize overlapping buffers, and thus can lead
 *          to propogation.
 *          For cases where propogation must be avoided, memmove() must be used.
 *
 * @param  dst    - pointer to destination buffer
 * @param  src    - pointer to source buffer
 * @param  count  - number of bytes to copy
 *
 * @retval Returns a pointer to the destination buffer
 */
void *lib_memcpy(void *dst, const void *src, uint16_t count)
{
  void * ret = dst;
  
  if (!count || (NULL == dst) || (NULL == src))
  {
    return (ret);
  }

  while(count)
  {
    count -= 1;
    *(uint8_t *)dst = *(uint8_t *)src;
    dst = (uint8_t *)dst + 1;
    src = (uint8_t *)src + 1;
  }

  return (ret);
}

/**
 * @brief   Copies a source memory buffer to a destination memory buffer invert.
 *          This routine does NOT recognize overlapping buffers, and thus can lead
 *          to propogation.
 *          For cases where propogation must be avoided, memmove() must be used.
 *
 * @param  dst    - pointer to destination buffer
 * @param  src    - pointer to source buffer
 * @param  count  - number of bytes to copy
 *
 * @retval Returns a pointer to the destination buffer
 */
void *lib_memcpy_invert(void *dst, const void *src, uint16_t count)
{
  void * ret = dst;
  
  if (!count || (NULL == dst) || (NULL == src))
  {
    return (ret);
  }
  
  src = (uint8_t *)src + count - 1;

  while(count)
  {
    count -= 1;
    *(uint8_t *)dst = *(uint8_t *)src;
    dst = (uint8_t *)dst + 1;
    src = (uint8_t *)src - 1;
  }

  return (ret);
}


/**
 * @brief  Searches at buf for the given character, stopping when chr is
 *         first found or cnt bytes have been searched through.
 *
 * @param  buf  - memory buffer to be searched
 * @param  chr  - character to search for
 * @param  cnt  - max number of bytes to search
 *
 * @retval returns pointer to first occurence of chr in buf
 *         returns NULL if chr not found in the first cnt bytes
 */
const void *lib_memchr(const void *buf, uint8_t chr, uint16_t cnt)
{
  if (!cnt || (NULL == buf))
  {
    return NULL;
  }
  
  while(cnt && (*(const uint8_t *)buf != (uint8_t)chr))
  {
    buf = (const uint8_t *)buf + 1;
    cnt -= 1;
  }

  return (cnt ? buf : NULL);
}

/**
 * @brief  Compares count bytes of memory starting at buf1 and buf2
 *         and find if equal or which one is first in lexical order.
 *
 * @param  *buf1, *buf2 - pointers to memory sections to compare
 * @param  count        - length of sections to compare
 *
 * @retval returns < 0 if buf1 < buf2
 *         returns = 0 if buf1 == buf2
 *         returns > 0 if buf1 > buf2
 */
int8_t lib_memcmp(const void *buf1, const void *buf2, uint16_t count)
{
  if (!count || (NULL == buf1) || (NULL == buf2))
  {
    return(0);
  }
  
  while(count && *(const uint8_t *)buf1 == *(const uint8_t *)buf2)
  {
    count -= 1;

    if (count)
    {
      buf1 = (const uint8_t *)buf1 + 1;
      buf2 = (const uint8_t *)buf2 + 1;
    }
  }

  return (*((const uint8_t *)buf1) - *((const uint8_t *)buf2));
}

/**
 * @brief  Invert compares count bytes of memory starting at buf1 and buf2
 *         and find if equal or which one is first in lexical order.
 *
 * @param  *buf1, *buf2 - pointers to memory sections to compare
 * @param  count        - length of sections to compare
 *
 * @retval returns < 0 if buf1 < buf2
 *         returns = 0 if buf1 == buf2
 *         returns > 0 if buf1 > buf2
 */
int8_t lib_memcmp_invert(const void *buf1, const void *buf2, uint16_t count)
{
  if (!count || (NULL == buf1) || (NULL == buf2))
  {
    return(0);
  }

  buf2 = (const uint8_t *)buf2 + count - 1;

  while(count && *(const uint8_t *)buf1 == *(const uint8_t *)buf2)
  {
    count -= 1;
    if (count)
    {
      buf1 = (const uint8_t *)buf1 + 1;
      buf2 = (const uint8_t *)buf2 - 1;
    }
  }

  return (*((const uint8_t *)buf1) - *((const uint8_t *)buf2));
}


/**
 * @brief  Compares count bytes of the two blocks of memory stored at first
 *         and last.  The characters are converted to lowercase before
 *         comparing (not permanently), so case is ignored in the search.
 *
 * @param  *first, *last - memory buffers to compare
 * @param  count         - maximum length to compare
 *
 * @retval returns < 0 if first < last
 *         returns = 0 if first == last
 *         returns > 0 if first > last
 */
int8_t lib_memicmp(const void *first, const void *last, uint16_t count)
{
  uint8_t f = 0;
  uint8_t l = 0;
  
  if (!count || (NULL == first) || (NULL == last))
  {
    return(0);
  }

  while(count)
  {
    count -= 1;
    if((*(unsigned char *)first == *(unsigned char *)last) \
      || ((f = _TOLOWER(*(unsigned char *)first )) ==
         (l = _TOLOWER(*(unsigned char *)last ))))
    {
      first = (char *)first + 1;
      last = (char *)last + 1;
    }
    else
    {
      break;
    }
  }

  return (f - l);
}

/**
 * @brief  Compares count bytes of memory starting at buf
 *          and  if equal all.
 *
 * @param  *buf         - pointers to memory sections to compare
 * @param  count        - length of sections to compare
 * @param  val          - value to compare
 *
 * @retval  returns < 0 if buf < value
 *          returns = 0 if buf == value
 *          returns > 0 if buf > value
 */
int8_t lib_memvalcmp(const void *buf, uint16_t count, uint8_t val)
{
  const uint8_t *p = buf;
  
  if (!count || (NULL == p))
  {
    return(0);
  }

  while(count)
  {
    if (*p != val)
    {
      break;
    }
    
    count -= 1;
    if (count)
    {
      p += 1;
    }
  }

  return (int8_t)(*p - val);
}

/**
 * @brief  Copies count characters from the source string to the
 *          destination.  If count is less than the length of source,
 *          NO NULL CHARACTER is put onto the end of the copied string.
 *          If count is greater than the length of sources, dest is padded
 *          with null characters to length count.
 *
 * @param  dest - pointer to destination
 * @param  source - source string for copy
 * @param  count - max number of characters to copy
 *
 * @retval dest
 */
char * lib_strncpy(char *dest, const char *source, uint32_t count)
{
  char *start = dest;
  
  if (!count || (NULL == dest) || (NULL == source))
  {
    return start;
  }

  /* copy string */
  while(count && *source)
  {
    *dest++ = *source++;
    count -= 1;
  }

  /* pad out with zeroes */
  while (count)
  {
    count -= 1;
    *dest++ = '\0';
  }

  return (start);
}


/**
 * @brief  Copies count characters from the source string to the
 *          destination.  If count is less than the length of source,
 *          NO NULL CHARACTER is put onto the end of the copied string.
 *          If count is greater than the length of sources, dest is padded
 *          with null characters to length count.
 *
 * @param  dest - pointer to destination
 * @param  source - source string for copy
 * @param  count - max number of characters to copy
 * @param  c     - break char
 *
 * @retval dest
 */
char * lib_strncpybrk(char *dest, const char *source, uint32_t count, char c)
{
  char *start = dest;
  
  if (!count || (NULL == dest) || (NULL == source))
  {
    return start;
  }

  while(count && *source && (*source != c))    /* copy string */
  {
    *dest++ = *source++;
    count -= 1;
  }
  
  while (count)  /* pad out with zeroes */
  {
    *dest++ = '\0';
    count -= 1;
  }

  return (start);
}

