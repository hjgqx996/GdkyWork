/**
  ******************************************************************************
  *               Copyright(C) 2016-2026 GDKY  All Rights Reserved
  *
  * @file     flash.c
  * @author   TianRuidong
  * @version  V1.00
  * @date     2017.04.05
  * @brief    msp430f448 uart board support package.
  ******************************************************************************
  */


/* INCLUDES ------------------------------------------------------------------- */
#include "includes.h"
#include "flash.h"
////#include "usart.h"
////#include "universalLib.h"
#include "in430.h"

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */

/* CONSTANTS  ----------------------------------------------------------------- */

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */


unsigned char flash_buf[128];


void W_flash(unsigned int addr,unsigned char *Data_ptr,unsigned char size)    //д��Ϣ�洢��
{
  unsigned char  *write_ptr;
  unsigned char   w;

  if((addr>=FSEG_A_ADD_START)&&(addr<=FSEG_A_ADD_END))
  {
  	write_ptr=(unsigned char*)FSEG_A_ADD_START;  
        R_flash(FSEG_A_ADD_START,flash_buf,128);        
  }
  else if((addr>=FSEG_B_ADD_START)&&(addr<=FSEG_B_ADD_END))
  {
  	write_ptr=(unsigned char*)FSEG_B_ADD_START;   
        R_flash(FSEG_B_ADD_START,flash_buf,128);        
  }
  else if((addr>=MEMORY_INFOC_START)&&(addr<=MEMORY_INFOC_END))
  {
  	write_ptr=(unsigned char*)MEMORY_INFOC_START;   
        R_flash(MEMORY_INFOC_START,flash_buf,128);        
  }
  else if((addr>=MEMORY_INFOD_START)&&(addr<=MEMORY_INFOD_END))
  {
  	write_ptr=(unsigned char*)MEMORY_INFOD_START;   
        R_flash(MEMORY_INFOD_START,flash_buf,128);        
  }
  else
  {
    return;
  }
	
  for(w=0;w<size;w++)
  	*(flash_buf+(addr-(unsigned int)write_ptr)+w) = *(Data_ptr+w);
  
  flash_buf[127] = chk_sum8_dec(flash_buf,127);  //У��λ
  
  _DINT();                  //������
  FCTL1 = 0x0A502;          /* ERASE = 1 */
  FCTL3 = 0x0A500;          /* Lock = 0 */
  *write_ptr=0;        /* erase Flash segment */
  FCTL1 = 0x0A540;          /* WRT = 1 */
  for( w=0; w<128; w++ )
   {
     *write_ptr++=flash_buf[w];
   } 
  FCTL1 = 0x0A500;          /* ERASE,WRITE = 0 ��ȫ����*/
  FCTL3 = 0x0A510;          /* Lock = 1 */
  _EINT(); 
}     


void R_flash(unsigned int addr,unsigned char *Data_ptr,unsigned char size)    //����Ϣ�洢��
{
  unsigned char  *read_ptr;
  unsigned char   w;
 
  read_ptr = (unsigned char *) addr;
  for( w=0; w<size; w++ )
   {
     *Data_ptr++=*read_ptr++;
   } 
}


