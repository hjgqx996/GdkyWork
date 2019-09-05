/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY All Rights Reserved
  *
  * @file     paramConfig.c
  * @author   Tian Ruidong
  * @version  V1.00
  * @date     2017.6.9
  * @brief    �������ó���.
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
 * @brief �������ó���
 * @{
 */


/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

#define FRANSIT_BUF_LENGTH   230

/* CONSTANTS  ----------------------------------------------------------------- */

#define len_comm  194    //command����������ֽ���
const uint8_t command[len_comm]={
"AT+IDNT 1"		 //�豸��
"AT+PHON 2"		 //�绰����
"AT+IPAD 3"		 //IP
"AT+PORT 4"		 //�˿�
"AT+IPSEC 5"		 //����IP
"AT+PTSEC 6"		 //���ݶ˿�
"AT+CONFIGEND 7"         //�˳�����״̬
"AT+TXTIME 8"            //����ʱ��
"AT+TEMP 9"              //�¶�
"AT+VERS a"              //�汾��
"AT+APN b"               //APN
"AT+USER c"              //APN�û���
"AT+PWD d"               //APN����
"AT+TEMPNUM e"           //һ�������а����¶ȸ���
"AT+IMEI f"              //IMEI��
"AT+DNDATIM g"           //�����·�ʱ��
"AT+NOMSIGVAL h"         //�ź�ǿ����ֵ
"AT+CLOCK i"             //ʱ��
"AT+CIMI j"              //SIM����IMSI
};


/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

uint32_t s_unEnterLocalConfigDoTick = 0;   //���뱾������ʱ���

uint8_t Transit_BUF[FRANSIT_BUF_LENGTH];
uint8_t FanHui_counts;
//uint8_t Local_IP[6];

uint8_t LocalConfigMode = 0;  //��������ģʽ��־
uint8_t ServerParamChange = 0;    //������������IP���˿ڵȣ��仯��־   0��û�и��Ĺ�     1�����Ĺ�

uint8_t  Weather = 0, m_ucTemperatureCorrectValue = 0;////????
uint16_t  heating_start = 0, heating_end = 0;////????

/* LOCAL FUNCTIONS ------------------------------------------------------------ */

uint8_t Param_IDNT(uint8_t *s2);      //"AT+IDNT 1"		  //�豸��
uint8_t Param_PHON(uint8_t *s2);      //"AT+PHON 2"		  //�绰����
uint8_t Param_IPAD(uint8_t *s2);      //"AT+IPAD 3"		  //IP
uint8_t Param_PORT(uint8_t *s2);      //"AT+PORT 4"		  //�˿�
uint8_t Param_IPSEC(uint8_t *s2);     //"AT+IPSEC 5"		 //����IP
uint8_t Param_PTSEC(uint8_t *s2);     //"AT+PTSEC 6"		 //���ݶ˿�
uint8_t Param_CONFIGEND(uint8_t *s2, uint8_t mode); //"AT+CONFIGEND 7" //�˳�����״̬
uint8_t Param_TXTIME(uint8_t *s2);    //"AT+TXTIME 8"    //����ʱ��
uint8_t Param_TEMP(uint8_t *s2);      //"AT+TEMP 9"      //�¶�
uint8_t Param_VERS(uint8_t *s2);      //"AT+VERS a"      //�汾��
uint8_t Param_APN(uint8_t *s2);       //"AT+APN b"       //APN
uint8_t Param_USER(uint8_t *s2);      //"AT+USER c"      //APN�û���
uint8_t Param_PWD(uint8_t *s2);       //'AT+PWD d"       //APN����
uint8_t Param_TEMPNUM(uint8_t *s2);   //"AT+TEMPNUM e"   //һ�������а����¶ȸ���
uint8_t Param_IMEI(uint8_t *s2);      //"AT+IMEI f"      //IMEI��
uint8_t Param_DNDATIM(uint8_t *s2);   //"AT+DNDATIM g"   //�����·�ʱ��
uint8_t Param_NOMSIGVAL(uint8_t *s2); //"AT+NOMSIGVAL h" //�ź�ǿ����ֵ
uint8_t Param_CLOCK(uint8_t *s2);     //"AT+CLOCK i"     //ʱ��
uint8_t Param_CIMI(uint8_t *s2);      //"AT+CIMI j"      //SIM����IMSI



/**
 * @brief  ��������, �����ϵ�ʱͨ������0����
 * @param  None
 * @return None
 */
void LocalConfigPoll(void)
{   
#if(DEBUG == 2)   //EEPROM�洢LOG
  uint16_t i;
  uint8_t  u8Tmp = 0;
#endif 
  if(LocalConfigMode == 1)
  {
    if(1 == USART0_CheckReceive())//���յ���Ч���ݰ�////(USART0_RX_STA&0x80)!=0)
    {
      s_unEnterLocalConfigDoTick = SysTickGetSec();   //��¼��������ʱ��
      if(1 == ConfigCommandHandler(g_ucUSART0RxBuf, USART0_GetReceiveLength(), 0))
      {
        LocalConfigMode = 0;  //�˳���������ģʽ��־
        W_flash(FSEG_B_ADD_START,(uint8_t*)FSEG_A_ADD_START,128); //��flashB�б���
      }
      USART0_ClearRxStatus();
    }
    if(SysTickGetSecDif(s_unEnterLocalConfigDoTick, SysTickGetSec()) > 60)//�����ʱ
    {
      LocalConfigMode = 0;  //�˳���������ģʽ��־
      W_flash(FSEG_B_ADD_START,(uint8_t*)FSEG_A_ADD_START,128); //��flashB�б���
    }
  }
  else
  {
    if(1 == USART0_CheckReceive())//���յ���Ч���ݰ�
    {
#if(DEBUG == 2)   //EEPROM�洢LOG
      if((str_str(g_ucUSART0RxBuf, "AT+READLOG", USART0_GetReceiveLength())) != NULL)   //�յ���ȷ����
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
 * @brief  ����NB���յ�������
 * @param[in]  *src   ���ݻ�����
 * @param[in]  u8Len  ���ݳ���
 * @return    1���ɹ��� 0��ʧ�ܣ�
 */
uint8_t ParaParseFromNB(uint8_t *src, uint8_t u8Len) 
{
  uint8_t  *p, *q, i;
  uint8_t  pu8SendBuf[10] = {0};
  
  pu8SendBuf[0] = 0x68;    /* ֡��ʼ�� 68H */
  pu8SendBuf[1] = DEVICE_TYPE;/* �豸���� 0���������������²ɼ���; 1�����������;
                                 2������ʽ���; 3����Ͳ״��ǣ�4�����Ե�����.*/
  pu8SendBuf[2] = 0;
  pu8SendBuf[3] = 0;
  pu8SendBuf[4] = 0;
  pu8SendBuf[5] = 0x00;//(*( (uint8_t *)DeviceNum_add));
  pu8SendBuf[6] = 0x00;//(*( (uint8_t *)DeviceNum_add + 1));
  pu8SendBuf[7] = 0x45;//(*( (uint8_t *)DeviceNum_add + 2));
  pu8SendBuf[8] = 0x76;//(*( (uint8_t *)DeviceNum_add + 3));
  
  p = src;
  q = src;
  if(0 == lib_memcmp(src, pu8SendBuf, 9))      //���յ���λ������
  {
    q = p;
    i = *(q+10);   //���ݳ���
    if(((*(q+9)) == 0x01) && ((*(q+i+12)) == 0x16) && ((*(q+i+11)) == (chk_sum8_hex(q, i+11)) ) ) //��֤����β��У��
    {
      q = p;
      q += 13;
      
      if(*q == g_u8SeqNum)   //������к�һ��,��ͨ������
      {
////        if((SocketInitErrorCode == 31) || (SocketInitErrorCode == 32))
////          SocketInitErrorCode = 0;                  
////        m_ucDataAlreadyReceived = 1;   //���������յ���λ�����ݱ�־
////        s_unGprsOnlinetimeDoTick = GetSysCount();   //��¼GPRS��������ʱ��
        
        q++;
        Weather = *q++;
        
        //                  #if(NB_MODEM != 1)  //�������NBģ��
        //                  {   
        g_tSysTime.year = (*q++);
        g_tSysTime.month = (*q++);
        g_tSysTime.day = (*q++);
        g_tSysTime.hour = (*q++);
        g_tSysTime.minute = (*q++);
        g_tSysTime.second = (*q++);
        g_tSysTime.week = (*q++);  
////        g_ucCalibrationTime = 1;   //��У׼��ʱ���־                  
        //                  }
        //                  #else
        //                  {
        //                    q+=7;
        //                  }
        //                  #endif 
        
        i = (*q++)&0x01;   //�Ƿ���ʾ�¶�
        if(i != (*(unsigned char*)g_u8TempDispEn))
        {
          g_u8TempDispEn = i;
          W_flash(DisTempFlag_add, &i, 1);
        }
        
        m_ucTemperatureCorrectValue = *q++;   //�¶�����ֵ
        
        i = (*q++);
        if((i>=1)&&(i<=12)&&((*q)>=1)&&((*q)<=sucMontable[i-1])) //��������Ƿ�Ϸ�
        {
          heating_start = ((i<<8)|(*q));  //��ů����ʼʱ��                    
        }
        q++;
        
        i = (*q++);
        if((i>=1)&&(i<=12)&&((*q)>=1)&&((*q)<=sucMontable[i-1]))  //��������Ƿ�Ϸ�
        {
          heating_end = ((i<<8)|(*q));  //��ů������ʱ��                    
        }
        q++;
        
////        i = (*q++);
////        if((*q)<24)
////          no_eating_TX_time = ((i<<8)|(*q++));  //�ǲ�ů������ʱ��                  
      }
    }
  }
  
  return 1;
}


/**
 * @brief  "AT+IDNT 1"		 //�豸��
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_IDNT(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i;
  uint8_t pu8Buf[30] = {'+', 'I', 'D', 'N', 'T', ':', 0};
  
  if(*s2=='=')    //�豸�Ź̶�������
  {
    //                                s2++;	
    //                                for(i=0;i<4;i++)
    //                                {
    //                                  pu8Buf[3-i]=((((*(s2+(i*2)))&0x0f)<<4)+((*(s2+(i*2)+1))&0x0f));// �ַ����豸��ת16�����豸��
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
    for(i=0;i<4;i++)    // 16�����豸��ת�ַ����豸��
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
 * @brief  "AT+PHON 2"		 //�绰����
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_PHON(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i;
  uint8_t pu8Buf[30] = {'+', 'P', 'H', 'O', 'N', ':', 0};
  
  if(*s2=='=')
  {
#if(NB_MODEM == 1)  //���ΪNBģ��
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
  if(0)////InSimCard == 0)  //���û�ж�����
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
#if(NB_MODEM == 1)  //���ΪNBģ��
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
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_IPAD(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<15));n++)  {  //����IP��ַ�ĳ���
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<25;n++) {
      pu8Buf[n] = 0X00;   //���IP��ַ����25���ֽڣ���0
    }
    W_flash(IP1_add, pu8Buf, 25);	
    
    ServerParamChange = 1;   //�÷������������ı�־
    
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
 * @brief  "AT+PORT 4"		 //�˿�
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_PORT(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;				
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<5));n++) {   //�������˿ڵĳ���
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<6;n++) {
      pu8Buf[n] = 0X00;   //����˿ڲ���5���ֽڣ���0
    }
    
    W_flash(Port1_add, pu8Buf,6);
    
    ServerParamChange = 1;   //�÷������������ı�־
    
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
 * @brief  "AT+IPSEC 5"		 //����IP
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_IPSEC(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<15)); n++) {   //����IP��ַ�ĳ���
      pu8Buf[n] = *(s2 + n);
    }
    for(; n<25; n++){
      pu8Buf[n] = 0X00;   //���IP��ַ����15���ֽڣ���0
    }
    W_flash(IP2_add, pu8Buf, 25);	
    
    ServerParamChange = 1;   //�÷������������ı�־
    
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
 * @brief  "AT+PTSEC 6"		 //���ݶ˿�
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_PTSEC(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;		 
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<5));n++) {   //���㱸�ݶ˿ڵĳ���
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<6;n++){
      pu8Buf[n] = 0X00;   //������ݶ˿ڲ���5���ֽڣ���0
    }
    W_flash(Port2_add, pu8Buf, 6);
    
    ServerParamChange = 1;   //�÷������������ı�־
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
 * @brief  "AT+CONFIGEND 7"         //�˳�����״̬
 * @param[in]  *s2��������� 
 * @param[in]  mode ��0�������ã�1Զ������
 * @return     0���ɹ���1��ʧ��
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
 * @brief  "AT+TXTIME 8"            //����ʱ��
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_TXTIME(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
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
 * @brief  "AT+TEMP 9"              //�¶�
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_TEMP(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  int32_t temp = 0;
  
  if(*s2=='=')    //�¶�ֻ��
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
    if(temp < 0) {    //����¶�Ϊ��ֵ
      temp = 0 - temp;
      pu8Buf[6] = '-';
    }
    else{
      pu8Buf[6] = 0x20; //�ո�
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
 * @brief  "AT+VERS a"              //�汾��
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_VERS(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')    //�汾��ֻ��
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
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_APN(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<15));n++){  //����APN�ĳ���
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<15;n++){
      pu8Buf[n] = 0X00;   //���APN����10���ֽڣ���0����
    }
    W_flash(Apn_add, pu8Buf, 15);	
    
    ServerParamChange = 1;   //�÷������������ı�־
    
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
 * @brief  "AT+USER c"              //APN�û���
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_USER(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<10));n++) { //�����û����ĳ���
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<10;n++){
      pu8Buf[n] = 0X00;   //����û�������10���ֽڣ���0����
    }
    W_flash(User_add, pu8Buf, 10);	
    
    ServerParamChange = 1;   //�÷������������ı�־
    
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
 * @brief  "AT+PWD d"               //APN����
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_PWD(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')
  {
    s2++;
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<10));n++){  //��������ĳ���
      pu8Buf[n] = *(s2 + n);
    }
    for(;n<10;n++){
      pu8Buf[n] = 0X00;   //������벻��10���ֽڣ���0����
    }
    W_flash(Pwd_add, pu8Buf, 10);	
    
    ServerParamChange = 1;   //�÷������������ı�־
    
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
 * @brief  "AT+TEMPNUM e"           //һ�������а����¶ȸ���
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_TEMPNUM(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  uint16_t  u16Tmp;
  
  if(*s2=='=')
  {
    s2++;
    i = (*s2)&0x0f;   //�¶ȸ���
    
    if((i>0)&&(i<=6))
    {
      R_flash(TxTime_add, pu8Buf, 2);      //��ȡ
      u16Tmp = pu8Buf[0];
      u16Tmp = (u16Tmp<<8)|pu8Buf[1];//����
      
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
 * @brief  "AT+IMEI f"              //IMEI��
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_IMEI(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')    //�豸�Ź̶�������
  {
    //                                s2++;	
    //                                for(i=0;i<4;i++)
    //                                {
    //                                  flash_buf[3-i]=((((*(s2+(i*2)))&0x0f)<<4)+((*(s2+(i*2)+1))&0x0f));// �ַ����豸��ת16�����豸��
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
    for(i=0;i<15;i++)    // 16�����豸��ת�ַ����豸��
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
 * @brief  "AT+DNDATIM g"           //�����·�ʱ��
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_DNDATIM(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t   pu8Buf[30] = {0};
  uint16_t  u16DownDataTime;/* �����·�ʱ�� ��λ������;��Χ:30-1440 */
  uint16_t  u16SendDataTime;/* ���ݷ���ʱ��*/
  
  if(*s2=='=')    //�����·�ʱ��
  {
    s2++;	
    
    for(n=0;((*(s2 + n) !=0x0d)&&(*(s2 + n) !='&')&&(n<4));n++)    //�������ݳ��Ȳ�����
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
 * @brief  "AT+NOMSIGVAL h"         //�ź�ǿ����ֵ
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_NOMSIGVAL(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  
  if(*s2=='=')              //�ź�ǿ����ֵ
  {
    s2++;
    
    if((*(s2+1) ==0x0d)||(*(s2+1) =='&'))
    {
      if(((*(s2)) >= '0') && ((*(s2)) <= '9')){
        i = (*s2)&0x0f;   //�ź�ǿ����ֵ
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
 * @brief  "AT+CLOCK i"             //ʱ��
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_CLOCK(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
  uint8_t pu8Buf[30] = {0};
  SysTime_t clock_temp;
  
  if(*s2=='=')              //ʱ��
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
 * @brief  "AT+CIMI j"              //SIM����IMSI
 * @param[in]  *s2��������� 
 * @return     0���ɹ���1��ʧ��
 */
uint8_t Param_CIMI(uint8_t *s2) 
{
  uint8_t u8Ret = 0;
  uint8_t i, n, *s1;////n�����Ż���
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
 * @brief  ������յ�����������
 * @param[in]  *RX_command��   ��Ҫ�������������洢λ��
 * @param[in]  command_length����Ҫ�������������ĳ���
 * @param[in]  mode��          0�������ã�1Զ������
 * @return     �յ�����������������1�����෵��0��
 */
uint8_t ConfigCommandHandler(uint8_t *RX_command, uint8_t command_length, uint8_t mode) 
{
	uint8_t *s1,*s2,*s3;
	uint8_t comm_num;  //�����
	uint8_t n,i,j; 
  
  FanHui_counts = 0; //���������ֽڸ�����0
  s3 = RX_command;   //���յ�������
  
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
      s1 = (uint8_t*)command;	  //�����
      s2 = s3;
      comm_num = 0;
      for (i=0; i<len_comm; i++)
      { 
        for (n=0; *(s1 + n) == *(s2 + n); n++) 	 //
        { 
          if ((*(s2 + n + 1) == '=')||(*(s2 + n + 1) == '?'))     //ֱ��s2�������ַ����Ƚ���
          {
            comm_num = *(s1+n+2);
            s2 = s2+n+1;
            s3 = s2;
            j = j+n+1;
            i=250;	   //�˳�ѭ��
            break; 			/////////////////////////////
          }
        } 
        s1++; 
      }
      
      switch(comm_num)
      {
      case '1':   
        Param_IDNT(s2); //"AT+IDNT 1"		 //�豸��
        break;
      case '2':   
        Param_PHON(s2);//"AT+PHON 2"		 //�绰����
        break;
      case '3':
        Param_IPAD(s2);//"AT+IPAD 3"		 //IP
        break;
      case '4':  
        Param_PORT(s2);//"AT+PORT 4"		 //�˿�
        break;
      case '5':  
        Param_IPSEC(s2);//"AT+IPSEC 5"		 //����IP
        break;
      case '6':  
        Param_PTSEC(s2);//"AT+PTSEC 6"		 //���ݶ˿�
        break;
      case '7':  
        Param_CONFIGEND(s2, mode);//"AT+CONFIGEND 7"         //�˳�����״̬
        return 1; break;  //�յ�������������1��
      case '8':  
        Param_TXTIME(s2);//"AT+TXTIME 8"            //����ʱ��
        break;                        
      case '9':  
        Param_TEMP(s2);//"AT+TEMP 9"              //�¶�
        break;   
      case 'a':            
        Param_VERS(s2);//"AT+VERS a"              //�汾��  
        break;                          
      case 'b':   
        Param_APN(s2);//"AT+APN b"               //APN 
        break;                    
      case 'c':     
       Param_USER(s2);//"AT+USER c"              //APN�û���  
        break;                    
      case 'd': 
        Param_PWD(s2);//"AT+PWD d"               //APN����
        break;
      case 'e': 
        Param_TEMPNUM(s2);//"AT+TEMPNUM e"           //һ�������а����¶ȸ���
        break;
      case 'f': 
        Param_IMEI(s2);//"AT+IMEI f"              //IMEI��
        break;
      case 'g': 
        Param_DNDATIM(s2);//"AT+DNDATIM g"           //�����·�ʱ��
        break;
      case 'h':  
        Param_NOMSIGVAL(s2);//"AT+NOMSIGVAL h"         //�ź�ǿ����ֵ
        break;
      case 'i':  
        Param_CLOCK(s2);//"AT+CLOCK i"             //ʱ��
        break;
      case 'j':  
        Param_CIMI(s2);//"AT+CIMI j"              //SIM����IMSI
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
////    FanHui_counts = addDataHead(Transit_BUF, FanHui_counts, FRANSIT_BUF_LENGTH);    //�������ӵ���ƽ̨�����Э��ͷ
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




