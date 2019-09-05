/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     paramConfig.c
  * @author   Tian Ruidong
  * @version  V1.00
  * @date     2017.6.9
  * @brief    参数配置程序.
  ******************************************************************************
  * @history
  */

/* INCLUDES ------------------------------------------------------------------- */
#include "includes.h"
#include "paramConfig.h"
#include "usart0.h"
#include "TemperatureMeasure.h"
#include "nb.h"
#include "systime.h"
#include "flash.h"
#include "record.h"
#include "record_app.h"

/**
 * @defgroup paramConfig
 * @brief 参数配置程序
 * @{
 */


/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

#define FRANSIT_BUF_LENGTH   230

/* CONSTANTS  ----------------------------------------------------------------- */

#define len_comm  194    //command数组的所有字节数
const uint8_t command[len_comm]={
"AT+IDNT 1"		 //设备号
"AT+PHON 2"		 //电话号码
"AT+IPAD 3"		 //IP
"AT+PORT 4"		 //端口
"AT+IPSEC 5"		 //备份IP
"AT+PTSEC 6"		 //备份端口
"AT+CONFIGEND 7"         //退出配置状态
"AT+TXTIME 8"            //发送时间
"AT+TEMP 9"              //温度
"AT+VERS a"              //版本号
"AT+APN b"               //APN
"AT+USER c"              //APN用户名
"AT+PWD d"               //APN密码
"AT+TEMPNUM e"           //一条命令中包含温度个数
"AT+IMEI f"              //IMEI号
"AT+DNDATIM g"           //数据下发时间
"AT+NOMSIGVAL h"         //信号强度阈值
"AT+CLOCK i"             //时钟
"AT+CIMI j"              //SIM卡的IMSI
};


/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

uint32_t s_unEnterLocalConfigDoTick = 0;   //进入本地配置时间戳

uint8_t Transit_BUF[FRANSIT_BUF_LENGTH];
uint8_t FanHui_counts;
//uint8_t Local_IP[6];

uint8_t LocalConfigMode = 0;  //本地配置模式标志
uint8_t ServerParamChange = 0;    //服务器参数（IP、端口等）变化标志   0、没有更改过     1、更改过

uint8_t  Weather = 0, m_ucTemperatureCorrectValue = 0;////????
uint16_t  heating_start = 0, heating_end = 0;////????

/* LOCAL FUNCTIONS ------------------------------------------------------------ */

uint8_t Param_IDNT(uint8_t *s2);      //"AT+IDNT 1"		  //设备号
uint8_t Param_PHON(uint8_t *s2);      //"AT+PHON 2"		  //电话号码
uint8_t Param_IPAD(uint8_t *s2);      //"AT+IPAD 3"		  //IP
uint8_t Param_PORT(uint8_t *s2);      //"AT+PORT 4"		  //端口
uint8_t Param_IPSEC(uint8_t *s2);     //"AT+IPSEC 5"		 //备份IP
uint8_t Param_PTSEC(uint8_t *s2);     //"AT+PTSEC 6"		 //备份端口
uint8_t Param_CONFIGEND(uint8_t *s2, uint8_t mode); //"AT+CONFIGEND 7" //退出配置状态
uint8_t Param_TXTIME(uint8_t *s2);    //"AT+TXTIME 8"    //发送时间
uint8_t Param_TEMP(uint8_t *s2);      //"AT+TEMP 9"      //温度
uint8_t Param_VERS(uint8_t *s2);      //"AT+VERS a"      //版本号
uint8_t Param_APN(uint8_t *s2);       //"AT+APN b"       //APN
uint8_t Param_USER(uint8_t *s2);      //"AT+USER c"      //APN用户名
uint8_t Param_PWD(uint8_t *s2);       //'AT+PWD d"       //APN密码
uint8_t Param_TEMPNUM(uint8_t *s2);   //"AT+TEMPNUM e"   //一条命令中包含温度个数
uint8_t Param_IMEI(uint8_t *s2);      //"AT+IMEI f"      //IMEI号
uint8_t Param_DNDATIM(uint8_t *s2);   //"AT+DNDATIM g"   //数据下发时间
uint8_t Param_NOMSIGVAL(uint8_t *s2); //"AT+NOMSIGVAL h" //信号强度阈值
uint8_t Param_CLOCK(uint8_t *s2);     //"AT+CLOCK i"     //时钟
uint8_t Param_CIMI(uint8_t *s2);      //"AT+CIMI j"      //SIM卡的IMSI



/**
 * @brief  本地配置, 重新上电时通过串口0配置
 * @param  None
 * @return None
 */
