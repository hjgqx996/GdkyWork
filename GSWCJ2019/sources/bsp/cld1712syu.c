/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     cld1712syu.c
  * @author   scf
  * @version  V1.00
  * @date     21-May-2019
  * @brief    LCD driver for cld1712syu.
  ******************************************************************************
  * @history
  */

/* INCLUDES ----------------------------------------------------------------- */
#include "includes.h"
#include "cld1712syu.h"
#include "lcd.h"
#include "systime.h"
#include "TemperatureMeasure.h"
#include "HDC1080.h" 
#include "record.h"
#include "nb.h"
#include "PowerManage.h"
#include "nb_app.h"
//#include "userAgreement.h"

/**
 * @defgroup cld1712syu
 * @brief cld1712syu��Һ��������
 * @{
 */

/* TYPEDEFS ------------------------------------------------------------------- */

/* MACROS  -------------------------------------------------------------------- */


//��������˵��
//����λ��ʾ�Ĵ����е�λ��
//��5λLCDMEM�Ĵ�����

#define FH1         (((0)<<3)|0)     //��������
#define FH2         (((1)<<3)|3)     //���߷���
#define FH3         (((3)<<3)|3)     //ʧ����־
#define FH4         (((3)<<3)|4)     //���ű�־
#define FH5         (((3)<<3)|5)     //���ձ�־
#define FH6         (((3)<<3)|6)     //�͵��־
#define FH7         (((0)<<3)|1)     //����
#define FH8         (((0)<<3)|2)     //�趨
#define FH9         (((0)<<3)|3)     //����-
#define FH10        (((2)<<3)|3)     //�¶�С����
#define FH11         (((3)<<3)|7)     //���϶ȷ���
#define FH12         (((10)<<3)|4)     //ʪ�ȷ���
#define FH13         (((18)<<3)|6)     //����
#define FH14         (((18)<<3)|7)     //ģʽ
#define FH15         (((17)<<3)|4)     //T1
#define FH16         (((15)<<3)|4)     //��ͨʱ��
#define FH17         (((14)<<3)|4)     //ʱ��
#define FH18         (((13)<<3)|4)     //T2
#define FH19         (((11)<<3)|4)     //�Ͽ�ʱ��
#define FH20         (((18)<<3)|5)     //����
#define FH21         (((18)<<3)|4)     //���
#define FH22         (((10)<<3)|3)     //KW��h
#define FH23         (((10)<<3)|2)     //MJ
#define FH24         (((10)<<3)|1)     //Сʱ
#define FH25         (((10)<<3)|0)     //��λ��־
#define FH26         (((5)<<3)|7)     //����
#define FH27         (((7)<<3)|7)     //ʱ��ð��
#define FH28         (((16)<<3)|4)     //T1С����
#define FH29         (((12)<<3)|4)     //T2С����
#define _12BC        (((4)<<3)|7)     //12BC
#define _16B         (((6)<<3)|7)     //16B
#define _16C         (((8)<<3)|7)     //16C
#define _16AGED      (((9)<<3)|7)     //16AGED

//�ַ���λ��
#define CHAR_POS_CODE_NUM_1   1//code number 1
#define CHAR_POS_CODE_NUM_2   2//code number 2
#define CHAR_POS_CODE_NUM_3   3//code number 3
#define CHAR_POS_CODE_NUM_4   4//code number 4
#define CHAR_POS_CODE_NUM_5   5//code number 5
#define CHAR_POS_CODE_NUM_6   6//code number 6
#define CHAR_POS_CODE_NUM_7   7//code number 7
#define CHAR_POS_CODE_NUM_8   8//code number 8
#define CHAR_POS_TEMP_TENTH   9//temperature��tenth' digit
#define CHAR_POS_TEMP_UINTS   10//temperature, units
#define CHAR_POS_TEMP_TENS    11//temperature, tens
#define CHAR_POS_MONTH_UNITS  13//month, units
#define CHAR_POS_DAY_TENS     14//day, tens
#define CHAR_POS_DAY_UNITS    15//day, uints
#define CHAR_POS_HOUR_UNITS   17//hour, uints
#define CHAR_POS_MIN_TENS    18//minitu, tens
#define CHAR_POS_MIN_UNITS    19//minitu, units


/* CONSTANTS  ----------------------------------------------------------------- */


/*
  a
f   b
  g
h   c
  d
*/
#define CHAR_MAX_NUM  25    //��ʾ�ַ��������
const uint8_t Cld1712_TAB[CHAR_MAX_NUM] =
{
0xD7,  // "0" /
0x06,  // "1" /
0xE3,  // "2" /
0xA7,  // "3" /
0x36,  // "4" /
0xB5,  // "5" /
0xF5,  // "6" /
0x07,  // "7" /
0xF7,  // "8" /
0xB7,  // "9" /
0x77,  // "A" /10
0xF4,  // "B" /11
0xD1,  // "C" /12
0xE6,  // "D" /13
0xF1,  // "E" /14
0x71,  // "F" /15
0x20,  // "-" / 16
0x73,  // "P" / 17
0x64,  // "N" / 18
0x60,  // "r" / 19
0xD0,   //"L"  /20
0x00,   // "�ַ�����" / 21
0xe4,   //"С0" /22
0x76,   //"H"  /23
0xC4    //"u"  /24
};

/* GLOBAL VARIABLES ----------------------------------------------------------- */

/* GLOBAL FUNCTIONS ----------------------------------------------------------- */

/* LOCAL VARIABLES ------------------------------------------------------------ */

