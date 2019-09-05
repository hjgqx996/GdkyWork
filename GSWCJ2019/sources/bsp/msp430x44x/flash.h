/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY  All Rights Reserved
  *
  * @file     flash.h
  * @author   TianRuidong
  * @version  V1.00
  * @date     2017.04。05
  * @brief    msp430f448 uart board support package.
  ******************************************************************************
  */



#ifndef __FLASH_H
#define __FLASH_H 	


/* INCLUDES ------------------------------------------------------------------- */
#include "includes.h"


/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

#define   FSEG_A_ADD_START     0x1000    /* Flash Segment A 起始地址 */
#define   FSEG_A_ADD_END       0x107F    /* Flash A结束地址 */   
#define   FSEG_B_ADD_START     0x1080    /* Flash Segment B 起始地址 */
#define   FSEG_B_ADD_END       0x10FF    /* Flash B结束地址 */
   
//#define   MEMORY_INFOC_START     0xFA00    /* Flash 程序区 C 起始地址 */
//#define   MEMORY_INFOC_END       0xFBFF    /* Flash 程序区 C结束地址 */ 
#define   MEMORY_INFOC_START     0xFC00    /* Flash 程序区 C 起始地址 */
#define   MEMORY_INFOC_END       0xFDFF    /* Flash 程序区 C结束地址 */ 

#define   MEMORY_INFOD_START     0xFC00    /* Flash 程序区 D 起始地址 */   
#define   MEMORY_INFOD_END       0xFDFF    /* Flash 程序区 D结束地址 */  

#define   CHECK      0x1079   //Flash 的校验位


#define  DeviceNum_add    0x4000    //设备号
#define  Vers_add	  g_cSoftVer     //版本号


#define  DisTempFlag_add  0x1000    //是否显示温度标志 0显示  1不显示
#define  TempNum_add      0x1001    //每条数据中的温度数量
#define  DownDataTime_add     0x1002    //数据下发时间
#define  SignalThreshold_add  0x1004    //信号阈值（高于此值则不重复发送数据）
#define  IP1_add          0x1005    //IP1     24
#define  Port1_add   	  0x101e    //端口1   5
#define  IP2_add          0x1024    //IP2     24
#define  Port2_add   	  0x103D    //端口2   5
#define  PhoneNum_add	  0x1043    //电话号码  11
#define  TxTime_add	  0x104f    //发送温度时间  2
#define  Apn_add   	  0x1052    //APN           15
#define  User_add	  0x1062    //APN用户名     10
#define  Pwd_add	  0x106D    //APN密码       10

#define IMEI_ADD MEMORY_INFOC_START    //IMEI号，存储到程序存储器C
  


/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern unsigned char flash_buf[128];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

void W_flash(unsigned int addr,unsigned char *Data_ptr,unsigned char size);    //写信息存储器
void R_flash(unsigned int addr,unsigned char *Data_ptr,unsigned char size);    //读信息存储器
void flash_clean(unsigned int addr); //擦除flash
void Check_Flash(void);//校验Flash
unsigned char  bidui(unsigned char *p1,unsigned char *p2,unsigned char size);    //比对size个连续数据是否一致

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#endif

