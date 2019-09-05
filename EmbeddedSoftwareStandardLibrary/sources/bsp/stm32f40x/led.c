/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file    led.c
  * @author  ZouZH
  * @version V1.02
  * @date    24-Jan-2018
  * @brief   This file contains the interface to the LED Service.
  ******************************************************************************
  * 2013-12-26 First create this file
  * 2014-12-22 Modify Led_GetSystemClock() unit is ms
  * 2018-01-24 Modify code organization
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "led.h"
#include "includes.h"

/* TYPEDEFS ----------------------------------------------------------------- */

/*
 * LED control structure
 */
typedef struct
{
    uint8_t mode;    /* Operation mode */

#ifdef LED_BLINK
    uint8_t todo;    /* Blink cycles left */
    uint8_t onPct;   /* On cycle percentage */
    uint16_t time;   /* On/Off cycle time(msec) */
    uint32_t next;   /* Time for next change */
#endif /* LED_BLINK */

}LedCtrl_t;

/*
 * LED status structure
 */
typedef struct
{
    LedCtrl_t ledCtrlTbl[LED_MAX_LEDS];
    uint8_t   sleepAct;
}LedObj_t;

/* MACROS  -------------------------------------------------------------------- */
#define LED_BOARD_TYPE 1

#if (LED_BOARD_TYPE == 1)

// GNBIoTMeterConcentratorApp
#define GPIO_PORT_LED1	GPIOC  // Run
#define GPIO_PORT_LED2	GPIOC  // Net
#define GPIO_PORT_LED3	GPIOC
#define GPIO_PORT_LED4	GPIOC

#define GPIO_PIN_LED1	GPIO_Pin_2
#define GPIO_PIN_LED2	GPIO_Pin_3
#define GPIO_PIN_LED3	GPIO_Pin_3
#define GPIO_PIN_LED4	GPIO_Pin_3

#define GPIO_CLK_LED1	RCC_AHBPeriph_GPIOC
#define GPIO_CLK_LED2	RCC_AHBPeriph_GPIOC
#define GPIO_CLK_LED3	RCC_AHBPeriph_GPIOC
#define GPIO_CLK_LED4	RCC_AHBPeriph_GPIOC

#endif /* LED_BOARD_TYPE */

/*
 * Turn on or off leds
 */
#define LED_TURN_ON_LED1()    GPIO_SetBits(GPIO_PORT_LED1, GPIO_PIN_LED1)
#define LED_TURN_ON_LED2()    GPIO_SetBits(GPIO_PORT_LED2, GPIO_PIN_LED2)
#define LED_TURN_ON_LED3()    GPIO_SetBits(GPIO_PORT_LED3, GPIO_PIN_LED3)
#define LED_TURN_ON_LED4()    GPIO_SetBits(GPIO_PORT_LED4, GPIO_PIN_LED4)

#define LED_TURN_OFF_LED1()   GPIO_ResetBits(GPIO_PORT_LED1, GPIO_PIN_LED1)
#define LED_TURN_OFF_LED2()   GPIO_ResetBits(GPIO_PORT_LED2, GPIO_PIN_LED2)
#define LED_TURN_OFF_LED3()   GPIO_ResetBits(GPIO_PORT_LED3, GPIO_PIN_LED3)
#define LED_TURN_OFF_LED4()   GPIO_ResetBits(GPIO_PORT_LED4, GPIO_PIN_LED4)

/*
 * Toggle led state
 */
#define LED_TOGGLE_LED1()     ST(GPIO_PORT_LED1->ODR ^= GPIO_PIN_LED1;)
#define LED_TOGGLE_LED2()     ST(GPIO_PORT_LED2->ODR ^= GPIO_PIN_LED2;)
#define LED_TOGGLE_LED3()     ST(GPIO_PORT_LED3->ODR ^= GPIO_PIN_LED3;)
#define LED_TOGGLE_LED4()     ST(GPIO_PORT_LED4->ODR ^= GPIO_PIN_LED4;)
/*
 * Get current led state
 */
