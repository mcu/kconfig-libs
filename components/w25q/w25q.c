/******************************************************************************
 * @copyright Copyright (C) Free Software Foundation, Inc.
 * @author    Nima Askari
 *****************************************************************************/

#pragma GCC diagnostic ignored "-Wunused-function"

#include "w25q.h"
#include <string.h>

#define W25Q_DEBUG  1
#define W25Q_DUMMY_BYTE  0xA5

static w25q_cfg_t w25q;

#if (W25Q_DEBUG)
  #include <stdio.h>
#endif

/*****************************************************************************/

static uint32_t W25q_ReadID()
{
  uint32_t id[3] = {0};

  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x9F);

    id[0] = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
    id[1] = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
    id[2] = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
  }
  w25q.hal_spi_chip_deselect();

  return (id[0] << 16) | (id[1] << 8) | id[2];
}

/*****************************************************************************/

static void W25q_ReadUniqID()
{
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x4B);

    for(uint8_t i = 0; i < 4; i++)
    {
      w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
    }

    for(uint8_t i = 0; i < 8; i++)
    {
      w25q.UniqID[i] = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
    }
  }
  w25q.hal_spi_chip_deselect();
}

/*****************************************************************************/

static void W25q_WriteEnable()
{
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x06);
  }
  w25q.hal_spi_chip_deselect();

  w25q.delay_ms(1);
}

/*****************************************************************************/

static void W25q_WriteDisable()
{
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x04);
  }
  w25q.hal_spi_chip_deselect();

  w25q.delay_ms(1);
}

/*****************************************************************************/

static uint8_t W25q_ReadStatusRegister(uint8_t SelectStatusRegister_1_2_3)
{
  uint8_t status = 0;

  w25q.hal_spi_chip_select();
  {
    if(SelectStatusRegister_1_2_3 == 1)
    {
      w25q.hal_spi_tr(0x05);
      status = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
      w25q.StatusRegister1 = status;
    }
    else if(SelectStatusRegister_1_2_3 == 2)
    {
      w25q.hal_spi_tr(0x35);
      status = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
      w25q.StatusRegister2 = status;
    }
    else
    {
      w25q.hal_spi_tr(0x15);
      status = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
      w25q.StatusRegister3 = status;
    }
  }
  w25q.hal_spi_chip_deselect();

  return status;
}

/*****************************************************************************/

static void W25q_WriteStatusRegister(uint8_t SelectStatusRegister_1_2_3, uint8_t Data)
{
  w25q.hal_spi_chip_select();
  {
    if(SelectStatusRegister_1_2_3 == 1)
    {
      w25q.hal_spi_tr(0x01);
      w25q.StatusRegister1 = Data;
    }
    else if(SelectStatusRegister_1_2_3 == 2)
    {
      w25q.hal_spi_tr(0x31);
      w25q.StatusRegister2 = Data;
    }
    else
    {
      w25q.hal_spi_tr(0x11);
      w25q.StatusRegister3 = Data;
    }
    w25q.hal_spi_tr(Data);
  }
  w25q.hal_spi_chip_deselect();
}

/*****************************************************************************/

static void W25q_WaitForWriteEnd()
{
  w25q.delay_ms(1);

  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x05);
    do
    {
      w25q.StatusRegister1 = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
      w25q.delay_ms(1);
    } while((w25q.StatusRegister1 & 0x01) == 0x01);
  }
  w25q.hal_spi_chip_deselect();
}

/*****************************************************************************/

static void W25q_WriteAddress(uint32_t address)
{
  if(w25q.ID >= W25Q256)
    w25q.hal_spi_tr((address & 0xFF000000) >> 24);
  w25q.hal_spi_tr((address & 0xFF0000) >> 16);
  w25q.hal_spi_tr((address & 0xFF00) >> 8);
  w25q.hal_spi_tr(address & 0xFF);
}

/*****************************************************************************/

