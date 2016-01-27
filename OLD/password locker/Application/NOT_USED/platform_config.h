// Date               : 09/22/2008
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H
#include "stm32f10x_lib.h"

#ifdef USE_USB_CDC
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#endif

extern  u8 sNIVC[2];
#define DISABLE_INT	sNIVC[0] = NVIC->ISER[0]; sNIVC[1] = NVIC->ISER[1]; NVIC->ICER[0] = 0xffffffff; NVIC->ICER[1] = 0xffffffff
#define RESTORE_INT	NVIC->ISER[0] = sNIVC[0]; NVIC->ISER[1] = sNIVC[1]


#define USART1_PRIORITY				7
#define USART2_PRIORITY				8
#define USART3_PRIORITY				9
#define RTC_PRIORITY				13
#define USB_PRIORITY				6
//Power fault markers
#define VALID_FLASH		0x02
#define VALID_RTC		0x10
#define VALID_SEND_SPI		0x100
//////////////////////////////////////////////////////////////////////////////// PIN CONFIG
#define GPIO_LEDD   GPIOD 
#define LED_11      (GPIO_Pin_2 | GPIO_Pin_3)    //0000000000001100
#define LED_12      (GPIO_Pin_4 | GPIO_Pin_5)    //0000000000110000
#define LED_13      (GPIO_Pin_6 | GPIO_Pin_7)    //0000000011000000
#define LED_21      (GPIO_Pin_8 | GPIO_Pin_9)    //0000001100000000
#define LED_22      (GPIO_Pin_10 | GPIO_Pin_11)  //0000110000000000
#define LED_23      (GPIO_Pin_12 | GPIO_Pin_13)  //0011000000000000
#define GPIO_LEDE   GPIOE 
#define LED_31      (GPIO_Pin_14 | GPIO_Pin_15)   //1100000000000000
#define LED_32      (GPIO_Pin_13 | GPIO_Pin_12)   //0011000000000000
#define LED_33      (GPIO_Pin_11 | GPIO_Pin_10)   //0000110000000000

#define TOREX_OFF()                    GPIO_ResetBits(GPIOE, GPIO_Pin_0)
#define TOREX_ON()                     GPIO_SetBits(GPIOE, GPIO_Pin_0) 
#define GSM_LOW()                      GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define GSM_HIGH()                     GPIO_SetBits(GPIOB, GPIO_Pin_0)

#define GSM_RESET()                    GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define GSM_RUN()                      GPIO_SetBits(GPIOB, GPIO_Pin_1)

#define USB_DETECT()                   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10 )   

/* Private define ------------------------------------------------------------*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
 
#define USE_USART1
#define USE_USART2
#define USE_USART3

#ifdef USE_USART1
  #define   PC_PORT1                   USART1
  #define   RS1_TxPin                 GPIO_Pin_9
  #define   RS1_RxPin                 GPIO_Pin_10
#endif

#ifdef USE_USART2
  #define  GSM_PORT                   USART2
  #define  GSM_TxPin                 GPIO_Pin_2
  #define  GSM_RxPin                 GPIO_Pin_3
#endif

#ifdef USE_USART3
  #define  PC_PORT2                   USART2
  #define  RS2_TxPin                 GPIO_Pin_10
  #define  RS2_RxPin                 GPIO_Pin_11
#endif
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void UARTS_Configuration(void);
void TIM2_Configuration(void);
void RTC_Configuration(void);
u8 USART_Scanf(u32 value);
#endif /* __PLATFORM_CONFIG_H */
