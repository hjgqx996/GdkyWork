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
   
static int16_t s_i16TempVal    = 0;//�¶�ֵ����λ��0.01��
static uint8_t s_u8HumidityVal = 0;//ʪ��ֵ����λ��%�� 1-100.

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
 * @brief  ��ʼ��IIC
 * @param  None
 * @return None
 */
void IIC1_Init(void)
{
  SET_IIC1_SCL();    //ʱ�ӿ��ø�
  IIC1_SCL_OUT();    //ʱ�ӿ���Ϊ���
  SET_IIC1_SDA();    //���ݿ��ø�
  IIC1_SDA_OUT();    //���ݿ���Ϊ���
}

/**
 * @brief  ����IIC��ʼ�ź�		
 * @param  None
 * @return None
 */
void IIC1_Start(void)
{
	IIC1_SDA_OUT();     //sda�����
	SET_IIC1_SDA();  
	SET_IIC1_SCL();
	DELAY_US(2);
 	CLR_IIC1_SDA();//START:when CLK is high,DATA change form high to low 
	DELAY_US(2);
	CLR_IIC1_SCL();//ǯסI2C���ߣ�׼�����ͻ�������� 
}

/**
 * @brief  ����IICֹͣ�ź�	
 * @param  None
 * @return None
 */
void IIC1_Stop(void)
{
	IIC1_SDA_OUT();//sda�����
	CLR_IIC1_SDA();//STOP:when CLK is high DATA change form low to high
	SET_IIC1_SCL(); 
  DELAY_US(2);
	SET_IIC1_SDA();//����I2C���߽����ź�
}

/**
 * @brief  IIC�ȴ�Ӧ���źŵ���		
 * @param  None
 * @return 1������Ӧ��ʧ��;0������Ӧ��ɹ�
 */
uint8_t IIC1_Wait_Ack(void)
{
	uint32_t ucErrTime=0;
	IIC1_SDA_IN();     //SDA����Ϊ����  
	SET_IIC1_SDA();//DELAY_US(1);	   
	SET_IIC1_SCL();DELAY_US(1);	 
	while(GET_IIC1_SDA())
	{
    //	        DELAY_US(10);
    ucErrTime++;
    if(ucErrTime>250)
    {
      IIC1_Stop();
      //	CLR_IIC1_SCL();//ʱ�����0 	
      return 1;
    }
	}
	CLR_IIC1_SCL();//ʱ�����0 	   
	return 0;  
} 

/**
 * @brief  IIC����ACKӦ��		
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
 * @brief  IIC������ACKӦ��		
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
 * @brief  IIC����һ���ֽ�	
 * @param[in]  txd: ���͵�����
 * @return None
 */
void IIC1_Send_Byte(uint8_t txd)
{                        
  uint8_t t;   
  IIC1_SDA_OUT(); 	    
  CLR_IIC1_SCL();//����ʱ�ӿ�ʼ���ݴ���
  for(t=0;t<8;t++)
  {              
    if(txd&0x80)
      SET_IIC1_SDA();
    else
      CLR_IIC1_SDA();
    txd<<=1; 	  
    DELAY_US(1);   //��TEA5767��������ʱ���Ǳ����
    SET_IIC1_SCL();
    DELAY_US(1); 
    CLR_IIC1_SCL();	
  }	 
} 	    

/**
 * @brief  IIC��1���ֽ�
 * @param[in]  ack: 1������ACK; 0��������nACK
 * @return ����������
 */
uint8_t IIC1_Read_Byte(uint8_t ack)
{
  uint8_t i,receive=0;
  IIC1_SDA_IN();//SDA����Ϊ����
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
    IIC1_NAck();//������nACK
  else
    IIC1_Ack(); //����ACK   
  return receive;
}

/**
 * @brief  ��ʼ��HDC1080
 * @param  None
 * @return None
 */
void HDC1080_Init(void)
{
  IIC1_Init();
}

/**
 * @brief  ��HDC1080ָ����ַ����һ������
 * @param[in]  ReadAddr:��ʼ�����ĵ�ַ  
 * @return ����������
 */
