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

extern uint8_t LocalConfigMode;  //��������ģʽ��־  
extern uint32_t s_unEnterLocalConfigDoTick;   //���뱾������ʱ���   
extern uint8_t ServerParamChange;    //������������IP���˿ڵȣ��仯��־   0��û�и��Ĺ�     1�����Ĺ�
   
/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

//�������������   
uint8_t ConfigCommandHandler(uint8_t *RX_command, uint8_t command_length, uint8_t mode);   
void LocalConfigPoll(void);//����������ѯ
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