/* LOCAL FUNCTIONS ------------------------------------------------------------ */
void Cld1712_ClrStaticItems(void); //Clear static items
void Cld1712_Page1(void);          //display page 1.//ʱ�ӡ��豸��
void Cld1712_Page2(void);          //display page 2.//Э��汾��
void Cld1712_Page3(void);          //display page 3.//����汾�š� GPRSģ��汾�š�EEPROM������롢�������в��衢GPRS���д������
void Cld1712_Page4(void);          //display page 4.//IMEI low 8 bytes 
void Cld1712_Page5(void);          //display page 5.//IMEI hith 7 bytes
void Cld1712_Page6(void);          //display page 6.//IMSI low 8 bytes
void Cld1712_Page7(void);          //display page 7.//IMSI high 7 bytes
void Cld1712_DispTemperature(void);//display Temperature
void Cld1712_DispOnlineFlag(void); //�����߱�־
  



/**
 * @brief  Init Cld1712_
 * @param  void
 * @retval None
 */
void Cld1712_Init(void)
{
  BTCTL  = BT_fLCD_512;
  LCDCTL = LCD4MUX + LCDSG0_7+LCDON;     //Һ����ʾ  
  P5SEL  = 0xFF;  //�ڶ�����
  P4SEL |= 0xF0;
}

/**
 * @brief  clear Cld1712syu
 * @param  void
 * @retval None
 */
void Cld1712_ClearLcd(void)
{
  LCDCTL &= ~ LCDON; //Һ����ʾ���� 
  uint8_t j;
  for(j=0;j<19;j++) {      
    LCDMEM[j] = 0x00;
  }
  LCDCTL |= LCDON; //Һ����ʾ����
}

/**
 * @brief  Cld1712_ show all items.
 * @param  void
 * @retval None
 */
void Cld1712_ShowAll(void)
{
  LCDCTL &= ~ LCDON; //Һ����ʾ���� 
  uint8_t j;
  for(j=0;j<19;j++) {      
    LCDMEM[j] = 0xff;
  }
  LCDCTL |= LCDON; //Һ����ʾ����
}

/**
 * @brief  Cld1712syu display character
 * @param[in] c:   character to be displayed, 0-17.
 * @param[in] pos: position of character.
 * @retval None
 */
