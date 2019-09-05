/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     rs485.c
  * @author   ZouZH
  * @version  V1.02
  * @date     12-June-2016
  * @brief    stm32l1xx uart5 board support package.
  ******************************************************************************
  */

/* INCLUDES ------------------------------------------------------------------- */
#include "rs485.h"
#include "includes.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/**
 * RS485电源控制
 */
#define GPIO_PORT_KZ    GPIOC
#define GPIO_PIN_KZ     GPIO_Pin_10
#define GPIO_CLK_KZ     RCC_AHBPeriph_GPIOC

#define GPIO_PORT_RE    GPIOC
#define GPIO_PIN_RE     GPIO_Pin_11
#define GPIO_CLK_RE     RCC_AHBPeriph_GPIOC


#define RS485_KZ_H()     GPIO_SetBits(GPIO_PORT_KZ, GPIO_PIN_KZ)
#define RS485_KZ_L()     GPIO_ResetBits(GPIO_PORT_KZ, GPIO_PIN_KZ)
#define RS485_KZ_Read()  GPIO_ReadOutputDataBit(GPIO_PORT_KZ, GPIO_PIN_KZ)

#define RS485_RE_H()     GPIO_SetBits(GPIO_PORT_RE, GPIO_PIN_RE)
#define RS485_RE_L()     GPIO_ResetBits(GPIO_PORT_RE, GPIO_PIN_RE)
#define RS485_RE_Read()  GPIO_ReadOutputDataBit(GPIO_PORT_RE, GPIO_PIN_RE)



/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/**
 * 串口接收数据缓存
 */
uint8_t g_ucRS485RxBuf[RS485_MAX_RLEN];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/**
 * 串口接收数据缓存计数和标志
 */
static volatile uint16_t s_usRS485RxStatus;

/**
 * 串口最后接收字符时间
 */
static volatile uint32_t s_ulLastRecvCharTick;

/**
 * Mark Space 校验标志
 */
static volatile uint8_t s_ucRS485MarkFlag = 0;


/* LOCAL FUNCTIONS ------------------------------------------------------------ */


/**
 * @brief  初始化串口
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
uint8_t RS485Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  s_ucRS485MarkFlag = 0;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | GPIO_CLK_KZ | GPIO_CLK_RE, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  /* USART1-Tx  AF_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART1-Rx AF_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  /* RS485-KZ output Low level valid */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KZ;
  GPIO_Init(GPIO_PORT_KZ, &GPIO_InitStructure);
  RS485_KZ_H();

  /* RS485-En  Out_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RE;
  GPIO_Init(GPIO_PORT_RE, &GPIO_InitStructure);
  RS485_RE_L();
  
  USART_DeInit(USART1);
  USART_InitStructure.USART_BaudRate = 2400;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_OverSampling8Cmd(USART1, DISABLE);

  USART_Init(USART1, &USART_InitStructure);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE);
  USART_GetFlagStatus(USART1, USART_FLAG_TC);

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
uint8_t RS485SetParam(uint32_t baud, uint8_t parity)
{
  USART_InitTypeDef USART_InitStructure;

  s_ucRS485MarkFlag = 0;

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
  else if (2 == parity)
  {
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
  }
  else
  {
    s_ucRS485MarkFlag = 1;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
  }

  USART_OverSampling8Cmd(USART1, DISABLE);

  USART_Init(USART1, &USART_InitStructure);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE);
  USART_GetFlagStatus(USART1, USART_FLAG_TC);

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
uint16_t RS485Send(const uint8_t *pbuf, uint16_t len)
{
	uint16_t tmout = 0;
	uint16_t len_tmp = 0;

  if((pbuf == NULL) || (len == 0))
    return 0;

  len_tmp = len;

  // Send Mode
  RS485_RE_H();
  for (tmout = 0; tmout < 100; tmout++);

  while(len_tmp)
  {
    if ((len_tmp == len) && s_ucRS485MarkFlag)
      USART1->DR = *pbuf | (1 << 8);
    else
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

  // Receive Mode
  RS485EnableRx();

  return (len - len_tmp);
}


/**
 * @brief  查询是否接收到有效数据包
 *
 * @param  无
 *
 * @retval 0无数据包,1有数据包
 */
uint8_t RS485CheckReceive(void)
{
  uint8_t ret = 0;

  SYSEnterCritical();

  if ((SYSGetTickCount() - s_ulLastRecvCharTick > SYS_MS_TO_TICKS(RS485_RX_TIMEOUT)) \
     && (s_usRS485RxStatus & RS485_RX_SIZE_MASK))
  {
    s_usRS485RxStatus |= RS485_RX_OK;
    ret = 1;
  }
  else if (s_usRS485RxStatus & RS485_RX_OK)
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
uint16_t RS485GetReceiveLength(void)
{
  return (s_usRS485RxStatus & RS485_RX_SIZE_MASK);
}


/**
 * @brief  获取串口接收最后字符时间
 *
 * @param  无
 *
 * @retval 串口接收最后字符时间
 */
uint32_t RS485GetLastRecvCharTick(void)
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
void RS485EnableRx(void)
{
  SYSEnterCritical();

#ifdef USE_FULL_ASSERT

  if (RS485CheckReceive() && (RS485GetReceiveLength() <= RS485_MAX_RLEN))
  {
    uint16_t i = 0;
    for (i = 0; i < (s_usRS485RxStatus & RS485_RX_SIZE_MASK); i++)
    {
      g_ucRS485RxBuf[i] = 0;
    }
  }
#endif
  s_usRS485RxStatus = 0;
  // Receive Mode
  RS485_RE_L();
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
void RS485DisableRx(void)
{
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}


/**
 * @brief  电源控制
 *
 * @param  0 or 1
 *
 * @retval 无
 */
void RS485Power(uint8_t onoff)
{
  if (onoff)
    RS485_KZ_L();
  else
    RS485_KZ_H();
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
  {
	  USART_ReceiveData(USART1);
  }

  if (RESET != USART_GetITStatus(USART1, USART_IT_RXNE))
  {
    rx_char = (uint8_t)(USART1->DR & (uint16_t)0x01FFu);

    if (s_usRS485RxStatus & RS485_RX_OK)
      return;

    cur_tick = SYSGetTickCountISR();
    if ((cur_tick > s_ulLastRecvCharTick) && \
       ((cur_tick - s_ulLastRecvCharTick) > RS485_RX_TIMEOUT))
    {
      s_usRS485RxStatus = 0;
    }

    s_ulLastRecvCharTick = cur_tick;

    //if (RS485_RX_START != g_ucRS485RxBuf[0])
    //{
    //  s_usRS485RxStatus = 0;
    //}

    //if ((RS485_RX_END == rx_char) && !s_usRS485RxStatus)
    //{
    //  s_usRS485RxStatus = 0;
    //  return;
    //}

    if ((s_usRS485RxStatus & RS485_RX_SIZE_MASK) >= RS485_MAX_RLEN)
    {
      s_usRS485RxStatus = 0;
    }
    g_ucRS485RxBuf[s_usRS485RxStatus & RS485_RX_SIZE_MASK] = rx_char;
    s_usRS485RxStatus++;

    //if((RS485_RX_TAIL == rx_char) && (s_usRS485RxStatus >= 9))
    //{
    //  RS485DisableRx();
    //  s_usRS485RxStatus |= RS485_RX_OK;
    //}
  }
}
