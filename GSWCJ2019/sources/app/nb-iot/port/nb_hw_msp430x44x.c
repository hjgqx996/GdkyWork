/**
  ******************************************************************************
  *               Copyright(C) 2019-2029 GDKY All Rights Reserved
  *
  * @file     nb_hw_msp430x44x.c
  * @author   ZouZH
  * @version  V1.00
  * @date     22-March-2019
  * @brief    NB-IoT hardware port of msp430x44x.
  ******************************************************************************
  * @history
  */


/* INCLUDES ----------------------------------------------------------------- */
#include "nb.h"
#include "includes.h"

#include <io430.h>
//#include "usart0.h"
#include "usart1.h"

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

#define RST_L() do{P6OUT_bit.P6 = 0;}while(0)
#define RST_H() do{P6OUT_bit.P6 = 1;}while(0)

#define PWR_L() do{P6OUT_bit.P3 = 0;}while(0)
#define PWR_H() do{P6OUT_bit.P3 = 1;}while(0)

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

    P6DIR_bit.P3 = 1;
    P6DIR_bit.P6 = 1;

    nbHWSetReset(0);
    nbHWSetPower(0);

//    USART0SetParam(baudrate, 0);
    USART1SetParam(baudrate, 0);//UART1 for NB86-G.

    return 0;
}

/**
 * @brief      Send data to NB module
 * @param[in]  pbuf: Pointer to data to send
 * @param[in]  len:  Number of bytes to send
 * @return     Number of bytes sent
 */
uint16_t nbHWSend(const void* pbuf, uint16_t len) {
//    USART0Send(pbuf, len);
    USART1Send(pbuf, len);
    return 0;
}


/**
 * @brief  Set Power On/Off
 * @param  stat: 1-On, 0-Off
 * @retval None
 */
void nbHWSetPower(uint8_t stat) {
    if (stat)
        PWR_H();////PWR_L();//20190516,scf.
    else
        PWR_L();////PWR_H();
}

/**
 * @brief  Set Reset Pin
 * @param  stat: 1-High, 0-Low
 * @retval None
 */
void nbHWSetReset(uint8_t stat) {
    if (stat)
        RST_H();////RST_L();
    else
        RST_L();////RST_H();
}


///**
// * @brief  UART interrupt handle function
// * @param  None
// * @retval None
// */
//#pragma vector=USART0RX_VECTOR
//__interrupt void USART0_IRQHandler(void) {
//    uint8_t rx_char = U0RXBUF;
//
//    nbDataReceived(&rx_char, 1);
//}

/**
 * @brief  串口中断处理程序
 *
 * @param  无
 *
 * @retval 无
 */
#pragma vector=USART1RX_VECTOR
__interrupt void USART1_IRQHandler(void)
{
    uint8_t rx_char = U1RXBUF;

    nbDataReceived(&rx_char, 1);
}

/**
 * @}
 */

/**
 * @}
 */

