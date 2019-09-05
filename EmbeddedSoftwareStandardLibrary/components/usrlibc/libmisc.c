/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     libmisc.c
  * @author   ZouZH
  * @version  V1.08
  * @date     12-July-2018
  * @brief    .
  ******************************************************************************
  * @Revision History:
  * V1.00 2012-08-13  Create the source file
  * V1.01 2013-08-19  Add Convert multi-byte integer types from host byte order to network byte order
  * V1.04 2015-07-31  Add isbcds
  * V1.05 2015-09-17  Add bcds2dec_ex, bcds2dec_invert, bcds2dec_ex_invert-12
  * V1.06 2015-12-23  Add lib_pow64 and fixed bcds2dec_ex bcds2dec_ex_invert
  * V1.07 2016-04-19  Move net function to libnet.c,
  *                   Add lib_atoi, lib_atol, lib_atof
  * V1.03 2018-07-12  Code format specification
  */

/* INCLUDES ------------------------------------------------------------------- */
#include "libmisc.h"

#include <ctype.h>
#include <string.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/** 大连世达密匙 */
const uint8_t  KEY_CODE_DLSD[104]={
  0x56,0x56,0x7F,0x51,0x57,0x5D,0x5A,0x7F,0x9A,0x9A,0xB3,0x9D,0x64,0x6E,0x69,0x4C,
  0x9A,0x9A,0x4C,0x62,0x9B,0x91,0x69,0x4C,0x9A,0x65,0xB3,0x62,0x9B,0x6E,0x96,0x4C,
  0x6A,0x6A,0x43,0x6D,0x6B,0x61,0x66,0x43,0x95,0x95,0xBC,0x92,0x94,0x9E,0x99,0xBC,
  0x65,0x9A,0x4C,0x9D,0x64,0x91,0x69,0xB3,0x65,0x65,0xB3,0x9D,0x64,0x6E,0x96,0xB3,
  0x65,0x65,0x4C,0x62,0x9B,0x91,0x96,0xB3,0xB3,0x4C,0xB3,0x43,0xBC,0x4C,0xB3,0x4C,
  0x9A,0x9A,0x65,0x6A,0x95,0x9A,0x65,0x65,0x5D,0x6E,0x91,0x6E,0x61,0x9E,0x91,0x6E,
  0xA1,0x1A,0xB2,0x3C,0x57,0x8A,0x4C,0xEC
};

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


/**
 * @brief  32位指数计算
 *
 * @param  底数
 * @param  指数
 *
 * @retval 结果
 */
uint32_t lib_pow(uint8_t base, uint8_t times)
{
  uint32_t res = 1;

  while (times--)
    res *= base;

  return res;
}

/**
 * @brief  64指数计算
 *
 * @param  底数
 * @param  指数
 *
 * @retval 结果
 */
uint64_t lib_pow64(uint8_t base, uint8_t times)
{
  uint64_t res = 1;

  while (times--)
    res *= base;

  return res;
}


/**
 * @brief is BCD code format?
 *
 * @param[in] data buffer point
 * @param[in] data buffer length bytes
 *
 * @retval 1-true, 0-false
 */
uint8_t isbcds(const void *pbuf, uint8_t len)
{
  uint8_t ret = 0;
  const uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return 0;

  while (len)
  {
    if (((*pdat & 0x0F) > 0x09) || ((*pdat >> 4) > 0x09))
      break;

    len -= 1;
  }

  if (!len)
    ret = 1;

  return ret;
}

/**
 * @brief  BCD to Decimal
 *
 * @param  1Byte BCD
 *
 * @retval 1Byte Decimal
 */
uint8_t bcd2dec(uint8_t bcd)
{
  return ((bcd & 0x0F) + 10 * ((bcd >> 4) & 0x0F));
}

/**
 * @brief  Decimal to BCD
 *
 * @param  1Byte Decimal
 *
 * @retval 1Byte BCD
 */
uint8_t dec2bcd(uint8_t dec)
{
  return ((dec % 10) | ((dec / 10) << 4));
}

