 
#include <stm_loader.h>
#include <hardware_config.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "minIni.h"
#include  "uart.h"

////////////////////////////////////////////////////////////////////////////// ini
extern __IO uint16_t beep;
//////////////////////////////////////////////////////////////////////////////// READ INI FILE
uint8_t Init_Loader_File(Work_File *BIN_File)
{
	
	    //Check Size and Sum of real file
   ini_gets("BOOTLOAD", "fname", ".hex",BIN_File->fname,MAX_FN,"boot.ini");
BIN_File->fsize =  ini_getl("BOOTLOAD", "fsize",  0, "boot.ini");
BIN_File->chk=     ini_getl("BOOTLOAD", "checksum", 0, "boot.ini");

return (uint8_t)(BIN_File->fsize)&&(BIN_File->chk); //>0 NO ERROR
}
//////////////////////////////////////////////////////////////////////////////// READ INI FILE
uint8_t Init_Test_File(Work_File *BIN_File)
{
	
	    //Check Size and Sum of real file
   ini_gets("TEST", "fname", ".hex",BIN_File->fname,MAX_FN,"boot.ini");
BIN_File->fsize =  ini_getl("BOOTLOAD", "fsize",  0,   "boot.ini");
BIN_File->chk=     ini_getl("BOOTLOAD", "checksum", 0, "boot.ini");

return (uint8_t)(BIN_File->fsize)&&(BIN_File->chk); //>0 NO ERROR
}
//////////////////////////////////////////////////////////////////////////////// READ INI FILE
uint8_t Init_Release_File(Work_File *BIN_File)
{
 
/*
file name -loader.bin<LF>
sz -1232<LF>
cs -987654<LF>
file name2 -relese.bin<LF>
sz2 -524288<LF>
cs2 -5556677<LF>
*/
   //Check Size and Sum of real file
   ini_gets("RELEASE", "fname", ".hex",BIN_File->fname,MAX_FN,"boot.ini");
BIN_File->fsize =  ini_getl("RELEASE", "fsize",  0, "boot.ini");
BIN_File->chk=     ini_getl("RELEASE", "checksum", 0, "boot.ini");
return  (uint8_t)(BIN_File->fsize)&&(BIN_File->chk); //>0 NO ERROR

}
////////////////////////////////////////////////////////////////////////////////Bootloader_CMD
uint8_t Bootloader_CMD(uint8_t cmd)
{   char  RxC=0;
 
	 if(UartPutChar(cmd)&&UartPutChar( ~cmd)) 
	 RxC=UartGetChar(TO);
         
         return RxC; 

}
////////////////////////////////////////////////////////////////////////////////SYNC
uint8_t SyncSTM(void)
{unsigned char  TxC,RxC=0;
 
   // write the sync byte to the chip
   TxC=SYNC;
   UartPutChar(TxC);
   RxC=UartGetChar(TO);
    // write the sync byte to the chip

   if(RxC!=ACKN)
   { UartPutChar(TxC);
     RxC=UartGetChar(TO); //Pak SYNC
   };

     if((RxC!=ACKN)&&(RxC!=NACK)) return 1; 
     else return 0;
}
////////////////////////////////////////////////////////////////////////////////ERASE   
uint8_t EraseSTM(void)
{ 
  
     if(Bootloader_CMD(CMD_ERASE)!=ACKN)  return 2;
     if(Bootloader_CMD(CMD_ALL)!=ACKN)    return 3;
     
 return 0;
}
/*
////////////////////////////////////////////////////////////////////////////////Get STM type
void GetSTM()  
{int8 msg[8];
 char cmd[2];
 int8 i;

   cmd[0] = CMD_GET_ID; // Get command
   cmd[1] = ~cmd[0];    // complement

   fputc(cmd[0],BOARD);
   fputc(cmd[1],BOARD);
   delay_ms(10);
  
    memset(msg, 0, sizeof(msg)); 
  for(i=0; bkbhit && i<sizeof(msg); ++i)
              msg[i] = bgetc();

  
  printf(lcd_putc,"\fPID=%0X%0X",msg[0],msg[1]);
  
  printf(lcd_putc,"\nB=%0X%0X",msg[2],msg[3]);

}
*/
 //#define KEY_GEN
////////////////////////////////////////////////////////////////////////////////WriteBINtoSTM
#define BLOCK_SIZE 256
	TCHAR ReadBuf[BLOCK_SIZE]; 
	
