/**
  ******************************************************************************
  *               Copyright(C) 2019-2029 GDKY All Rights Reserved
  *
  * @file     usart0.c
  * @author   ZouZH
  * @version  V1.01
  * @date     14-Feb-2019
  * @brief    msp430f448 usart0 board support package.
  ******************************************************************************
  */

/* INCLUDES ------------------------------------------------------------------- */
#include "usart0.h"
#include "includes.h"
#include <io430.h>
#include "paramConfig.h"


/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* 串口接收数据缓存 */
uint8_t g_ucUSART0RxBuf[USART0_MAX_RLEN];

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* 接收标志和字节计数 */
static volatile uint16_t s_usUSART0RxStatus;

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
uint8_t USART0_Init(void)
{
  return USART0_SetParam(4800, 0);
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
uint8_t USART0_SetParam(uint32_t baud, uint8_t parity)
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
  //        |             P2.4|------------->
  //        |                 | baud - 8N1
  //        |             P2.5|<------------
  IE1_bit.URXIE0 = 0;
  U0CTL_bit.SWRST = 1;

  P2SEL  |= BIT4 + BIT5;    // P2.4/P2.5 = USART0 TXD/RXD
  ME1    |= UTXE0 + URXE0;  // Enable USART0 TXD/RXD
  U0CTL  |= CHAR;           // 8-bit DataBits
  U0CTL  &= ~SPB;           // 1-bit StopBit

  if (baud > 4800)
  {
    // UCLK = SMCLK
    U0TCTL_bit.SSEL0 = 0;
    U0TCTL_bit.SSEL1 = 1;
    br_clk = SYS_CPU_CLOCK_HZ;
  }
  else
  {
    // UCLK = ACLK
    U0TCTL_bit.SSEL0 = 1;
    U0TCTL_bit.SSEL1 = 0;
    br_clk = SYS_LSE_CLOCK_HZ;
  }

  br_div = br_clk / baud;
  U0BR0  = UINT16_LO(br_div);
  U0BR1  = UINT16_HI(br_div);
  U0MCTL = 0;

  for(i = 0; i < 8; i++)
  {
    txEr0 = ((100ul * baud * ((i + 1) * br_div + mSum)) / br_clk) - 100ul * (i + 1);
    txEr1 = ((100ul * baud * ((i + 1) * br_div + mSum + 1)) / br_clk) - 100ul * (i + 1);
    if(ABS(txEr1) < ABS(txEr0))
    {
      mSum++;
      U0MCTL |= (1 << i);
    }
  }

  if (0 == parity)
  {
    U0CTL &= ~PENA;
  }
  else if (1 == parity)
  {
    U0CTL |= PENA;
    U0CTL &= ~PEV;
  }
  else
  {
    U0CTL |= PENA + PEV;
  }

  U0CTL_bit.SWRST = 0;
  IE1_bit.URXIE0 = 1;

  s_usUSART0RxStatus = 0;

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
uint16_t USART0_Send(const uint8_t *pbuf, uint16_t len)
{
	uint16_t tmout = 0;
	uint16_t len_tmp = 0;

  ASSERT((pbuf != NULL) && (len != 0));

  len_tmp = len;

  while(len_tmp)
  {
    U0TXBUF = *pbuf;
    xprintf("%c", *pbuf);

    tmout = 0xFFFF;
    while((IFG1_bit.UTXIFG0 == 0) && tmout)
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
uint8_t USART0_CheckReceive(void)
{
  uint8_t ret = 0;

  SYSEnterCritical();

  if ((SYSGetTickCount() - s_ulLastRecvCharTick > USART0_RX_TIMEOUT) \
     && (s_usUSART0RxStatus & USART0_RX_SIZE_MASK))
  {
    s_usUSART0RxStatus |= USART0_RX_OK;
    ret = 1;
  }
  else if (s_usUSART0RxStatus & USART0_RX_OK)
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
 * @brief  清除接收状态
 *
 * @param  None
 *
 * @retval None
 */
void USART0_ClearRxStatus(void)
{
  s_usUSART0RxStatus = 0;
  USART0_EnableRx(); //使能接收
}

/**
 * @brief  获取接收数据长度
 *
 * @param  无
 *
 * @retval 接收数据长度
 */
uint16_t USART0_GetReceiveLength(void)
{
  return (s_usUSART0RxStatus & USART0_RX_SIZE_MASK);
}


/**
 * @brief  获取串口接收最后字符时间
 *
 * @param  无
 *
 * @retval 串口接收最后字符时间
 */
uint32_t USART0_GetLastRecvCharTick(void)
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
void USART0_EnableRx(void)
{
  SYSEnterCritical();

#ifdef USE_FULL_ASSERT
  IE1_bit.URXIE0 = 0;
  if (USART0CheckReceive() && (USART0GetReceiveLength() <= USART0_MAX_RLEN))
  {
    uint16_t i = 0;

    for (i = 0; i < (s_usUSART0RxStatus & USART0_RX_SIZE_MASK); i++)
    {
      g_ucUSART0RxBuf[i] = 0;
    }
  }
#endif

  s_usUSART0RxStatus = 0;

  IFG1_bit.URXIFG0 = 0;
  IE1_bit.URXIE0 = 1;

  SYSExitCritical();
}


/**
 * @brief  关闭串口数据接收
 *
 * @param  无
 *
 * @retval 无
 */
void USART0_DisableRx(void)
{
  IE1_bit.URXIE0 = 0;
}

#if 1
/**
 * @brief  串口中断处理程序
 *
 * @param  无
 *
 * @retval 无
 */
#pragma vector=USART0RX_VECTOR
__interrupt void USART0_IRQHandler(void)
{
  uint8_t rx_char = 0;
  uint32_t cur_tick = 0;

  //if (0 != IFG1_bit.URXIFG0)
  {
    rx_char = U0RXBUF;

    if (s_usUSART0RxStatus & USART0_RX_OK)
      return;

    cur_tick = SYSGetTickCount();////SYSGetTickCountISR();
    if ((cur_tick > s_ulLastRecvCharTick) && \
       ((cur_tick - s_ulLastRecvCharTick) > USART0_RX_TIMEOUT))
    {
      s_usUSART0RxStatus = 0;
    }

    s_ulLastRecvCharTick = cur_tick;

    //if (USART0_RX_START != g_ucUSART0RxBuf[0])
    //  s_usUSART0RxStatus = 0;

    if ((s_usUSART0RxStatus & USART0_RX_SIZE_MASK) >= USART0_MAX_RLEN)
      s_usUSART0RxStatus = 0;

    g_ucUSART0RxBuf[s_usUSART0RxStatus & USART0_RX_SIZE_MASK] = rx_char;
    s_usUSART0RxStatus++;

    if(g_ucUSART0RxBuf[s_usUSART0RxStatus - 1] == USART0_RX_TAIL)
    {
      s_usUSART0RxStatus |= USART0_RX_OK;
      
      if(lib_memcmp(g_ucUSART0RxBuf, "COMMIT CONFIG\r\n", USART0_GetReceiveLength()) == 0)  //如果收到进入本地配置命令
      {
        LocalConfigMode = 1;  //置本地配置模式标志        
        s_unEnterLocalConfigDoTick = SysTickGetSec();   //记录本地配置时间
        USART0_Send("+Enter config:OK!\r\n",19); ////????应该优化掉  
        USART0_ClearRxStatus(); 
      }
      else
      {
        USART0_DisableRx();
      }
      if(LocalConfigMode == 0)  //如果没有在本地配置状态
      {
        //                      USART0_RX_STA = 0;
        //                      USART0_RX_COUNTS=0;
      }
      LPM3_EXIT;
    }
  }
}

#endif