/**
 * @brief BCD to Decimal
 *
 * @param[in] data buffer point(MSB -> LSB(0~len-1))
 * @param[in] data buffer length bytes
 *
 * @retval 32bit decimal number
 */
uint32_t bcds2dec(const void *pbuf, uint8_t len)
{
  uint8_t dectmp = 0;
  uint32_t decres = 0;
  const uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return 0;

  while (len)
  {
    dectmp = (((*pdat >> 4) & 0x0F) * 10) + (*pdat & 0x0F);
    if (dectmp > 99)
    {
      decres = 0;
      break;
    }

    len -= 1;
    pdat += 1;

    decres +=  dectmp * lib_pow(100, len);
  }

  return decres;
}

/**
 * @brief BCD to Decimal
 *
 * @param[in] data buffer point(MSB -> LSB(0~len-1))
 * @param[in] data buffer length bytes
 *
 * @retval 64bit decimal number
 */
uint64_t bcds2dec_ex(const void *pbuf, uint8_t len)
{
  uint8_t dectmp = 0;
  uint64_t decres = 0;
  const uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return 0;

  while (len)
  {
    dectmp = (((*pdat >> 4) & 0x0F) * 10) + (*pdat & 0x0F);
    if (dectmp > 99)
    {
      decres = 0;
      break;
    }

    len -= 1;
    pdat += 1;

    decres +=  dectmp * lib_pow(100, len);
  }

  return decres;
}


/**
 * @brief BCD to Decimal
 *
 * @param[in] data buffer point(LSB -> MSB(0~len-1))
 * @param[in] data buffer length bytes
 *
 * @retval 32bit decimal number
 */
uint32_t bcds2dec_invert(const void *pbuf, uint8_t len)
{
  uint8_t i = 0;
  uint8_t dectmp = 0;
  uint32_t decres = 0;
  const uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return 0;

  for(i = 0; i < len; i++)
  {
    dectmp = ((*(pdat + i) >> 4) & 0x0F) * 10 + (*(pdat + i) & 0x0F);
    if (dectmp > 99)
    {
      decres = 0;
      break;
    }

    decres +=  dectmp * lib_pow(100, i);
  }

  return decres;
}

/**
 * @brief BCD to Decimal
 *
 * @param[in] data buffer point(LSB -> MSB(0~len-1))
 * @param[in] data buffer length bytes
 *
 * @retval 64bit decimal number
 */
uint64_t bcds2dec_ex_invert(const void *pbuf, uint8_t len)
{
  uint8_t i = 0;
  uint8_t dectmp = 0;
  uint64_t decres = 0;
  const uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return 0;

  for(i = 0; i < len; i++)
  {
    dectmp = ((*(pdat + i) >> 4) & 0x0F) * 10 + (*(pdat + i) & 0x0F);
    if (dectmp > 99)
    {
      decres = 0;
      break;
    }

    decres +=  dectmp * lib_pow(100, i);
  }

  return decres;
}


/**
 * @brief 32Bit Decimal to BCD
 *
 * @param Decimal number
 * @param[out] data buffer point(MSB -> LSB(0~len-1))
 * @param[in]  data buffer length bytes
 *
 * @retval None
 */
void dec2bcds(uint32_t dec, void *pbuf, uint8_t len)
{
  uint8_t tmp = 0;
  uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return;

  while (len)
  {
    tmp = dec % 100;
    pdat[len - 1] = ((tmp / 10) << 4) | (tmp % 10);
    dec /= 100;
    len -= 1;
  }
}

/**
 * @brief 32Bit Decimal to BCD
 *
 * @param Decimal number
 * @param[out] data buffer point(LSB -> MSB(0~len-1))
 * @param[in]  data buffer length bytes
 *
 * @retval None
 */
void dec2bcds_invert(uint32_t dec, void *pbuf, uint8_t len)
{
  uint8_t i = 0;
  uint8_t tmp = 0;
  uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return;

  for(i = 0; i < len; i++)
  {
    tmp = dec % 100;
    pdat[i] = ((tmp / 10) << 4) + (tmp % 10);
    dec /= 100;
  }
}


