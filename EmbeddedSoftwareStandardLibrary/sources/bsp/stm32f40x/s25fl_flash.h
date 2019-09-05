/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY All Rights Reserved
  *
  * @file     s25fl_flash.h
  * @author   ZouZH
  * @version  V1.00
  * @date     31-March-2017
  * @brief    s25fl132k s25fl164k spi flash driver.
  ******************************************************************************
  * @history
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __S25FL_FLASH_H
#define __S25FL_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* S25FL SPI Flash supported commands */
#define S25FL_CMD_WRITE          0x02  /* Write to Memory instruction */
#define S25FL_CMD_WRSR           0x01  /* Write Status Register instruction */
#define S25FL_CMD_WREN           0x06  /* Write enable instruction */
#define S25FL_CMD_READ           0x03  /* Read from Memory instruction */
#define S25FL_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define S25FL_CMD_RDID           0x9F  /* Read identification */
#define S25FL_CMD_SE             0x20  /* Sector Erase instruction */
#define S25FL_CMD_BLKE           0xD8  /* Block Erase instruction */
#define S25FL_CMD_BE             0xC7  /* Bulk Erase instruction */

/* SR1 */
#define S25FL_WIP_FLAG           0x01  /* Write In Progress (WIP) flag */
#define S25FL_PERR_FLAG          0x40  /* Programming Error Occurred */
#define S25FL_EERR_FLAG          0x20  /* Erase Error Occurred */

#define S25FL_DUMMY_BYTE         0xA5
#define S25FL_SPI_PAGESIZE       0x100  /* 256 Bytes*/
#define S25FL_SPI_CHIPSIZE       0x4000 /* 16M Bytes*/

#define S25FL_32K_ID             0x014016
#define S25FL_64K_ID             0x014017

#define S25FL_32K_SIZE           (4 * 1024 *1024)
#define S25FL_64K_SIZE           (8 * 1024 *1024)

#define S25FL_DEV_32K            0x00
#define S25FL_DEV_64K            0x01

/* S25FL FLASH SPI Interface pins  */
#define S25FL_SPI                           SPI1
#define S25FL_SPI_CLK                       RCC_APB2Periph_SPI1
#define S25FL_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define S25FL_SPI_SCK_PIN                   GPIO_Pin_5
#define S25FL_SPI_SCK_GPIO_PORT             GPIOA
#define S25FL_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define S25FL_SPI_SCK_SOURCE                GPIO_PinSource5
#define S25FL_SPI_SCK_AF                    GPIO_AF_SPI1

#define S25FL_SPI_MISO_PIN                  GPIO_Pin_6
#define S25FL_SPI_MISO_GPIO_PORT            GPIOA
#define S25FL_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define S25FL_SPI_MISO_SOURCE               GPIO_PinSource6
#define S25FL_SPI_MISO_AF                   GPIO_AF_SPI1

#define S25FL_SPI_MOSI_PIN                  GPIO_Pin_5
#define S25FL_SPI_MOSI_GPIO_PORT            GPIOB
#define S25FL_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define S25FL_SPI_MOSI_SOURCE               GPIO_PinSource5
#define S25FL_SPI_MOSI_AF                   GPIO_AF_SPI1

#define S25FL_CS_PIN_64K                    GPIO_Pin_8  /* S25FL164K */
#define S25FL_CS_PIN_32K                    GPIO_Pin_4  /* S25FL132K */
#define S25FL_CS_GPIO_PORT                  GPIOA
#define S25FL_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOA

#define S25FL_WP_PIN_32K                    GPIO_Pin_3
#define S25FL_WP_GPIO_PORT_32K              GPIOA
#define S25FL_WP_GPIO_CLK_32K               RCC_AHB1Periph_GPIOA

#define S25FL_WP_PIN_64K                    GPIO_Pin_8
#define S25FL_WP_GPIO_PORT_64K              GPIOC
#define S25FL_WP_GPIO_CLK_64K               RCC_AHB1Periph_GPIOC

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/* High layer functions  */
void     S25FL_DeInit(void);
uint8_t  S25FL_Init(void);
uint8_t  S25FL_EraseSector(uint8_t DevIndex, uint32_t SectorAddr);
uint8_t  S25FL_EraseBulk(uint8_t DevIndex);
uint8_t  S25FL_WritePage(uint8_t DevIndex, const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
uint8_t  S25FL_WriteBuffer(uint8_t DevIndex, const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void     S25FL_ReadBuffer(uint8_t DevIndex, uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t S25FL_ReadID(uint8_t DevIndex);
void     S25FL_StartReadSequence(uint8_t DevIndex, uint32_t ReadAddr);

/* Low layer functions */
void     S25FL_SetCS(uint8_t DevIndex, uint8_t PinLevel);
void     S25FL_WriteEnable(uint8_t DevIndex);
uint8_t  S25FL_WaitForWriteEnd(uint8_t DevIndex);

uint8_t  S25FL_SPIReadByte(void);
uint8_t  S25FL_SPISendByte(uint8_t byte);
uint16_t S25FL_SPISendHalfWord(uint16_t HalfWord);

#ifdef __cplusplus
}
#endif

#endif /* __S25FL_FLASH_H */


/************************ (C) COPYRIGHT GDKY *****END OF FILE****/