void Cld1712_PutChar(uint8_t c, uint8_t pos)
{
  if((pos > 20) || (pos ==0 ) ||(c > (CHAR_MAX_NUM-1)))		//λ��Խ����ַ��Ƿ�ʱֱ�ӷ���
  {
    return;	
  }
  switch(pos)
  {
  case CHAR_POS_CODE_NUM_1://code number 1
    LCDM18 &= 0x1f;
    LCDM19 &= 0xf0;
    LCDM18 |= ((Cld1712_TAB[c]&0x01)<<7)+((Cld1712_TAB[c]&0x02)<<5)+((Cld1712_TAB[c]&0x04)<<3);
    LCDM19 |= ((Cld1712_TAB[c]&0x80)>>7)+((Cld1712_TAB[c]&0x40)>>5)+((Cld1712_TAB[c]&0x20)>>3)+((Cld1712_TAB[c]&0x10)>>1);
    break;
  case CHAR_POS_CODE_NUM_2://code number 2
    LCDM17 &= 0x1f;
    LCDM18 &= 0xf0;
    LCDM17 |= ((Cld1712_TAB[c]&0x01)<<7)+((Cld1712_TAB[c]&0x02)<<5)+((Cld1712_TAB[c]&0x04)<<3);
    LCDM18 |= ((Cld1712_TAB[c]&0x80)>>7)+((Cld1712_TAB[c]&0x40)>>5)+((Cld1712_TAB[c]&0x20)>>3)+((Cld1712_TAB[c]&0x10)>>1);
    break;
  case CHAR_POS_CODE_NUM_3://code number 3
    LCDM16 &= 0x1f;
    LCDM17 &= 0xf0;
    LCDM16 |= ((Cld1712_TAB[c]&0x01)<<7)+((Cld1712_TAB[c]&0x02)<<5)+((Cld1712_TAB[c]&0x04)<<3);
    LCDM17 |= ((Cld1712_TAB[c]&0x80)>>7)+((Cld1712_TAB[c]&0x40)>>5)+((Cld1712_TAB[c]&0x20)>>3)+((Cld1712_TAB[c]&0x10)>>1);
    break;
  case CHAR_POS_CODE_NUM_4://code number 4
    LCDM15 &= 0x1f;
    LCDM16 &= 0xf0;
    LCDM15 |= ((Cld1712_TAB[c]&0x01)<<7)+((Cld1712_TAB[c]&0x02)<<5)+((Cld1712_TAB[c]&0x04)<<3);
    LCDM16 |= ((Cld1712_TAB[c]&0x80)>>7)+((Cld1712_TAB[c]&0x40)>>5)+((Cld1712_TAB[c]&0x20)>>3)+((Cld1712_TAB[c]&0x10)>>1);
    break;
  case CHAR_POS_CODE_NUM_5://code number 5
    LCDM14 &= 0x1f;
    LCDM15 &= 0xf0;
    LCDM14 |= ((Cld1712_TAB[c]&0x01)<<7)+((Cld1712_TAB[c]&0x02)<<5)+((Cld1712_TAB[c]&0x04)<<3);
    LCDM15 |= ((Cld1712_TAB[c]&0x80)>>7)+((Cld1712_TAB[c]&0x40)>>5)+((Cld1712_TAB[c]&0x20)>>3)+((Cld1712_TAB[c]&0x10)>>1);
    break;
  case CHAR_POS_CODE_NUM_6://code number 6
    LCDM13 &= 0x1f;
    LCDM14 &= 0xf0;
    LCDM13 |= ((Cld1712_TAB[c]&0x01)<<7)+((Cld1712_TAB[c]&0x02)<<5)+((Cld1712_TAB[c]&0x04)<<3);
    LCDM14 |= ((Cld1712_TAB[c]&0x80)>>7)+((Cld1712_TAB[c]&0x40)>>5)+((Cld1712_TAB[c]&0x20)>>3)+((Cld1712_TAB[c]&0x10)>>1);
    break;
  case CHAR_POS_CODE_NUM_7://code number 7
    LCDM12 &= 0x1f;
    LCDM13 &= 0xf0;
    LCDM12 |= ((Cld1712_TAB[c]&0x01)<<7)+((Cld1712_TAB[c]&0x02)<<5)+((Cld1712_TAB[c]&0x04)<<3);
    LCDM13 |= ((Cld1712_TAB[c]&0x80)>>7)+((Cld1712_TAB[c]&0x40)>>5)+((Cld1712_TAB[c]&0x20)>>3)+((Cld1712_TAB[c]&0x10)>>1);
    break;
  case CHAR_POS_CODE_NUM_8://code number 8
    LCDM11 &= 0x1f;
    LCDM12 &= 0xf0;
    LCDM11 |= ((Cld1712_TAB[c]&0x01)<<7)+((Cld1712_TAB[c]&0x02)<<5)+((Cld1712_TAB[c]&0x04)<<3);
    LCDM12 |= ((Cld1712_TAB[c]&0x80)>>7)+((Cld1712_TAB[c]&0x40)>>5)+((Cld1712_TAB[c]&0x20)>>3)+((Cld1712_TAB[c]&0x10)>>1);
    break;
  case CHAR_POS_TEMP_TENTH://temperature��tenth' digit
    LCDM3 &= 0x0f;
    LCDM4 &= 0xf8;
    LCDM3 |= Cld1712_TAB[c]&0xf0;
    LCDM4 |= Cld1712_TAB[c]&0x07;
    break;
  case CHAR_POS_TEMP_UINTS://temperature, units
    LCDM2 &= 0x0f;
    LCDM3 &= 0xf8;
    LCDM2 |= Cld1712_TAB[c]&0xf0;
    LCDM3 |= Cld1712_TAB[c]&0x07;
    break;
  case CHAR_POS_TEMP_TENS://temperature, tens
    LCDM1 &= 0x0f;
    LCDM2 &= 0xf8;
    LCDM1 |= Cld1712_TAB[c]&0xf0;
    LCDM2 |= Cld1712_TAB[c]&0x07;
    break;
  case CHAR_POS_MONTH_UNITS://month, units
    LCDM5 &= 0x80;
    LCDM5 |= (Cld1712_TAB[c]<<4)&0x70;
    LCDM5 |= (Cld1712_TAB[c]>>4)&0x0f;
    break;
  case CHAR_POS_DAY_TENS://day, tens
    LCDM6 &= 0x80;
    LCDM6 |= (Cld1712_TAB[c]<<4)&0x70;
    LCDM6 |= (Cld1712_TAB[c]>>4)&0x0f;
    break;
  case CHAR_POS_DAY_UNITS://day, uints
    LCDM7 &= 0x80;
    LCDM7 |= (Cld1712_TAB[c]<<4)&0x70;
    LCDM7 |= (Cld1712_TAB[c]>>4)&0x0f;
    break;
  case CHAR_POS_HOUR_UNITS://hour, uints
    LCDM8 &= 0x80;
    LCDM8 |= (Cld1712_TAB[c]<<4)&0x70;
    LCDM8 |= (Cld1712_TAB[c]>>4)&0x0f;
    break;
  case CHAR_POS_MIN_TENS://minitu, tens
    LCDM9 &= 0x80;
    LCDM9 |= (Cld1712_TAB[c]<<4)&0x70;
    LCDM9 |= (Cld1712_TAB[c]>>4)&0x0f;
    break;
  case CHAR_POS_MIN_UNITS://minitu, units
    LCDM10 &= 0x80;
    LCDM10 |= (Cld1712_TAB[c]<<4)&0x70;
    LCDM10 |= (Cld1712_TAB[c]>>4)&0x0f;
    break;
  default:
    break;
  }
}

/**
 * @brief  Cld1712syu display symbol
 * @param[in] s:   symbol to be diplayed.
 * @param[in] dis: 0:display; 1:not dispaly.
 * @retval None
 */
void Cld1712_PutSymbol(uint8_t s, uint8_t dis)
{
  uint8_t bitpos = s & 0x07;	//�ڼĴ�����,ռ�ڼ�λ
  s = s >> 3;			//LCDMEM�Ĵ�����
  if((s > 19) || (bitpos > 7))	//����Խ����
  {
    return;
  }
  if(dis)
  {
    LCDMEM[s] |= 1 << bitpos;
  }
  else
  {
    LCDMEM[s] &= ~(1 << bitpos);
  }
}

/**
 * @brief  Clear static items
 * @param  None
 * @retval None
 */