#define LED_STATE_LED1()      GPIO_ReadOutputDataBit(GPIO_PORT_LED1, GPIO_PIN_LED1)
#define LED_STATE_LED2()      GPIO_ReadOutputDataBit(GPIO_PORT_LED2, GPIO_PIN_LED2)
#define LED_STATE_LED3()      GPIO_ReadOutputDataBit(GPIO_PORT_LED3, GPIO_PIN_LED3)
#define LED_STATE_LED4()      GPIO_ReadOutputDataBit(GPIO_PORT_LED4, GPIO_PIN_LED4)

/*
 * Get current system tick count
 */
#ifdef LED_BLINK
    #define Led_GetSystemClock() (SYSGetTickCount() * portTICK_PERIOD_MS)
#endif /* LED_BLINK && FreeRTOS */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

static LedObj_t sxLedObj;
static uint8_t sucLedCurState;          /* LED current state */
static uint8_t sucLedSleepState;        /* LED state before going to sleep */

#ifdef LED_BLINK
    static uint8_t sucLedPreBlinkState; /* LED state before going to blink mode */
    static TimerHandle_t shTmrLed;
#endif /* LED_BLINK */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */
static void LedLowLevelInit(FunctionalState NewState);
static void LedOnOff(uint8_t leds, uint8_t mode);

#ifdef LED_BLINK
    static void LedUpdate(void);
    static void vLedTmrCB(TimerHandle_t pxTimer);
#endif /* LED_BLINK */


/**
 * @brief  Initialize LED Service
 * @param  None
 * @retval 0 SUCCESS.
 */
uint8_t LedInit(void)
{
    uint8_t ret = 0;

    LedLowLevelInit(ENABLE);

    /* Set all LEDs to OFF */
    LedSet(LED_ALL, LED_MODE_OFF);

  #ifdef LED_BLINK
    /* Initialize sleepActive to FLASE */
    sxLedObj.sleepAct = 0;

    /* Create led blink update timer */
    shTmrLed = xTimerCreate("LedTmr", pdMS_TO_TICKS(LED_FLASH_TIME), pdFALSE, (void *)1U, vLedTmrCB);
    if (NULL == shTmrLed)
    {
        xprintf("Led blink timer create failure!\r\n");
        ret = 1;
    }
  #endif /* LED_BLINK */

    return ret;
}


/**
 * @brief  Tun ON/OFF/TOGGLE given LEDs
 * @param  leds - bit mask value of leds to be turned ON/OFF/TOGGLE
 *         mode - BLINK, FLASH, TOGGLE, ON, OFF
 * @retval LED state at last set/clr/blink/ update
 */
uint8_t LedSet(uint8_t leds, uint8_t mode)
{
    uint8_t led;
    LedCtrl_t *pledctrl;

    switch (mode)
    {
      #ifdef LED_BLINK
        case LED_MODE_BLINK:
        {
            /* Default blink 1 time, D% duty cycle */
            LedBlink(leds, 1, LED_DUTY_CYCLE, LED_FLASH_TIME);
        }break;

        case LED_MODE_FLASH:
        {
            LedBlink(leds, LED_FLASH_COUNT, LED_DUTY_CYCLE, LED_FLASH_TIME);
        }break;
      #endif /* LED_BLINK */

        case LED_MODE_ON:
        case LED_MODE_OFF:
        case LED_MODE_TOGGLE:
        {
            led = 1;
            leds &= LED_ALL;
            pledctrl = sxLedObj.ledCtrlTbl;

            while (leds)
            {
                if (leds & led)
                {
                    if (mode != LED_MODE_TOGGLE)
                    {
                        pledctrl->mode = mode;  /* ON or OFF */
                    }
                    else
                    {
                        pledctrl->mode ^= LED_MODE_ON;  /* Toggle */
                    }
                    LedOnOff(led, pledctrl->mode);
                    leds ^= led;
                }
                led <<= 1;
                pledctrl++;
            }
        }break;

        default:
        break;
    }

    return sucLedCurState;
}


/**
 * @brief  Turns specified LED ON or OFF
 * @param  leds - LED bit mask
 *         mode - LED_ON,LED_OFF,
 * @retval None
 */