bool W25q_Init(w25q_cfg_t *w25q_cfg)
{
  memcpy(&w25q, w25q_cfg, sizeof(w25q_cfg_t));

  w25q.delay_ms(100);
  w25q.hal_spi_chip_deselect();
  w25q.delay_ms(100);

  const struct {
    uint16_t chip_id;
    w25q_id_t appl_id;
    uint32_t block_count;
    const char *name;
  } w25q_list[] = {
    {0x401A, W25Q512, 1024, "w25q512"},
    {0x4019, W25Q256, 512,  "w25q256"},
    {0x4018, W25Q128, 256,  "w25q128"},
    {0x4017, W25Q64,  128,  "w25q64" },
    {0x4016, W25Q32,  64,   "w25q32" },
    {0x4015, W25Q16,  32,   "w25q16" },
    {0x4014, W25Q80,  16,   "w25q80" },
    {0x4013, W25Q40,  8,    "w25q40" },
    {0x4012, W25Q20,  4,    "w25q20" },
    {0x4011, W25Q10,  2,    "w25q10" },
  };

#if (W25Q_DEBUG)
  printf("w25q: Init Begin...\r\n");
#endif

  uint32_t id = W25q_ReadID();
#if (W25Q_DEBUG)
  printf("w25q: ID: 0x%lX\r\n", id);
#endif

  bool unknown_id = true;
  for(uint32_t i = 0; i < sizeof(w25q_list)/sizeof(w25q_list[0]); i++)
  {
    if((id & 0x0000FFFF) == w25q_list[i].chip_id)
    {
      w25q.ID = w25q_list[i].appl_id;
      w25q.BlockCount = w25q_list[i].block_count;
#if (W25Q_DEBUG)
      printf("w25q: Chip: %s\r\n", w25q_list[i].name);
#endif
      unknown_id = false;
    }
  }

  if(unknown_id)
  {
#if (W25Q_DEBUG)
    printf("w25q: Unknown ID\r\n");
#endif
    return false;
  }

  w25q.PageSize = 256;
  w25q.SectorSize = 0x1000;
  w25q.SectorCount = w25q.BlockCount * 16;
  w25q.PageCount = (w25q.SectorCount * w25q.SectorSize) / w25q.PageSize;
  w25q.BlockSize = w25q.SectorSize * 16;
  w25q.CapacityInKiloByte = (w25q.SectorCount * w25q.SectorSize) / 1024;
  W25q_ReadUniqID();
  W25q_ReadStatusRegister(1);
  W25q_ReadStatusRegister(2);
  W25q_ReadStatusRegister(3);
#if (W25Q_DEBUG)
  printf("w25q: Page Size: %d Bytes\r\n", w25q.PageSize);
  printf("w25q: Page Count: %ld\r\n", w25q.PageCount);
  printf("w25q: Sector Size: %ld Bytes\r\n", w25q.SectorSize);
  printf("w25q: Sector Count: %ld\r\n", w25q.SectorCount);
  printf("w25q: Block Size: %ld Bytes\r\n", w25q.BlockSize);
  printf("w25q: Block Count: %ld\r\n", w25q.BlockCount);
  printf("w25q: Capacity: %ld KiloBytes\r\n", w25q.CapacityInKiloByte);
  printf("w25q: Init Done\r\n");
#endif
  return true;
}

/*****************************************************************************/

void W25q_EraseChip()
{
#if (W25Q_DEBUG)
  uint32_t StartTime = w25q.time_ms();
  printf("w25q: EraseChip Begin...\r\n");
#endif
  W25q_WriteEnable();
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0xC7);
  }
  w25q.hal_spi_chip_deselect();
  W25q_WaitForWriteEnd();
#if (W25Q_DEBUG)
  printf("w25q: EraseBlock done after %ld ms!\r\n", w25q.time_ms() - StartTime);
#endif
  w25q.delay_ms(10);
}

/*****************************************************************************/

