/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_hw_stm32l15x.c
  * @author   ZouZH
  * @version  V1.00
  * @date     01-Aug-2018
  * @brief    NB-IoT hardware port of stm32l15x.
  ******************************************************************************
  * @history
  */


/* INCLUDES ----------------------------------------------------------------- */
#include "nb.h"

#include "includes.h"

#include "usart2.h"

/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @defgroup NB-Hardware
 * @brief NB-IoT硬件接口
 * @{
 */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

#define GPIO_PORT_RST	  GPIOB
#define GPIO_PORT_PWR	  GPIOB

#define GPIO_PIN_RST	  GPIO_Pin_0
#define GPIO_PIN_PWR	  GPIO_Pin_12

#define GPIO_CLK_RST	  RCC_AHBPeriph_GPIOB
#define GPIO_CLK_PWR	  RCC_AHBPeriph_GPIOB

#define RST_L()         GPIO_ResetBits(GPIO_PORT_RST, GPIO_PIN_RST)
#define RST_H()         GPIO_SetBits(GPIO_PORT_RST, GPIO_PIN_RST)

#define PWR_L()         GPIO_ResetBits(GPIO_PORT_PWR, GPIO_PIN_PWR)
#define PWR_H()         GPIO_SetBits(GPIO_PORT_PWR, GPIO_PIN_PWR)


/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

/**
 * @brief  Init Hardware
 * @param  baudrate: Baudrate to use on AT port
 * @return Operation result
 * @retval 0-Success
 */
uint8_t nbHWInit(uint32_t baudrate) {
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(GPIO_CLK_RST | GPIO_CLK_PWR, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RST;
    GPIO_Init(GPIO_PORT_RST, &GPIO_InitStructure);
    nbHWSetReset(0);

    GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_PWR;
    GPIO_Init(GPIO_PORT_PWR, &GPIO_InitStructure);
    nbHWSetPower(0);

    USART2Init();
    USART2SetParam(baudrate, 0);

    return 0;
}

/**
 * @brief      Send data to NB module
 * @param[in]  pbuf: Pointer to data to send
 * @param[in]  len:  Number of bytes to send
 * @return     Number of bytes sent
 */
uint16_t nbHWSend(const void* pbuf, uint16_t len) {
    USART2Send(pbuf, len);
    return 0;
}


/**
 * @brief  Set Power On/Off
 * @param  stat: 1-On, 0-Off
 * @retval None
 */
void nbHWSetPower(uint8_t stat) {
    if (stat)
        PWR_L();
    else
        PWR_H();
}

/**
 * @brief  Set Reset Pin
 * @param  stat: 1-High, 0-Low
 * @retval None
 */
void nbHWSetReset(uint8_t stat) {
    if (stat)
        RST_H();
    else
        RST_L();
}


/**
 * @brief  UART interrupt handle function
 * @param  None
 * @retval None
 */
void USART2_IRQHandler(void) {
    uint8_t rx_char = 0;
    
    if (USART_GetITStatus(USART2, USART_IT_ORE_RX) != RESET)
        USART_ReceiveData(USART2);

    if (RESET != USART_GetITStatus(USART2, USART_IT_RXNE)) {
        rx_char = (uint8_t)(USART2->DR & (uint16_t)0x01FFu);
        nbDataReceived(&rx_char, 1);
    }
}

/**
 * @}
 */

/**
 * @}
 */