void LocalConfigPoll(void)
{   
#if(DEBUG == 2)   //EEPROM存储LOG
  uint16_t i;
  uint8_t  u8Tmp = 0;
#endif 
  if(LocalConfigMode == 1)
  {
    if(1 == USART0_CheckReceive())//接收到有效数据包////(USART0_RX_STA&0x80)!=0)
    {
      s_unEnterLocalConfigDoTick = SysTickGetSec();   //记录本地配置时间
      if(1 == ConfigCommandHandler(g_ucUSART0RxBuf, USART0_GetReceiveLength(), 0))
      {
        LocalConfigMode = 0;  //退出本地配置模式标志
        W_flash(FSEG_B_ADD_START,(uint8_t*)FSEG_A_ADD_START,128); //在flashB中备份
      }
      USART0_ClearRxStatus();
    }
    if(SysTickGetSecDif(s_unEnterLocalConfigDoTick, SysTickGetSec()) > 60)//如果超时
    {
      LocalConfigMode = 0;  //退出本地配置模式标志
      W_flash(FSEG_B_ADD_START,(uint8_t*)FSEG_A_ADD_START,128); //在flashB中备份
    }
  }
  else
  {
    if(1 == USART0_CheckReceive())//接收到有效数据包
    {
#if(DEBUG == 2)   //EEPROM存储LOG
      if((str_str(g_ucUSART0RxBuf, "AT+READLOG", USART0_GetReceiveLength())) != NULL)   //收到正确命令
      {
        for(i=LOG_STO== USART0_CheckReceiveRAGE_LOCATION_ADD; i<=LOG_END_ADD; i++)
        {
          u8Tmp = 1;//AT24CXX_ReadOneByte(i)
          USART0_Send(u8Tmp, 1);
        }
      }
#endif
      USART0_ClearRxStatus();
    }
  }
}

/**
 * @brief  解析NB接收到的数据
 * @param[in]  *src   数据缓冲区
 * @param[in]  u8Len  数据长度
 * @return    1：成功， 0：失败；
 */
uint8_t ParaParseFromNB(uint8_t *src, uint8_t u8Len) 
{
  uint8_t  *p, *q, i;
  uint8_t  pu8SendBuf[10] = {0};
  
  pu8SendBuf[0] = 0x68;    /* 帧起始符 68H */
  pu8SendBuf[1] = DEVICE_TYPE;/* 设备类型 0：无线自组网室温采集器; 1：万年历外壳;
                                 2：插座式外壳; 3：笔筒状外壳；4：碱性电池外壳.*/
  pu8SendBuf[2] = 0;
  pu8SendBuf[3] = 0;
  pu8SendBuf[4] = 0;
  pu8SendBuf[5] = 0x00;//(*( (uint8_t *)DeviceNum_add));
  pu8SendBuf[6] = 0x00;//(*( (uint8_t *)DeviceNum_add + 1));
  pu8SendBuf[7] = 0x45;//(*( (uint8_t *)DeviceNum_add + 2));
  pu8SendBuf[8] = 0x76;//(*( (uint8_t *)DeviceNum_add + 3));
  
  p = src;
  q = src;
  if(0 == lib_memcmp(src, pu8SendBuf, 9))      //接收到上位机数据
  {
    q = p;
    i = *(q+10);   //数据长度
    if(((*(q+9)) == 0x01) && ((*(q+i+12)) == 0x16) && ((*(q+i+11)) == (chk_sum8_hex(q, i+11)) ) ) //验证数据尾、校验
    {
      q = p;
      q += 13;
      
      if(*q == g_u8SeqNum)   //如果序列号一致,则通信正常
      {
////        if((SocketInitErrorCode == 31) || (SocketInitErrorCode == 32))
////          SocketInitErrorCode = 0;                  
////        m_ucDataAlreadyReceived = 1;   //置周期中收到上位机数据标志
////        s_unGprsOnlinetimeDoTick = GetSysCount();   //记录GPRS接收数据时间
        
        q++;
        Weather = *q++;
        
        //                  #if(NB_MODEM != 1)  //如果不是NB模块
        //                  {   
        g_tSysTime.year = (*q++);
        g_tSysTime.month = (*q++);
        g_tSysTime.day = (*q++);
        g_tSysTime.hour = (*q++);
        g_tSysTime.minute = (*q++);
        g_tSysTime.second = (*q++);
        g_tSysTime.week = (*q++);  
////        g_ucCalibrationTime = 1;   //置校准过时间标志                  
        //                  }
        //                  #else
        //                  {
        //                    q+=7;
        //                  }
        //                  #endif 
        
        i = (*q++)&0x01;   //是否显示温度
        if(i != (*(unsigned char*)g_u8TempDispEn))
        {
          g_u8TempDispEn = i;
          W_flash(DisTempFlag_add, &i, 1);
        }
        
        m_ucTemperatureCorrectValue = *q++;   //温度修正值
        
        i = (*q++);
        if((i>=1)&&(i<=12)&&((*q)>=1)&&((*q)<=sucMontable[i-1])) //检测数据是否合法
        {
          heating_start = ((i<<8)|(*q));  //采暖季开始时间                    
        }
        q++;
        
        i = (*q++);
        if((i>=1)&&(i<=12)&&((*q)>=1)&&((*q)<=sucMontable[i-1]))  //检测数据是否合法
        {
          heating_end = ((i<<8)|(*q));  //采暖季结束时间                    
        }
        q++;
        
////        i = (*q++);
////        if((*q)<24)
////          no_eating_TX_time = ((i<<8)|(*q++));  //非采暖季发送时间                  
      }
    }
  }
  
  return 1;
}