void W25q_EraseSector(uint32_t SectorAddr)
{
#if (W25Q_DEBUG)
  uint32_t StartTime = w25q.time_ms();
  printf("w25q: EraseSector %ld Begin...\r\n", SectorAddr);
#endif
  W25q_WaitForWriteEnd();
  SectorAddr = SectorAddr * w25q.SectorSize;
  W25q_WriteEnable();
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x20);
    W25q_WriteAddress(SectorAddr);
  }
  w25q.hal_spi_chip_deselect();
  W25q_WaitForWriteEnd();
#if (W25Q_DEBUG)
  printf("w25q: EraseSector done after %ld ms\r\n", w25q.time_ms() - StartTime);
#endif
  w25q.delay_ms(1);
}

/*****************************************************************************/

void W25q_EraseBlock(uint32_t BlockAddr)
{
#if (W25Q_DEBUG)
  printf("w25q: EraseBlock %ld Begin...\r\n", BlockAddr);
  w25q.delay_ms(100);
  uint32_t StartTime = w25q.time_ms();
#endif
  W25q_WaitForWriteEnd();
  BlockAddr = BlockAddr * w25q.SectorSize * 16;
  W25q_WriteEnable();
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0xD8);
    W25q_WriteAddress(BlockAddr);
  }
  w25q.hal_spi_chip_deselect();
  W25q_WaitForWriteEnd();
#if (W25Q_DEBUG)
  printf("w25q: EraseBlock done after %ld ms\r\n", w25q.time_ms() - StartTime);
  w25q.delay_ms(100);
#endif
  w25q.delay_ms(1);
}

/*****************************************************************************/

uint32_t W25q_PageToSector(uint32_t PageAddress)
{
  return ((PageAddress * w25q.PageSize) / w25q.SectorSize);
}

/*****************************************************************************/

uint32_t W25q_PageToBlock(uint32_t PageAddress)
{
  return ((PageAddress * w25q.PageSize) / w25q.BlockSize);
}

/*****************************************************************************/

uint32_t W25q_SectorToBlock(uint32_t SectorAddress)
{
  return ((SectorAddress * w25q.SectorSize) / w25q.BlockSize);
}

/*****************************************************************************/

uint32_t W25q_SectorToPage(uint32_t SectorAddress)
{
  return (SectorAddress * w25q.SectorSize) / w25q.PageSize;
}

/*****************************************************************************/

uint32_t W25q_BlockToPage(uint32_t BlockAddress)
{
  return (BlockAddress * w25q.BlockSize) / w25q.PageSize;
}

/*****************************************************************************/

bool W25q_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte,
                        uint32_t NumByteToCheck_up_to_PageSize)
{
  if(((NumByteToCheck_up_to_PageSize + OffsetInByte) > w25q.PageSize) ||
     (NumByteToCheck_up_to_PageSize == 0))
  {
    NumByteToCheck_up_to_PageSize = w25q.PageSize - OffsetInByte;
  }
#if (W25Q_DEBUG)
  printf("w25q: CheckPage %ld, Offset %ld, Bytes %ld begin...\r\n", Page_Address, OffsetInByte, NumByteToCheck_up_to_PageSize);
  w25q.delay_ms(100);
  uint32_t StartTime = w25q.time_ms();
#endif
  uint8_t pBuffer[32];
  uint32_t WorkAddress;
  uint32_t i;
  for(i = OffsetInByte; i < w25q.PageSize; i += sizeof(pBuffer))
  {
    w25q.hal_spi_chip_select();
    {
      WorkAddress = (i + Page_Address * w25q.PageSize);
      w25q.hal_spi_tr(0x0B);
      W25q_WriteAddress(WorkAddress);
      w25q.hal_spi_tr(0);
      w25q.hal_spi_receive(pBuffer, sizeof(pBuffer));
    }
    w25q.hal_spi_chip_deselect();
    for(uint8_t x = 0; x < sizeof(pBuffer); x++)
    {
      if(pBuffer[x] != 0xFF)
      {
        goto NOT_EMPTY;
      }
    }
  }
  if((w25q.PageSize + OffsetInByte) % sizeof(pBuffer) != 0)
  {
    i -= sizeof(pBuffer);
    for(; i < w25q.PageSize; i++)
    {
      w25q.hal_spi_chip_select();
      {
        WorkAddress = (i + Page_Address * w25q.PageSize);
        w25q.hal_spi_tr(0x0B);
        W25q_WriteAddress(WorkAddress);
        w25q.hal_spi_tr(0);
        w25q.hal_spi_receive(pBuffer, 1);
      }
      w25q.hal_spi_chip_deselect();
      if(pBuffer[0] != 0xFF)
      {
        goto NOT_EMPTY;
      }
    }
  }
#if (W25Q_DEBUG)
  printf("w25q: CheckPage is Empty in %ld ms\r\n", w25q.time_ms() - StartTime);
  w25q.delay_ms(100);
#endif
  return true;

  NOT_EMPTY:
#if (W25Q_DEBUG)
  printf("w25q: CheckPage is Not Empty in %ld ms\r\n", w25q.time_ms() - StartTime);
  w25q.delay_ms(100);
#endif
  return false;
}

