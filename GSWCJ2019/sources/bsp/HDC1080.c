/**
  ******************************************************************************
  *               Copyright(C) 2019-2029 GDKY All Rights Reserved
  *
  * @file     HDC1080.c
  * @author   scf
  * @version  V1.00
  * @date     27-May-2019
  * @brief    HDC1080 driver.
  ******************************************************************************
  * @history
  */

/* INCLUDES ----------------------------------------------------------------- */

/** 
 * @defgroup HDC1080
 * @brief HDC1080 driver
 * @{
 */

#include "includes.h"
#include "HDC1080.h" 
#include "TemperatureMeasure.h" 

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */
   
static int16_t s_i16TempVal    = 0;//温度值，单位：0.01℃
static uint8_t s_u8HumidityVal = 0;//湿度值，单位：%， 1-100.

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

void IIC1_Init(void);
void IIC1_Start(void);
void IIC1_Stop(void);
uint8_t IIC1_Wait_Ack(void);
void IIC1_Ack(void);
void IIC1_NAck(void);
void IIC1_Send_Byte(uint8_t txd);
uint8_t IIC1_Read_Byte(uint8_t ack);



//////////////////////////////////////////////


/**
 * @brief  初始化IIC
 * @param  None
 * @return None
 */
void IIC1_Init(void)
{
  SET_IIC1_SCL();    //时钟口置高
  IIC1_SCL_OUT();    //时钟口置为输出
  SET_IIC1_SDA();    //数据口置高
  IIC1_SDA_OUT();    //数据口置为输出
}

/**
 * @brief  产生IIC起始信号		
 * @param  None
 * @return None
 */
void IIC1_Start(void)
{
	IIC1_SDA_OUT();     //sda线输出
	SET_IIC1_SDA();  
	SET_IIC1_SCL();
	DELAY_US(2);
 	CLR_IIC1_SDA();//START:when CLK is high,DATA change form high to low 
	DELAY_US(2);
	CLR_IIC1_SCL();//钳住I2C总线，准备发送或接收数据 
}

/**
 * @brief  产生IIC停止信号	
 * @param  None
 * @return None
 */
void IIC1_Stop(void)
{
	IIC1_SDA_OUT();//sda线输出
	CLR_IIC1_SDA();//STOP:when CLK is high DATA change form low to high
	SET_IIC1_SCL(); 
  DELAY_US(2);
	SET_IIC1_SDA();//发送I2C总线结束信号
}

/**
 * @brief  IIC等待应答信号到来		
 * @param  None
 * @return 1，接收应答失败;0，接收应答成功
 */
uint8_t IIC1_Wait_Ack(void)
{
	uint32_t ucErrTime=0;
	IIC1_SDA_IN();     //SDA设置为输入  
	SET_IIC1_SDA();//DELAY_US(1);	   
	SET_IIC1_SCL();DELAY_US(1);	 
	while(GET_IIC1_SDA())
	{
    //	        DELAY_US(10);
    ucErrTime++;
    if(ucErrTime>250)
    {
      IIC1_Stop();
      //	CLR_IIC1_SCL();//时钟输出0 	
      return 1;
    }
	}
	CLR_IIC1_SCL();//时钟输出0 	   
	return 0;  
} 

/**
 * @brief  IIC产生ACK应答		
 * @param  None
 * @return None
 */
void IIC1_Ack(void)
{
	CLR_IIC1_SCL();
	IIC1_SDA_OUT();
	CLR_IIC1_SDA();
	DELAY_US(2);
	SET_IIC1_SCL();
	DELAY_US(2);
	CLR_IIC1_SCL();
}

/**
 * @brief  IIC不产生ACK应答		
 * @param  None
 * @return None
 */
void IIC1_NAck(void)
{
	CLR_IIC1_SCL();
  //	IIC2_SDA_OUT();
	IIC1_SDA_IN();
	DELAY_US(4);
	SET_IIC1_SCL();
	DELAY_US(2);
	CLR_IIC1_SCL();
}	

/**
 * @brief  IIC发送一个字节	
 * @param[in]  txd: 发送的数据
 * @return None
 */
void IIC1_Send_Byte(uint8_t txd)
{                        
  uint8_t t;   
  IIC1_SDA_OUT(); 	    
  CLR_IIC1_SCL();//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
    if(txd&0x80)
      SET_IIC1_SDA();
    else
      CLR_IIC1_SDA();
    txd<<=1; 	  
    DELAY_US(1);   //对TEA5767这三个延时都是必须的
    SET_IIC1_SCL();
    DELAY_US(1); 
    CLR_IIC1_SCL();	
  }	 
} 	    

/**
 * @brief  IIC读1个字节
 * @param[in]  ack: 1，发送ACK; 0，不发送nACK
 * @return 读到的数据
 */
uint8_t IIC1_Read_Byte(uint8_t ack)
{
  uint8_t i,receive=0;
  IIC1_SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
  {
    CLR_IIC1_SCL(); 
    DELAY_US(2);
    SET_IIC1_SCL();
    receive<<=1;
    if(GET_IIC1_SDA())receive++;   
    DELAY_US(1); 
  }					 
  if (!ack)
    IIC1_NAck();//不发送nACK
  else
    IIC1_Ack(); //发送ACK   
  return receive;
}

