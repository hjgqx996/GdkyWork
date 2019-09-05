/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     cmdproc.h
  * @author   ZouZH
  * @version  V1.00
  * @date     25-July-2018
  * @brief    command process routine for NB-IoT GCJJZQ.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __CMDPROC_H
#define __CMDPROC_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ------------------------------------------------------------------- */
#include <stdint.h>

/* TYPEDEFS ------------------------------------------------------------------- */

/**
 * 运行错误代码
 */
typedef enum
{
  CMD_OK = 0x00,
  CMD_ERR_Recv,
  CMD_ERR_Param,
  CMD_ERR_Command,

  CMD_ERR_Malloc,
  CMD_ERR_Record,
  CMD_ERR_Send,
  CMD_ERR_Busy,
  CMD_ERR_Timeout,
  CMD_ERR_Run,
  CMD_ERR_Full,

}CMD_Error_t;

/**
 * 接收命令缓存队列
 */
__packed typedef struct
{
  uint8_t port;   /* 接收数据端口: 0-MBUS, 1-RS485, 2-NB */
  uint16_t len;   /* 接收命令长度 */
  uint8_t *pbuf;  /* 缓存命令数组指针 */
}CmdQueue_t;


/* MACROS  -------------------------------------------------------------------- */

/**
 * 端口序号
 */
#define CMD_PORT_MBUS   0
#define CMD_PORT_RS485  1
#define CMD_PORT_NB     2

/* CONSTANTS  ----------------------------------------------------------------- */

/**
 * 功能码定义
 */
// IAP升级指令
#define CMD_IAP_UPDATE                            ((uint16_t)0x3C12)   /* !<IAP 升级指令 */
#define CMD_IAP_UP_DEV_INFO                       ((uint16_t)0x3C15)   /* !<IAP后第一次启动时上传 */

// 设置命令
#define CMD_GET_HAMS_PARAM                        ((uint16_t)0xCBCC)   /*!< 读取采集计算器参数 */
#define CMD_GET_NB_INFO                           ((uint16_t)0x94A3)   /*!< 读取NB模块网络信息 */

#define CMD_SET_HAMS_ALLOC_FACTOR                 ((uint16_t)0xACBD)   /*!< 设置分摊热量系数 */
#define CMD_GET_HAMS_ALLOC_FACTOR                 ((uint16_t)0xACDD)   /*!< 读取分摊热量系数 */


#define CMD_SET_HAMS_CMD_INTERVAL                 ((uint16_t)0xCDD5)   /*!< 设置每条指令的时间间隔 */

#define CMD_SET_HAMS_PROTOCOL_VER                 ((uint16_t)0x01AD)   /*!< 设置采集计算器协议版本号 */
#define CMD_GET_HAMS_PROTOCOL_VER                 ((uint16_t)0x03B9)   /*!< 读取采集计算器协议版本号 */

#define CMD_SET_ALL_USER_HEAT_SEASON              ((uint16_t)0xDBCA)   /*!< 设置群表采暖季 */
#define CMD_REPLY_SET_ALL_USER_HEAT_SEASON_END    ((uint16_t)0xDBAA)   /*!< 回复设置群表采暖季完毕 */

// 校时命令
#define CMD_SET_TIME_BROADCAST                    ((uint16_t)0xAAAD)   /*!< 广播校时 */
#define CMD_SET_TIME_ALL                          ((uint16_t)0xBDBC)   /*!< 群表校时 */
#define CMD_REPLY_SET_TIME_ALL_END                ((uint16_t)0xBDBB)   /*!< 回复群表校时完成 */

#define CMD_GET_TIME_ALL                          ((uint16_t)0x92C1)   /*!< 群表读时间 */
#define CMD_REPLY_GET_TIME_HAMS                   ((uint16_t)0x92C3)   /*!< 回复集中器时间 */
#define CMD_REPLY_GET_TIME_ALL_END                ((uint16_t)0x07D3)   /*!< 回复群表读时间完成 */


// 初始化命令
#define CMD_INIT_ALL_USER_INFO_START              ((uint16_t)0xDCDC)   /*!< 初始化所有住户信息开始 */
#define CMD_INIT_ALL_USER_INFO_EVERY              ((uint16_t)0xDDDD)   /*!< 初始化每个住户信息 */
#define CMD_INIT_ALL_USER_INFO_END                ((uint16_t)0xDCDB)   /*!< 初始化所有住户信息结束 */
#define CMD_INIT_ONE_USER_INFO                    ((uint16_t)0xDDAA)   /*!< 初始化单个住户信息 */
#define CMD_GET_INIT_ONE_USER_INFO                ((uint16_t)0x03C9)   /*!< 读取初始化单个住户信息 */

// 读取周期数据命令
#define CMD_GET_ALL_USER_METER_CYCLE_DATA         ((uint16_t)0x03C5)   /*!< 读取所有住户热表周期数据 */
#define CMD_REPLY_GET_ALL_USER_METER_CYCLE_DATA   ((uint16_t)0x03C5)   /*!< 回复读取所有住户热表周期数据 */
#define CMD_GET_ALL_USER_METER_CYCLE_DATA_END     ((uint16_t)0x03B2)   /*!< 回复读取所有住户热表周期数据结束 */
#define CMD_GET_ONE_USER_METER_CYCLE_DATA         ((uint16_t)0xEAAA)   /*!< 采集单个住户热表周期数据 */

// 采集实时(Real Time)数据命令
#define CMD_GET_USER_METER_RT_DATA                ((uint16_t)0x03C1)   /*!< 采集户用热表的实时数据 */


/**
 * 命令偏移
 */
#define CMD_OFS_HDR                               (uint8_t)0
#define CMD_OFS_FUNC                              (uint8_t)1
#define CMD_OFS_ID                                (uint8_t)3
#define CMD_OFS_ADDR                              (uint8_t)7
#define CMD_OFS_DATA                              (uint8_t)11

/**
 * 命令错误返回代码
 */
#define CMD_ERR_BIT_PARAM                         (uint8_t)0x01
#define CMD_ERR_BIT_COMMAND                       (uint8_t)0x02
#define CMD_ERR_BIT_RUN                           (uint8_t)0x04
#define CMD_ERR_BIT_TIMEOUT                       (uint8_t)0x10


/**
 * 最大缓存命令条数
 */
#define CMD_RECV_BUF_MAX_NUM                      ((uint8_t)10)

/**
 * 通信波特率
 */
#define CMD_COM_BAUDRATE                          (4800)


/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */
CMD_Error_t CmdInit(void);

CMD_Error_t CmdRecvQueueWriteMBUS(void);
CMD_Error_t CmdRecvQueueWriteRS485(void);
CMD_Error_t CmdRecvQueueWriteNB(void *pdat, uint8_t datlen);

CMD_Error_t CmdRecvQueueWriteCmdToNB(uint16_t cmd);

uint8_t     CmdRecvQueueRead(void);
uint8_t     CmdIsIdle(void);


CMD_Error_t CmdRecvMsgParse(void);
CMD_Error_t CmdSendMsg(uint16_t cmd, uint8_t err, void *pbuf, uint16_t dat);

uint16_t    CmdSendToHost(uint16_t datalen, uint8_t recverr, uint8_t senderr);

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __CMDPROC_H */

/***************************** END OF FILE *************************************/


