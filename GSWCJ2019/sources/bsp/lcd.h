/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     lcd.h
  * @author   ZouZH
  * @version  V1.00
  * @date     22-April-2016
  * @brief    lcd.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H_
#define __LCD_H_

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */

#include "app_cfg.h"
   
#if(DISPLAY_TYPE == 1)
  #include "dg14093lcd.h"
#elif(DISPLAY_TYPE == 2)
  #include "zx21455a.h"
#elif(DISPLAY_TYPE == 3)
  #include "cld1712syu.h"
#elif(DISPLAY_TYPE == 4)
  #include "gtkb21p.h"
#else
#endif
   
/** 
 * @addtogroup lcd
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern uint8_t g_ucPage;    //液晶显示页
extern uint8_t g_ucScreenRefresh;  //如果为1屏幕立即刷新一次

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

void lcd_init(void);   //液晶初始化
void LcdOn(void);   //打开液晶
void LcdOff(void);   //关闭液晶
void clear_lcd(void);  //清屏
void AllShowLcd(void);  //全显示
void LcdPutChar(unsigned char c, unsigned char pos);  //7段码位置显示数字或字母信息
void LcdPutSymbol(unsigned char s, unsigned char dis);  //显示符号
void DisplayPoll(void);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __LCD_H_ */

/**
 * @}
 */

/***************************** END OF FILE ************************************/

