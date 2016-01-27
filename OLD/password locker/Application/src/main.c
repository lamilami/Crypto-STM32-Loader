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



/*
 

*/
/* Private define ------------------------------------------------------------*/

#define MAX_FN_SIZE 40

#define sizearray(a)    (sizeof(a) / sizeof((a)[0]))
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
TCHAR loader_fn[MAX_FN_SIZE];
TCHAR release_fn[MAX_FN_SIZE]; 

/* Private variables ---------------------------------------------------------*/
 char  Buff[40];




FATFS fs; /* Work area (file system object) for logical drives */
__IO uint16_t CCR1_Val = 40961;
__IO uint16_t CCR2_Val = 27309;
__IO uint16_t CCR3_Val = 13654;
__IO uint16_t CCR4_Val = 6826;
uint16_t PrescalerValue = 0;

 __IO uint16_t beep = 0;


/* Private function prototypes -----------------------------------------------*/

/* Private functions - -------------------------------------------------------*/



/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{ 
	
  uint8_t b = 0;
  GPIO_Config();
  Timer2_Config();
	Timer11_Config();

  BUZZER_OFF;

  USART1_Config();
  LED_ON;

	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////WHLE 1
	while(1)
{ 
	  
  if(BUTTON){ beep=100;
		          EX_MCU_RST;    Delay(0x00fffff);
         		  EX_MCU_BOOT;   Delay(0x00fffff);
              EX_MCU_RUN;    Delay(0x0ffffff);
		               
              b=SyncEraseSTM(); 
		  
		
  	            if(!b) LED_OFF;
                else while(b){ b--;beep=20;   Delay(0x01fffff);  }
				         Delay(0x0ffffff); 
                
							  f_mount(0, &fs);
		             							
 					 	   if(GetLoaderFN(loader_fn,sizearray(loader_fn)))
               {  LED_ON;
								 
							    //printf("file name -%s\n",loader_fn);
						   
                    
				      	 b=WriteHEXtoSTM("loader.bin");
								 
								    if(!b)beep=100;
								     else while(b){ b--;beep=50;   Delay(0x0ffffff);  }
                 
                };
							 
		           
              EX_MCU_RST;      Delay(0x00fffff);		  
	            EX_MCU_UNBOOT;   Delay(0x00fffff);
              EX_MCU_RUN;      Delay(0x0ffffff);								
							
						}else 
  {
		
	memcpy(Buff,'\0',sizeof(Buff));	
	UartGetStringEx(Buff, 40, 200,'c', 'd');
  if(strstr (Buff, "card")) Card_UTL();
		//else printf("\r\nTry to be faster...");
	
	};					
		   
 

};

}






/*
void TimerConfig(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
   TIM_OCInitTypeDef TIM_OCInitStruct;
 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;// Buzzer
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
 
   GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
 
   // TIM3 60 KHZ, ie 24 MHz / 400 or 72 MHz / 1200
   //  60000 Hz chosen as a large, clean number fitting in 16-bit
 
   TIM_TimeBaseInitStruct.TIM_Prescaler = 400 - 1; // 60 KHz timebase
   TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1; // Arbitary placeholder 60 Hz
   TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
   TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
  
   TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
   TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStruct.TIM_Pulse = (TIM_TimeBaseInitStruct.TIM_Period + 1) / 2; // 50% Duty
   TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
 
   TIM_OC2Init(TIM3, &TIM_OCInitStruct); // Channel 2 PC7 Buzzer
 
   TIM_Cmd(TIM3, ENABLE);
} // sourcer32@gmail.com
 
int setfreq(int freq) // 1 to 3000 Hz
{
 int period = 60000 / freq; // compute period as function of 60KHz ticks
 
 TIM3->AAR = period - 1;
 TIM3->CCR2 = period / 2; // Channel 2 50/50
}
*/


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