void Cld1712_ClrStaticItems(void) {
  LcdPutSymbol(FH1, 0);
  //      LcdPutSymbol(FH2, 0);
  LcdPutSymbol(FH3, 0);
  LcdPutSymbol(FH4, 0);
  LcdPutSymbol(FH5, 0);
  LcdPutSymbol(FH6, 0);
  //      LcdPutSymbol(FH7, 0);
  LcdPutSymbol(FH8, 0);
  //      LcdPutSymbol(FH9, 0);
  //      LcdPutSymbol(FH10, 0);
  //      LcdPutSymbol(FH11, 0);
  //      LcdPutSymbol(FH12, 0);
  LcdPutSymbol(FH13, 0);
  LcdPutSymbol(FH14, 0);
  LcdPutSymbol(FH15, 0);
  LcdPutSymbol(FH16, 0);
  LcdPutSymbol(FH17, 0);
  LcdPutSymbol(FH18, 0);
  LcdPutSymbol(FH19, 0);
  LcdPutSymbol(FH20, 0);
  //      LcdPutSymbol(FH21, 0);
  LcdPutSymbol(FH22, 0);
  LcdPutSymbol(FH23, 0);
  LcdPutSymbol(FH24, 0);
  LcdPutSymbol(FH25, 0);
  //      LcdPutSymbol(FH26, 0);
  //      LcdPutSymbol(FH27, 0);
  LcdPutSymbol(FH28, 0);
  LcdPutSymbol(FH29, 0);
}

/**
 * @brief  Cld1712syu display page 1.//ʱ�ӡ��豸��
 * @param  None
 * @retval None
 */
void Cld1712_Page1(void) {
#if(USER_AGREEMENT != 2)    //���Ϊ��������MODBUSЭ�飬��û�д���
  {
    LcdPutSymbol(FH26, 1); //���ڷָ��
    
    if((g_tSysTime.month % 100/10) != 0)
      LcdPutSymbol(_12BC, 1); //�µ�ʮλ
    else
      LcdPutSymbol(_12BC, 0); //�µ�ʮλ
    LcdPutChar(g_tSysTime.month%10, CHAR_POS_MONTH_UNITS);
    LcdPutChar(g_tSysTime.day%100/10, CHAR_POS_DAY_TENS);
    LcdPutChar(g_tSysTime.day%10, CHAR_POS_DAY_UNITS);  
    
    LcdPutSymbol(FH27, 1); //ʱ�ӷָ��
////    LcdPutChar(g_tSysTime.hour%100/10, 16);
    
    switch(g_tSysTime.hour%100/10)
    {
    case 1:
      {
        LcdPutSymbol(_16B, 1);
        LcdPutSymbol(_16C, 1);
        LcdPutSymbol(_16AGED, 0);
      }break;
    case 2:
      {
        LcdPutSymbol(_16B, 1);
        LcdPutSymbol(_16C, 0);
        LcdPutSymbol(_16AGED, 1);
      }break;
    default:
      {
        LcdPutSymbol(_16B, 0);
        LcdPutSymbol(_16C, 0);
        LcdPutSymbol(_16AGED, 0);
      }break;              
    }
    
    LcdPutChar(g_tSysTime.hour%10, CHAR_POS_HOUR_UNITS);
    LcdPutChar(g_tSysTime.minute%100/10, CHAR_POS_MIN_TENS);
    LcdPutChar(g_tSysTime.minute%10, CHAR_POS_MIN_UNITS);         
  }
#else
  {
    LcdPutSymbol(FH26, 0); //���ڷָ��
    LcdPutSymbol(FH27, 0); //ʱ�ӷָ��
    LcdPutSymbol(_12BC, 0); //�µ�ʮλ
    LcdPutSymbol(_16B, 0);
    LcdPutSymbol(_16C, 0);
    LcdPutSymbol(_16AGED, 0);
    LcdPutChar(21, CHAR_POS_MONTH_UNITS);
    LcdPutChar(21, CHAR_POS_DAY_TENS);
    LcdPutChar(21, CHAR_POS_DAY_UNITS);  
    LcdPutChar(21, CHAR_POS_HOUR_UNITS);
    LcdPutChar(21, CHAR_POS_MIN_TENS);
    LcdPutChar(21, CHAR_POS_MIN_UNITS);  
  }
#endif
  
  LcdPutSymbol(FH21, 1); //���
  LcdPutChar( ((g_tSysParam.deviceNum & 0xF0000000)>>28), CHAR_POS_CODE_NUM_1);
  LcdPutChar( ((g_tSysParam.deviceNum & 0x0F000000)>>24), CHAR_POS_CODE_NUM_2);
  LcdPutChar( ((g_tSysParam.deviceNum & 0x00F00000)>>20), CHAR_POS_CODE_NUM_3);
  LcdPutChar( ((g_tSysParam.deviceNum & 0x000F0000)>>16), CHAR_POS_CODE_NUM_4);
  LcdPutChar( ((g_tSysParam.deviceNum & 0x0000F000)>>12), CHAR_POS_CODE_NUM_5);
  LcdPutChar( ((g_tSysParam.deviceNum & 0x00000F00)>>8),  CHAR_POS_CODE_NUM_6);
  LcdPutChar( ((g_tSysParam.deviceNum & 0x000000F0)>>4),  CHAR_POS_CODE_NUM_7);
  LcdPutChar( ((g_tSysParam.deviceNum & 0x0000000F)),     CHAR_POS_CODE_NUM_8);   
}

/**
 * @brief  Cld1712syu display page 2.//Э��汾��
 * @param  None
 * @retval None
 */
