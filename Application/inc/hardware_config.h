//#include hardware_config.h
#include <stdio.h>
#include "stm32f10x.h"

#ifndef __HARDWARE_CONFIG_H
#define __HARDWARE_CONFIG_H
 
 
 #define LED_ON         GPIO_SetBits(GPIOE, GPIO_Pin_0 )  //set high. 
 #define LED_OFF        GPIO_ResetBits(GPIOE, GPIO_Pin_0 )
 #define TOGGLE_LED     GPIOE->ODR ^= GPIO_Pin_0
 #define EX_MCU_BOOT    GPIO_SetBits(GPIOE, GPIO_Pin_14 )  //set high.
 #define EX_MCU_UNBOOT  GPIO_ResetBits(GPIOE, GPIO_Pin_14 )
 #define EX_MCU_RUN     GPIO_SetBits(GPIOE, GPIO_Pin_15 )  //set high.
 #define EX_MCU_RST     GPIO_ResetBits(GPIOE, GPIO_Pin_15 )   
 
 	  /* Enable TIM */
#define BUZZER_ON   TIM_Cmd(TIM11, ENABLE)
#define BUZZER_OFF  TIM_Cmd(TIM11, DISABLE) 
 
 
 #define BUTTON !(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7))
 #define NO_SD   (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9))

void RCC_Configuration(void);
void USART1_Config(void);
void NVIC_Configuration(void);

void GPIO_Config(void);
 
void LED_on(void);
void LED_off(void);
void EXTI9_Config(void);
void  Timer2_Config(void);
void  Timer11_Config(void);
void Delay(__IO uint32_t nCount);
void delay_ms(uint16_t delay_temp);
#endif


