/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     usart2.c
  * @author   ZouZH
  * @version  V2.02
  * @date     12-June-2016
  * @brief    stm32f10x usart2 board support package.
  ******************************************************************************
  */

/* INCLUDES ------------------------------------------------------------------- */
#include "usart2.h"
#include "includes.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/**
 * 串口接收数据缓存
 */
uint8_t g_ucUSART2RxBuf[USART2_MAX_RLEN];

/* LOCAL VARIABLES ------------------------------------------------------------ */

/**
 * 串口接收数据缓存计数和标志
 */
static volatile uint16_t s_usUSART2RxStatus;

/**
 * 串口最后接收字符时间
 */
static volatile uint32_t s_ulLastRecvCharTick;

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


/**
 * @brief  初始化串口
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
uint8_t USART2Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  /* USART2-Tx  AF_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART2-Rx AF_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_DeInit(USART2);
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_OverSampling8Cmd(USART2, DISABLE);

  USART_Init(USART2, &USART_InitStructure);

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART2, ENABLE);
  USART_GetFlagStatus(USART2, USART_FLAG_TC);

  return 0;
}


/**
 * @brief  设置串口参数
 *
 * @param  baud 波特率
 * @param  parity 校验位,0无, 1奇, 2偶
 *
 * @retval 0 成功，其他失败
 */
uint8_t USART2SetParam(uint32_t baud, uint8_t parity)
{
  USART_InitTypeDef USART_InitStructure;

  USART_DeInit(USART2);
  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  if (0 == parity)
  {
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
  }
  else if (1 == parity)
  {
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_Parity = USART_Parity_Odd;
  }
  else
  {
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
  }

  USART_OverSampling8Cmd(USART2, DISABLE);

  USART_Init(USART2, &USART_InitStructure);

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART2, ENABLE);
  USART_GetFlagStatus(USART2, USART_FLAG_TC);

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
uint16_t USART2Send(const uint8_t *pbuf, uint16_t len)
{
	uint16_t tmout = 0;
	uint16_t len_tmp = 0;

  if((pbuf == NULL) || (len == 0))
    return 0;

  len_tmp = len;

  while(len_tmp)
  {
    USART2->DR = *pbuf;
    xprintf("%c", *pbuf);

    tmout = 0xFFFF;
    while((USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET) && tmout)
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
uint8_t USART2CheckReceive(void)
{
  uint8_t ret = 0;

  SYSEnterCritical();

  if ((SYSGetTickCount() - s_ulLastRecvCharTick > SYS_MS_TO_TICKS(USART2_RX_TIMEOUT)) \
     && (s_usUSART2RxStatus & USART2_RX_SIZE_MASK))
  {
    s_usUSART2RxStatus |= USART2_RX_OK;
    ret = 1;
  }
  else if (s_usUSART2RxStatus & USART2_RX_OK)
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
uint16_t USART2GetReceiveLength(void)
{
  return (s_usUSART2RxStatus & USART2_RX_SIZE_MASK);
}


/**
 * @brief  获取串口接收最后字符时间
 *
 * @param  无
 *
 * @retval 串口接收最后字符时间
 */
uint32_t USART2GetLastRecvCharTick(void)
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
void USART2EnableRx(void)
{
  SYSEnterCritical();

#ifdef USE_FULL_ASSERT

  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
  if (USART2CheckReceive() && (USART2GetReceiveLength() <= USART2_MAX_RLEN))
  {
    uint16_t i = 0;
    for (i = 0; i < (s_usUSART2RxStatus & USART2_RX_SIZE_MASK); i++)
    {
      g_ucUSART2RxBuf[i] = 0;
    }
  }
#endif

  s_usUSART2RxStatus = 0;

  USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  SYSExitCritical();
}


/**
 * @brief  关闭串口数据接收
 *
 * @param  无
 *
 * @retval 无
 */
void USART2DisableRx(void)
{
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
}


/**
 * @brief  串口中断处理程序
 *
 * @param  无
 *
 * @retval 无
 */
void USART2_IRQHandler1(void)
{
  uint8_t rx_char = 0;
  uint32_t cur_tick = 0;

  if (USART_GetITStatus(USART2, USART_IT_ORE_RX) != RESET)
    USART_ReceiveData(USART2);

  if (RESET != USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    rx_char = (uint8_t)(USART2->DR & (uint16_t)0x01FFu);

    if (s_usUSART2RxStatus & USART2_RX_OK)
      return;

    cur_tick = SYSGetTickCountISR();
    if ((cur_tick > s_ulLastRecvCharTick) && \
       ((cur_tick - s_ulLastRecvCharTick) > USART2_RX_TIMEOUT))
    {
      s_usUSART2RxStatus = 0;
    }

    s_ulLastRecvCharTick = cur_tick;

    if (USART2_RX_START != g_ucUSART2RxBuf[0])
      s_usUSART2RxStatus = 0;

    if ((s_usUSART2RxStatus & USART2_RX_SIZE_MASK) >= USART2_MAX_RLEN)
      s_usUSART2RxStatus = 0;

    g_ucUSART2RxBuf[s_usUSART2RxStatus & USART2_RX_SIZE_MASK] = rx_char;
    s_usUSART2RxStatus++;

    if((s_usUSART2RxStatus >= 20) && (g_ucUSART2RxBuf[10] + 13 == s_usUSART2RxStatus) && \
       (g_ucUSART2RxBuf[s_usUSART2RxStatus - 1] == USART2_RX_TAIL))
    {
      s_usUSART2RxStatus |= USART2_RX_OK;
      USART2DisableRx();
    }
  }
}


/**
 * 串口printf功能定义
 */
#if USART2_EN_PRINT
#pragma import(__use_no_semihosting)

struct __FILE
{
  int handle;
};

FILE __stdout;

// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
  x = x;
}

// 重定义fputc函数
int fputc(int ch, FILE *f)
{
  USART_SendData(USART2,(uint8_t)ch);
  while (USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  return ch;
}

int fgetc(FILE *f)
{
  while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
  return (int)USART_ReceiveData(USART2);
}
#endif  /*  USART2_EN_PRINT */