void Cld1712_Page2(void) {
  
  LcdPutSymbol(FH21, 0); //���
  LcdPutSymbol(FH26, 0); //���ڷָ��
  LcdPutSymbol(FH27, 0); //ʱ�ӷָ��
  LcdPutSymbol(_12BC, 0); //�µ�ʮλ
  LcdPutSymbol(_16B, 0);
  LcdPutSymbol(_16C, 0);
  LcdPutSymbol(_16AGED, 0);
  
  LcdPutChar(17, CHAR_POS_CODE_NUM_1);   //P
  LcdPutChar(18, CHAR_POS_CODE_NUM_2);   //n 
  LcdPutChar((g_tSysParam.protocolVer[0] & 0xF0)>>4, CHAR_POS_CODE_NUM_3);
  LcdPutChar((g_tSysParam.protocolVer[0] & 0x0F),     CHAR_POS_CODE_NUM_4);
  LcdPutChar((g_tSysParam.protocolVer[1] & 0xF0)>>4, CHAR_POS_CODE_NUM_5);
  LcdPutChar((g_tSysParam.protocolVer[1] & 0x0F),     CHAR_POS_CODE_NUM_6);
  LcdPutChar((g_tSysParam.protocolVer[2] & 0xF0)>>4, CHAR_POS_CODE_NUM_7);
  LcdPutChar((g_tSysParam.protocolVer[2] & 0x0F),     CHAR_POS_CODE_NUM_8); 
  LcdPutChar((g_tSysParam.protocolVer[3] & 0xF0)>>4, CHAR_POS_MONTH_UNITS);
  LcdPutChar((g_tSysParam.protocolVer[3] & 0x0F),     CHAR_POS_DAY_TENS);
  LcdPutChar((g_tSysParam.protocolVer[0] & 0xF0)>>4, CHAR_POS_DAY_UNITS);
  LcdPutChar((g_tSysParam.protocolVer[0] & 0x0F),     CHAR_POS_HOUR_UNITS);
  LcdPutChar((g_tSysParam.protocolVer[1] & 0xF0)>>4, CHAR_POS_MIN_TENS);
  LcdPutChar((g_tSysParam.protocolVer[1] & 0x0F),     CHAR_POS_MIN_UNITS); 
}

/**
 * @brief  Cld1712syu display page 3. //����汾�š� GPRSģ��汾�š�EEPROM������롢�������в��衢GPRS���д������
 * @param  None
 * @retval None
 */
void Cld1712_Page3(void) {

  uint8_t GprsModeVerNum  = 101;
  
  LcdPutSymbol(FH21, 0); //���
  LcdPutSymbol(FH26, 0); //���ڷָ��
  LcdPutSymbol(FH27, 0); //ʱ�ӷָ��
  LcdPutSymbol(_12BC, 0); //�µ�ʮλ
  LcdPutSymbol(_16B, 0);
  LcdPutSymbol(_16C, 0);
  LcdPutSymbol(_16AGED, 0);
  
  LcdPutChar(24, CHAR_POS_CODE_NUM_1);   //V
  LcdPutChar( ((g_tSysParam.version & 0x0000F000)>>12), CHAR_POS_CODE_NUM_2);
  LcdPutChar( ((g_tSysParam.version & 0x00000F00)>>8),  CHAR_POS_CODE_NUM_3);
  LcdPutChar( ((g_tSysParam.version & 0x000000F0)>>4),  CHAR_POS_CODE_NUM_4);
  LcdPutChar( ((g_tSysParam.version & 0x0000000F)),     CHAR_POS_CODE_NUM_5);
  
  LcdPutChar(21, CHAR_POS_CODE_NUM_6);
  LcdPutChar(GprsModeVerNum%100/10, CHAR_POS_CODE_NUM_7);  //��ʾGPRSģ�����汾�ŵ����ڶ�λ////
  LcdPutChar(GprsModeVerNum%10, CHAR_POS_CODE_NUM_8);      //��ʾGPRSģ�����汾�ŵ�����һλ////
  
  if(0)////g_ucEepromError == 0)    //EEPROM�����־
    LcdPutChar(0, CHAR_POS_MONTH_UNITS);  
  else
    LcdPutChar(1, CHAR_POS_MONTH_UNITS);  
  
////  LcdPutChar(GprsRunStepNum/10, CHAR_POS_DAY_TENS);
////  LcdPutChar(GprsRunStepNum%10, CHAR_POS_DAY_UNITS);    //�������в���
  
  
////  if((g_ucDisOnlineFlag == 1) && (g_ucTxDataErrorFlag >= 2))    //���������ʾ
////  {
////    if((g_ucTxDataErrorFlag >= 2) && (g_ucTxDataErrorFlag != 4))
////    {
////      LcdPutChar(0xe, CHAR_POS_HOUR_UNITS);
////      LcdPutChar(2, CHAR_POS_MIN_TENS);
////      LcdPutChar(2, CHAR_POS_MIN_UNITS);   
////    }
////    else if(g_ucTxDataErrorFlag == 4)
////    {
////      LcdPutChar(0xe, CHAR_POS_HOUR_UNITS);
////      LcdPutChar(2, CHAR_POS_MIN_TENS);
////      LcdPutChar(1, CHAR_POS_MIN_UNITS);   
////    }
////  }
////  else
  {
    LcdPutChar(0xe, CHAR_POS_HOUR_UNITS);
////    LcdPutChar(SocketInitErrorCode/10, CHAR_POS_MIN_TENS);
////    LcdPutChar(SocketInitErrorCode%10, CHAR_POS_MIN_UNITS);            
  }
}

/**
 * @brief  Cld1712syu display page 4. IMEI low 8 bytes
 * @param  None
 * @retval None
 */
