/**
  ******************************************************************************
  *               Copyright(C) 2019-2029 GDKY All Rights Reserved
  *
  * @file     usart1.c
  * @author   ZouZH
  * @version  V1.01
  * @date     22-March-2019
  * @brief    msp430f448 usart1 board support package.
  ******************************************************************************
  */


/* INCLUDES ------------------------------------------------------------------- */
#include "usart1.h"
#include "includes.h"

#include <io430.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* 串口接收数据缓存 */
uint8_t g_ucUSART1RxBuf[USART1_MAX_RLEN];

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* 接收标志和字节计数 */
static volatile uint16_t s_usUSART1RxStatus;

/* 最后接收字符时间 */
static volatile uint32_t s_ulLastRecvCharTick;

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


/**
 * @brief  初始化串口
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
uint8_t USART1Init(void)
{
  return USART1SetParam(4800, 0);
}



/**
 * @brief  设置串口参数
 * @note   根据波特率自动选择合适的时钟源，配置波特率分频系数，
 *         ACLK(<=4800bps),SMCLK(>4800bps), CPU Clock > 3 * Baudrate
 *
 * @param  baud 波特率(300-115200)
 * @param  parity 校验位,0无, 1奇, 2偶
 *
 * @retval 0 成功，其他失败
 */
uint8_t USART1SetParam(uint32_t baud, uint8_t parity)
{
  uint16_t br_div = 0;
  uint32_t br_clk = 0;

  uint8_t mSum = 0;       //Σmi
  int16_t txEr0 = 0;      //Bit=0时错误率
  int16_t txEr1 = 0;      //Bit=1时错误率
  uint8_t i = 0;

  //         -----------------
  //     /|\|              XIN|-
  //      | |                 | 32kHz
  //      --|RST          XOUT|-
  //        |                 |
  //        |             P4.0|------------->
  //        |                 | baud - 8N1
  //        |             P4.1|<------------
  IE2_bit.URXIE1 = 0;
  U1CTL_bit.SWRST = 1;

  P4SEL  |= BIT0 + BIT1;    // P4.0/P4.1 = USART1 TXD/RXD
  ME2    |= UTXE1 + URXE1;  // Enable USART1 TXD/RXD
  U1CTL  |= CHAR;           // 8-bit DataBits
  U1CTL  &= ~SPB;           // 1-bit StopBit

  if (baud > 4800)
  {
    // UCLK = SMCLK
    U1TCTL_bit.SSEL0 = 0;
    U1TCTL_bit.SSEL1 = 1;
    br_clk = SYS_CPU_CLOCK_HZ;
  }
  else
  {
    // UCLK = ACLK
    U1TCTL_bit.SSEL0 = 1;
    U1TCTL_bit.SSEL1 = 0;
    br_clk = SYS_LSE_CLOCK_HZ;
  }

  br_div = br_clk / baud;
  U1BR0  = UINT16_LO(br_div);
  U1BR1  = UINT16_HI(br_div);
  U1MCTL = 0;

  for(i = 0; i < 8; i++)
  {
    txEr0 = ((100ul * baud * ((i + 1) * br_div + mSum)) / br_clk) - 100ul * (i + 1);
    txEr1 = ((100ul * baud * ((i + 1) * br_div + mSum + 1)) / br_clk) - 100ul * (i + 1);
    if(ABS(txEr1) < ABS(txEr0))
    {
      mSum++;
      U1MCTL |= (1 << i);
    }
  }

  if (0 == parity)
  {
    U1CTL &= ~PENA;
  }
  else if (1 == parity)
  {
    U1CTL |= PENA;
    U1CTL &= ~PEV;
  }
  else
  {
    U1CTL |= PENA + PEV;
  }

  U1CTL_bit.SWRST = 0;
  IE2_bit.URXIE1 = 1;

  s_usUSART1RxStatus = 0;

  return 0;
}



/**
 * @brief  发送串口数据包
 *
 * @param  pbuf 数据包缓存
 * @param  len  数据包长度
 *
 * @retval 实际发送数据长度
 */
