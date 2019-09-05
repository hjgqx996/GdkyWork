/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     key.c
  * @author   ZouZH
  * @version  V1.00
  * @date     27-July-2018
  * @brief    key detection.
  ******************************************************************************
  * @history
  */
#define LOG_TAG    "KEY"

/* INCLUDES ----------------------------------------------------------------- */
#include "key.h"
#include "includes.h"
#include "systick.h"
#include "lcd.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

unsigned char m_ucKeyCode = 0;        //短按键键值
unsigned char m_ucLongKeyCode = 0;    //长按键键值

uint32_t s_u32KeyPressDoTick = 0;   //关机键按下时间戳

////uint8_t m_ucKey1PressTimes = 0;  //按键1按下次数
unsigned char m_ucMoveFlag = 0;    //移位标志   0、没有移位     1、移位

/* LOCAL VARIABLES ---------------------------------------------------------- */

unsigned char m_ucPressButtonFlag = 0;//有按键被按下标志
uint32_t s_u32MoveTestDoTick = 0;   //移位测试时间戳

/* LOCAL FUNCTIONS ---------------------------------------------------------- */



/**
 * @brief  Key init
 * @param  None
 * @return
 * @retval 0-Success
 */
uint8_t KeyInit(void)
{

    return 0;
}

/**
 * @brief  Key is down
 * @param  None
 * @return Key Status
 * @retval 0up 1down
 */
uint8_t KeyIsDwon(void)
{
  return 0;
}

/**
 * @brief  长按键检测轮询
 * @param  None
 * @retval None
 */
void LongButtonTestPoll(void)
{
#if(DEVICE_TYPE == 2)  //如果为锂电池型
  {
    if(GET_KEY2() == 0)   //如果关机键被按下
    {
      if(  (SysTickGetMsDif(s_u32KeyPressDoTick, SysTickGetMs()) > KEY_LONG_PRESS_TIME)) 
         &&(m_ucPressButtonFlag == 0))  //如果超时则认为按键长按 且 没有置长按键标志
      {
        m_ucPressButtonFlag = 1;   //有按键被按下标志
        m_ucLongKeyCode |= 0x02;    //赋长按键键值
      }
    }
    else
    {
      s_u32KeyPressDoTick = SysTickGetMs();
      m_ucPressButtonFlag = 0;   //有按键被按下标志
////      if(JudgmentSecondTimeout(s_u32ShutKeyStartTime,3))    //如果距离最后一次按键超时
////      {
////        m_ucKey1PressTimes = 0;
////      }
    }
  }
#elif(DEVICE_TYPE == 3)  //如果为 型
  {
    
  }
#endif
}

/**
 * @brief  移位检测轮询
 * @param  None
 * @retval None
 */
void MoveTestPoll(void)
{
////  if(SysTickGetMsDif(s_unMoveTestDoTick, SysTickGetMs()) > MOVE_TEST_CYCLE))
////  {
////    s_u32MoveTestDoTick = SysTickGetMs();   //获取系统时间
////    
////////    MOVE_TEST_IN();    //移位检测引脚输入
////////    SET_MOVE_TEST();    
////////    SET_MOVE_TEST_VDD();    //打开移位检测引脚电源
////////    MOVE_TEST_VDD_OUT();  
////////    
////    SysTickDelayMs(2);
////    if(GET_MOVE_TEST() == 0)
////    {
////      m_ucMoveFlag = 1;    //移位
////    }
////    else
////    {
////      m_ucMoveFlag = 0;    //正常
////    }
////////    CLR_MOVE_TEST_VDD();   //关闭移位检测电源
////  }
}

/**
 * @brief  翻页按键检测
 * @param  None
 * @retval None
 */
void KeySkipScreen(void) 
{
  static uint32_t s_u8KeepTime = 0;
  uint8_t u8Stat = 1;
  
  u8Stat = GET_KEY5();
  if((u8Stat == 0)&&(SysTickGetMsDif(s_u8KeepTime, SysTickGetMs()) > KEY_RELEASE_TIME))
  {
    s_u8KeepTime = SysTickGetMs();
    if(++g_ucPage > 6) {//液晶显示翻页
      g_ucPage = 0;
    }
  }
}

/**
 * @brief  按键轮询函数
 * @param  None
 * @retval None
 */
void KeyPoll(void)
{
//  LongButtonTestPoll();
//  MoveTestPoll;
  KeySkipScreen();

}



/**
 * @}
 */