static void LedOnOff(uint8_t leds, uint8_t mode)
{
    if (leds & LED_1)
    {
        if (LED_MODE_ON & mode)
        {
            LED_TURN_ON_LED1();
        }
        else
        {
            LED_TURN_OFF_LED1();
        }
    }

    if (leds & LED_2)
    {
        if (LED_MODE_ON & mode)
        {
            LED_TURN_ON_LED2();
        }
        else
        {
            LED_TURN_OFF_LED2();
        }
    }

    if (leds & LED_3)
    {
        if (LED_MODE_ON & mode)
        {
            LED_TURN_ON_LED3();
        }
        else
        {
            LED_TURN_OFF_LED3();
        }
    }

    if (leds & LED_4)
    {
        if (LED_MODE_ON & mode)
        {
            LED_TURN_ON_LED4();
        }
        else
        {
            LED_TURN_OFF_LED4();
        }
    }

    /* Remember current led state */
    if (LED_MODE_ON & mode)
    {
        sucLedCurState |= leds;
    }
    else
    {
        sucLedCurState &= (leds ^ 0xFF);
    }
}


/**
 * @brief  Initialize the led low level hardware
 * @param  None
 * @retval None
 */
static void LedLowLevelInit(FunctionalState NewState)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Open the GPIO CLK */
    RCC_AHBPeriphClockCmd(GPIO_CLK_LED1 | GPIO_CLK_LED2 | GPIO_CLK_LED3 | GPIO_CLK_LED4, NewState);

    if(DISABLE == NewState)
        return ;
    GPIO_ResetBits(GPIO_PORT_LED1,  GPIO_PIN_LED1);
    GPIO_ResetBits(GPIO_PORT_LED2,  GPIO_PIN_LED2);
    GPIO_ResetBits(GPIO_PORT_LED3,  GPIO_PIN_LED3);
    GPIO_ResetBits(GPIO_PORT_LED4,  GPIO_PIN_LED4);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1;
    GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED2;
    GPIO_Init(GPIO_PORT_LED2, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED3;
    GPIO_Init(GPIO_PORT_LED3, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED4;
    GPIO_Init(GPIO_PORT_LED4, &GPIO_InitStructure);
}


/**
 * @brief  Get current led state
 *
 * @param  None
 *
 * @retval None
 */
uint8_t LedGetState(void)
{
    return sucLedCurState;
}


/**
 * @brief  Store current LEDs state before sleep
 *
 * @param  None
 *
 * @retval None
 */
void LedEnterSleep(void)
{
  #ifdef LED_BLINK
    /* Sleep ON */
    sxLedObj.sleepAct = 1;

    if (xTimerStop(shTmrLed, pdMS_TO_TICKS(100)) != pdPASS)
    {
        xprintf("Led blink timer stop failure!\r\n");
    }

  #endif /* LED_BLINK */

    /* Save the state of each led */
    sucLedSleepState = 0;
    sucLedSleepState |= LED_STATE_LED1();
    sucLedSleepState |= LED_STATE_LED2() << 1;
    sucLedSleepState |= LED_STATE_LED3() << 2;
    sucLedSleepState |= LED_STATE_LED4() << 3;

    /* TURN OFF all LEDS to save power */
    LedOnOff(LED_ALL, LED_MODE_OFF);

    LedLowLevelInit(DISABLE);
}


/**
 * @brief  Restore current LEDs state after sleep
 *
 * @param  None
 *
 * @retval None
 */
void LedExitSleep(void)
{
    LedLowLevelInit(ENABLE);

    /* Load back the saved state */
    LedOnOff(sucLedSleepState, LED_MODE_ON);

  #ifdef LED_BLINK
    /* Restart - This takes case BLINKING LEDS */
    LedUpdate();

    /* Sleep OFF */
    sxLedObj.sleepAct = 0;
  #endif /* LED_BLINK */
}


#ifdef LED_BLINK

/**
 * @brief  Blink the leds
 * @param  leds       - bit mask value of leds to be blinked
 *         numBlinks  - number of blinks
 *         percent    - the percentage in each period where the led will be on
 *         period     - length of each cycle in milliseconds
 * @retval None
 */
void LedBlink(uint8_t leds, uint8_t numBlinks, uint8_t onPct, uint16_t period)
{
    uint8_t led;
    LedCtrl_t *pledctrl;

    if (!leds || !onPct || !period)
    {
        LedSet(leds, LED_MODE_OFF);
        return;
    }

    if (onPct >= 100)
    {
        LedSet(leds, LED_MODE_ON);
        return;
    }

    led = 1;
    leds &= LED_ALL;
    pledctrl = sxLedObj.ledCtrlTbl;

    while (leds)
    {
        if (leds & led)
        {
            /* Store the current state of the led before going to blinking if not already blinking */
            if (pledctrl->mode < LED_MODE_BLINK)
                sucLedPreBlinkState |= (led & sucLedCurState);
            pledctrl->mode = LED_MODE_OFF;  /* Stop previous blink */
            pledctrl->time = period;        /* Time for one on/off cycle */
            pledctrl->onPct = onPct;        /* % of cycle LED is on */
            pledctrl->todo  = numBlinks;    /* Number of blink cycles */
            if (!numBlinks)
                pledctrl->mode |= LED_MODE_FLASH;   /* Continuous */
            pledctrl->next = Led_GetSystemClock();  /* Start now */
            pledctrl->mode |= LED_MODE_BLINK;       /* Enable blinking */
            leds ^= led;
        }
        led <<= 1;
        pledctrl++;
    }

    /* Cancel any overlapping timer for blink events */
    if (xTimerStop(shTmrLed, pdMS_TO_TICKS(100)) != pdPASS)
    {
        xprintf("Led blink timer stop failure!\r\n");
    }
    else
    {
        LedUpdate();
    }
}


/**
 * @brief  Update leds to work with blink
 * @param  None
 * @retval None
 */
void LedUpdate(void)
{
    uint8_t led;
    uint8_t pct = 10;
    uint8_t leds;
    LedCtrl_t *pledctrl;
    uint32_t time;
    uint16_t next;
    uint16_t wait;

    /* Check if sleep is active or not */
    if (sxLedObj.sleepAct)
    {
        return;
    }

    next = 0;
    led = 1;
    leds = LED_ALL;
    pledctrl = sxLedObj.ledCtrlTbl;

    while (leds)
    {
        if (!(leds & led))
        {
            led <<= 1;
            pledctrl++;
            continue;
        }

        if (!(pledctrl->mode & LED_MODE_BLINK))
        {
            leds ^= led;
            led <<= 1;
            pledctrl++;
            continue;
        }

        /* Get current system tick count(ms) */
        time = Led_GetSystemClock();

        if (time >= pledctrl->next)
        {
           if (pledctrl->mode & LED_MODE_ON)
           {
               pct = 100U - pledctrl->onPct;   /* Percentage of cycle for off */
               pledctrl->mode &= ~LED_MODE_ON; /* Say it's not on */
               LedOnOff(led, LED_MODE_OFF);    /* Turn it off */

               if (!(pledctrl->mode & LED_MODE_FLASH))
               {
                   pledctrl->todo--;  /* Not continuous, reduce count */
               }
           }
           else if ((!pledctrl->todo) && !(pledctrl->mode & LED_MODE_FLASH))
           {
               pledctrl->mode ^= LED_MODE_BLINK;  /* No more blinks */
           }
           else
           {
               pct = pledctrl->onPct;          /* Percentage of cycle for on */
               pledctrl->mode |= LED_MODE_ON;  /* Say it's on */
               LedOnOff(led, LED_MODE_ON);     /* Turn it on */
           }

           if (pledctrl->mode & LED_MODE_BLINK)
           {
               wait = (((uint32_t)pct * (uint32_t)pledctrl->time) / 100U);
               pledctrl->next = time + wait;
           }
           else
           {
               /* No more blink, No more wait */
               wait = 0;

               /* After blinking, set the LED back to the state before it blinks */
               LedSet(led, ((sucLedPreBlinkState & led) != 0U)? LED_MODE_ON : LED_MODE_OFF);

               /* Clear the saved bit */
               sucLedPreBlinkState &= led ^ 0xFF;
           }
        }
        else
        {
           wait = pledctrl->next - time;  /* Time left */
        }

        if (!next || (wait && (wait < next)))
        {
           next = wait;
        }

        leds ^= led;
        led <<= 1;
        pledctrl++;
    }

    if (next)
    {
       /* Start timer to schedule event */
       if (xTimerChangePeriod(shTmrLed,  pdMS_TO_TICKS(next), 0) != pdPASS)
       {
           xprintf("Led blink timer start failure!\r\n");
       }
    }
}


static void vLedTmrCB( TimerHandle_t pxTimer )
{
    LedUpdate();
}
#endif /* LED_BLINK */

