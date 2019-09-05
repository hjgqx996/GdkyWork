/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY All Rights Reserved
  *
  * @file     s25fl_flash.c
  * @author   ZouZH
  * @version  V1.00
  * @date     31-March-2017
  * @brief    s25fl132k s25fl164k spi flash driver.
  ******************************************************************************
  * @history
  */

/* Includes ------------------------------------------------------------------*/
#include "s25fl_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define S25FL132K_WP_LOW()       GPIO_ResetBits(S25FL_WP_GPIO_PORT_32K, S25FL_WP_PIN_32K)
#define S25FL132K_WP_HIGH()      GPIO_SetBits(S25FL_WP_GPIO_PORT_32K, S25FL_WP_PIN_32K)

#define S25FL164K_WP_LOW()       GPIO_ResetBits(S25FL_WP_GPIO_PORT_64K, S25FL_WP_PIN_64K)
#define S25FL164K_WP_HIGH()      GPIO_SetBits(S25FL_WP_GPIO_PORT_64K, S25FL_WP_PIN_64K)


#define S25FL132K_CS_LOW()       GPIO_ResetBits(S25FL_CS_GPIO_PORT, S25FL_CS_PIN_32K)
#define S25FL132K_CS_HIGH()      GPIO_SetBits(S25FL_CS_GPIO_PORT, S25FL_CS_PIN_32K)

#define S25FL164K_CS_LOW()       GPIO_ResetBits(S25FL_CS_GPIO_PORT, S25FL_CS_PIN_64K)
#define S25FL164K_CS_HIGH()      GPIO_SetBits(S25FL_CS_GPIO_PORT, S25FL_CS_PIN_64K)


/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void S25FL_LowLevel_DeInit(void);
void S25FL_LowLevel_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void S25FL_DeInit(void)
{
  S25FL_LowLevel_DeInit();
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval 0 Success
  */
uint8_t S25FL_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  S25FL_LowLevel_Init();

  /*!< Deselect the FLASH: Chip Select high */
  S25FL132K_CS_HIGH();
  S25FL132K_WP_HIGH();

  S25FL164K_CS_HIGH();
  S25FL164K_WP_HIGH();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(S25FL_SPI, &SPI_InitStructure);

  /*!< Enable the S25FL_SPI  */
  SPI_Cmd(S25FL_SPI, ENABLE);

  return 0;
}

/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval 0 Success
  */