/**
 * @brief  "AT+IDNT 1"		 //设备号
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_IDNT(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i;
  uint8_t pu8Buf[30] = {'+', 'I', 'D', 'N', 'T', ':', 0};
  
  if(*s2=='=')    //设备号固定不处理
  {
    //                                s2++;	
    //                                for(i=0;i<4;i++)
    //                                {
    //                                  pu8Buf[3-i]=((((*(s2+(i*2)))&0x0f)<<4)+((*(s2+(i*2)+1))&0x0f));// 字符串设备号转16进制设备号
    //                                }
    //                                W_flash(DeviceNum_add,pu8Buf,4);
    //                                
    //                                if(mode == 1)
    //                                    GPRS_TX_Date("OK\r\n",4);
    //                                else
    //                                    Usart0SendDat("OK\r\n",4);
  }
  else if(*s2=='?')
  {
    for(i=0;i<4;i++)    // 16进制设备号转字符串设备号
    {
      pu8Buf[6+(i*2)] = (((*((uint8_t *)DeviceNum_add+i))>>4)+0x30);
      pu8Buf[7+(i*2)] = (((*((uint8_t *)DeviceNum_add+i))&0x0f)+0x30);  
    }
    pu8Buf[14]='\r';
    pu8Buf[15]='\n';     
    
    for(i=0; i<16; i++)
    {
      Transit_BUF[FanHui_counts++] = pu8Buf[i]; 
      if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
      {
        FanHui_counts = (FRANSIT_BUF_LENGTH-3);
      }
    }
  }
  return u8Ret;
}

/**
 * @brief  "AT+PHON 2"		 //电话号码
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_PHON(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i;
  uint8_t pu8Buf[30] = {'+', 'P', 'H', 'O', 'N', ':', 0};
  
  if(*s2=='=')
  {
#if(NB_MODEM == 1)  //如果为NB模块
    {
      s2++;								
     W_flash(PhoneNum_add, s2, 11);
     lib_memcpy(SimCardNum, s2, 11);
    }
#endif
    
    //                                          s1 = "+PHON:OK\r\n";
    //                                          for(i=0;i<10;i++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++;     
  }
  //                                  else if(*s2=='?')
  //                                  {
  if(0)////InSimCard == 0)  //如果没有读出卡
  {
    pu8Buf[6] = '0'; 
    pu8Buf[7] = '\r';
    pu8Buf[8] = '\n'; 
    
    for(i=0;i<9;i++)
    {
      Transit_BUF[FanHui_counts++] = pu8Buf[i];    
      if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
      {
        FanHui_counts = (FRANSIT_BUF_LENGTH-3);
      }
    }
  }
  else
  {
#if(NB_MODEM == 1)  //如果为NB模块
    {
      for(i=6;i<26;i++) {
        pu8Buf[i] = SimCardNum[i-6]; 
      }
      pu8Buf[26] = '\r';
      pu8Buf[27] = '\n';                                               
      for(i=0;i<28;i++)
      {
        Transit_BUF[FanHui_counts++] = pu8Buf[i];    
        if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
        {
          FanHui_counts = (FRANSIT_BUF_LENGTH-3);
        }
      } 
    }
#else
    {
      s1 = (unsigned char*)PhoneNum_add;
      for(i=6;i<17;i++) {
        pu8Buf[i] = *s1++; ////pu8PhoneNum[i-6]; 
      }
      pu8Buf[17] = '\r';
      pu8Buf[18] = '\n';                                               
      for(i=0;i<19;i++)
      {
        Transit_BUF[FanHui_counts++] = pu8Buf[i];    
        if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
        {
          FanHui_counts = (FRANSIT_BUF_LENGTH-3);
        }
      }                                               
    }
#endif
  }
  return u8Ret;
}

/**
 * @brief  "AT+IPAD 3"		 //IP
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_IPAD(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<15));n++)  {  //计算IP地址的长度
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<25;n++) {
      pu8Buf[n] = 0X00;   //如果IP地址不够25个字节，补0
    }
    W_flash(IP1_add, pu8Buf, 25);	
    
    ServerParamChange = 1;   //置服务器参数更改标志
    
    //                                          s1 = "+IPAD:OK\r\n";
    //                                          for(n=0;n<10;n++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++; 
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+IPAD:"; 
  for(i=0;i<6;i++) {
    pu8Buf[i] = *s1++;
  }
  s1 = (uint8_t*)IP1_add;
  for(i=6;((i<21)&&(*s1!=0x00));i++) {
    pu8Buf[i] = *s1++;
  }
  pu8Buf[i++] = '\r';
  pu8Buf[i++] = '\n';  
  
  for(n=0;n<i;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];   
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  // 
  return u8Ret;
}

/**
 * @brief  "AT+PORT 4"		 //端口
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_PORT(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;				
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<5));n++) {   //计算主端口的长度
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<6;n++) {
      pu8Buf[n] = 0X00;   //如果端口不够5个字节，补0
    }
    
    W_flash(Port1_add, pu8Buf,6);
    
    ServerParamChange = 1;   //置服务器参数更改标志
    
    //                                          s1 = "+PORT:OK\r\n";
    //                                          for(n=0;n<10;n++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++;  
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+PORT:"; 
  for(i=0;i<6;i++) {
    pu8Buf[i] = *s1++;   
  }                           
  s1 = (uint8_t*)Port1_add;
  for(i=6;((i<11)&&(*s1 != 0));i++) {
    pu8Buf[i] = *s1++;
  }
  pu8Buf[i++] = '\r';
  pu8Buf[i++] = '\n';  
  
  for(n=0;n<i;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n]; 
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  //      
  
  return u8Ret;
}

/**
 * @brief  "AT+IPSEC 5"		 //备份IP
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_IPSEC(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<15)); n++) {   //计算IP地址的长度
      pu8Buf[n] = *(s2 + n);
    }
    for(; n<25; n++){
      pu8Buf[n] = 0X00;   //如果IP地址不够15个字节，补0
    }
    W_flash(IP2_add, pu8Buf, 25);	
    
    ServerParamChange = 1;   //置服务器参数更改标志
    
    //                                          s1 = "+IPSEC:OK\r\n";
    //                                          for(n=0;n<11;n++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++; 
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+IPSEC:"; 
  for(i=0;i<7;i++){
    pu8Buf[i] = *s1++;                            
  }
  s1 = (uint8_t*)IP2_add;
  for(i=7;((i<22)&&(*s1!=0x00));i++){
    pu8Buf[i] = *s1++;
  }
  pu8Buf[i++] = '\r';
  pu8Buf[i++] = '\n';    
  
  for(n=0;n<i;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n]; 
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  //   
  return u8Ret;
}

/**
 * @brief  "AT+PTSEC 6"		 //备份端口
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_PTSEC(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;		 
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<5));n++) {   //计算备份端口的长度
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<6;n++){
      pu8Buf[n] = 0X00;   //如果备份端口不够5个字节，补0
    }
    W_flash(Port2_add, pu8Buf, 6);
    
    ServerParamChange = 1;   //置服务器参数更改标志
    //
    //                                          s1 = "+PTSEC:OK\r\n";
    //                                          for(n=0;n<11;n++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++;                                          
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+PTSEC:"; 
  for(i=0;i<7;i++){
    pu8Buf[i] = *s1++;                          
  }
  s1 = (uint8_t*)Port2_add;
  for(i=7;((i<12)&&(*s1 != 0));i++){
    pu8Buf[i] = *s1++;
  }
  pu8Buf[i++] = '\r';
  pu8Buf[i++] = '\n';
  
  for(n=0;n<i;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];  
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  //  
  return u8Ret;
}

/**
 * @brief  "AT+CONFIGEND 7"         //退出配置状态
 * @param[in]  *s2：命令缓冲区 
 * @param[in]  mode ：0本地配置；1远程配置
 * @return     0；成功；1；失败
 */