/*****************************************************************************/

bool W25q_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte,
                          uint32_t NumByteToCheck_up_to_SectorSize)
{
  if((NumByteToCheck_up_to_SectorSize > w25q.SectorSize) ||
     (NumByteToCheck_up_to_SectorSize == 0))
  {
    NumByteToCheck_up_to_SectorSize = w25q.SectorSize;
  }
#if (W25Q_DEBUG)
  printf("w25q: CheckSector %ld, Offset %ld, Bytes %ld begin...\r\n", Sector_Address, OffsetInByte, NumByteToCheck_up_to_SectorSize);
  w25q.delay_ms(100);
  uint32_t StartTime = w25q.time_ms();
#endif
  uint8_t pBuffer[32];
  uint32_t WorkAddress;
  uint32_t i;
  for(i = OffsetInByte; i < w25q.SectorSize; i += sizeof(pBuffer))
  {
    w25q.hal_spi_chip_select();
    {
      WorkAddress = (i + Sector_Address * w25q.SectorSize);
      w25q.hal_spi_tr(0x0B);
      W25q_WriteAddress(WorkAddress);
      w25q.hal_spi_tr(0);
      w25q.hal_spi_receive(pBuffer, sizeof(pBuffer));
    }
    w25q.hal_spi_chip_deselect();
    for(uint8_t x = 0; x < sizeof(pBuffer); x++)
    {
      if(pBuffer[x] != 0xFF)
      {
        goto NOT_EMPTY;
      }
    }
  }
  if((w25q.SectorSize + OffsetInByte) % sizeof(pBuffer) != 0)
  {
    i -= sizeof(pBuffer);
    for(; i < w25q.SectorSize; i++)
    {
      w25q.hal_spi_chip_select();
      {
        WorkAddress = (i + Sector_Address * w25q.SectorSize);
        w25q.hal_spi_tr(0x0B);
        W25q_WriteAddress(WorkAddress);
        w25q.hal_spi_tr(0);
        w25q.hal_spi_receive(pBuffer, 1);
      }
      w25q.hal_spi_chip_deselect();
      if(pBuffer[0] != 0xFF)
      {
        goto NOT_EMPTY;
      }
    }
  }
#if (W25Q_DEBUG)
  printf("w25q: CheckSector is Empty in %ld ms\r\n", w25q.time_ms() - StartTime);
  w25q.delay_ms(100);
#endif
  return true;

  NOT_EMPTY:
#if (W25Q_DEBUG)
  printf("w25q: CheckSector is Not Empty in %ld ms\r\n", w25q.time_ms() - StartTime);
  w25q.delay_ms(100);
#endif
  return false;
}
/*****************************************************************************/

