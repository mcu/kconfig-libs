/******************************************************************************
 * @copyright Copyright (C) Free Software Foundation, Inc.
 * @author    Nima Askari
 *****************************************************************************/

#pragma once /****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  W25Q10 = 1,
  W25Q20,
  W25Q40,
  W25Q80,
  W25Q16,
  W25Q32,
  W25Q64,
  W25Q128,
  W25Q256,
  W25Q512,
} w25q_id_t;

typedef struct {
  w25q_id_t ID;
  uint8_t UniqID[8];
  uint16_t PageSize;
  uint32_t PageCount;
  uint32_t SectorSize;
  uint32_t SectorCount;
  uint32_t BlockSize;
  uint32_t BlockCount;
  uint32_t CapacityInKiloByte;
  uint8_t StatusRegister1;
  uint8_t StatusRegister2;
  uint8_t StatusRegister3;
  void (*hal_spi_chip_select)();
  void (*hal_spi_chip_deselect)();
  void (*hal_spi_transmit)(uint8_t *tx_data, uint16_t size);
  void (*hal_spi_receive)(uint8_t *rx_data, uint16_t size);
  uint8_t (*hal_spi_tr)(uint8_t tx_data); //spi transmit receive
  void (*delay_ms)(uint32_t ms);
  uint32_t (*time_ms)();
} w25q_cfg_t;

//In Page, Sector and block read/write functions, can put 0 to read maximum bytes
bool W25q_Init(w25q_cfg_t *w25q_cfg);

void W25q_EraseChip();
void W25q_EraseSector(uint32_t SectorAddr);
void W25q_EraseBlock(uint32_t BlockAddr);

uint32_t W25q_PageToSector(uint32_t PageAddress);
uint32_t W25q_PageToBlock(uint32_t PageAddress);
uint32_t W25q_SectorToBlock(uint32_t SectorAddress);
uint32_t W25q_SectorToPage(uint32_t SectorAddress);
uint32_t W25q_BlockToPage(uint32_t BlockAddress);

bool W25q_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize);
bool W25q_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize);
bool W25q_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize);

void W25q_WriteByte(uint8_t pBuffer, uint32_t Bytes_Address);
void W25q_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize);
void W25q_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
void W25q_WriteBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize);

void W25q_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address);
void W25q_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
void W25q_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize);
void W25q_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);
void W25q_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize);

#ifdef __cplusplus
}
#endif

/*****************************************************************************/
