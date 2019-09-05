/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file    led.h
  * @author  ZouZH
  * @version V1.02
  * @date    24-Jan-2018
  * @brief   This file contains the interface to the LED Service.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

#ifndef LED_BLINK
//    #define LED_BLINK 1
#endif /* LED_BLINK */

/* CONSTANTS  --------------------------------------------------------------- */

/* LEDS - The LED number is the same as the bit position */
#define LED_1     0x01
#define LED_2     0x02
#define LED_3     0x04
#define LED_4     0x08
#define LED_ALL   (LED_1 | LED_2 | LED_3 | LED_4)

/* Modes */
#define LED_MODE_OFF      0x00
#define LED_MODE_ON       0x01
#define LED_MODE_BLINK    0x02
#define LED_MODE_FLASH    0x04
#define LED_MODE_TOGGLE   0x08

/* Defaults */
#define LED_MAX_LEDS      4
#define LED_DUTY_CYCLE    5
#define LED_FLASH_COUNT   5
#define LED_FLASH_TIME    1000

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/*
 * Initialize LED Service.
 */
extern uint8_t LedInit( void );

/*
 * Set the LED ON/OFF/TOGGLE.
 */
extern uint8_t LedSet( uint8_t led, uint8_t mode );

/*
 * Blink the LED.
 */
extern void LedBlink(uint8_t leds, uint8_t numBlinks, uint8_t onPct, uint16_t period);

/*
 * Put LEDs in sleep state - store current values
 */
extern void LedEnterSleep( void );

/*
 * Retore LEDs from sleep state
 */
extern void LedExitSleep( void );

/*
 * Return LED state
 */
extern uint8_t LedGetState( void );

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __LED_H */

/***************************** END OF FILE ************************************/