uint8_t S25FL_EraseSector(uint8_t DevIndex, uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  S25FL_WriteEnable(DevIndex);

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_LOW();
  else
    S25FL164K_CS_LOW();
  /*!< Send Sector Erase instruction */
  S25FL_SPISendByte(S25FL_CMD_SE);
  /*!< Send SectorAddr high nibble address byte */
  S25FL_SPISendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  S25FL_SPISendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  S25FL_SPISendByte(SectorAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_HIGH();
  else
    S25FL164K_CS_HIGH();

  /*!< Wait the end of Flash writing */
  if (S25FL_WaitForWriteEnd(DevIndex) & S25FL_EERR_FLAG)
    return 1;

  return 0;
}

/**
  * @brief  Erases the entire FLASH.
  * @param  None
  * @retval 0 Success
  */
uint8_t S25FL_EraseBulk(uint8_t DevIndex)
{
  /*!< Send write enable instruction */
  S25FL_WriteEnable(DevIndex);

  /*!< Bulk Erase */
  /*!< Select the FLASH: Chip Select low */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_LOW();
  else
    S25FL164K_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  S25FL_SPISendByte(S25FL_CMD_BE);
  /*!< Deselect the FLASH: Chip Select high */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_HIGH();
  else
    S25FL164K_CS_HIGH();

  /*!< Wait the end of Flash writing */
  if (S25FL_WaitForWriteEnd(DevIndex) & S25FL_EERR_FLAG)
    return 1;

  return 0;
}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "S25FL_PAGESIZE" value.
  * @retval 0 Success
  */
uint8_t S25FL_WritePage(uint8_t DevIndex, const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  S25FL_WriteEnable(DevIndex);

  /*!< Select the FLASH: Chip Select low */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_LOW();
  else
    S25FL164K_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  S25FL_SPISendByte(S25FL_CMD_WRITE);
  /*!< Send WriteAddr high nibble address byte to write to */
  S25FL_SPISendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  S25FL_SPISendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  S25FL_SPISendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    S25FL_SPISendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_HIGH();
  else
    S25FL164K_CS_HIGH();

  /*!< Wait the end of Flash writing */
  if (S25FL_WaitForWriteEnd(DevIndex) & S25FL_PERR_FLAG)
    return 1;

  return 0;
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval 0 Success
  */
uint8_t S25FL_WriteBuffer(uint8_t DevIndex, const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  uint8_t ret = 0;

  Addr = WriteAddr % S25FL_SPI_PAGESIZE;
  count = S25FL_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / S25FL_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % S25FL_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is S25FL_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < S25FL_PAGESIZE */
    {
      ret = S25FL_WritePage(DevIndex, pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > S25FL_PAGESIZE */
    {
      while (!ret && NumOfPage--)
      {
        ret = S25FL_WritePage(DevIndex, pBuffer, WriteAddr, S25FL_SPI_PAGESIZE);
        WriteAddr +=  S25FL_SPI_PAGESIZE;
        pBuffer += S25FL_SPI_PAGESIZE;
      }

      ret = S25FL_WritePage(DevIndex, pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not S25FL_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < S25FL_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > S25FL_PAGESIZE */
      {
        temp = NumOfSingle - count;

        ret = S25FL_WritePage(DevIndex, pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        S25FL_WritePage(DevIndex, pBuffer, WriteAddr, temp);
      }
      else
      {
        ret = S25FL_WritePage(DevIndex, pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > S25FL_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / S25FL_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % S25FL_SPI_PAGESIZE;

      ret = S25FL_WritePage(DevIndex, pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        ret = S25FL_WritePage(DevIndex, pBuffer, WriteAddr, S25FL_SPI_PAGESIZE);
        WriteAddr +=  S25FL_SPI_PAGESIZE;
        pBuffer += S25FL_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        ret = S25FL_WritePage(DevIndex, pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }

  return ret;
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void S25FL_ReadBuffer(uint8_t DevIndex, uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_LOW();
  else
    S25FL164K_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  S25FL_SPISendByte(S25FL_CMD_READ);

  /*!< Send ReadAddr high nibble address byte to read from */
  S25FL_SPISendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  S25FL_SPISendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  S25FL_SPISendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = S25FL_SPISendByte(S25FL_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_HIGH();
  else
    S25FL164K_CS_HIGH();
}

/**
  * @brief  Reads FLASH identification.
  * @param  None
  * @retval FLASH identification
  */
uint32_t S25FL_ReadID(uint8_t DevIndex)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the FLASH: Chip Select low */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_LOW();
  else
    S25FL164K_CS_LOW();

  /*!< Send "RDID " instruction */
  S25FL_SPISendByte(0x9F);

  /*!< Read a byte from the FLASH */
  Temp0 = S25FL_SPISendByte(S25FL_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp1 = S25FL_SPISendByte(S25FL_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp2 = S25FL_SPISendByte(S25FL_DUMMY_BYTE);

  /*!< Deselect the FLASH: Chip Select high */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_HIGH();
  else
    S25FL164K_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/**
  * @brief  Initiates a read data byte (READ) sequence from the Flash.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the Flash still being selected. With this
  *   technique the whole content of the Flash is read with a single READ instruction.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @retval None
  */
void S25FL_StartReadSequence(uint8_t DevIndex, uint32_t ReadAddr)
{
  /*!< Select the FLASH: Chip Select low */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_LOW();
  else
    S25FL164K_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  S25FL_SPISendByte(S25FL_CMD_READ);

  /*!< Send the 24-bit address of the address to read from -------------------*/
  /*!< Send ReadAddr high nibble address byte */
  S25FL_SPISendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte */
  S25FL_SPISendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte */
  S25FL_SPISendByte(ReadAddr & 0xFF);
}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
uint8_t S25FL_SPIReadByte(void)
{
  return (S25FL_SPISendByte(S25FL_DUMMY_BYTE));
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t S25FL_SPISendByte(uint8_t byte)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(S25FL_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(S25FL_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(S25FL_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(S25FL_SPI);
}

/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
uint16_t S25FL_SPISendHalfWord(uint16_t HalfWord)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(S25FL_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send Half Word through the S25FL peripheral */
  SPI_I2S_SendData(S25FL_SPI, HalfWord);

  /*!< Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(S25FL_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(S25FL_SPI);
}

/**
  * @brief  Set the FLASH CS Pin.
  * @param  None
  * @retval None
  */
void S25FL_SetCS(uint8_t DevIndex, uint8_t PinLevel)
{
  /*!< Select the FLASH: Chip Select low */
  if (!PinLevel)
  {
    if (DevIndex == S25FL_DEV_32K)
      S25FL132K_CS_LOW();
    else
      S25FL164K_CS_LOW();
  }
  else
  {
    /*!< Deselect the FLASH: Chip Select high */
    if (DevIndex == S25FL_DEV_32K)
      S25FL132K_CS_HIGH();
    else
      S25FL164K_CS_HIGH();
  }
}


/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void S25FL_WriteEnable(uint8_t DevIndex)
{
  /*!< Select the FLASH: Chip Select low */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_LOW();
  else
    S25FL164K_CS_LOW();

  /*!< Send "Write Enable" instruction */
  S25FL_SPISendByte(S25FL_CMD_WREN);

  /*!< Deselect the FLASH: Chip Select high */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_HIGH();
  else
    S25FL164K_CS_HIGH();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write operation has completed.
  * @param  None
  * @retval SR1 value
  */
uint8_t S25FL_WaitForWriteEnd(uint8_t DevIndex)
{
  uint8_t flashstatus = 0;

  __IO uint32_t tmout = 0x00FFFFFF;

  /*!< Select the FLASH: Chip Select low */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_LOW();
  else
    S25FL164K_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  S25FL_SPISendByte(S25FL_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = S25FL_SPISendByte(S25FL_DUMMY_BYTE);
    if (--tmout == 0)
    {
      flashstatus = 0xFF;
      break;
    }
  }
  while ((flashstatus & S25FL_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  if (DevIndex == S25FL_DEV_32K)
    S25FL132K_CS_HIGH();
  else
    S25FL164K_CS_HIGH();

  return flashstatus;
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void S25FL_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Enable the SPI clock */
  S25FL_SPI_CLK_INIT(S25FL_SPI_CLK, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(S25FL_SPI_SCK_GPIO_CLK | S25FL_SPI_MISO_GPIO_CLK |
                         S25FL_SPI_MOSI_GPIO_CLK | S25FL_CS_GPIO_CLK |
                         S25FL_WP_GPIO_CLK_32K | S25FL_WP_GPIO_CLK_64K, ENABLE);

  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF5 */
  GPIO_PinAFConfig(S25FL_SPI_SCK_GPIO_PORT, S25FL_SPI_SCK_SOURCE, S25FL_SPI_SCK_AF);
  GPIO_PinAFConfig(S25FL_SPI_MISO_GPIO_PORT, S25FL_SPI_MISO_SOURCE, S25FL_SPI_MISO_AF);
  GPIO_PinAFConfig(S25FL_SPI_MOSI_GPIO_PORT, S25FL_SPI_MOSI_SOURCE, S25FL_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = S25FL_SPI_SCK_PIN;
  GPIO_Init(S25FL_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  S25FL_SPI_MOSI_PIN;
  GPIO_Init(S25FL_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  S25FL_SPI_MISO_PIN;
  GPIO_Init(S25FL_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure S25FL Card CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = S25FL_CS_PIN_32K | S25FL_CS_PIN_64K;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(S25FL_CS_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_InitStructure.GPIO_Pin = S25FL_WP_PIN_32K;
  GPIO_Init(S25FL_WP_GPIO_PORT_32K, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = S25FL_WP_PIN_64K;
  GPIO_Init(S25FL_WP_GPIO_PORT_64K, &GPIO_InitStructure);
}

/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void S25FL_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Disable the S25FL_SPI  ************************************************/
  SPI_Cmd(S25FL_SPI, DISABLE);

  /*!< DeInitializes the S25FL_SPI *******************************************/
  SPI_I2S_DeInit(S25FL_SPI);

  /*!< S25FL_SPI Periph clock disable ****************************************/
  S25FL_SPI_CLK_INIT(S25FL_SPI_CLK, DISABLE);

  /*!< Configure all pins used by the SPI as input floating *******************/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = S25FL_SPI_SCK_PIN;
  GPIO_Init(S25FL_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = S25FL_SPI_MISO_PIN;
  GPIO_Init(S25FL_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = S25FL_SPI_MOSI_PIN;
  GPIO_Init(S25FL_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = S25FL_CS_PIN_32K | S25FL_CS_PIN_64K;
  GPIO_Init(S25FL_CS_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = S25FL_WP_PIN_32K;
  GPIO_Init(S25FL_WP_GPIO_PORT_32K, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = S25FL_WP_PIN_64K;
  GPIO_Init(S25FL_WP_GPIO_PORT_64K, &GPIO_InitStructure);
}

/************************ (C) COPYRIGHT GDKY *****END OF FILE****/