/**
 * @brief 64Bit Decimal to BCD
 *
 * @param Decimal number
 * @param[out] data buffer point(MSB -> LSB(0~len-1))
 * @param[in]  data buffer length bytes
 *
 * @retval None
 */
void dec2bcds_ex(uint64_t dec, void *pbuf, uint8_t len)
{
  uint8_t tmp = 0;
  uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return;

  while (len)
  {
    tmp = dec % 100;
    pdat[len - 1] = ((tmp / 10) << 4) | (tmp % 10);
    dec /= 100;
    len -= 1;
  }
}


/**
 * @brief 64Bit Decimal to BCD
 *
 * @param Decimal number
 * @param[out] data buffer point(LSB -> MSB(0~len-1))
 * @param[in]  data buffer length bytes
 *
 * @retval None
 */
void dec2bcds_ex_invert(uint64_t dec, void *pbuf, uint8_t len)
{
  uint8_t i = 0;
  uint8_t tmp = 0;
  uint8_t *pdat = pbuf;

  if (NULL == pdat)
    return;

  for(i = 0; i < len; i++)
  {
    tmp = dec % 100;
    pdat[i] = ((tmp / 10) << 4) + (tmp % 10);
    dec /= 100;
  }
}

/**
 * @brief  Gets the base in a string
 *
 * @param  The string to check
 * @param  save the base
 *
 * @retval the string after remove base chars
 */
static char *_getbase(char *p, int *basep)
{
	if (p[0] == '0')
	{
		switch (p[1])
		{
		case 'x':
			*basep = 16;
			break;
		case 't': case 'n':
			*basep = 10;
			break;
		case 'o':
			*basep = 8;
			break;
		default:
			*basep = 10;
			return (p);
		}
		return (p + 2);
	}
	*basep = 10;
	return (p);
}


/**
 * @brief  converts p to binary result in vp
 *
 * @param  Save the binary
 * @param  The string to check
 * @param  the base
 *
 * @retval 1-success, 0-failed
 */
static int _atob (uint64_t *vp, char *p, int base)
{
	uint64_t value, v1, v2;
	char *q, tmp[20];
	int digit;

	if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
	{
		base = 16;
		p += 2;
	}

	if (base == 16 && (q = strchr (p, '.')) != 0)
	{
		if (q - p > sizeof(tmp) - 1)
			return (0);

		strncpy (tmp, p, q - p);
		tmp[q - p] = '\0';
		if (!_atob (&v1, tmp, 16))
			return (0);

		q++;
		if (strchr (q, '.'))
			return (0);

		if (!_atob (&v2, q, 16))
			return (0);
		*vp = (v1 << 16) + v2;
		return (1);
	}

	value = *vp = 0;
	for (; *p; p++) {
		if (*p >= '0' && *p <= '9')
			digit = *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			digit = *p - 'a' + 10;
		else if (*p >= 'A' && *p <= 'F')
			digit = *p - 'A' + 10;
		else
			return (0);

		if (digit >= base)
			return (0);
		value *= base;
		value += digit;
	}
	*vp = value;
	return (1);
}


/**
 * @brief  converts p to binary result in vp
 *
 * @param  Save the binary
 * @param  The string to check
 * @param  the base
 *
 * @retval 1-success, 0-failed
 */
int lib_atob(uint32_t *vp, char *p, int base)
{
	uint64_t v;

	if (base == 0)
		p = _getbase (p, &base);
	if (_atob(&v, p, base))
	{
		*vp = v;
		return (1);
	}
	return (0);
}


/**
 * @brief  Convert a string to an Integer
 *
 * @param  The string to convert(max 11 chars)
 *
 * @retval int32
 */
int32_t lib_atoi(const char *p)
{
	int32_t digit, isneg;
	int32_t value;

	isneg = 0;
	value = 0;
	for (; isspace (*p); p++);	/* gobble up leading whitespace */

	/* do I have a sign? */
	if (*p == '-')
	{
		isneg = 1;
		p++;
	}
	else if (*p == '+')
		p++;

	for (; *p; p++)
	{
		if (*p >= '0' && *p <= '9')
			digit = *p - '0';
		else
			break;
		value *= 10;
		value += digit;
	}

	if (isneg)
		value = 0 - value;
	return (value);
}