uint8_t Param_CONFIGEND(uint8_t *s2, uint8_t mode) 
{
  uint8_t u8Ret = 0;
  
  if(mode == 1)
  {
    //   GPRS_TX_Date("+Exit Config:Restart...\r\n",25);                                      
  }
  else 
  {
    USART0_Send("+Exit Config:OK!\r\n", 18);                        
  }
  
  return u8Ret;
}

/**
 * @brief  "AT+TXTIME 8"            //发送时间
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_TXTIME(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  uint32_t TxTime_T;
  
  if(*s2=='=')    
  {
    for(i=0;i<5;i++)
    {
      s2++;
      if(((*s2) == '\r')||((*s2) == '&'))
      {
        break;
      }
    }
    s2 -= i;
    
    TxTime_T = 0;
    switch(i)
    {
    case 4: TxTime_T += (((*s2++)&0x0f)*1000);
    case 3: TxTime_T += (((*s2++)&0x0f)*100);
    case 2: TxTime_T += (((*s2++)&0x0f)*10);
    case 1: TxTime_T += ((*s2)&0x0f);                                          
    default: break;
    }
    
    if(TxTime_T>=1440){
      TxTime_T=1440;
    }
    else if(TxTime_T<30){
      TxTime_T=30;
    }
    //                                          else if(i == 0)
    //                                            i=1;
    
    for(;((1440%TxTime_T) != 0)&&(TxTime_T<=1440); TxTime_T++);
    
    if(TxTime_T>=1440){
      TxTime_T=1440;
    }
    else if(TxTime_T<30){
      TxTime_T=30;
    }
#if(NB_MODEM == 1)
    {
      if(TxTime_T<60)
      {
        TxTime_T = 60;
////        break;
      }
    }
#endif
    
    pu8Buf[0] = (TxTime_T>>8);
    pu8Buf[1] = (TxTime_T&0xff);
    
    W_flash(TxTime_add, pu8Buf, 2);     
    g_u8SendInterval = TxTime_T;
    
    //                                          s1 = "+TXTIME:OK\r\n";
    //                                          for(n=0;n<12;n++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++;                                          
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+TXTIME:"; 
  for(i=0;i<8;i++){
    pu8Buf[i] = *s1++;     
  }
  TxTime_T = *(uint8_t*)TxTime_add;
  TxTime_T = ((TxTime_T<<8)|(*(uint8_t*)(TxTime_add+1)));
  
  pu8Buf[8] = ((TxTime_T%10000/1000)+0x30);
  pu8Buf[9] = ((TxTime_T%1000/100)+0x30);  
  pu8Buf[10] = ((TxTime_T%100/10)+0x30);
  pu8Buf[11] = ((TxTime_T%10)+0x30);
  pu8Buf[12] = '\r';
  pu8Buf[13] = '\n'; 
  
  for(n=0;n<14;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];      
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  //   
  
  return u8Ret;
}

/**
 * @brief  "AT+TEMP 9"              //温度
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_TEMP(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  int32_t temp = 0;
  
  if(*s2=='=')    //温度只读
  {
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+TEMP:"; 
  for(i=0;i<6;i++){
    pu8Buf[i] = *s1++;
  }
  if(T_able == 0)
  {
    temp = Hdc1080_GetTempVal();
    if(temp < 0) {    //如果温度为负值
      temp = 0 - temp;
      pu8Buf[6] = '-';
    }
    else{
      pu8Buf[6] = 0x20; //空格
    }
    pu8Buf[7] = (temp%10000/1000+0x30);
    pu8Buf[8] = (temp%1000/100+0x30);
    pu8Buf[9] = '.';
    pu8Buf[10] = (temp%100/10+0x30);
  }
  else
  {
    pu8Buf[6] = 'E';
    pu8Buf[7] = 'R';
    pu8Buf[8] = 'R';
    pu8Buf[9] = 'O';
    pu8Buf[10] = 'R';   
  }
  
  pu8Buf[11] = '\r';
  pu8Buf[12] = '\n';
  
  for(n=0;n<13;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];  
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  //   
  return u8Ret;
}

/**
 * @brief  "AT+VERS a"              //版本号
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_VERS(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')    //版本号只读
  {
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+VERS:"; 
  for(i=0;i<6;i++){
    pu8Buf[i] = *s1++;
  }
  pu8Buf[6] = ((g_cSoftVer[0]>>4)   + 0x30);
  pu8Buf[7] = ((g_cSoftVer[0]&0x0f) + 0x30);
  pu8Buf[8] = ((g_cSoftVer[1]>>4)   + 0x30);
  pu8Buf[9] = ((g_cSoftVer[1]&0x0f) + 0x30);
  pu8Buf[10] = '\r';
  pu8Buf[11] = '\n';    
  
  for(n=0;n<12;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n]; 
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  //          
  return u8Ret;
}

/**
 * @brief  "AT+APN b"               //APN
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_APN(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<15));n++){  //计算APN的长度
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<15;n++){
      pu8Buf[n] = 0X00;   //如果APN不够10个字节，用0补齐
    }
    W_flash(Apn_add, pu8Buf, 15);	
    
    ServerParamChange = 1;   //置服务器参数更改标志
    
    //                                          s1 = "+APN:OK\r\n";
    //                                          for(n=0;n<9;n++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++;                                          
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+APN:"; 
  for(i=0;i<5;i++){
    pu8Buf[i] = *s1++;
  }
  s1 = (uint8_t*)Apn_add;
  for(i=0;((i<15)&&(*s1!=0x00));i++){
    pu8Buf[i+5] = *s1++;
  }
  pu8Buf[i+5] = '\r';
  pu8Buf[i+6] = '\n';
  
  for(n=0;n<(i+7);n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];   
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  //    
  return u8Ret;
}

/**
 * @brief  "AT+USER c"              //APN用户名
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_USER(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<10));n++) { //计算用户名的长度
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<10;n++){
      pu8Buf[n] = 0X00;   //如果用户名不够10个字节，用0补齐
    }
    W_flash(User_add, pu8Buf, 10);	
    
    ServerParamChange = 1;   //置服务器参数更改标志
    
    //                                          s1 = "+USER:OK\r\n";
    //                                          for(n=0;n<10;n++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++;                                          
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+USER:"; 
  for(i=0;i<6;i++){
    pu8Buf[i] = *s1++;    
  }
  s1 = (uint8_t*)User_add;
  for(i=0;((i<10)&&(*s1!=0x00));i++){
    pu8Buf[i+6] = *s1++;
  }
  pu8Buf[i+6] = '\r';
  pu8Buf[i+7] = '\n';
  
  for(n=0;n<(i+8);n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];   
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  //   
  return u8Ret;
}

/**
 * @brief  "AT+PWD d"               //APN密码
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_PWD(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<10));n++){  //计算密码的长度
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<10;n++){
      pu8Buf[n] = 0X00;   //如果密码不够10个字节，用0补齐
    }
    W_flash(Pwd_add, pu8Buf, 10);	
    
    ServerParamChange = 1;   //置服务器参数更改标志
    
    //                                          s1 = "+PWD:OK\r\n";
    //                                          for(n=0;n<9;n++)
    //                                              Transit_BUF[FanHui_counts++] = *s1++;                                          
  }
  //                                  else if(*s2=='?')
  //                                  {
  s1 = "+PWD:"; 
  for(i=0;i<5;i++){
    pu8Buf[i] = *s1++;       
  }                       
  s1 = (uint8_t*)Pwd_add;
  for(i=0;((i<10)&&(*s1!=0x00));i++){
    pu8Buf[i+5] = *s1++;
  }
  pu8Buf[i+5] = '\r';
  pu8Buf[i+6] = '\n';    
  
  for(n=0; n<(i+7); n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];    
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  
  return u8Ret;
}

/**
 * @brief  "AT+TEMPNUM e"           //一条命令中包含温度个数
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_TEMPNUM(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  uint16_t  u16Tmp;
  
  if(*s2=='=')
  {
    s2++;
    i = (*s2)&0x0f;   //温度个数
    
    if((i>0)&&(i<=6))
    {
      R_flash(TxTime_add, pu8Buf, 2);      //读取
      u16Tmp = pu8Buf[0];
      u16Tmp = (u16Tmp<<8)|pu8Buf[1];//发送
      
      if((u16Tmp/10) < i)
      {
        i = u16Tmp/10;
        W_flash(TempNum_add, &i, 1);
      }
      else
      {
        W_flash(TempNum_add, &i, 1);
      }
    } 
  }
  s1 = "+TEMPNUM:";
  for(i=0;i<9;i++){
    pu8Buf[i] = *s1++;
  }
  pu8Buf[9] = (*(uint8_t*)TempNum_add)|0x30;
  pu8Buf[10] = '\r';
  pu8Buf[11] = '\n';
  
  for(n=0;n<12;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  return u8Ret;
}

/**
 * @brief  "AT+IMEI f"              //IMEI号
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_IMEI(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')    //设备号固定不处理
  {
    //                                s2++;	
    //                                for(i=0;i<4;i++)
    //                                {
    //                                  flash_buf[3-i]=((((*(s2+(i*2)))&0x0f)<<4)+((*(s2+(i*2)+1))&0x0f));// 字符串设备号转16进制设备号
    //                                }
    //                                W_flash(DeviceNum_add,flash_buf,4);
    //                                
    //                                if(mode == 1)
    //                                    GPRS_TX_Date("OK\r\n",4);
    //                                else
    //                                    Usart0SendDat("OK\r\n",4);
  }
  else if(*s2=='?')
  {
    s1 = "+IMEI:"; 
    for(i=0;i<6;i++){
      pu8Buf[i] = *s1++;
    }
    for(i=0;i<15;i++)    // 16进制设备号转字符串设备号
    {
      pu8Buf[6+i] = (nbobj.imei[i]);                                       
    }
    pu8Buf[21]='\r';
    pu8Buf[22]='\n';
    
    for(n=0;n<23;n++)
    {
      Transit_BUF[FanHui_counts++] = pu8Buf[n];   
      if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
      {
        FanHui_counts = (FRANSIT_BUF_LENGTH-3);
      }
    }
  }
  return u8Ret;
}

/**
 * @brief  "AT+DNDATIM g"           //数据下发时间
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_DNDATIM(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t   pu8Buf[30] = {0};
  uint16_t  u16DownDataTime;/* 数据下发时间 单位：分钟;范围:30-1440 */
  uint16_t  u16SendDataTime;/* 数据发送时间*/
  
  if(*s2=='=')    //数据下发时间
  {
    s2++;	
    
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<4));n++)    //计算数据长度并保存
    {
      if(((*(s2 + n)) >= '0') && ((*(s2 + n)) <= '9'))
      {
        pu8Buf[10+n] = *(s2 + n);
      }
      else
      {
        n = 20;
      }
    }
    
    if(n == 20)
    {
      u8Ret = 1;
      return u8Ret;
    }
    else
    {
      switch(n)
      {
      case 1: pu8Buf[3] = pu8Buf[9+n]; pu8Buf[2] = 0;           pu8Buf[1] = 0;           pu8Buf[0] = 0;break;
      case 2: pu8Buf[3] = pu8Buf[9+n]; pu8Buf[2] = pu8Buf[8+n]; pu8Buf[1] = 0;           pu8Buf[0] = 0;break;
      case 3: pu8Buf[3] = pu8Buf[9+n]; pu8Buf[2] = pu8Buf[8+n]; pu8Buf[1] = pu8Buf[7+n]; pu8Buf[0] = 0;break;
      case 4: pu8Buf[3] = pu8Buf[9+n]; pu8Buf[2] = pu8Buf[8+n]; pu8Buf[1] = pu8Buf[7+n]; pu8Buf[0] = pu8Buf[6+n];break;
      default: n = 100;break;
      }
    }
    
    if(n == 100){
      u8Ret = 1;
      return u8Ret;
    }
    u16DownDataTime = (pu8Buf[0]&0x0f)*1000 + (pu8Buf[1]&0x0f)*100 + (pu8Buf[2]&0x0f)*10 + (pu8Buf[3]&0x0f);
    
    if(u16DownDataTime <= 1440)
    {
      u16SendDataTime = (*(uint8_t*)TxTime_add);
      u16SendDataTime = ((u16SendDataTime<<8)|(*(uint8_t*)(TxTime_add+1)));                                            
      
      if((u16DownDataTime % u16SendDataTime) != 0)
      {
        u16DownDataTime = (u16DownDataTime/u16SendDataTime)*u16SendDataTime;
      }   
      pu8Buf[0] = u16DownDataTime>>8;
      pu8Buf[1] = u16DownDataTime;
      W_flash(DownDataTime_add, pu8Buf, 2);
      
      g_u16DownDataTime = u16DownDataTime;
    }      
  }
  s1 = "+DNDATIM:"; 
  for(i=0;i<9;i++){
    pu8Buf[i] = *s1++;
  }
  u16DownDataTime = (*(uint8_t*)DownDataTime_add);
  u16DownDataTime = ((u16DownDataTime<<8) | (*(uint8_t*)(DownDataTime_add+1))); 
  
  pu8Buf[9] = ((u16DownDataTime%10000/1000)|0x30);
  pu8Buf[10] = ((u16DownDataTime%1000/100)|0x30);  
  pu8Buf[11] = ((u16DownDataTime%100/10)|0x30);
  pu8Buf[12] = ((u16DownDataTime%10)|0x30);
  pu8Buf[13] = '\r';
  pu8Buf[14] = '\n'; 
  
  for(n=0;n<15;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];   
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  return u8Ret;
}