bool W25q_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte,
                         uint32_t NumByteToCheck_up_to_BlockSize)
{
  if((NumByteToCheck_up_to_BlockSize > w25q.BlockSize) ||
     (NumByteToCheck_up_to_BlockSize == 0))
  {
    NumByteToCheck_up_to_BlockSize = w25q.BlockSize;
  }
#if (W25Q_DEBUG)
  printf("w25q: CheckBlock %ld, Offset %ld, Bytes %ld begin...\r\n", Block_Address, OffsetInByte, NumByteToCheck_up_to_BlockSize);
  w25q.delay_ms(100);
  uint32_t StartTime = w25q.time_ms();
#endif
  uint8_t pBuffer[32];
  uint32_t WorkAddress;
  uint32_t i;
  for(i = OffsetInByte; i < w25q.BlockSize; i += sizeof(pBuffer))
  {
    w25q.hal_spi_chip_select();
    {
      WorkAddress = (i + Block_Address * w25q.BlockSize);
      w25q.hal_spi_tr(0x0B);
      W25q_WriteAddress(WorkAddress);
      w25q.hal_spi_tr(0);
      w25q.hal_spi_receive(pBuffer, sizeof(pBuffer));
    }
    w25q.hal_spi_chip_deselect();
    for(uint8_t x = 0; x < sizeof(pBuffer); x++)
    {
      if(pBuffer[x] != 0xFF)
      {
        goto NOT_EMPTY;
      }
    }
  }
  if((w25q.BlockSize + OffsetInByte) % sizeof(pBuffer) != 0)
  {
    i -= sizeof(pBuffer);
    for(; i < w25q.BlockSize; i++)
    {
      w25q.hal_spi_chip_select();
      {
        WorkAddress = (i + Block_Address * w25q.BlockSize);
        w25q.hal_spi_tr(0x0B);
        W25q_WriteAddress(WorkAddress);
        w25q.hal_spi_tr(0);
        w25q.hal_spi_receive(pBuffer, 1);
      }
      w25q.hal_spi_chip_deselect();
      if(pBuffer[0] != 0xFF)
      {
        goto NOT_EMPTY;
      }
    }
  }
#if (W25Q_DEBUG)
  printf("w25q: CheckBlock is Empty in %ld ms\r\n", w25q.time_ms() - StartTime);
  w25q.delay_ms(100);
#endif
  return true;

  NOT_EMPTY:
#if (W25Q_DEBUG)
  printf("w25q: CheckBlock is Not Empty in %ld ms\r\n", w25q.time_ms() - StartTime);
  w25q.delay_ms(100);
#endif
  return false;
}

/*****************************************************************************/

void W25q_WriteByte(uint8_t pBuffer, uint32_t WriteAddr_inBytes)
{
#if (W25Q_DEBUG)
  uint32_t StartTime = w25q.time_ms();
  printf("w25q: WriteByte 0x%02X at address %ld begin...", pBuffer, WriteAddr_inBytes);
#endif
  W25q_WaitForWriteEnd();
  W25q_WriteEnable();
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x02);
    W25q_WriteAddress(WriteAddr_inBytes);
    w25q.hal_spi_tr(pBuffer);
  }
  w25q.hal_spi_chip_deselect();
  W25q_WaitForWriteEnd();
#if (W25Q_DEBUG)
  printf("w25q: WriteByte done after %ld ms\r\n", w25q.time_ms() - StartTime);
#endif
}

/*****************************************************************************/

void W25q_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte,
                      uint32_t NumByteToWrite_up_to_PageSize)
{
  if(((NumByteToWrite_up_to_PageSize + OffsetInByte) > w25q.PageSize) ||
     (NumByteToWrite_up_to_PageSize == 0))
  {
    NumByteToWrite_up_to_PageSize = w25q.PageSize - OffsetInByte;
  }
  if((OffsetInByte + NumByteToWrite_up_to_PageSize) > w25q.PageSize)
  {
    NumByteToWrite_up_to_PageSize = w25q.PageSize - OffsetInByte;
  }
#if (W25Q_DEBUG)
  printf("w25q: WritePage %ld, Offset %ld, Writes %ld Bytes, begin...\r\n", Page_Address, OffsetInByte, NumByteToWrite_up_to_PageSize);
  w25q.delay_ms(100);
  uint32_t StartTime = w25q.time_ms();
#endif
  W25q_WaitForWriteEnd();
  W25q_WriteEnable();
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x02);
    Page_Address = (Page_Address * w25q.PageSize) + OffsetInByte;
    W25q_WriteAddress(Page_Address);
    w25q.hal_spi_transmit(pBuffer, NumByteToWrite_up_to_PageSize);
  }
  w25q.hal_spi_chip_deselect();
  W25q_WaitForWriteEnd();