uint8_t WriteHEXtoSTM(const TCHAR *Filename,const uint32_t base_address) /* Pointer to the file name */
{
  
  #ifdef KEY_GEN     
     uint8_t   My_AESkey[32];    
     uint32_t  k=0;
      FIL fa;
      UINT dw;
  #endif 
   
          u32 q;
	  FIL f;
    UINT      nRx;    
	
 unsigned char RxC;
            u8 checksum;    //address checksum
 

	union{
      char bytes[4];    // sending one byte, in reverse order
       u32 value;   // for assigning
      }address;
 
         u32 remaining_bytes;    // bytes left to program
         u32 buffer_size;     //  data in buffer 
  

   #ifdef KEY_GEN  
    memset(My_AESkey,0xFF,sizeof(My_AESkey));
   #endif 
////////////////////////////////////////////////////////// INIT MMC and FAT
 if(f_open(&f,Filename, FA_READ|FA_OPEN_EXISTING)==FR_OK) remaining_bytes = f.fsize; //MAX 512KB	
  else return 1;
  if(remaining_bytes>524288) remaining_bytes=524288;
  //printf("r1= %u\n",res);								
  

   //printf("r2= %u\n",res);
	 //printf("r2= %lu\n",remaining_bytes);
  address.value = base_address;  
 
			
while(remaining_bytes)
  { 
 if(!(remaining_bytes%30))  beep=10;
 
 
	         if(Bootloader_CMD(CMD_WRITE_MEMORY)==ACKN)
                 {
                   
                 //printf("Write=%u",remaining_bytes);  Delay(10000);
                 if(!(UartPutChar(address.bytes[3])))return 3;	 
                 if(!(UartPutChar(address.bytes[2])))return 3;   
                 if(!(UartPutChar(address.bytes[1])))return 3; 
	         if(!(UartPutChar(address.bytes[0])))return 3;
	  
	  
          
     checksum = address.bytes[0] ^ address.bytes[1] ^ address.bytes[2] ^ address.bytes[3]; // this is not a checksum
           
                if(UartPutChar(checksum)) 
                RxC=UartGetChar(TO);
               
                if(RxC==ACKN){  //printf(lcd_putc,"\fAdd=%ld",address.value);  Delay_s(1);

////////////////////////////////////////send byte count for Write Memory command
      // buffer_size = (remaining_bytes > 252) ? 252 : remaining_bytes; // 252 bytes, ili remaining 
        buffer_size  = (remaining_bytes > BLOCK_SIZE) ? BLOCK_SIZE :remaining_bytes;
       
	UartPutChar((u8)(buffer_size-1)); //255				 
 
    
       // Read Block from MMC and send to STM
       memset(ReadBuf,0xFF,BLOCK_SIZE);
    
       if(f_read (&f,ReadBuf,buffer_size,&nRx)==FR_OK)	
       {         
        checksum= buffer_size-1;
        for(q = 0; q < buffer_size; q++){       if(UartPutChar(ReadBuf[q])) //256 danni
					           checksum ^= ReadBuf[q];  
				        }; //Send data and calc check sum
        
  
             //printf("CHECK SUM=%U",checksum);   fputc(checksum,BOARD);
            if(UartPutChar(checksum))
                 RxC=UartGetChar(2*TO);
        if(RxC==ACKN){
#ifdef KEY_GEN
          k++;       
          if((k>299)&&(k<332)) My_AESkey[k-300]=~checksum;  
#endif
           
            remaining_bytes-=buffer_size; // that many less to do
            address.value+= buffer_size;
            
          //printf(lcd_putc,"\fNEXT=%LD",address.value);  Delay_s(1);
                     }else  return 4; //printf("\f\fCHECK SUM=NACKN");

        }else return 5;  //printf("BLOCK READ ERROR");
               
       }else return 6;  //printf("ADDRESS CMD=NACK");
		 }else return 7;  //printf("WRITE CMD=NACK");
 
 } //WHILE
	
	
 if(ini_close(&f))
 {
  #ifdef KEY_GEN
   if(f_open(&fa,"AES.KEY", FA_WRITE|FA_CREATE_ALWAYS)==FR_OK)
   {
     
    f_write (
	&fa,			/* Pointer to the file object */
	My_AESkey,	        /* Pointer to the data to be written */
	32,			/* Number of bytes to write */
	 &dw			/* Pointer to number of bytes written */
                );

   };
   ini_close(&fa);
 
  #endif 

   return 0; 
 }
  
 else return 8;
 

}
////////////////////////////////////////////////////////////////////////////////Run_Application
u8 Run_Application(const uint32_t base_address)
{   
  u8 RxC=NACK;
  u8 checksum=0;
  
  union{
      char bytes[4];    // sending one byte, in reverse order
      uint32_t value;   // for assigning
      }address;
  
  address.value = base_address; 
  
  if( Bootloader_CMD((uint8_t)CMD_GO)==ACKN )
  {
                //printf("Write=%u",remaining_bytes);  Delay(10000);
                 if(!(UartPutChar(address.bytes[3])))return 0;	 
                 if(!(UartPutChar(address.bytes[2])))return 0;   
                 if(!(UartPutChar(address.bytes[1])))return 0;
	         if(!(UartPutChar(address.bytes[0])))return 0; 
	  
	  
          
     checksum = address.bytes[0] ^ address.bytes[1] ^ address.bytes[2] ^ address.bytes[3]; // this is not a checksum
               if(UartPutChar(checksum)) 
                RxC=UartGetChar(TO);
               
               if(RxC==ACKN) return 1;
               else return RxC;
    
  
  }else return RxC;

}

////////////////////////////////////////////////////////////////////////////////ReadUnLock
u8 ReadUnProtect(void)
{ unsigned char  TxC,RxC=0;

     TxC=CMD_R_ENABLE;  
     UartPutChar(TxC);
     UartPutChar(~TxC); 
     RxC=UartGetChar(TO);
     RxC=UartGetChar(TO);
  if((RxC!=ACKN)&&(RxC!=NACK)) return 1;  
 
  
  return 0;

  
      
}