uint32_t HDC1080_ReadOneByte(uint8_t ReadAddr)
{				  
  uint8_t data;
  uint32_t temp;
  
  SYSEnterCritical();////_DINT();              //�����ж�
  
  IIC1_Start();
  
  IIC1_Send_Byte(0X80);    //����������ַ0X80,д���� 	   
  IIC1_Wait_Ack(); 
  IIC1_Send_Byte(ReadAddr);   //���͵͵�ַ  (�����¶ȵ�ַ��������һ���¶�ת��������ʪ�ȵ�ַ��������һ��ʪ��ת��)
  IIC1_Wait_Ack();	
  IIC1_Stop();//����һ��ֹͣ����
  
  DELAY_MS(30);   //�ȴ�ת����� ������ʱ15ms
  
  IIC1_Start();    
  IIC1_Send_Byte(0X81);    //����������ַ0XA0,������ 	
  
  if(IIC1_Wait_Ack() != 0)
  {
    temp = 0xffff;
    SYSExitCritical();////_EINT();       //�����ж� 
    return temp;
  }
  
  temp = IIC1_Read_Byte(1);
  data = IIC1_Read_Byte(0);	
  
  IIC1_Stop();//����һ��ֹͣ����
  
  //    DELAY_MS(8);
  //    
  //    IIC_Start();
  //
  //    IIC_Send_Byte(0X80);    //����������ַ0XA0,д���� 	   
  //    IIC_Wait_Ack(); 
  //    IIC_Send_Byte(ReadAddr);   //���͵͵�ַ
  //    IIC_Wait_Ack();	    
  //    IIC_Stop();//����һ��ֹͣ����
  
  temp = (temp<<8)|data;
  
  SYSExitCritical();////_EINT();       //�����ж� 
  return temp;
}

/**
 * @brief  ��HDC1080ָ����ַд��һ������
 * @param[in]  WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ 
 * @param[in]  DataToWrite:Ҫд�������
 * @return None
 */
void HDC1080_WriteOneByte(uint8_t WriteAddr,uint32_t DataToWrite)
{	
  SYSEnterCritical();////_DINT();              //�����ж�
  
  IIC1_Start();
  
  IIC1_Send_Byte(0X80);    //����������ַ0XA0,д���� 	   
  IIC1_Wait_Ack(); 
  IIC1_Send_Byte(WriteAddr);   //���͵͵�ַ
  IIC1_Wait_Ack();	
  
  IIC1_Send_Byte(DataToWrite>>8);     //�����ֽ�							   
  IIC1_Wait_Ack(); 
  IIC1_Send_Byte(DataToWrite&0xff);     //�����ֽ�							   
  IIC1_Wait_Ack(); 
  
  IIC1_Stop();//����һ��ֹͣ���� 
  
  SYSExitCritical();////_EINT();       //�����ж�  
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
    if((ceshi != 0xffff))  //&&(ceshi != 0)  �¶���0ʱ����ȡ���ݻ���ȫ0
    {
      i = 10;
      T_able = 0;  
    }
  }
  if(i <= 5) {
    T_able = 1;
    return;
  }
  
  s_i16TempVal = ((ceshi*16500/0xffff)-4000); //�����¶�ֵ,��λ��0.01�档
  
  //11.10.10�����¶����� -10----85��
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
  s_u8HumidityVal = (uint8_t)(humidity1*100/0xffff); //����ʪ��ֵ 
}

/**
 * @brief   ��ȡ�¶�
 * @param   ��
 * @retval  �¶�ֵ����λ��0.01��
 */
int16_t Hdc1080_GetTempVal(void)
{
  return s_i16TempVal;
}

/**
 * @brief   ��ȡʪ��
 * @param   ��
 * @retval  ʪ��ֵ����λ��%�� 1-100.
 */
int16_t Hdc1080_GetHumidityVal(void)
{
  return s_u8HumidityVal;
}




/**
 * @}
 */



