//stm_loader.h

#ifndef __STM_LOADER_H
#define __STM_LOADER_H

#include "stm32f10x.h"
#include "ff.h"





#define SYNC   0x7F
#define ACK    0x79
#define NACK   0x1F

// commands used by the STM32 bootloader protocol

#define CMD_GET               0x00    // get bootloader version and list of supported commands
#define CMD_GET_VERSION_RPS   0x01    // get bootloader version and read protection status
#define CMD_GET_ID            0x02    // get device ID
#define CMD_READ_MEMORY       0x11    // read memory
#define CMD_GO                0x21    // go
#define CMD_WRITE_MEMORY      0x31    // write memory
#define CMD_ERASE             0x43    // erase





 bool GetLoaderFN(TCHAR *fl,int FnSize);
  uint8_t SyncEraseSTM(void);
 bool   ReadProtect(void);
 bool   ReadUnProtect(void);
uint8_t WriteHEXtoSTM(const TCHAR *Filename); /* Pointer to the file name */

#endif
