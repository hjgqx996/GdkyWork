/**
  ******************************************************************************
  *               Copyright(C) 2019-2029 GDKY All Rights Reserved
  *
  * @file     HDC1080.h
  * @author   scf
  * @version  V1.00
  * @date     27-May-2019
  * @brief    HDC1080 driver.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HDC1080_H
#define __HDC1080_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */

/** 
 * @defgroup HDC1080
 * @brief HDC1080 driver
 * @{
 */
   
#include "digitio.h"   
   
/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */
   
#define  DELAY_US(us)  __delay_cycles((long)us*8)   //__delay_cycles(1): delay time = 1 system clock.
#define  DELAY_MS(ms)  __delay_cycles((long)ms*8000)
   
/////////////////////时钟引脚 SCL//////////////////////////////////////
#define IIC1_SCL_PIN                 P2                                 //端口号
#define IIC1_SCL_DDR                 P3DIR                              //端口方向寄存器
#define IIC1_SCL_ODR                 P3OUT                              //输出寄存器
#define IIC1_SCL_IDR                 P3IN                               //输入寄存器
#define IIC1_SCL_OUT()               DIO_SetRegBit(&IIC1_SCL_DDR, IIC1_SCL_PIN)   
#define IIC1_SCL_IN()                DIO_ClrRegBit(&IIC1_SCL_DDR, IIC1_SCL_PIN)  
#define CLR_IIC1_SCL()               DIO_ClrRegBit(&IIC1_SCL_ODR, IIC1_SCL_PIN) 
#define SET_IIC1_SCL()               DIO_SetRegBit(&IIC1_SCL_ODR, IIC1_SCL_PIN)      
#define GET_IIC1_SCL()               DIO_GetRegBit(&IIC1_SCL_IDR, IIC1_SCL_PIN)
#define CPL_IIC1_SCL()               CPL_Bit(&IIC1_SCL_ODR, IIC1_SCL_PIN)


/////////////////////数据引脚 SDA//////////////////////////////////////
#define IIC1_SDA_PIN                 P1                                 //端口号
#define IIC1_SDA_DDR                 P3DIR                              //端口方向寄存器
#define IIC1_SDA_ODR                 P3OUT                              //输出寄存器
#define IIC1_SDA_IDR                 P3IN                               //输入寄存器
#define IIC1_SDA_OUT()               DIO_SetRegBit(&IIC1_SDA_DDR, IIC1_SDA_PIN)   
#define IIC1_SDA_IN()                DIO_ClrRegBit(&IIC1_SDA_DDR, IIC1_SDA_PIN)  
#define CLR_IIC1_SDA()               DIO_ClrRegBit(&IIC1_SDA_ODR, IIC1_SDA_PIN) 
#define SET_IIC1_SDA()               DIO_SetRegBit(&IIC1_SDA_ODR, IIC1_SDA_PIN)      
#define GET_IIC1_SDA()               DIO_GetRegBit(&IIC1_SDA_IDR, IIC1_SDA_PIN)
#define CPL_IIC1_SDA()               CPL_Bit(&IIC1_SDA_ODR, IIC1_SDA_PIN)   
   
/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */
   
uint32_t HDC1080_ReadOneByte(uint8_t ReadAddr);				//指定地址读取一个字节
void HDC1080_WriteOneByte(uint8_t WriteAddr, uint32_t DataToWrite);		//指定地址写入一个字节

void HDC1080_Init(void); //初始化IIC
void Hdc1080_UpdateTemperature(void);
void Hdc1080_UpdateHumidity(void);

int16_t Hdc1080_GetTempVal(void);
int16_t Hdc1080_GetHumidityVal(void);

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

   

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __HDC1080_H */

/**
 * @}
 */

















