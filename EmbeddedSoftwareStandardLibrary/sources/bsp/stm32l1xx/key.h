/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     key.h
  * @author   ZouZH
  * @version  V1.00
  * @date     27-July-2018
  * @brief    Wake up key for NB-IoT Collector.
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

/* CONSTANTS  --------------------------------------------------------------- */

/* Key config param */

/* Key press valid time(500ms) */
#define KEY_POLL_CYCLE    500

/* Key press valid time(3S) */
#define KEY_VALID_TIME    6

/* Key release valid time(1S) */
#define KEY_RELEASE_TIME  2

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

uint8_t     KeyInit(void);
uint8_t     KeyIsDwon(void);

KeyStatus_t KeyGetStatus(void);
void        KeyPoll(void);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __KEY_H */

/***************************** END OF FILE ************************************/