/**
 * @brief  "AT+NOMSIGVAL h"         //信号强度阈值
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_NOMSIGVAL(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')              //信号强度阈值
  {
    s2++;
    
    if((*(s2+1) ==0x0d)||(*(s2+1) =='&'))
    {
      if(((*(s2)) >= '0') && ((*(s2)) <= '9')){
        i = (*s2)&0x0f;   //信号强度阈值
      }
      W_flash(SignalThreshold_add, &i, 1);
    }
    else if((*(s2+2) ==0x0d)||(*(s2+2) =='&'))
    {
      if(((*(s2)) >= '0') && ((*(s2)) <= '9') && ((*(s2+1)) >= '0') && ((*(s2+1)) <= '9')){
        i = ((*s2)&0x0f) *10 + (*(s2+1)&0x0f);
      }
      if(i<=32){
        W_flash(SignalThreshold_add, &i, 1);
      }
    }
    else
    {
      
    }
  }
  s1 = "+NOMSIGVAL:";
  for(i=0;i<11;i++){
    pu8Buf[i] = *s1++;
  }
  i = (*(uint8_t*)SignalThreshold_add);
  pu8Buf[11] = (i%100/10)|0x30;
  pu8Buf[12] = (i%10)|0x30;
  pu8Buf[13] = '\r';
  pu8Buf[14] = '\n';
  
  for(n=0;n<15;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  return u8Ret;
}

/**
 * @brief  "AT+CLOCK i"             //时钟
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_CLOCK(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  SysTime_t clock_temp;
  
  if(*s2=='=')              //时钟
  {
    if((*(s2+13) ==0x0d)||(*(s2+13) =='&'))
    {
      clock_temp.year =   ((*(s2+1))&0x0f)*10 + ((*(s2+2))&0x0f);
      clock_temp.month =  ((*(s2+3))&0x0f)*10 + ((*(s2+4))&0x0f);
      clock_temp.day =    ((*(s2+5))&0x0f)*10 + ((*(s2+6))&0x0f);
      clock_temp.hour =   ((*(s2+7))&0x0f)*10 + ((*(s2+8))&0x0f);
      clock_temp.minute = ((*(s2+9))&0x0f)*10 + ((*(s2+10))&0x0f);
      clock_temp.second = ((*(s2+11))&0x0f)*10 + ((*(s2+12))&0x0f);
      if(   (clock_temp.year < 100)  && (clock_temp.month > 0) 
         && (clock_temp.month < 13)  && (clock_temp.day > 0) 
         && (clock_temp.day < 32)    && (clock_temp.hour < 24) 
         && (clock_temp.minute < 60) && (clock_temp.second < 60))
      {
        g_tSysTime.year   = clock_temp.year;
        g_tSysTime.month  = clock_temp.month;
        g_tSysTime.day    = clock_temp.day;
        g_tSysTime.hour   = clock_temp.hour;
        g_tSysTime.minute = clock_temp.minute;
        g_tSysTime.second = clock_temp.second;                                            
      }
    }
    else
    {
      
    }
  }
  s1 = "+CLOCK:";
  for(i=0;i<7;i++){
    pu8Buf[i] = *s1++;
  }
  pu8Buf[7] = (g_tSysTime.year%100/10)|0x30;
  pu8Buf[8] = (g_tSysTime.year%10)|0x30;
  pu8Buf[9] = (g_tSysTime.month%100/10)|0x30;
  pu8Buf[10] = (g_tSysTime.month%10)|0x30;
  pu8Buf[11] = (g_tSysTime.day%100/10)|0x30;
  pu8Buf[12] = (g_tSysTime.day%10)|0x30;
  pu8Buf[13] = (g_tSysTime.hour%100/10)|0x30;
  pu8Buf[14] = (g_tSysTime.hour%10)|0x30;
  pu8Buf[15] = (g_tSysTime.minute%100/10)|0x30;
  pu8Buf[16] = (g_tSysTime.minute%10)|0x30;
  pu8Buf[17] = (g_tSysTime.second%100/10)|0x30;
  pu8Buf[18] = (g_tSysTime.second%10)|0x30;
  pu8Buf[19] = '\r';
  pu8Buf[20] = '\n';
  
  for(n=0;n<21;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  return u8Ret;
}

/**
 * @brief  "AT+CIMI j"              //SIM卡的IMSI
 * @param[in]  *s2：命令缓冲区 
 * @return     0；成功；1；失败
 */