#if (W25Q_DEBUG)
  StartTime = w25q.time_ms() - StartTime;
  for(uint32_t i=0;i<NumByteToWrite_up_to_PageSize ; i++)
  {
    if((i%8 == 0) && (i > 2))
    {
      printf("\r\n");
      w25q.delay_ms(10);
    }
    printf("0x%02X,", pBuffer[i]);
  }
  printf("\r\n");
  printf("w25q: WritePage done after %ld ms\r\n", StartTime);
  w25q.delay_ms(100);
#endif
  w25q.delay_ms(1);
}

/*****************************************************************************/

void W25q_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte,
                        uint32_t NumByteToWrite_up_to_SectorSize)
{
  if((NumByteToWrite_up_to_SectorSize > w25q.SectorSize) ||
     (NumByteToWrite_up_to_SectorSize == 0))
  {
    NumByteToWrite_up_to_SectorSize = w25q.SectorSize;
  }
#if (W25Q_DEBUG)
  printf("+++w25q: WriteSector %ld, Offset %ld, Write %ld Bytes, begin...\r\n", Sector_Address, OffsetInByte, NumByteToWrite_up_to_SectorSize);
  w25q.delay_ms(100);
#endif
  if(OffsetInByte >= w25q.SectorSize)
  {
#if (W25Q_DEBUG)
    printf("---w25q: WriteSector Faild!\r\n");
    w25q.delay_ms(100);
#endif
    return;
  }
  uint32_t StartPage;
  int32_t BytesToWrite;
  uint32_t LocalOffset;
  if((OffsetInByte + NumByteToWrite_up_to_SectorSize) > w25q.SectorSize)
    BytesToWrite = w25q.SectorSize - OffsetInByte;
  else
    BytesToWrite = NumByteToWrite_up_to_SectorSize;
  StartPage = W25q_SectorToPage(Sector_Address) + (OffsetInByte / w25q.PageSize);
  LocalOffset = OffsetInByte % w25q.PageSize;
  do
  {
    W25q_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
    StartPage++;
    BytesToWrite -= w25q.PageSize - LocalOffset;
    pBuffer += w25q.PageSize - LocalOffset;
    LocalOffset = 0;
  } while(BytesToWrite > 0);
#if (W25Q_DEBUG)
  printf("---w25q: WriteSector Done\r\n");
  w25q.delay_ms(100);
#endif
}

/*****************************************************************************/

void W25q_WriteBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte,
                       uint32_t NumByteToWrite_up_to_BlockSize)
{
  if((NumByteToWrite_up_to_BlockSize > w25q.BlockSize) ||
     (NumByteToWrite_up_to_BlockSize == 0))
  {
    NumByteToWrite_up_to_BlockSize = w25q.BlockSize;
  }
#if (W25Q_DEBUG)
  printf("+++w25q: WriteBlock %ld, Offset: %ld, Write %ld Bytes, begin...\r\n", Block_Address, OffsetInByte, NumByteToWrite_up_to_BlockSize);
  w25q.delay_ms(100);
#endif
  if(OffsetInByte >= w25q.BlockSize)
  {
#if (W25Q_DEBUG)
    printf("---w25q: WriteBlock Faild!\r\n");
    w25q.delay_ms(100);
#endif
    return;
  }
  uint32_t StartPage;
  int32_t BytesToWrite;
  uint32_t LocalOffset;
  if((OffsetInByte + NumByteToWrite_up_to_BlockSize) > w25q.BlockSize)
    BytesToWrite = w25q.BlockSize - OffsetInByte;
  else
    BytesToWrite = NumByteToWrite_up_to_BlockSize;
  StartPage = W25q_BlockToPage(Block_Address)
      + (OffsetInByte / w25q.PageSize);
  LocalOffset = OffsetInByte % w25q.PageSize;
  do
  {
    W25q_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
    StartPage++;
    BytesToWrite -= w25q.PageSize - LocalOffset;
    pBuffer += w25q.PageSize - LocalOffset;
    LocalOffset = 0;
  } while(BytesToWrite > 0);
#if (W25Q_DEBUG)
  printf("---w25q: WriteBlock Done\r\n");
  w25q.delay_ms(100);
#endif
}