/**
 * @brief  Convert a string to an long Integer
 *
 * @param  The string to convert(max 11 chars)
 *
 * @retval long
 */
long lib_atol(const char *p)
{
	int digit, isneg;
	long value;

	isneg = 0;
	value = 0;
	for (; isspace (*p); p++);

	if (*p == '-')
	{
		isneg = 1;
		p++;
	}
	else if (*p == '+')
		p++;

	for (; *p; p++)
	{
		if (*p >= '0' && *p <= '9')
			digit = *p - '0';
		else
			break;
		value *= 10;
		value += digit;
	}

	if (isneg)
		value = 0 - value;
	return (value);
}

/**
 * @brief  Convert a string to double
 *
 * @param  The string to convert
 *
 * @retval double
 */
double lib_atof(char *p)
{
  double d, t;
  int    len, val, sz, div, isneg;
  char   tmp[18];

  if (*p == '-')
  {
    isneg = 1;
    p++;
  }
  else
    isneg = 0;

  sz = strcspn (p, ".");
  if (sz > 0)
  {
    strncpy (tmp, p, sz);
    tmp[sz] = 0;
    if (!lib_atob ((uint32_t *)&val, tmp, 10))
    return (d);
  }
  else
    val = 0;

  d = (double)val;
  p += sz;
  if (*p)
    p++;
  if (*p)
  {
    len = strlen (p);
    if (!lib_atob ((uint32_t *)&val, p, 10))
      return (0);

    div = 1;
    for (; len > 0; len--)
      div *= 10;

    t = (double)val;
    t /= div;

    d += t;
  }
  if (isneg)
    d = 0 - d;
  return (d);
}


/**
 * @brief  Search Frame with Head and End Byte
 *
 * @param  The buffer point to convert
 * @param  The buffer length
 *
 * @retval new buffer length
 */
uint16_t lib_findframe188(uint8_t **ppbuf, uint16_t len)
{
  uint16_t ofs = 0;
  uint8_t *pbuf = NULL;

  if ((NULL == ppbuf) || !len)
    return len;

  pbuf = *ppbuf;

  for (ofs = 0; ofs < len; ofs++)
  {
    if ((0x68 != pbuf[0]) && (0x68 == pbuf[ofs]) && (ofs < len))
    {
      pbuf = &pbuf[ofs];
      len -= ofs;
    }

    if ((0x16 != pbuf[len - 1]) && (0x16 == pbuf[len - 1 - ofs]) && (ofs < len))
      len -= ofs;

    if ((0x68 == pbuf[0]) && (0x16 == pbuf[len - 1]))
      break;
  }

  *ppbuf = pbuf;
  return len;
}


uint16_t lib_findframegdky(uint8_t **ppbuf, uint16_t len)
{
  uint16_t ofs = 0;
  uint8_t *pbuf = NULL;

  if ((NULL == ppbuf) || !len)
    return len;

  pbuf = *ppbuf;

  for (ofs = 0; ofs < len; ofs++)
  {
    if ((0xFE == pbuf[ofs]) && (ofs < len))
    {
      pbuf = &pbuf[ofs];
      len -= ofs;
    }

    if ((0xED == pbuf[len - 1 - ofs]) && (ofs < len))
      len -= ofs;

    if ((0xFE == pbuf[0]) && (0xFE != pbuf[1]) && (0xED == pbuf[len - 1]))
      break;
  }

  *ppbuf = pbuf;

  return len;
}

