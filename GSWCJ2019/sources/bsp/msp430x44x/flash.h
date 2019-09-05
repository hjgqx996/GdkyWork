/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY  All Rights Reserved
  *
  * @file     flash.h
  * @author   TianRuidong
  * @version  V1.00
  * @date     2017.04��05
  * @brief    msp430f448 uart board support package.
  ******************************************************************************
  */



#ifndef __FLASH_H
#define __FLASH_H 	


/* INCLUDES ------------------------------------------------------------------- */
#include "includes.h"


/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

#define   FSEG_A_ADD_START     0x1000    /* Flash Segment A ��ʼ��ַ */
#define   FSEG_A_ADD_END       0x107F    /* Flash A������ַ */   
#define   FSEG_B_ADD_START     0x1080    /* Flash Segment B ��ʼ��ַ */
#define   FSEG_B_ADD_END       0x10FF    /* Flash B������ַ */
   
//#define   MEMORY_INFOC_START     0xFA00    /* Flash ������ C ��ʼ��ַ */
//#define   MEMORY_INFOC_END       0xFBFF    /* Flash ������ C������ַ */ 
#define   MEMORY_INFOC_START     0xFC00    /* Flash ������ C ��ʼ��ַ */
#define   MEMORY_INFOC_END       0xFDFF    /* Flash ������ C������ַ */ 

#define   MEMORY_INFOD_START     0xFC00    /* Flash ������ D ��ʼ��ַ */   
#define   MEMORY_INFOD_END       0xFDFF    /* Flash ������ D������ַ */  

#define   CHECK      0x1079   //Flash ��У��λ


#define  DeviceNum_add    0x4000    //�豸��
#define  Vers_add	  g_cSoftVer     //�汾��


#define  DisTempFlag_add  0x1000    //�Ƿ���ʾ�¶ȱ�־ 0��ʾ  1����ʾ
#define  TempNum_add      0x1001    //ÿ�������е��¶�����
#define  DownDataTime_add     0x1002    //�����·�ʱ��
#define  SignalThreshold_add  0x1004    //�ź���ֵ�����ڴ�ֵ���ظ��������ݣ�
#define  IP1_add          0x1005    //IP1     24
#define  Port1_add   	  0x101e    //�˿�1   5
#define  IP2_add          0x1024    //IP2     24
#define  Port2_add   	  0x103D    //�˿�2   5
#define  PhoneNum_add	  0x1043    //�绰����  11
#define  TxTime_add	  0x104f    //�����¶�ʱ��  2
#define  Apn_add   	  0x1052    //APN           15
#define  User_add	  0x1062    //APN�û���     10
#define  Pwd_add	  0x106D    //APN����       10

#define IMEI_ADD MEMORY_INFOC_START    //IMEI�ţ��洢������洢��C
  


/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

extern unsigned char flash_buf[128];

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

void W_flash(unsigned int addr,unsigned char *Data_ptr,unsigned char size);    //д��Ϣ�洢��
void R_flash(unsigned int addr,unsigned char *Data_ptr,unsigned char size);    //����Ϣ�洢��
void flash_clean(unsigned int addr); //����flash
void Check_Flash(void);//У��Flash
unsigned char  bidui(unsigned char *p1,unsigned char *p2,unsigned char size);    //�ȶ�size�����������Ƿ�һ��

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


#endif