/*****************************************************************************/

void W25q_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
#if (W25Q_DEBUG)
  uint32_t StartTime = w25q.time_ms();
  printf("w25q: ReadByte at address %ld begin...\r\n", Bytes_Address);
#endif
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x0B);
    W25q_WriteAddress(Bytes_Address);
    w25q.hal_spi_tr(0);
    *pBuffer = w25q.hal_spi_tr(W25Q_DUMMY_BYTE);
  }
  w25q.hal_spi_chip_deselect();
#if (W25Q_DEBUG)
  printf("w25q: ReadByte 0x%02X done after %ld ms\r\n", *pBuffer, w25q.time_ms() - StartTime);
#endif
}

/*****************************************************************************/

void W25q_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
#if (W25Q_DEBUG)
  uint32_t StartTime = w25q.time_ms();
  printf("w25q: ReadBytes at Address %ld, %ld Bytes begin...\r\n", ReadAddr, NumByteToRead);
#endif
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x0B);
    W25q_WriteAddress(ReadAddr);
    w25q.hal_spi_tr(0);
    w25q.hal_spi_receive(pBuffer, NumByteToRead);
  }
  w25q.hal_spi_chip_deselect();
#if (W25Q_DEBUG)
  StartTime = w25q.time_ms() - StartTime;
  for(uint32_t i = 0; i < NumByteToRead; i++)
  {
    if((i%8 == 0) && (i > 2))
    {
      printf("\r\n");
      w25q.delay_ms(10);
    }
    printf("0x%02X,",pBuffer[i]);
  }
  printf("\r\n");
  printf("w25q: ReadBytes done after %ld ms\r\n", StartTime);
  w25q.delay_ms(100);
#endif
  w25q.delay_ms(1);
}

/*****************************************************************************/

void W25q_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte,
                     uint32_t NumByteToRead_up_to_PageSize)
{
  if((NumByteToRead_up_to_PageSize > w25q.PageSize) ||
     (NumByteToRead_up_to_PageSize == 0))
  {
    NumByteToRead_up_to_PageSize = w25q.PageSize;
  }
  if((OffsetInByte + NumByteToRead_up_to_PageSize) > w25q.PageSize)
  {
    NumByteToRead_up_to_PageSize = w25q.PageSize - OffsetInByte;
  }
#if (W25Q_DEBUG)
  printf("w25q: ReadPage %ld, Offset %ld, Read %ld Bytes, begin...\r\n", Page_Address, OffsetInByte, NumByteToRead_up_to_PageSize);
  w25q.delay_ms(100);
  uint32_t StartTime = w25q.time_ms();
#endif
  Page_Address = Page_Address * w25q.PageSize + OffsetInByte;
  w25q.hal_spi_chip_select();
  {
    w25q.hal_spi_tr(0x0B);
    W25q_WriteAddress(Page_Address);
    w25q.hal_spi_tr(0);
    w25q.hal_spi_receive(pBuffer, NumByteToRead_up_to_PageSize);
  }
  w25q.hal_spi_chip_deselect();
#if (W25Q_DEBUG)
  StartTime = w25q.time_ms() - StartTime;
  for(uint32_t i = 0; i < NumByteToRead_up_to_PageSize; i++)
  {
    if((i%8 == 0) && (i > 2))
    {
      printf("\r\n");
      w25q.delay_ms(10);
    }
    printf("0x%02X,",pBuffer[i]);
  }
  printf("\r\n");
  printf("w25q: ReadPage done after %ld ms\r\n", StartTime);
  w25q.delay_ms(100);
#endif
  w25q.delay_ms(1);
}