void Cld1712_Page4(void) {
////#if(NB_MODEM == 1)
  {
    LcdPutSymbol(FH21, 0); //���
    LcdPutSymbol(FH26, 0); //���ڷָ��
    LcdPutSymbol(FH27, 0); //ʱ�ӷָ��
    //            LcdPutSymbol(_12BC, 0); //�µ�ʮλ
    LcdPutSymbol(_16B, 0);
    LcdPutSymbol(_16C, 0);
    LcdPutSymbol(_16AGED, 0);
    
////    for(temp=0;(temp<15)&&(ImeiNum[temp]<=9);temp++);
////    
////    if((ImeiNum[15]=='\r')&&(ImeiNum[16]=='\n')&&(temp==15))   //IMEI��ȷ
////    {
      LcdPutChar(nbobj.imei[0]-0x30, CHAR_POS_CODE_NUM_1);
      LcdPutChar(nbobj.imei[1]-0x30, CHAR_POS_CODE_NUM_2);
      LcdPutChar(nbobj.imei[2]-0x30, CHAR_POS_CODE_NUM_3);
      LcdPutChar(nbobj.imei[3]-0x30, CHAR_POS_CODE_NUM_4);
      LcdPutChar(nbobj.imei[4]-0x30, CHAR_POS_CODE_NUM_5);
      LcdPutChar(nbobj.imei[5]-0x30, CHAR_POS_CODE_NUM_6);
      LcdPutChar(nbobj.imei[6]-0x30, CHAR_POS_CODE_NUM_7);
      LcdPutChar(nbobj.imei[7]-0x30, CHAR_POS_CODE_NUM_8);
////    }
////    else
////    {
////      LcdPutChar(16, CHAR_POS_CODE_NUM_1);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_2);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_3);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_4);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_5);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_6);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_7);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_8);
////      R_flash(IMEI_ADD,ImeiNum,20);  //����IMEI�� 
////    }
    
    LcdPutSymbol(_12BC, 1); //�µ�ʮλ
    LcdPutChar(18, CHAR_POS_MONTH_UNITS);
    LcdPutChar(14, CHAR_POS_DAY_TENS);
    LcdPutChar(1,  CHAR_POS_DAY_UNITS);   //IME2
    
 ////   LcdPutChar(16, 16);
    LcdPutChar(23, CHAR_POS_HOUR_UNITS);
////    LcdPutChar(SignalIntensity/10%10, CHAR_POS_MIN_TENS);
////    LcdPutChar(SignalIntensity%10, CHAR_POS_MIN_UNITS);    //�ź�ǿ��
  }
////#else
////  {
////////    g_ucPage++;
////////    g_ucScreenRefresh = 1;
////  }
////#endif
}

/**
 * @brief  Cld1712syu display page 5, IMEI hith 7 bytes
 * @param  None
 * @retval None
 */
void Cld1712_Page5(void) {
////#if(NB_MODEM == 1)
  {
    LcdPutSymbol(FH21, 0); //���
    LcdPutSymbol(FH26, 0); //���ڷָ��
    LcdPutSymbol(FH27, 0); //ʱ�ӷָ��
    LcdPutSymbol(_16AGED, 0);
    
    
////    for(temp=0;(temp<15)&&(ImeiNum[temp]<=9);temp++);
////    if((ImeiNum[15]=='\r')&&(ImeiNum[16]=='\n')&&(temp==15))   //IMEI��ȷ
////    {
      LcdPutChar(nbobj.imei[8]-0x30,  CHAR_POS_CODE_NUM_1);
      LcdPutChar(nbobj.imei[9]-0x30,  CHAR_POS_CODE_NUM_2);
      LcdPutChar(nbobj.imei[10]-0x30, CHAR_POS_CODE_NUM_3);
      LcdPutChar(nbobj.imei[11]-0x30, CHAR_POS_CODE_NUM_4);
      LcdPutChar(nbobj.imei[12]-0x30, CHAR_POS_CODE_NUM_5);
      LcdPutChar(nbobj.imei[13]-0x30, CHAR_POS_CODE_NUM_6);
      LcdPutChar(nbobj.imei[14]-0x30, CHAR_POS_CODE_NUM_7);
////    }
////    else
////    {
////      LcdPutChar(16, CHAR_POS_CODE_NUM_1);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_2);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_3);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_4);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_5);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_6);
////      LcdPutChar(16, CHAR_POS_CODE_NUM_7);
////////      R_flash(IMEI_ADD,ImeiNum,20);  //����IMEI�� 
////    }
    LcdPutChar(21, CHAR_POS_CODE_NUM_8);
    
    LcdPutSymbol(_12BC, 1); //�µ�ʮλ
    LcdPutChar(18, CHAR_POS_MONTH_UNITS);
    LcdPutChar(14, CHAR_POS_DAY_TENS);
    LcdPutChar(2,  CHAR_POS_DAY_UNITS);   //IME2
    
#if(DEVICE_TYPE == 3)  //���Ϊ������
////    {
////      temp = GetNbPowerVoltage();
////      LcdPutSymbol(_16B, 0);  //
////      LcdPutSymbol(_16C, 0);  //
////      LcdPutChar(12, CHAR_POS_HOUR_UNITS);
////      LcdPutChar(temp%100/10, CHAR_POS_MIN_TENS);
////      LcdPutChar(temp%10, CHAR_POS_MIN_UNITS);    //�������ݵ�ѹ        
////    }
#else
    {
      LcdPutSymbol(_16B, 1);  //
      LcdPutSymbol(_16C, 1);  //
      LcdPutChar(18, CHAR_POS_HOUR_UNITS);
      LcdPutChar(14, CHAR_POS_MIN_TENS);
      LcdPutChar(1, CHAR_POS_MIN_UNITS);    //IMEI            
    }
#endif 
  }
////#else
////  {
////////    g_ucPage = 0;
////////    g_ucScreenRefresh = 1;
////  }
////#endif
}