uint16_t USART1Send(const uint8_t *pbuf, uint16_t len)
{
	uint16_t tmout = 0;
	uint16_t len_tmp = 0;

  ASSERT((pbuf != NULL) && (len != 0));

  len_tmp = len;

  while(len_tmp)
  {
    U1TXBUF = *pbuf;

    tmout = 0xFFFF;
    while((IFG2_bit.UTXIFG1 == 0) && tmout)
    {
      tmout--;
    }

    if (!tmout)
    {
      break;
    }
    pbuf++;
    len_tmp--;
  }

  return (len - len_tmp);
}


/**
 * @brief  查询是否接收到有效数据包
 *
 * @param  无
 *
 * @retval 0无数据包,1有数据包
 */
uint8_t USART1CheckReceive(void)
{
  uint8_t ret = 0;

  SYSEnterCritical();

  if ((SYSGetTickCount() - s_ulLastRecvCharTick > USART1_RX_TIMEOUT) \
     && (s_usUSART1RxStatus & USART1_RX_SIZE_MASK))
  {
    s_usUSART1RxStatus |= USART1_RX_OK;
    ret = 1;
  }
  else if (s_usUSART1RxStatus & USART1_RX_OK)
  {
    ret = 1;
  }
  else
  {
    ret = 0;
  }

  SYSExitCritical();

  return ret;
}



/**
 * @brief  获取接收数据长度
 *
 * @param  无
 *
 * @retval 接收数据长度
 */
uint16_t USART1GetReceiveLength(void)
{
  return (s_usUSART1RxStatus & USART1_RX_SIZE_MASK);
}


/**
 * @brief  获取串口接收最后字符时间
 *
 * @param  无
 *
 * @retval 串口接收最后字符时间
 */
uint32_t USART1GetLastRecvCharTick(void)
{
  return s_ulLastRecvCharTick;
}


/**
 * @brief  清空接收缓冲区并使能串口接收
 *
 * @param  无
 *
 * @retval 无
 */
void USART1EnableRx(void)
{
  SYSEnterCritical();

#ifdef USE_FULL_ASSERT
  IE2_bit.URXIE1 = 0;
  if (USART1CheckReceive() && (USART1GetReceiveLength() <= USART1_MAX_RLEN))
  {
    uint16_t i = 0;

    for (i = 0; i < (s_usUSART1RxStatus & USART1_RX_SIZE_MASK); i++)
    {
      g_ucUSART1RxBuf[i] = 0;
    }
  }
#endif

  s_usUSART1RxStatus = 0;

  IFG2_bit.URXIFG1 = 0;
  IE2_bit.URXIE1 = 1;

  SYSExitCritical();
}


/**
 * @brief  关闭串口数据接收
 *
 * @param  无
 *
 * @retval 无
 */
void USART1DisableRx(void)
{
  IE2_bit.URXIE1 = 0;
}

#if 0  //uart0 用于和NB通讯，中断函数定义在nb_hw_msp430x44x.c中，这里屏蔽中断定义。
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
  uint8_t rx_char = 0;
  uint32_t cur_tick = 0;

  //if (0 != IFG2_bit.URXIFG1)
  {
    rx_char = U1RXBUF;

    if (s_usUSART1RxStatus & USART1_RX_OK)
      return;

    cur_tick = SYSGetTickCount();////SYSGetTickCountISR();
    if ((cur_tick > s_ulLastRecvCharTick) && \
       ((cur_tick - s_ulLastRecvCharTick) > USART1_RX_TIMEOUT))
    {
      s_usUSART1RxStatus = 0;
    }

    s_ulLastRecvCharTick = cur_tick;

    if (USART1_RX_START != g_ucUSART1RxBuf[0])
      s_usUSART1RxStatus = 0;

    if ((s_usUSART1RxStatus & USART1_RX_SIZE_MASK) >= USART1_MAX_RLEN)
      s_usUSART1RxStatus = 0;

    g_ucUSART1RxBuf[s_usUSART1RxStatus & USART1_RX_SIZE_MASK] = rx_char;
    s_usUSART1RxStatus++;

    if((s_usUSART1RxStatus >= 20) && (g_ucUSART1RxBuf[10] + 13 == s_usUSART1RxStatus) && \
       (g_ucUSART1RxBuf[s_usUSART1RxStatus - 1] == USART1_RX_TAIL))
    {
      s_usUSART1RxStatus |= USART1_RX_OK;
      USART1DisableRx();
    }
  }
}
#endif

