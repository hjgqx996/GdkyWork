/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     key.h
  * @author   ZouZH
  * @version  V1.00
  * @date     27-July-2018
  * @brief    key detection.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>
#include "digitio.h"
   
/* TYPEDEFS ----------------------------------------------------------------- */

/*
 * Key current status
 */
typedef enum
{
    KEY_RELEASE =    (0),  /* Release */
    KEY_DEPRESS =    (1),  /* Press down */
    KEY_VALID   =    (2),  /* Up and Down */
    KEY_LONG    =    (3),  /* Long Press */
    KEY_REPEAT  =    (4),  /* Send repeat */

} KeyStatus_t;;

/* MACROS  ------------------------------------------------------------------ */
   
#define GET_KEY1()               DIO_GetInStatus(DIO_IN_P10)
#define GET_KEY2()               DIO_GetInStatus(DIO_IN_K1)
#define GET_KEY3()               DIO_GetInStatus(DIO_IN_P11)
#define GET_KEY4()               DIO_GetInStatus(DIO_IN_K2) 
#define GET_KEY5()               DIO_GetInStatus(DIO_IN_K3)
#define GET_MOVE_TEST_VDD()      0//DIO_GetInStatus()
#define GET_MOVE_TEST()          0//DIO_GetInStatus()


#if(DEVICE_TYPE == 3)
#define GET_LIGHT1_STATE()       DIO_GetInStatus(DIO_IN_LOS1)
#define GET_LIGHT2_STATE()       0//DIO_GetInStatus()
#endif

/* CONSTANTS  --------------------------------------------------------------- */

//Key config param

#define KEY_POLL_CYCLE      500   //Key press valid time(500ms)
#define KEY_VALID_TIME      6     //Key press valid time(3S)
#define KEY_RELEASE_TIME    1000     //Key release valid time(1S)
#define KEY_LONG_PRESS_TIME 6000  //
#define KEY_MOVE_TEST_CYCLE 500   ////

/* GLOBAL VARIABLES --------------------------------------------------------- */

extern uint32_t s_unMoveTestDoTick;   //移位测试时间戳
extern unsigned char m_ucKeyCode;    //按键值
extern unsigned char m_ucLongKeyCode;//长按键键值
extern uint32_t s_unButtonPressDoTick;   //关机键按下时间戳
extern unsigned char m_ucMoveFlag;    //移位标志   0、没有移位     1、移位
////extern uint8_t m_ucKey1PressTimes;  //按键1按下次数

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

uint8_t     KeyInit(void);
uint8_t     KeyIsDwon(void);

void LongButtonTestPoll(void);  //长按键检测轮询
void MoveTestPoll(void);        //移位检测轮询
void KeySkipScreen(void);       //翻页按键检测

void KeyPoll(void);
/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __KEY_H */

/***************************** END OF FILE ************************************/