/**
 * @brief  初始化HDC1080
 * @param  None
 * @return None
 */
void HDC1080_Init(void)
{
  IIC1_Init();
}

/**
 * @brief  在HDC1080指定地址读出一个数据
 * @param[in]  ReadAddr:开始读数的地址  
 * @return 读到的数据
 */
uint32_t HDC1080_ReadOneByte(uint8_t ReadAddr)
{				  
  uint8_t data;
  uint32_t temp;
  
  SYSEnterCritical();////_DINT();              //关总中断
  
  IIC1_Start();
  
  IIC1_Send_Byte(0X80);    //发送器件地址0X80,写数据 	   
  IIC1_Wait_Ack(); 
  IIC1_Send_Byte(ReadAddr);   //发送低地址  (设置温度地址，则启动一次温度转换；设置湿度地址，则启动一次湿度转换)
  IIC1_Wait_Ack();	
  IIC1_Stop();//产生一个停止条件
  
  DELAY_MS(30);   //等待转换完毕 最少延时15ms
  
  IIC1_Start();    
  IIC1_Send_Byte(0X81);    //发送器件地址0XA0,读数据 	
  
  if(IIC1_Wait_Ack() != 0)
  {
    temp = 0xffff;
    SYSExitCritical();////_EINT();       //开总中断 
    return temp;
  }
  
  temp = IIC1_Read_Byte(1);
  data = IIC1_Read_Byte(0);	
  
  IIC1_Stop();//产生一个停止条件
  
  //    DELAY_MS(8);
  //    
  //    IIC_Start();
  //
  //    IIC_Send_Byte(0X80);    //发送器件地址0XA0,写数据 	   
  //    IIC_Wait_Ack(); 
  //    IIC_Send_Byte(ReadAddr);   //发送低地址
  //    IIC_Wait_Ack();	    
  //    IIC_Stop();//产生一个停止条件
  
  temp = (temp<<8)|data;
  
  SYSExitCritical();////_EINT();       //开总中断 
  return temp;
}

/**
 * @brief  在HDC1080指定地址写入一个数据
 * @param[in]  WriteAddr  :写入数据的目的地址 
 * @param[in]  DataToWrite:要写入的数据
 * @return None
 */
void HDC1080_WriteOneByte(uint8_t WriteAddr,uint32_t DataToWrite)
{	
  SYSEnterCritical();////_DINT();              //关总中断
  
  IIC1_Start();
  
  IIC1_Send_Byte(0X80);    //发送器件地址0XA0,写数据 	   
  IIC1_Wait_Ack(); 
  IIC1_Send_Byte(WriteAddr);   //发送低地址
  IIC1_Wait_Ack();	
  
  IIC1_Send_Byte(DataToWrite>>8);     //发送字节							   
  IIC1_Wait_Ack(); 
  IIC1_Send_Byte(DataToWrite&0xff);     //发送字节							   
  IIC1_Wait_Ack(); 
  
  IIC1_Stop();//产生一个停止条件 
  
  SYSExitCritical();////_EINT();       //开总中断  
}

/**
 * @brief  update temperature
 * @param  None
 * @return None
 */
void Hdc1080_UpdateTemperature(void)
{
  uint32_t ceshi;
  uint8_t i;
  
  for(i=0;i<3;i++)
  {
    ceshi = HDC1080_ReadOneByte(0x00);  
    if((ceshi != 0xffff))  //&&(ceshi != 0)  温度是0时，读取数据会是全0
    {
      i = 10;
      T_able = 0;  
    }
  }
  if(i <= 5) {
    T_able = 1;
    return;
  }
  
  s_i16TempVal = ((ceshi*16500/0xffff)-4000); //计算温度值,单位：0.01℃。
  
  //11.10.10增加温度限制 -10----85度
  if((s_i16TempVal > -1000)||(s_i16TempVal<8500)) {
    T_able=0;
  }
  else {
    T_able=1;
  }
}

/**
 * @brief  update humdity
 * @param  None
 * @return None
 */
void Hdc1080_UpdateHumidity(void)
{
  float humidity1;
  uint32_t ceshi;
  uint8_t i;
  
  for(i=0;i<3;i++)
  {
    ceshi = HDC1080_ReadOneByte(0x01);  
    if((ceshi != 0xffff)&&(ceshi != 0))
    {
      i = 10;
      H_able = 0;  
    }
  }
  if(i <= 5)
  {
    H_able = 1; 
    return;
  }
  
  humidity1 = (float)HDC1080_ReadOneByte(0x01); 
  s_u8HumidityVal = (uint8_t)(humidity1*100/0xffff); //计算湿度值 
}

/**
 * @brief   获取温度
 * @param   无
 * @retval  温度值，单位：0.01℃
 */
int16_t Hdc1080_GetTempVal(void)
{
  return s_i16TempVal;
}

/**
 * @brief   获取湿度
 * @param   无
 * @retval  湿度值，单位：%， 1-100.
 */
int16_t Hdc1080_GetHumidityVal(void)
{
  return s_u8HumidityVal;
}




/**
 * @}
 */