uint8_t Param_CIMI(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n可以优化掉
  uint8_t pu8Buf[30] = {0};
  
  s1 = "+CIMI:";
  for(i=0;i<6;i++){
    pu8Buf[i] = *s1++;
  }
  s1 = (uint8_t *)nbobj.imsi;
  for(i=6;i<21;i++)
  {
    pu8Buf[i] = *s1++;
  }
  pu8Buf[21] = '\r';
  pu8Buf[22] = '\n';
  
  for(n=0;n<23;n++)
  {
    Transit_BUF[FanHui_counts++] = pu8Buf[n];
    if(FanHui_counts >= FRANSIT_BUF_LENGTH-3)
    {
      FanHui_counts = (FRANSIT_BUF_LENGTH-3);
    }
  }
  return u8Ret;
}

/**
 * @brief  处理接收到的配置命令
 * @param[in]  *RX_command：   需要处理的配置命令存储位置
 * @param[in]  command_length：需要处理的配置命令的长度
 * @param[in]  mode：          0本地配置；1远程配置
 * @return     收到配置完成重启命令返回1；其余返回0；
 */
uint8_t ConfigCommandHandler(uint8_t *RX_command, uint8_t command_length, uint8_t mode) 
{
	uint8_t *s1,*s2,*s3;
	uint8_t comm_num;  //命令号
	uint8_t n,i,j; 
  
  FanHui_counts = 0; //返回命令字节个数清0
  s3 = RX_command;   //接收到的命令
  
  for(;((*s3!='A')&&(command_length>0));command_length--)
  {
    s3++;
  }
  
  for(j=0;(*s3!=0x0d)&&(j<command_length);j++)
  {
    if((*s3++ == '&')||(j==0))
    {
      if(j==0) {
        s3--;
      }
      s1 = (uint8_t*)command;	  //命令库
      s2 = s3;
      comm_num = 0;
      for (i=0; i<len_comm; i++)
      { 
        for (n=0; *(s1 + n) == *(s2 + n); n++) 	 //
        { 
          if ((*(s2 + n + 1) == '=')||(*(s2 + n + 1) == '?'))     //直到s2中所有字符串比较完
          {
            comm_num = *(s1+n+2);
            s2 = s2+n+1;
            s3 = s2;
            j = j+n+1;
            i=250;	   //退出循环
            break; 			/////////////////////////////
          }
        } 
        s1++; 
      }
      
      switch(comm_num)
      {
      case '1':   
        Param_IDNT(s2); //"AT+IDNT 1"		 //设备号
        break;
      case '2':   
        Param_PHON(s2);//"AT+PHON 2"		 //电话号码
        break;
      case '3':
        Param_IPAD(s2);//"AT+IPAD 3"		 //IP
        break;
      case '4':  
        Param_PORT(s2);//"AT+PORT 4"		 //端口
        break;
      case '5':  
        Param_IPSEC(s2);//"AT+IPSEC 5"		 //备份IP
        break;
      case '6':  
        Param_PTSEC(s2);//"AT+PTSEC 6"		 //备份端口
        break;
      case '7':  
        Param_CONFIGEND(s2, mode);//"AT+CONFIGEND 7"         //退出配置状态
        return 1; break;  //收到配置完成命令返回1。
      case '8':  
        Param_TXTIME(s2);//"AT+TXTIME 8"            //发送时间
        break;                        
      case '9':  
        Param_TEMP(s2);//"AT+TEMP 9"              //温度
        break;   
      case 'a':            
        Param_VERS(s2);//"AT+VERS a"              //版本号  
        break;                          
      case 'b':   
        Param_APN(s2);//"AT+APN b"               //APN 
        break;                    
      case 'c':     
       Param_USER(s2);//"AT+USER c"              //APN用户名  
        break;                    
      case 'd': 
        Param_PWD(s2);//"AT+PWD d"               //APN密码
        break;
      case 'e': 
        Param_TEMPNUM(s2);//"AT+TEMPNUM e"           //一条命令中包含温度个数
        break;
      case 'f': 
        Param_IMEI(s2);//"AT+IMEI f"              //IMEI号
        break;
      case 'g': 
        Param_DNDATIM(s2);//"AT+DNDATIM g"           //数据下发时间
        break;
      case 'h':  
        Param_NOMSIGVAL(s2);//"AT+NOMSIGVAL h"         //信号强度阈值
        break;
      case 'i':  
        Param_CLOCK(s2);//"AT+CLOCK i"             //时钟
        break;
      case 'j':  
        Param_CIMI(s2);//"AT+CIMI j"              //SIM卡的IMSI
        break;
      default:break;
      }
   
      if(FanHui_counts >= 200)
      {
        if(mode == 1)
        {
////          Gprs_TxDataStructure.pData = Transit_BUF;
////          Gprs_TxDataStructure.DataNum = FanHui_counts;
////          Gprs_TxDataStructure.Escape = GPRS_DATA_ESCAPE;
////          Gprs_TxDataStructure.TestAck = 1;
////          Gprs_TxDataStructure.EnSleep = 1;
////          
////          StartGprsModemTxDate(&Gprs_TxDataStructure);                          
        }
        else
        {
          USART0_Send(Transit_BUF, FanHui_counts);                                                
        }
        FanHui_counts = 0;
      }
    }
  }
  
  if(mode == 1)
  {
////#if(USER_AGREEMENT == 4)
////    FanHui_counts = addDataHead(Transit_BUF, FanHui_counts, FRANSIT_BUF_LENGTH);    //增加增加电信平台插件的协议头
////#endif           
////    Gprs_TxDataStructure.pData = Transit_BUF;
////    Gprs_TxDataStructure.DataNum = FanHui_counts;
////    Gprs_TxDataStructure.Escape = GPRS_DATA_ESCAPE;
////    Gprs_TxDataStructure.TestAck = 1;
////    Gprs_TxDataStructure.EnSleep = 1;
////    
////    StartGprsModemTxDate(&Gprs_TxDataStructure);      
  }
  else
  {
    USART0_Send(Transit_BUF, FanHui_counts);                                                
  }
  
  return 0;
}




/**
 * @}
 */




