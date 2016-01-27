/**
  ******************************************************************************
  * @file    sd/main.c 
  * @author  
  * @version V1.0
  * @date     
  * @brief   Main program body.
  ******************************************************************************
 */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm_loader.h"
#include  "hardware_config.h"
#include  "ff.h"
#include  "diskio.h"
#include  "card_utl.h"
#include  "uart.h"

//#include  "tropicssl\aes.h"

#define DEBUG_MODE
/* Private define ------------------------------------------------------------*/
#define FLASH_BEGIN  0x08000000 // beginning of FLASH memory
#define FLASH_HIDDEN 0x08011000 
#define RAM_BEGIN    0x20000800 

                   //0x0807FFFF
#define SEC 1000   //one sec
//#define sizearray(a)    (sizeof(a) / sizeof((a)[0]))
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////BUZZER
 __IO uint16_t beep = 0;
////////////////////////////////////////////////////////////////////////////////SERIAL COMM

#define RX_SIZE 14
unsigned char  Rx_Buff[RX_SIZE];
BOOL PUB_KEY_OK=FALSE;


/* Private function prototypes -----------------------------------------------*/
/* Private functions - -------------------------------------------------------*/
///////////////////////////////////////////////////////////////////////////////FAT FS
  Work_File loader_file;
  Work_File test_file;
  Work_File release_file;
FATFS	fs;
////////////////////////////////////////////////////////////////////////////////mmc_mount
bool mmc_mount(void)
{
  
	//if (disk_initialize(0) != RES_OK) return false;
	if (f_mount(&fs, "", 1) != FR_OK) return false;
	return true;
}
////////////////////////////////////////////////////////////////////////////////BeepERR
void BeepERR(uint8_t err,uint16_t beep_t)
{
while(err){ err--; beep=beep_t;  delay_ms(1000);   } 
}
////////////////////////////////////////////////////////////////////////////////LoaderHALT
//Tireta kodirat greshkite
void    Loader_Halt(uint8_t ti)
{  
 uint8_t beep_cnt=ti;
              EX_MCU_RST;      delay_ms(500);
	      EX_MCU_UNBOOT;   delay_ms(900);
              EX_MCU_RUN;      delay_ms(900);
 //if error, r times beep on evry\\\\\\\\\\\\\\\\\ toggle sec;
 while(1){
    while(beep_cnt){beep=200;   delay_ms(600); beep_cnt--;};
                      delay_ms(1200); beep_cnt=ti; 
         };
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////MAIN////////////////////MAIN//////////////////MAIN/////////////////
////////////////////////////////////////////////////////////////////////////////
int main(void)
{ 
	 uint8_t res=0;
         // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);  
	 SysTick_Config(SystemCoreClock/1000);   //1000);//1 ms

            GPIO_Config();
            EX_MCU_RST;
 	  
  EXTI9_Config();
  //Timer2_Config();
  Timer11_Config();
  BUZZER_OFF;
  USART1_Config();
 // printf(" INCOTEX AES LOADER v01\r\n");
  beep=1000;
     while(NO_SD) beep=1; 
    
     delay_ms(600);
     if(! mmc_mount()) Loader_Halt(4);  //ERROR  taaa taaa  taaa pausa
 
   LED_ON;  //READY 
 
 
////////////////////////////////////////////////////////////////////////////////WHLE 1
while(1)
{

if(BUTTON)
{  
   
 
   beep=1000;
   EX_MCU_RST;  delay_ms(500);
   EX_MCU_BOOT; delay_ms(1000);
   EX_MCU_RUN;  delay_ms(800);
    
LED_OFF;
         res=SyncSTM();         
         res=EraseSTM();            
 
         if(res)
         {
         
        //  res=SyncSTM();             if(res)Loader_Halt(res); 
          ReadUnProtect();
           beep=2000; 
           delay_ms(2000);
           
               EX_MCU_RST;      delay_ms(500);
               EX_MCU_BOOT;     delay_ms(1000);
               EX_MCU_RUN;      delay_ms(800);
           UartFix();
           res=SyncSTM();   if(res)Loader_Halt(res);       
           
         };
                 
	 res=(Init_Loader_File(&loader_file)!=true);         if(res)Loader_Halt(5);     //ERROR  
         res=WriteHEXtoSTM(loader_file.fname,RAM_BEGIN);     if(res)Loader_Halt(5+res); //5-12
	 beep=1000;
 
    EX_MCU_UNBOOT;
    if(Run_Application(RAM_BEGIN)==ACKN) LED_ON;
    else  if(res)Loader_Halt(1);
 
////////////////////////////////////////////////////////////////////////////////Upload AES KEY
do{//POOL SYNC TO TACHO 

      if(UartPutChar(SYNC)) TOGGLE_LED;
                               
}while(ACKN!=UartGetChar(5000));
 
	 res=(Init_Test_File(&test_file)!=true);           if(res)Loader_Halt(1);      //ERROR  
         res=WriteHEXtoSTM(test_file.fname,FLASH_BEGIN);   if(res)Loader_Halt(1+res);  //5-12
	 beep=1000;
 
////////////////////////////////////////////////////////////////////////////////Upload Release
do{//POOL SYNC TO TACHO 
  
   if(UartPutChar(SYNC)) TOGGLE_LED;
                               
}while(ACKN!=UartGetChar(5000));
  
	 res=(Init_Release_File(&release_file)!=true);              if(res)Loader_Halt(1);   //ERROR  
         res=WriteHEXtoSTM(release_file.fname,(u32)FLASH_BEGIN);    if(res)Loader_Halt(1+res); //5-12
	 beep=1000;
		   
};
////////////////////////////////////////////////////////////////////////////////
//CARD UTL
	memset(Rx_Buff,'\0',RX_SIZE);	
	UartGetStringEx(Rx_Buff, RX_SIZE,SEC,'c', 'd');
        if(strstr ((const char*)Rx_Buff, "card"))Card_UTL();
//else printf("\r\nTry to be faster...");

}//while

}//main


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