/**
 * @brief  Cld1712syu display page 6. IMSI low 8 bytes
 * @param  None
 * @retval None
 */
void Cld1712_Page6(void) {
  LcdPutSymbol(FH21, 0); //���
  LcdPutSymbol(FH26, 0); //���ڷָ��
  LcdPutSymbol(FH27, 0); //ʱ�ӷָ��
  //            LcdPutSymbol(_12BC, 0); //�µ�ʮλ
  LcdPutSymbol(_16B, 0);
  LcdPutSymbol(_16C, 0);
  LcdPutSymbol(_16AGED, 0);
  
  LcdPutChar(nbobj.imsi[0]-0x30, CHAR_POS_CODE_NUM_1);
  LcdPutChar(nbobj.imsi[1]-0x30, CHAR_POS_CODE_NUM_2);
  LcdPutChar(nbobj.imsi[2]-0x30, CHAR_POS_CODE_NUM_3);
  LcdPutChar(nbobj.imsi[3]-0x30, CHAR_POS_CODE_NUM_4);
  LcdPutChar(nbobj.imsi[4]-0x30, CHAR_POS_CODE_NUM_5);
  LcdPutChar(nbobj.imsi[5]-0x30, CHAR_POS_CODE_NUM_6);
  LcdPutChar(nbobj.imsi[6]-0x30, CHAR_POS_CODE_NUM_7);
  LcdPutChar(nbobj.imsi[7]-0x30, CHAR_POS_CODE_NUM_8);
  
  LcdPutSymbol(_12BC, 1); //�µ�ʮλ
  LcdPutChar(18, CHAR_POS_MONTH_UNITS);
  LcdPutChar(5, CHAR_POS_DAY_TENS);
  LcdPutChar(1, CHAR_POS_DAY_UNITS);   //IMS1
  
////  LcdPutChar(21, 16);
  LcdPutChar(21, CHAR_POS_HOUR_UNITS);
  LcdPutChar(21, CHAR_POS_MIN_TENS);
  LcdPutChar(21, CHAR_POS_MIN_UNITS);    //
}


/**
 * @brief  Cld1712syu display page 7. IMSI high 7 bytes
 * @param  None
 * @retval None
 */
void Cld1712_Page7(void) {
  LcdPutSymbol(FH21, 0); //���
  LcdPutSymbol(FH26, 0); //���ڷָ��
  LcdPutSymbol(FH27, 0); //ʱ�ӷָ��
  //            LcdPutSymbol(_12BC, 0); //�µ�ʮλ
  LcdPutSymbol(_16B, 0);
  LcdPutSymbol(_16C, 0);
  LcdPutSymbol(_16AGED, 0);
  
  LcdPutChar(nbobj.imsi[8]-0x30,  CHAR_POS_CODE_NUM_1);
  LcdPutChar(nbobj.imsi[9]-0x30,  CHAR_POS_CODE_NUM_2);
  LcdPutChar(nbobj.imsi[10]-0x30, CHAR_POS_CODE_NUM_3);
  LcdPutChar(nbobj.imsi[11]-0x30, CHAR_POS_CODE_NUM_4);
  LcdPutChar(nbobj.imsi[12]-0x30, CHAR_POS_CODE_NUM_5);
  LcdPutChar(nbobj.imsi[13]-0x30, CHAR_POS_CODE_NUM_6);
  LcdPutChar(nbobj.imsi[14]-0x30, CHAR_POS_CODE_NUM_7);
  LcdPutChar(21, CHAR_POS_CODE_NUM_8);
  
  LcdPutSymbol(_12BC, 1); //�µ�ʮλ
  LcdPutChar(18, CHAR_POS_MONTH_UNITS);
  LcdPutChar(5, CHAR_POS_DAY_TENS);
  LcdPutChar(2, CHAR_POS_DAY_UNITS);   //IMS1
  
////  LcdPutChar(21, 16);
  LcdPutChar(21, CHAR_POS_HOUR_UNITS);
  LcdPutChar(21, CHAR_POS_MIN_TENS);
  LcdPutChar(21, CHAR_POS_MIN_UNITS);    //
}

/**
 * @brief  Cld1712syu display Temperature
 * @param  None
 * @retval None
 */
