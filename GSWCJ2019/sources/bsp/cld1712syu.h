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
   
#define CLD1712SYU_MAX_PAGE      6    //最大显示页数
   
/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */
   
/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
   
void Cld1712_Init(void);  //液晶初始化
void Cld1712_ClearLcd(void); //清屏
void Cld1712_ShowAll(void);  //全显示
void Cld1712_PutChar(uint8_t c, uint8_t pos);  //7段码位置显示数字或字母信息
void Cld1712_PutSymbol(uint8_t s, uint8_t dis);  //显示符号
void Cld1712_DisplayPoll(void);  //轮询显示

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



