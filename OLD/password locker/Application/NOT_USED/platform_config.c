// File Name          : platform_config.c
// Author             :
// Version            : 
// Date               : 
// Description        : 
#include "platform_config.h"
ErrorStatus HSEStartUpStatus;

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
   
   RCC_DeInit(); /* RCC system reset(for debug purpose) */
   RCC_HSEConfig(RCC_HSE_ON); /* Enable HSE */ 
   HSEStartUpStatus = RCC_WaitForHSEStartUp(); /* Wait till HSE is ready */

  if(HSEStartUpStatus == SUCCESS)
  {
   
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);  /* Enable Prefetch Buffer */
    FLASH_SetLatency(FLASH_Latency_2);//Flash 2 wait state 
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  //HCLK = SYSCLK
    RCC_PCLK2Config(RCC_HCLK_Div1);   //PCLK2 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div8);   //PCLK1 = HCLK/2
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_12); // PLLCLK = 6MHz * 12 = 72 MHz 
    RCC_PLLCmd(ENABLE);                                    // Enable PLL
   
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) // Wait till PLL is ready
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);   // Select PLL as system clock source 
  
    while(RCC_GetSYSCLKSource() != 0x08)         // Wait till PLL is used as system clock source 
    {
    }
  }
    
  /*  Enable clocks */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB  | RCC_APB2Periph_GPIOC | 
                          RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE  | RCC_APB2Periph_AFIO  | 
                          RCC_APB2Periph_USART1| RCC_APB2Periph_SPI1,  ENABLE);

  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3 | RCC_APB1Periph_TIM2 |
                          RCC_APB1Periph_PWR    | RCC_APB1Periph_BKP, ENABLE);
  
  #ifdef USE_USB_CDC
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5); // USBCLK = PLLCLK/1.5
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE); // Enable USB clock 
  #endif
  


}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
//////////////////////////////////////////////////////////////////////////////// SPI 1  UART1
  //  SPI1 pins: SCK, MISO and MOSI, USART1 Tx (PA.09): Mode_AF_PP
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9; ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Configure I/O for Flash Chip select */
  GPIO_InitStructure.GPIO_Pin =    GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Configure USART1 Rx (PA.10) as input floating */ 
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
//////////////////////////////////////////////////////////////////////////////// UART 2 MODEM
  // USART2 RTS pin-1)  (USART2 Tx-pin 2) as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_1 |  GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // USART2 CTS pin-0 (USART2 Rx pin-3) as input floating 
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //////////////////////////////////////////////////////////////////////////////// UART 3 PC2
  /* Configure USART3 Tx (PB.10)as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* (GSM_DCD pin-6 GSM_RI pin-7  GSM_DSR pin-8)( USART3 Rx PB.11 )input floating */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8| GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Configure (GSM_ON pin 0) (EXTRSTN pin 1) (GSM_DTR pin 5) Output push-pull */ 
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    // Configure (USB_VCC pin 10) 
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* Configure GPIO LEDD pin 2-3-4-5-6-7-8-9-10--12-13  Output push-pull */
  GPIO_InitStructure.GPIO_Pin = LED_11|LED_12|LED_13|
                                LED_21|LED_22|LED_23;
                                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  /* Configure TOREX_CTRL pin-0 GPIO LEDE pin-15-14-13-12-11-10  Output push-pull */ 
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0|LED_31| LED_32| LED_33;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);  
  // Configure POWER_MON pin-3 IN_FLOATING 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
  /* Enable the USART Interrupt PC1*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* Enable the USART2 Interrupt MODEM*/
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* Enable the USART2 Interrupt PC2*/
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);        
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#ifdef  USE_USB_CDC
    /* Enable the USB_LP_CAN_RX0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USB_HP_CAN_TX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN_TX_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
  

} 
/////////////////////////////////////////////////////////////////////////////////TIM_Configuration
void TIM2_Configuration(void)
{
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
vu16 CCR1_Val = 0xffff;
vu16 CCR2_Val = 0xffff;
vu16 CCR3_Val = 0xffff;
vu16 CCR4_Val = 0xffff;
  
/*  
// ---------------------------------------------------------------
    TIM2 Configuration: Output Compare Timing Mode:
    TIM2CLK = 36 MHz, Prescaler = 4, TIM2 counter clock = 7.2 MHz
    CC1 update rate = TIM2 counter clock / CCR1_Val = 146.48 Hz
    CC2 update rate = TIM2 counter clock / CCR2_Val = 219.7 Hz
    CC3 update rate = TIM2 counter clock / CCR3_Val = 439.4 Hz
    CC4 update rate = TIM2 counter clock / CCR4_Val =  878.9 Hz
// --------------------------------------------------------------- 
*/
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 0xffff;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2,40, TIM_PSCReloadMode_Immediate);

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

  TIM_OC3Init(TIM2, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

  TIM_OC4Init(TIM2, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, DISABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, DISABLE);
}   

