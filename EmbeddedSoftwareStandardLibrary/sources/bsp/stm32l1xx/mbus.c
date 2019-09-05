/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     mbus.c
  * @author   ZouZH
  * @version  V2.02
  * @date     12-June-2016
  * @brief    stm32l1xx usart3 board support package.
  ******************************************************************************
  */
#define LOG_TAG    "MBUS"

/* INCLUDES ------------------------------------------------------------------- */
#include "mbus.h"
#include "includes.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/**
 * MBUS过载和切换引脚定义控制
 */
#define GPIO_PORT_OV    GPIOB
#define GPIO_PORT_KZ    GPIOC

#define GPIO_PIN_OV     GPIO_Pin_1
#define GPIO_PIN_KZ     GPIO_Pin_4

#define GPIO_CLK_OV     RCC_AHBPeriph_GPIOB
#define GPIO_CLK_KZ     RCC_AHBPeriph_GPIOC

#define MBUS_OV_Read()  GPIO_ReadInputDataBit(GPIO_PORT_OV, GPIO_PIN_OV)

#define MBUS_KZ_H()     GPIO_SetBits(GPIO_PORT_KZ, GPIO_PIN_KZ)
#define MBUS_KZ_L()     GPIO_ResetBits(GPIO_PORT_KZ, GPIO_PIN_KZ)
#define MBUS_KZ_Read()  GPIO_ReadOutputDataBit(GPIO_PORT_KZ, GPIO_PIN_KZ)

/* CONSTANTS  ----------------------------------------------------------------- */

/* MBUS过载保护时间 S */
#define MBUS_PROTECT_TIME 5

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/**
 * 接收数据缓存
 */
uint8_t g_ucMBUSRxBuf[MBUS_MAX_RLEN];

/**
 * 接收数据缓存计数和标志
 */
static volatile uint16_t s_usMBUSRxStatus;

/**
 * 串口最后接收字符时间
 */
static volatile uint32_t s_ulLastRecvCharTick;

/**
 * Mark Space 校验标志
 */
static volatile uint8_t s_ucMBUSMarkFlag = 0;


/**
 * 是否使能和保护
 */
static volatile uint8_t s_ucMBUSIsEnable = 0;
static volatile uint8_t s_ucMBUSIsProtect = 0;


/* LOCAL FUNCTIONS ------------------------------------------------------------ */


/**
 * @brief  初始化串口
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
uint8_t MBUSInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  s_ucMBUSMarkFlag = 0;
  s_ucMBUSIsProtect = 0;
  s_ucMBUSIsEnable = 0;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB | GPIO_CLK_OV | GPIO_CLK_KZ, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  /* MBUS-OV input low level valid */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_OV;
  GPIO_Init(GPIO_PORT_OV, &GPIO_InitStructure);

  /* MBUS-KZ output High level valid */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KZ;
  GPIO_Init(GPIO_PORT_KZ, &GPIO_InitStructure);
  MBUSPower(0);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  /* USART3-Tx  AF_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* USART3-Rx AF_PP */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  USART_DeInit(USART3);
  USART_InitStructure.USART_BaudRate = 4800;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_OverSampling8Cmd(USART3, DISABLE);

  USART_Init(USART3, &USART_InitStructure);

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART3, ENABLE);
  USART_GetFlagStatus(USART3, USART_FLAG_TC);

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
uint8_t MBUSSetParam(uint32_t baud, uint8_t parity)
{
  USART_InitTypeDef USART_InitStructure;

  s_ucMBUSMarkFlag = 0;

  USART_DeInit(USART3);
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
    s_ucMBUSMarkFlag = 1;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
  }

  USART_OverSampling8Cmd(USART3, DISABLE);

  USART_Init(USART3, &USART_InitStructure);

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART3, ENABLE);
  USART_GetFlagStatus(USART3, USART_FLAG_TC);

  return 0;
}

/**
 * @brief  过载保护检测
 *
 * @param  无
 *
 * @retval 0 成功，其他失败
 */
void MBUSProtectPoll(void)
{
  static uint32_t protectTime;

  if (!s_ucMBUSIsEnable)
    return;

  if (!MBUS_OV_Read() && MBUS_KZ_Read())
  {
    s_ucMBUSIsProtect = 1;
    log_w("MBUS Overload!!!");
    MBUS_KZ_L();
    protectTime = SYSGetTickCount();
  }

  if (!MBUS_KZ_Read() && (SYSGetTickCount() - protectTime > \
      SYS_MS_TO_TICKS(SYSMakeMs(0, MBUS_PROTECT_TIME, 0))))
  {
    s_ucMBUSIsProtect = 0;
    log_w("MBUS Restart...");
    MBUS_KZ_H();
  }

  if (!s_ucMBUSIsEnable)
    MBUS_KZ_L();
}

/**
 * @brief  发送串口数据包
 *
 * @param  pbuf 数据包缓存
 * @param  len  数据包长度
 *
 * @retval 实际发送数据长度
 */
