/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 ZouZH  All Rights Reserved
  *
  * @file     paramConfig.h
  * @author   TianRuidong
  * @version  V1.00
  * @date     2017.04.05
  * @brief    
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __PARAM_CONFIG_H
#define __PARAM_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern uint8_t LocalConfigMode;  //本地配置模式标志  
extern uint32_t s_unEnterLocalConfigDoTick;   //进入本地配置时间戳   
extern uint8_t ServerParamChange;    //服务器参数（IP、端口等）变化标志   0、没有更改过     1、更改过
   
/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

//接收配置命令处理   
uint8_t ConfigCommandHandler(uint8_t *RX_command, uint8_t command_length, uint8_t mode);   
void LocalConfigPoll(void);//本地配置轮询
uint8_t ParaParseFromNB(uint8_t *src, uint8_t u8Len);

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __PARAM_CONFIG_H */


/**
 * @}
 */

/***************************** END OF FILE *************************************/



