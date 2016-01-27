 
#include <stm_loader.h>
#include <hardware_config.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "minIni.h"
#include  "uart.h"

////////////////////////////////////////////////////////////////////////////// ini
#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))
#define TO 3200

extern __IO uint16_t beep;


//////////////////////////////////////////////////////////////////////////////// READ INI FILE
bool GetLoaderFN(TCHAR *fl,int FnSize)
{
	
	const char inifile[] = "boot.ini";
 //TCHAR buff[MAX_FN_SIZE];
	/*
file name -loader.bin<LF>
sz -1232<LF>
cs -987654<LF>
file name2 -relese.bin<LF>
sz2 -524288<LF>
cs2 -5556677<LF>
*/

	int8_t n=0; 
  int32_t fz=0; 
  int32_t chk=0;

	    //Check Size and Sum of real file
	     n = ini_gets("BOOTLOAD", "fname", "selfload.hex", fl,FnSize, inifile);
	    fz = ini_getl("BOOTLOAD", "fsize",    -1, inifile);
		 chk = ini_getl("BOOTLOAD", "checksum", -1, inifile);
		   
	   

					
		//  printf("file name -%s\n",buff);
	  //    printf("sz -%d\n",fz);
		//	   printf("cs -%d\n",chk);

 
			
	return TRUE;

}


//////////////////////////////////////////////////////////////////////////////// SYNC AND ERASE
uint8_t SyncEraseSTM(void)
{ char  TxC,RxC;
 
   // write the sync byte to the chip
   //////////////////////////////////////buffer_flush;
   TxC=SYNC;
   if(UartPutChar(TxC))
    RxC=UartGetChar(TO);
	
	// write the sync byte to the chip
   if(RxC!=ACK) 
   if(UartPutChar(TxC))  RxC=UartGetChar(TO);
   
	 if((RxC==ACK)||(RxC==NACK))
   {		 
    
   //if(RxC==NACK) // printf("ALREDY SYNC\n");
   TxC=CMD_ERASE;
	 if(UartPutChar(TxC)&&UartPutChar( ~TxC)) 
												RxC=UartGetChar(TO);
													 
                     if(RxC==ACK)
                     {
                                     // 0xFF = ALL
                                    TxC = 0xFF;                                                
                                    if(UartPutChar(TxC)&&UartPutChar( ~TxC))  
                                    RxC=UartGetChar(TO);
                                    if(RxC==ACK) return 0;
																		else  return 3; //ERASE ERROR
										 }else return 4; //ERASE ERROR
											                
    }else return 2; 
}
////////////////////////////////////////////////////////////////////////////////ReadLock
bool Protect(void)
{ char RxC;
 
	
                     if(UartPutChar(0x82)&&UartPutChar(0x7D))  
                     RxC=UartGetChar(TO);
                     if(ACK==RxC)
                     {  
											//printf("Read is DISABLED OK\n");               
                     return TRUE;
                     }else  if(RxC==NACK);//  printf("Read DISABLE NACK\n");                      
                         
return FALSE;
}
////////////////////////////////////////////////////////////////////////////////ReadUnLock
bool  UnProtect(void)
{ char RxC;
	
	                          
	                  if(UartPutChar(0x92)&&UartPutChar(0x6D)) 
										{
                     RxC=UartGetChar(TO);
                     
										if(RxC==ACK)   return TRUE;        
                    if(RxC==NACK)  return FALSE;
											
										};
 
										return FALSE;
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
////////////////////////////////////////////////////////////////////////////////WriteBINtoSTM
		#define BLOCK_SIZE 252
	TCHAR ReadBuf[BLOCK_SIZE]; 
	
uint8_t WriteHEXtoSTM(const TCHAR *Filename) /* Pointer to the file name */
{

  const uint32_t base_address = 0x08000000; // beginning of FLASH memory

		uint16_t i;
	  FIL f;
	  FILINFO fno;
    FRESULT fr;
    UINT nRx;    
	
 unsigned char RxC;
 uint8_t checksum;    //address checksum
 

 
	union{
      char bytes[4]; // sending one byte, in reverse order
      uint32_t value;   // for assigning
      }address;
 
   DWORD remaining_bytes;  // bytes left to program
   uint32_t buffer_size;     // size of data in buffer
  
   
			
////////////////////////////////////////////////////////// INIT MMC and FAT 
  if(ini_openread("loader.bin",&f))	fr = f_stat("loader.bin", &fno);
	else return 20;
  //printf("r1= %u\n",res);		
						
  if(fr==FR_OK) remaining_bytes = fno.fsize;
  else return 30;
   //printf("r2= %u\n",res);
	 //printf("r2= %lu\n",remaining_bytes);
  address.value = base_address;  
 

			
			
			
  while(remaining_bytes)
  { 
 if(!(remaining_bytes%10))  beep=1;
	         if(UartPutChar(CMD_WRITE_MEMORY)&&UartPutChar(~CMD_WRITE_MEMORY))  
           RxC=UartGetChar(TO);
	           
 if(RxC==ACK){
                 //printf("Write=%u",remaining_bytes);  Delay(10000);
                 if(!(UartPutChar(address.bytes[3])))return 25;	 
                 if(!(UartPutChar(address.bytes[2])))return 25;   
                 if(!(UartPutChar(address.bytes[1])))return 25; ;
	               if(!(UartPutChar(address.bytes[0])))return 25; ;
	  
	  
     checksum = address.bytes[0] ^ address.bytes[1] ^ address.bytes[2] ^ address.bytes[3]; // this is not a checksum
           
                if(UartPutChar(checksum)) 
                RxC=UartGetChar(TO);
               
               if(RxC==ACK){  //printf(lcd_putc,"\fAdd=%ld",address.value);  Delay_s(1);

////////////////////////////////////////send byte count for Write Memory command
     // buffer_size = (remaining_bytes > 252) ? 252 : remaining_bytes; // 252 bytes, ili remaining 
        buffer_size  = (remaining_bytes > BLOCK_SIZE) ? BLOCK_SIZE : remaining_bytes;
       
			  UartPutChar(BLOCK_SIZE-1);				 
 
    
       // Read Block from MMC and send to STM
       memset(ReadBuf,0xFF,BLOCK_SIZE);
    
    //ini_read(ReadBuf,buffer_size,&f))
		if(f_read (&f,ReadBuf,buffer_size,&nRx)==FR_OK)	
		{         
        checksum= BLOCK_SIZE-1;
        for(i = 0; i < BLOCK_SIZE; i++){   if(UartPutChar(ReadBuf[i]))
					                                    checksum ^= ReadBuf[i];	
				                               }; //Send data and calc check sum
        
   
		     //printf("CHECK SUM=%U",checksum);   fputc(checksum,BOARD);
            if(UartPutChar(checksum))
                 RxC=UartGetChar(TO);
        if(RxC==ACK){ 
            remaining_bytes-=buffer_size; // that many less to do
            address.value+= buffer_size;
            
          //printf(lcd_putc,"\fNEXT=%LD",address.value);  Delay_s(1);
                     }else  return 3; //printf("\f\fCHECK SUM=NACK");

        }else return 15;  //printf("BLOCK READ ERROR");
               
       }else return 5;  //printf("ADDRESS CMD=NACK");
		 }else return 6;  //printf("WRITE CMD=NACK");
 
 } //WHILE
	
	
 if(ini_close(&f)) return 0; 
 else return 10;
 

}