uint16_t MBUSSend(const uint8_t *pbuf, uint16_t len)
{
  uint16_t tmout = 0;
  uint16_t len_tmp = 0;

  if((pbuf == NULL) || (len == 0))
    return 0;

  len_tmp = len;

  // MBUS短路保护状态检查
  if (s_ucMBUSIsProtect || !MBUS_KZ_Read())
    return 0;

  while(len_tmp)
  {
    if ((len_tmp == len) && s_ucMBUSMarkFlag)
      USART3->DR = *pbuf | (1 << 8);
    else
      USART3->DR = *pbuf;

    tmout = 0xFFFF;
    while((USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) && tmout--)

    if (!tmout)
      break;

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
uint8_t MBUSCheckReceive(void)
{
  uint8_t ret = 0;

  SYSEnterCritical();

  if ((SYSGetTickCount() - s_ulLastRecvCharTick > SYS_MS_TO_TICKS(MBUS_RX_TIMEOUT)) \
     && (s_usMBUSRxStatus & MBUS_RX_SIZE_MASK))
  {
    s_usMBUSRxStatus |= MBUS_RX_OK;
    ret = 1;
  }
  else if (s_usMBUSRxStatus & MBUS_RX_OK)
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
uint16_t MBUSGetReceiveLength(void)
{
  return (s_usMBUSRxStatus & MBUS_RX_SIZE_MASK);
}


/**
 * @brief  获取串口接收最后字符时间
 *
 * @param  无
 *
 * @retval 串口接收最后字符时间
 */
uint32_t MBUSGetLastRecvCharTick(void)
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
void MBUSEnableRx(void)
{
  SYSEnterCritical();
  
#ifdef USE_FULL_ASSERT
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  if (MBUSCheckReceive() && (MBUSGetReceiveLength() <= MBUS_MAX_RLEN))
  {
    uint16_t i = 0;
    for (i = 0; i < (s_usMBUSRxStatus & MBUS_RX_SIZE_MASK); i++)
    {
      g_ucMBUSRxBuf[i] = 0;
    }
  }
#endif

  s_usMBUSRxStatus = 0;
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

  SYSExitCritical();
}


/**
 * @brief  关闭串口数据接收
 *
 * @param  无
 *
 * @retval 无
 */
void MBUSDisableRx(void)
{
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
}

/**
 * @brief  MBUS电源控制
 *
 * @param  0 or 1
 *
 * @retval 无
 */
void MBUSPower(uint8_t onoff)
{
  if (s_ucMBUSIsProtect && onoff)
    return;
    
  if (!s_ucMBUSIsEnable && onoff)
    log_i("MBUS Power On!");

  if (s_ucMBUSIsEnable && !onoff)
    log_i("MBUS Power Off!");

  s_ucMBUSIsEnable = onoff;
  s_ucMBUSIsProtect = 0;
  
  if (onoff)
    MBUS_KZ_H();
  else
    MBUS_KZ_L();
}


/**
 * @brief  串口中断处理程序
 *
 * @param  无
 *
 * @retval 无
 */
void USART3_IRQHandler(void)
{
  uint8_t rx_char = 0;
  uint32_t cur_tick = 0;

  if (USART_GetITStatus(USART3, USART_IT_ORE_RX) != RESET)
  {
	  USART_ReceiveData(USART3);
  }

  if (RESET != USART_GetITStatus(USART3, USART_IT_RXNE))
  {
    rx_char = (uint8_t)(USART3->DR & (uint16_t)0x01FFu);

    if (s_usMBUSRxStatus & MBUS_RX_OK)
      return;

    cur_tick = SYSGetTickCountISR();
    if ((cur_tick > s_ulLastRecvCharTick) && \
       ((cur_tick - s_ulLastRecvCharTick) > MBUS_RX_TIMEOUT))
    {
      s_usMBUSRxStatus = 0;
    }

    s_ulLastRecvCharTick = cur_tick;

    //if (MBUS_RX_START != g_ucMBUSRxBuf[0])
    //{
    //  s_usMBUSRxStatus = 0;
    //}

    //if ((MBUS_RX_END == rx_char) && !s_usMBUSRxStatus)
    //{
    //  s_usMBUSRxStatus = 0;
    //  return;
    //}

    if ((s_usMBUSRxStatus & MBUS_RX_SIZE_MASK) >= MBUS_MAX_RLEN)
    {
      s_usMBUSRxStatus = 0;
    }
    g_ucMBUSRxBuf[s_usMBUSRxStatus & MBUS_RX_SIZE_MASK] = rx_char;
    s_usMBUSRxStatus++;

    //if((MBUS_RX_TAIL == rx_char) && (s_usMBUSRxStatus >= 9))
    //{
    //  MBUSDisableRx();
    //  s_usMBUSRxStatus |= MBUS_RX_OK;
    //}
  }
}

