//stm_loader.h

#ifndef __STM_LOADER_H
#define __STM_LOADER_H

#include "stm32f10x.h"
#include "ff.h"

#define MAX_FN 40 //size of file name
#define TO (u32)2000   //Serial RxD Time Out


#define SYNC    0x7F
#define ACKN    0x79
#define NACK    0x1F

// commands used by the STM32 bootloader protocol

#define CMD_GET               0x00    // get bootloader version and list of supported commands
#define CMD_GET_VERSION_RPS   0x01    // get bootloader version and read protection status
#define CMD_GET_ID            0x02    // get device ID
#define CMD_READ_MEMORY       0x11    // read memory
#define CMD_GO                0x21    // go
#define CMD_WRITE_MEMORY      0x31    // write memory
#define CMD_ERASE             0x43    // 
#define CMD_ALL               0xFF    // 
#define CMD_R_DISABLE         0x82
#define CMD_R_ENABLE          0x92 

typedef struct
{
 TCHAR    fname[MAX_FN];
 long     fsize;
 uint32_t chk;
   
}Work_File;


uint8_t Init_Loader_File(Work_File *BIN_File);
uint8_t Init_Test_File(Work_File *BIN_File);
uint8_t Init_Release_File(Work_File *BIN_File);



uint8_t SyncSTM(void);
uint8_t EraseSTM(void);
uint8_t WriteHEXtoSTM(const TCHAR *Filename,const uint32_t base_address); /* Pointer to the file name */
u8 Run_Application(const uint32_t base_address);
u8 ReadUnProtect(void);

#endif
