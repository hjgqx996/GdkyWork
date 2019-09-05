/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     usart1.c
  * @author   ZouZH
  * @version  V2.02
  * @date     12-June-2016
  * @brief    stm32f10x usart1 board support package.
  ******************************************************************************
  */

/* INCLUDES ------------------------------------------------------------------- */
#include "usart1.h"
#include "includes.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/**
 * 串口接收数据缓存
 */
uint8_t g_ucUSART1RxBuf[USART1_MAX_RLEN];

/* LOCAL VARIABLES ------------------------------------------------------------ */

/**
 * 串口接收数据缓存计数和标志
 */
static volatile uint16_t s_usUSART1RxStatus;

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
uint8_t USART1Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  SYSEnterCritical();

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  /* USART1-Tx  AF_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART1-Rx AF_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_DeInit(USART1);
  USART_InitStructure.USART_BaudRate = 4800;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_OverSampling8Cmd(USART1, DISABLE);

  USART_Init(USART1, &USART_InitStructure);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE);
  USART_GetFlagStatus(USART1, USART_FLAG_TC);

  SYSExitCritical();
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
uint8_t USART1SetParam(uint32_t baud, uint8_t parity)
{
  USART_InitTypeDef USART_InitStructure;

  SYSEnterCritical();

  USART_DeInit(USART1);
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

  USART_OverSampling8Cmd(USART1, DISABLE);

  USART_Init(USART1, &USART_InitStructure);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE);
  USART_GetFlagStatus(USART1, USART_FLAG_TC);

  SYSExitCritical();

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

  assert_param((pbuf != NULL) && (len != 0));

  len_tmp = len;

  while(len_tmp)
  {
    USART1->DR = *pbuf;

    tmout = 0xFFFF;
    while((USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) && tmout)
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
  uint16_t i = 0;

  SYSEnterCritical();

  if (USART1CheckReceive() && (USART1GetReceiveLength() <= USART1_MAX_RLEN))
  {
    for (i = 0; i < (s_usUSART1RxStatus & USART1_RX_SIZE_MASK); i++)
    {
      g_ucUSART1RxBuf[i] = 0;
    }
  }
  s_usUSART1RxStatus = 0;

  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

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
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}


/**
 * @brief  串口中断处理程序
 *
 * @param  无
 *
 * @retval 无
 */
void USART1_IRQHandler(void)
{
  uint8_t rx_char = 0;
  uint32_t cur_tick = 0;

  if (USART_GetITStatus(USART1, USART_IT_ORE_RX) != RESET)
	  USART_ReceiveData(USART1);

  if (RESET != USART_GetITStatus(USART1, USART_IT_RXNE))
  {
    rx_char = (uint8_t)(USART1->DR & (uint16_t)0x01FFu);

    if (s_usUSART1RxStatus & USART1_RX_OK)
      return;

    cur_tick = SYSGetTickCountISR();
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


/**
 * 串口printf功能定义
 */
#if USART1_EN_PRINT
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
  USART_SendData(USART1,(uint8_t)ch);
  while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
  return ch;
}

int fgetc(FILE *f)
{
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
  return (int)USART_ReceiveData(USART1);
}
#endif  /*  USART1_EN_PRINT */


