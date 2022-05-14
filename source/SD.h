#include <MKL25Z4.H>
#include "SPI.h"
#include <stdint.h>


#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4

// SD card commands
#define  CMD0  (0x00)// GO_IDLE_STATE - init card in spi mode if CS low
#define CMD8  (0x08) // SEND_IF_COND - verify SD Memory Card interface operating condition.
#define CMD9  (0x09) // SEND_CSD - read the Card Specific Data (CSD register)
#define CMD10 (0x0A)// SEND_CID - read the card identification information (CID register)
#define CMD12  (0x0C) //stop transmission
#define CMD13  (0x0D) // SEND_STATUS - read the card status register
#define  CMD17 (0X11)// READ_BLOCK - read a single data block from the card
#define CMD18	(0x12)//read multiple blocks
#define CMD16 (0x10) //set block size
#define CMD24  (0X18) // WRITE_BLOCK - write a single data block to the card
#define CMD25  (0X19) // WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION
#define CMD32  (0X20) // ERASE_WR_BLK_START - sets the address of the first block to be erased
#define CMD33  (0X21) //ERASE_WR_BLK_END - sets the address of the last block of the continuousrange to be erased
#define CMD38  (0X26) // ERASE - erase all previously selected blocks
#define CMD55  (0X37) // APP_CMD - escape for application specific command
#define CMD58  (0X3A) //READ_OCR - read the OCR register of a card
#define ACMD23 (0X17) // SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be pre-erased before writing
#define ACMD41 (0X29) // SD_SEND_OP_COMD - Sends host capacity support information and activates the card's initialization process







//------------------------------------------------------------------------------------
#define R1_READY_STATE  ( 0X00)// status for card in the ready state
#define R1_IDLE_STATE  (0X01) // status for card in the idle state
#define R1_ILLEGAL_COMMAND  (0X04)//status bit for illegal command
#define DATA_START_BLOCK (0XFE) // start data token for read or write single block
#define STOP_TRAN_TOKEN  (0XFD) // stop token for write multiple blocks
#define WRITE_MULTIPLE_TOKEN (0XFC) // start data token for write multiple blocks
#define DATA_RES_MASK  (0X1F) // mask for data response tokens after a write block operation
#define DATA_RES_ACCEPTED (0X05) // write data accepted token





uint8_t MicroSDcard_command(uint8_t cmd,uint32_t arg, uint8_t crc);
uint8_t Wait_Read();
uint8_t init_MicroSDcard();
uint8_t Read_CSD_CID(uint8_t cmd,uint8_t *buff);
uint8_t readSingleBlock(uint32_t block,uint8_t *buff);
uint8_t readMultipleBlock(uint32_t block,uint8_t *buff,uint8_t count);
uint8_t writeSingleBlock(uint32_t block,const uint8_t *buff);
uint8_t SD_WaitReady();
uint8_t writeMultipleBlocks(uint32_t block,const uint8_t *buff,uint8_t count);

