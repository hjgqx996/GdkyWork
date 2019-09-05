/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     lcd.c
  * @author   ZouZH
  * @version  V1.00
  * @date     22-April-2016
  * @brief    lcd.
  ******************************************************************************
  * @history
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "includes.h"
#include "lcd.h"

/** 
 * @defgroup lcd
 * @brief lcd display
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */
uint8_t g_ucPage = 0;    //液晶显示页
uint8_t g_ucScreenRefresh = 0;  //如果为1屏幕立即刷新一次

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */



/**
 * @brief  Init LCD
 * @param  void
 * @retval None
 */
void lcd_init(void)
{
#if(DISPLAY_TYPE == 1)
  Dg14093Init();
#elif(DISPLAY_TYPE == 2)
  Zx21455aInit();  //清屏
#elif(DISPLAY_TYPE == 3)
  Cld1712_Init();
#elif(DISPLAY_TYPE == 4)
  Gtkb21pInit();
#else

#endif
}

/**
 * @brief  LCD on 
 * @param  void
 * @retval None
 */
void LcdOn(void)
{
  LCDCTL |= LCDON; //开启液晶显示开关
}

/**
 * @brief  LCD off
 * @param  void
 * @retval None
 */
void LcdOff(void)
{
  LCDCTL &= ~ LCDON; //关闭液晶显示开关 
}

/**
 * @brief  clear LCD
 * @param  void
 * @retval None
 */
void clear_lcd(void)
{
#if(DISPLAY_TYPE == 1)
  ClearDg14093Lcd();
#elif(DISPLAY_TYPE == 2)
  ClearZx21455aLcd();  //清屏
#elif(DISPLAY_TYPE == 3)
  Cld1712_ClearLcd();  //清屏
#elif(DISPLAY_TYPE == 4)
  ClearGtkb21pLcd();  //清屏
#else

#endif
}

/**
 * @brief  LCD show all items.
 * @param  void
 * @retval None
 */
void AllShowLcd(void)
{
#if(DISPLAY_TYPE == 1)
  AllShowDg14093Lcd();
#elif(DISPLAY_TYPE == 2)
  AllShowZx21455aLcd();  //全显示
#elif(DISPLAY_TYPE == 3)
  Cld1712_ShowAll();    //全显示
#elif(DISPLAY_TYPE == 4)
  AllShowGtkb21pLcd();  //全显示
#else

#endif
}

/**
 * @brief  LCD display character
 * @param[in] c:   character to be displayed, 0-17.
 * @param[in] pos: position of character.
 * @retval None
 */
void LcdPutChar(unsigned char c, unsigned char pos)
{
#if(DISPLAY_TYPE == 1)
  Dg14093PutChar(c,pos);
#elif(DISPLAY_TYPE == 2)
  Zx21455aPutChar(c,pos);  //7段码位置显示数字或字母信息
#elif(DISPLAY_TYPE == 3)
  Cld1712_PutChar(c,pos);  //7段码位置显示数字或字母信息
#elif(DISPLAY_TYPE == 4)
  Gtkb21pPutChar(c,pos);  //7段码位置显示数字或字母信息
#else

#endif
}

/**
 * @brief  LCD display symbol
 * @param[in] s:   symbol to be diplayed.
 * @param[in] dis: 0:display; 1:not dispaly.
 * @retval None
 */
void LcdPutSymbol(unsigned char s, unsigned char dis)
{
#if(DISPLAY_TYPE == 1)
  Dg14093PutSymbol(s,dis);
#elif(DISPLAY_TYPE == 2)
  Zx21455aPutSymbol(s,dis);  //显示符号
#elif(DISPLAY_TYPE == 3)
  Cld1712_PutSymbol(s,dis);  //显示符号
#elif(DISPLAY_TYPE == 4)
  Gtkb21pPutSymbol(s,dis);  //显示符号
#else

#endif
}

/**
 * @brief  LCD run.
 * @param  void
 * @retval None
 */
void DisplayPoll(void)
{
#if(DISPLAY_TYPE == 1)
  Dg14093DisplayPoll();
#elif(DISPLAY_TYPE == 2)
  Zx21455aDisplayPoll();  //轮询显示
#elif(DISPLAY_TYPE == 3)
  Cld1712_DisplayPoll();  //轮询显示
#elif(DISPLAY_TYPE == 4)
  Gtkb21pDisplayPoll();  //轮询显示
#else

#endif
}



/**
 * @}
 */