/*****************************************************************************/

void W25q_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte,
                       uint32_t NumByteToRead_up_to_SectorSize)
{
  if((NumByteToRead_up_to_SectorSize > w25q.SectorSize) ||
     (NumByteToRead_up_to_SectorSize == 0))
  {
    NumByteToRead_up_to_SectorSize = w25q.SectorSize;
  }
#if (W25Q_DEBUG)
  printf("+++w25q: ReadSector %ld, Offset %ld, Read %ld Bytes, begin...\r\n", Sector_Address, OffsetInByte, NumByteToRead_up_to_SectorSize);
  w25q.delay_ms(100);
#endif
  if(OffsetInByte >= w25q.SectorSize)
  {
#if (W25Q_DEBUG)
    printf("---w25q: ReadSector Faild!\r\n");
    w25q.delay_ms(100);
#endif
    return;
  }
  uint32_t StartPage;
  int32_t BytesToRead;
  uint32_t LocalOffset;
  if((OffsetInByte + NumByteToRead_up_to_SectorSize) > w25q.SectorSize)
    BytesToRead = w25q.SectorSize - OffsetInByte;
  else
    BytesToRead = NumByteToRead_up_to_SectorSize;
  StartPage = W25q_SectorToPage(Sector_Address)
      + (OffsetInByte / w25q.PageSize);
  LocalOffset = OffsetInByte % w25q.PageSize;
  do
  {
    W25q_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
    StartPage++;
    BytesToRead -= w25q.PageSize - LocalOffset;
    pBuffer += w25q.PageSize - LocalOffset;
    LocalOffset = 0;
  } while(BytesToRead > 0);
#if (W25Q_DEBUG)
  printf("---w25q: ReadSector Done\r\n");
  w25q.delay_ms(100);
#endif
}

/*****************************************************************************/

void W25q_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte,
                      uint32_t NumByteToRead_up_to_BlockSize)
{
  if((NumByteToRead_up_to_BlockSize > w25q.BlockSize) ||
     (NumByteToRead_up_to_BlockSize == 0))
  {
    NumByteToRead_up_to_BlockSize = w25q.BlockSize;
  }
#if (W25Q_DEBUG)
  printf("+++w25q: ReadBlock %ld, Offset %ld, Read %ld Bytes, begin...\r\n", Block_Address, OffsetInByte, NumByteToRead_up_to_BlockSize);
  w25q.delay_ms(100);
#endif
  if(OffsetInByte >= w25q.BlockSize)
  {
#if (W25Q_DEBUG)
    printf("w25q: ReadBlock Faild!\r\n");
    w25q.delay_ms(100);
#endif
    return;
  }
  uint32_t StartPage;
  int32_t BytesToRead;
  uint32_t LocalOffset;
  if((OffsetInByte + NumByteToRead_up_to_BlockSize) > w25q.BlockSize)
    BytesToRead = w25q.BlockSize - OffsetInByte;
  else
    BytesToRead = NumByteToRead_up_to_BlockSize;
  StartPage = W25q_BlockToPage(Block_Address)
      + (OffsetInByte / w25q.PageSize);
  LocalOffset = OffsetInByte % w25q.PageSize;
  do
  {
    W25q_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
    StartPage++;
    BytesToRead -= w25q.PageSize - LocalOffset;
    pBuffer += w25q.PageSize - LocalOffset;
    LocalOffset = 0;
  } while(BytesToRead > 0);
#if (W25Q_DEBUG)
  printf("---w25q: ReadBlock Done\r\n");
  w25q.delay_ms(100);
#endif
}

/*****************************************************************************/