void flash_clean(unsigned int addr)
{
  unsigned char  *write_ptr,w,*p;
  const unsigned char *IP = INIT_IP;  //ע��IP��ַΪ15�ֽڲ��������ո�
  const unsigned char *PORT = INIT_PORT;
  const unsigned char *TELNUM = INIT_TELNUM;
  const unsigned int TxTime = INIT_TX_TIME;
  const unsigned char *APN = INIT_APN;
  const unsigned char *USER = INIT_USER;
  const unsigned char *PWD = INIT_PWD;  

  p = flash_buf;
  for( w=0; w<128; w++ )   //flash��0
       *p++ = 0;
  
  flash_buf[0] = DIS_TEMP;
  flash_buf[1] = TEMP_NUM;
  
  flash_buf[2] = DOWN_DATA_TIME>>8;
  flash_buf[3] = DOWN_DATA_TIME&0xff;
  flash_buf[4] = SIGNAL_THRESHOLD;

  p = (unsigned char *)IP;
  write_ptr = &flash_buf[5];
  
  for( w=0; ((*p!=0)&&(w<24)); w++ )   //IP
      *write_ptr++ = *p++;
  for( ; w<25; w++ )
      *write_ptr++ = 0;
  
  p = (unsigned char *)PORT;  
  for( w=0; ((*p!=0)&&(w<5)); w++ )    //port
      *write_ptr++ = *p++;
  for(; w<5; w++ )    //port
      *write_ptr++ = 0;  
  
  *write_ptr++ = 0;
  
  p = (unsigned char *)IP;  
  for( w=0; ((*p!=0)&&(w<24)); w++ )   //IP
      *write_ptr++ = *p++;
  for( ; w<25; w++ )
      *write_ptr++ = 0;
  
  p = (unsigned char *)PORT;    
  for( w=0; ((*p!=0)&&(w<5)); w++ )    //port
      *write_ptr++ = *p++;
  for(; w<5; w++ )    //port
      *write_ptr++ = 0;  
  *write_ptr++ = 0;
  
  for( w=0; w<11; w++ )  //�绰����
      *write_ptr++ = *TELNUM++;
  *write_ptr++ = 0; 
  
  *write_ptr++ = (TxTime>>8);
  *write_ptr++ = (TxTime&0xff);    //����ʱ��
  
  *write_ptr++ = 0; 
  
  p = (unsigned char *)APN;
  for( w=0; *p!=0; w++ )  //APN
      *write_ptr++ = *p++;
  for( ; w<16; w++ )
      *write_ptr++ = 0;  

  p = (unsigned char *)USER;
  for( w=0; *p!=0; w++ )  //�û���
      *write_ptr++ = *p++;
  for( ; w<11; w++ )
      *write_ptr++ = 0;   
  
  p = (unsigned char *)PWD;
  for( w=0; *p!=0; w++ )  //����
      *write_ptr++ = *p++;
  for( ; w<11; w++ )
      *write_ptr++ = 0;  

  flash_buf[127] = chk_sum8_dec(flash_buf,127);
   
  _DINT();              //�����ж�    
  write_ptr = (unsigned char *) addr;//��ʼ��ָ��
  FCTL1 = 0x0A502;          /* ERASE = 1 */  //����������
  FCTL3 = 0x0A500;          /* Lock = 0 */   //flash����
  *write_ptr=0;        /* erase Flash segment */  
  FCTL1 = 0x0A540;          /* WRT = 1 */
  write_ptr = (unsigned char *) addr;
 
  p = flash_buf;
  for( w=0; w<128; w++ )   
      *write_ptr++ = *p++;
  
  FCTL1 = 0x0A500;          /* ERASE,WRITE = 0 ��ȫ����*/
  FCTL3 = 0x0A510;          /* Lock = 1 */
  _EINT();       //�����ж�  
  
}


void Check_Flash(void)//У��Flash
{
  
  _DINT();
  
  R_flash(FSEG_A_ADD_START,flash_buf,128);//��flashA��tt 
  if(bidui((unsigned char *)FSEG_A_ADD_START,(unsigned char *)FSEG_B_ADD_START,128) == 0) //�����ͬ
  {   
          if((flash_buf[127] != chk_sum8_dec(flash_buf,127))||(flash_buf[0] == 0xff)||(flash_buf[1] == 0xff)||(flash_buf[126] == 0xff)||(flash_buf[127] == 0xff))    //���flashA����
          {
                flash_clean(FSEG_A_ADD_START);
                flash_clean(FSEG_B_ADD_START);   //����flash
          }      
  }
  else
  {
      if((flash_buf[127] != chk_sum8_dec(flash_buf,127))||(flash_buf[0] == 0xff)||(flash_buf[1] == 0xff)||(flash_buf[126] == 0xff)||(flash_buf[127] == 0xff))    //���flashA����
      {
            R_flash(FSEG_B_ADD_START,flash_buf,128);//��flashB��tt  
            if((flash_buf[127] != chk_sum8_dec(flash_buf,127))||(flash_buf[0] == 0xff)||(flash_buf[1] == 0xff)||(flash_buf[126] == 0xff)||(flash_buf[127] == 0xff))    //���flashB����
            {
                  flash_clean(FSEG_A_ADD_START);
                  flash_clean(FSEG_B_ADD_START);   //����flash                   
            }
            else
            {
                  W_flash(FSEG_A_ADD_START,(unsigned char *)FSEG_B_ADD_START,128);            
            }
      }
      else
      { 
            W_flash(FSEG_B_ADD_START,(unsigned char *)FSEG_A_ADD_START,128);
      }
  } 
  _EINT();
}


unsigned char  bidui(unsigned char *p1,unsigned char *p2,unsigned char size)    //�ȶ�size�����������Ƿ�һ��
{
  unsigned char w,RD_OK;
  RD_OK=0;
  for(w=0;w<size;w++)  
  {
    if(*p1==*p2)
    {
         p1++;
         p2++;
         RD_OK=0;
    }
    else
    {
         RD_OK=1;
         break;
    }
  } 
  return RD_OK;  
}











