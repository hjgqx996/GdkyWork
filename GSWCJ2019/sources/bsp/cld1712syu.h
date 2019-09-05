/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     cld1712syu.h
  * @author   scf
  * @version  V1.00
  * @date     21-May-2019
  * @brief    LCD driver for cld1712syu.
  ******************************************************************************
  * @history
  */


/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __CLD1712SYU_H
#define __CLD1712SYU_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include "includes.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */
   
#define CLD1712SYU_MAX_PAGE      6    //�����ʾҳ��
   
/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */
   
/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
   
void Cld1712_Init(void);  //Һ����ʼ��
void Cld1712_ClearLcd(void); //����
void Cld1712_ShowAll(void);  //ȫ��ʾ
void Cld1712_PutChar(uint8_t c, uint8_t pos);  //7����λ����ʾ���ֻ���ĸ��Ϣ
void Cld1712_PutSymbol(uint8_t s, uint8_t dis);  //��ʾ����
void Cld1712_DisplayPoll(void);  //��ѯ��ʾ

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __ZX21455A_H */

/**
 * @}
 */

/***************************** END OF FILE ************************************/