/* USART1 and USART2 configuration ---------------------------------------------*/
  /* USART and USART2 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
////////////////////////////////////////////////////////////////////////////////UARTS_Configuration
void UARTS_Configuration(void)
{ USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure; 
  
  USART_InitStructure.USART_BaudRate   = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits   = USART_StopBits_1;
  USART_InitStructure.USART_Parity     = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure);
  USART_Init(USART2, &USART_InitStructure);/* Configure USART2 MODEM*/
  USART_Init(USART3, &USART_InitStructure);/* Configure USART3 PC2*/
  
  
USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
USART_ClockStructInit(&USART_ClockInitStructure); 
  

  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /* Enable USART1 Receive and Transmit interrupts */
//  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

   USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  /* Enable USART2 Receive and Transmit interrupts */
//  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
  
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  /* Enable USART3 Receive and Transmit interrupts */
//  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  
  USART_Cmd(USART1, ENABLE); /* Enable the USART1 */
  USART_Cmd(USART2, ENABLE); /* Enable the USART2 */
  USART_Cmd(USART3, ENABLE); /* Enable the USART3 */
  
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
  
  PWR_BackupAccessCmd(ENABLE);  // Allow access to BKP Domain 
  BKP_DeInit();                 // Reset Backup Domain 
  RCC_LSEConfig(RCC_LSE_ON);    // Enable LSE

  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)  // Wait till LSE is ready 
  {
 /// printf(".");
  }
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);// Select LSE as RTC Clock Source
  RCC_RTCCLKCmd(ENABLE);// Enable RTC Clock
  RTC_WaitForSynchro();// Wait for RTC registers synchronization 
  RTC_WaitForLastTask();// Wait until last write operation on RTC registers has finished
  RTC_ITConfig(RTC_IT_SEC, ENABLE);  // Enable the RTC Second
  RTC_WaitForLastTask(); //Wait until last write operation on RTC registers has finished
  // Set RTC prescaler: set RTC period to 1sec
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
  RTC_WaitForLastTask();   // Wait until last write operation on RTC registers has finished
}
         
         
         
         
/*******************************************************************************
* Function Name  : PUTCHAR_PROTOTYPE
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
PUTCHAR_PROTOTYPE
{
  /* Write a character to the USART */
  USART_SendData(USART1, (u8) ch);

  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }

  return ch;
}

/*******************************************************************************
* Function Name  : USART_Scanf
* Description    : Gets numeric values from the hyperterminal.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 USART_Scanf(u32 value)
{
  u32 index = 0;
  u32 tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
    {}
    tmp[index++] = (USART_ReceiveData(USART1));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
      printf("\n\rPlease enter valid number between 0 and 9");
      index--;
    }
  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks */
  if (index > value)
  {
    printf("\n\rPlease enter valid number between 0 and %d", value);
    return 0xFF;
  }
  return index;
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
 
  }
}
#endif