void Cld1712_DispTemperature(void) {
  int32_t temp = 0;
 
  if(g_u8TempDispEn == 0)     //�����ʾ�¶�
  {
    ////if(((GetSysCount()%10) >= 5) && (TEMPERATURE_SENSOR_TYPE == 2))
    if(((g_ucPage & 0x01) == 0x01) && (TEMPERATURE_SENSOR_TYPE == 2))
    {
      if((H_able == 0) && (TEMPERATURE_SENSOR_TYPE == 2))  
      {
        LcdPutSymbol(FH9, 0);  //�����-��
        LcdPutChar(Hdc1080_GetHumidityVal()%100/10, CHAR_POS_TEMP_TENS);
        LcdPutChar(Hdc1080_GetHumidityVal()%10, CHAR_POS_TEMP_UINTS); 
        LcdPutChar(0, CHAR_POS_TEMP_TENTH); 
        LcdPutSymbol(FH10, 1);  //С����   
      }
      else
      {
        LcdPutSymbol(FH9, 1);  //��ʾ��-��
        LcdPutChar(16, CHAR_POS_TEMP_TENS);
        LcdPutChar(16, CHAR_POS_TEMP_UINTS); 
        LcdPutChar(16, CHAR_POS_TEMP_TENTH); 
        LcdPutSymbol(FH10, 0);//���С����   
      }
      LcdPutSymbol(FH7, 0); //�¶�
      LcdPutSymbol(FH11, 0); //���϶� 
      LcdPutSymbol(FH12, 1); //��ʾ��%RH�� 
    }
    else
    {
      if(T_able == 1)
      {
        LcdPutSymbol(FH9, 1);  //��ʾ��-��
        LcdPutChar(16, CHAR_POS_TEMP_TENS);  //��ʾ��-��
        LcdPutChar(16, CHAR_POS_TEMP_UINTS);    
        LcdPutChar(21, CHAR_POS_TEMP_TENTH);    
        LcdPutSymbol(FH10, 0);//���С����   
      }
      else 
      {
        temp = Hdc1080_GetTempVal();
////          if(m_ucTemperatureCorrectFlag == 1)  //����¶�����
////          {
////            temp = temp + m_ucTemperatureCorrectValue;    //�¶�����
////          }
        
        if(temp < 0) {    //����¶�Ϊ��ֵ
          temp = 0 - temp;
          LcdPutSymbol(FH9, 1);  //��ʾ��-��
        }
        else {
          LcdPutSymbol(FH9, 0);  //��-��
        }
        LcdPutChar(temp%10000/1000, CHAR_POS_TEMP_TENS);
        LcdPutChar(temp%1000/100, CHAR_POS_TEMP_UINTS);
        LcdPutChar(temp%100/10, CHAR_POS_TEMP_TENTH);
        LcdPutSymbol(FH10, 1);//��ʾС����      
      }
      LcdPutSymbol(FH7, 1); //�¶�    
      LcdPutSymbol(FH11, 1); //���϶�    
      LcdPutSymbol(FH12, 0); //��ʾ��%RH��
    }
    
    if(0)////GprsFirmwareUpdate == 1)   //���ģ�����������̼�����С������˸
    {
      if(0)////(GetSysCount()%2) == 0)
      {
        LcdPutSymbol(FH10, 1);//��ʾС����    
      }
      else
      {
        LcdPutSymbol(FH10, 0);//���С����
      }
    }
  }
  else
  {
    LcdPutSymbol(FH7, 0); //�¶�
    LcdPutSymbol(FH10, 0);//��ʾС����      
    LcdPutSymbol(FH11, 0); //���϶�
    LcdPutSymbol(FH12, 0); //��ʾ��%RH�� 
    LcdPutSymbol(FH9, 0);  //�����-��
    
    LcdPutChar(23, CHAR_POS_TEMP_TENS);////"H"
    LcdPutChar(nbAppGetRSSI()%100/10, CHAR_POS_TEMP_UINTS);
    LcdPutChar(nbAppGetRSSI()%10,     CHAR_POS_TEMP_TENTH);
  }
  
}

/**
 * @brief  �����߱�־������ڷ����У����־��˸�����߿��У�����ʾ��־�������ߣ�����ʾ��
 * @param  None
 * @retval None
 */
void Cld1712_DispOnlineFlag(void) 
{
  if(1 == nbAppGetNetStat())//����
  {
    if(1 == nbAppIsSending()) //���ڷ��ͣ����߱�־��˸
    {  
      if((SysTickGetSec() % 2) == 0){
        LcdPutSymbol(FH2, 1);  
      }
      else{
        LcdPutSymbol(FH2, 0);
      }
    }
    else{
      LcdPutSymbol(FH2, 1);//�����߱�־   
    }
  }
  else{
    LcdPutSymbol(FH2, 0);//�����߱�־       
  }
}


/**
 * @brief  Cld1712_ display run
 * @param  None
 * @retval None
 */
void Cld1712_DisplayPoll(void)
{
  static uint32_t s_u32TimeStart = 0;  
  
  if((SysTickGetMsDif(s_u32TimeStart, SysTickGetMs()) > 1000) || (g_ucScreenRefresh == 1))   //ÿ��������һ��
  {
    s_u32TimeStart = SysTickGetMs(); 
    g_ucScreenRefresh = 0;
    
    //�رչ̶���Ŀ��ʾ
    Cld1712_ClrStaticItems();
    
    //ҳ����ʾ
    switch(g_ucPage)
    {
    case 0: {         //ʱ�ӡ��豸��
      Cld1712_Page1();
      }break;
    case 1: {         //Э��汾��
      Cld1712_Page2();
      }break;
    case 2: {         //����汾�š� GPRSģ��汾�š�EEPROM������롢�������в��衢GPRS���д������
      Cld1712_Page3();
      }break;
    case 3: {         //IMEI low 8 bytes 
      Cld1712_Page4();
      }break;
    case 4: {         //IMEI hith 7 bytes
      Cld1712_Page5();
      }break;
    case 5: {         //IMSI low 8 bytes
      Cld1712_Page6();
      }break;
    case 6: {         //IMSI high 7 bytes
      Cld1712_Page7();
      }break;
    default:{
        g_ucPage=0;
      }break;
    }
    
//    LcdPutChar(g_ucPage, CHAR_POS_CODE_NUM_1);////
//    if(++g_ucPage > 6)
//    {
//      g_ucPage = 0;
//    }
    
    
    //�����ʾ�¶�
    Cld1712_DispTemperature();

    //�����߱�־
    Cld1712_DispOnlineFlag();
    
    //�������ݵ���״̬  0����  1�͵� 
    if(PM_GetFlagLowVolt() == 0) 
    {
      LcdPutSymbol(FH6, 0);
    }
    else
    {
      LcdPutSymbol(FH6, 1);
    }
  }
    
}
    


/**
 * @}
 */