uint16_t lib_findframegdkyhex(uint8_t **ppbuf, uint16_t len)
{
  uint16_t ofs = 0;
  uint8_t *pbuf = NULL;

  if ((NULL == ppbuf) || !len)
    return len;

  pbuf = *ppbuf;

  for (ofs = 0; ofs < len; ofs++)
  {
    if ((0xFE!= pbuf[0]) && (0xFE == pbuf[ofs]) && (ofs < len))
    {
      pbuf = &pbuf[ofs];
      len -= ofs;
    }

    if ((0xED != pbuf[len - 1]) && (0xED == pbuf[len - 1 - ofs]) && (ofs < len))
      len -= ofs;

    if ((0xFE == pbuf[0]) && (0xED == pbuf[len - 1]))
      break;
  }

  *ppbuf = pbuf;
  return len;
}

uint16_t lib_findframejgsh(uint8_t **ppbuf, uint16_t len)
{
  uint16_t ofs = 0;
  uint8_t *pbuf = NULL;

  if ((NULL == ppbuf) || !len)
    return len;

  pbuf = *ppbuf;

  for (ofs = 0; ofs < len; ofs++)
  {
    if ((0xF3 != pbuf[0]) && (0xF3 == pbuf[ofs]) && (ofs < len))
    {
      pbuf = &pbuf[ofs];
      len -= ofs;
    }

    if ((0xF3 == pbuf[0]) && (0x19 == pbuf[1]))
      break;
  }

  *ppbuf = pbuf;
  return len;
}

uint16_t lib_findframedlsd(uint8_t **ppbuf, uint16_t len)
{
  uint8_t ofs = 0;
  uint8_t *pbuf = NULL;

  if ((NULL == ppbuf) || !len)
    return len;

  pbuf = *ppbuf;

  for (ofs = 0; ofs < len; ofs++)
  {
    if ((0x68 != pbuf[0]) && (0x68 == pbuf[ofs]) && (ofs < len))
    {
      pbuf = &pbuf[ofs];
      len -= ofs;
    }

    if ((0x16 != pbuf[len - 1]) && (0x16 == pbuf[len - 1 - ofs]) && (ofs < len))
      len -= ofs;

    if ((0x68 == pbuf[0]) && (0x16 == pbuf[len - 1]))
      break;
  }
  
  if ((len >= 14) && (14 + pbuf[11] != len) && (0x16 == pbuf[13 + pbuf[11]]))
    len = 14 + pbuf[11];

  *ppbuf = pbuf;
  return len;
}

/**
 * @brief  大连世达阀门加密/解密功能
 *
 * @param  pbuf: The buffer point to convert
 * @param  len: The buffer length
 *
 * @retval 0 Success, other Failed
 */
uint8_t lib_encrypt_frame_dlsd(uint8_t *pbuf, uint16_t len)
{
  uint8_t  i, j;

  if ((NULL == pbuf) || (len < 14) || (14 + pbuf[11] != len))
    return 1;

  for (i = 2; i <= 11 + pbuf[11]; i++)
  {
    if (i == 11) 
      continue;

    pbuf[i] ^= pbuf[0];
  }

  for (i = 2; i <= 11 + pbuf[11]; i++)
  {
    if (i == 11) 
      continue;

    if (2 + i + pbuf[11] >= sizeof(KEY_CODE_DLSD)) 
      j = 2 + i + pbuf[11] - sizeof(KEY_CODE_DLSD);
    else
      j = 2 + i + pbuf[11];
      
    pbuf[i] ^= KEY_CODE_DLSD[j];
  }
  
  return 0;
}


uint8_t lib_decrypt_frame_dlsd(uint8_t *pbuf, uint16_t len)
{
  uint8_t  i, j;

  if ((NULL == pbuf) || (len < 14) || (14 + pbuf[11] != len))
    return 1;

  for (i = 2; i <= 11 + pbuf[11]; i++)
  {
    if(i == 11) 
      continue;
      
    if (2 + i + pbuf[11] >= sizeof(KEY_CODE_DLSD)) 
      j = 2 + i + pbuf[11] - sizeof(KEY_CODE_DLSD);
    else
      j= 2 + i + pbuf[11];

    pbuf[i] ^= KEY_CODE_DLSD[j];
  }

  for (i = 2; i <= 11 + pbuf[11]; i++)
  {
    if( i == 11) 
      continue;
      
    pbuf[i] ^= pbuf[0];   
  }
  
  return 0;
}
