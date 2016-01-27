#include <stm32f10x.h>
#include <stdio.h>
 
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
	
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(__IO uint32_t nCount);
void GPIO_Config(void);
 
 
void USART1_Config(void);
void  Timer2_Config(void);
void  Timer11_Config(void);

/*******************************************************************************
* Function Name  :  USART_Config_Default.
* Description    :  configure the EVAL_COM1 with default values.
* Input          :  None.
* Return         :  None.
*******************************************************************************/
void USART1_Config(void)
{
	
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
      /*
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Parity Odd
        - Hardware flow control disabled
        - Receive and transmit enabled
       */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure and enable the USART */
 
	  USART_Init(USART1, &USART_InitStructure);
	  /* Enable USART */

	
  /* Configure and enable the USART */
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
 
  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	 USART_Init(USART1, &USART_InitStructure);
   USART_Cmd(USART1, ENABLE);

  /* Enable the USART Receive interrupt */


  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the USARTy Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

  
	//////////////////////////////////////////////////////////////////////////////////////////////////////GPIO_Config

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB |  RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE   | RCC_APB2Periph_TIM11  |
	                            RCC_APB2Periph_AFIO  , 	ENABLE); 
													
   RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR|RCC_APB1Periph_SPI2,ENABLE);
	
 //////////////////////////////////////////////////////////////////////////////////////////  SPI2
	/* PB13=CLK, PB15=MOSI alternate function push-pull output */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
		
	/* PB14=MISO pull-up input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	/* Configure PC13 as CS  open drain push-pull output */
	GPIO_SetBits(GPIOC, GPIO_Pin_13 ); //set high.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 

		/* SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; // 72000kHz/256=281kHz < 400kHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_CalculateCRC(SPI2, DISABLE);
	SPI_Cmd(SPI2, ENABLE);

	//SPI_SSOutputCmd(SPI1, ENABLE); //使能NSS脚可用
		
	/* Enable SPI2  */
	SPI_Cmd(SPI2, ENABLE);
//--------------------------------------复位一下总线，这个比较重要	 
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
		 	
    /* Loop while DR register in not emplty */
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI2, 0xff);

    /* Wait to receive a byte */
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    SPI_I2S_ReceiveData(SPI2);
////////////////////////////////////////////////////////////////////////////////////////////////////////LED PIN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	      //Make sure the LED's are workin

//////////////////////////////////////////////////////////////////////////////////////////////////////Buzzer PIN
  /* GPIOF Configuration: TIM11 Channel1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
//////////////////////////////////////////////////////////////////////////////////////////////////////EX MCU RST
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
//////////////////////////////////////////////////////////////////////////////////////////////////////EX MCU RST
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
//////////////////////////////////////////////////////////////////////////////////////////////////////Button PIN
GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOE,&GPIO_InitStructure);
}
 
 


//////////////////////////////////////////////////////////////////////////////////////////////////////////Timer2_Config
void  Timer2_Config(void)
{
/* ---------------------------------------------------------------
    TIM2 Configuration: Output Compare Timing Mode:
    TIM2 counter clock at 6 MHz
    CC1 update rate = TIM2 counter clock / CCR1_Val = 146.48 Hz
    CC2 update rate = TIM2 counter clock / CCR2_Val = 219.7 Hz
    CC3 update rate = TIM2 counter clock / CCR3_Val = 439.4 Hz
    CC4 update rate = TIM2 counter clock / CCR4_Val = 878.9 Hz
  --------------------------------------------------------------- */
 
  

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
__IO uint16_t CCR1_Val = 40961;
__IO uint16_t CCR2_Val = 27309;
__IO uint16_t CCR3_Val = 13654;
__IO uint16_t CCR4_Val = 6826;
uint16_t PrescalerValue = 0;
 
	 NVIC_InitTypeDef NVIC_InitStructure;
   /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
   /* TIM2 clock enable */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 12000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);


/*
  // Output Compare Timing Mode configuration: Channel2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

  // Output Compare Timing Mode configuration: Channel3
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

  //Output Compare Timing Mode configuration: Channel4 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
*/
  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update , ENABLE);// TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4,

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);


}
//////////////////////////////////////////////////////////////////////////////////////////////////////////Timer11_Config
 void  Timer11_Config(void)
{
/* -----------------------------------------------------------------------
    TIM11 Configuration: generate 1 PWM signal.
    The TIM11CLK frequency is set to SystemCoreClock (72 MHz), to get TIM10 counter
    clock at 24 MHz the Prescaler is computed as following:
     - Prescaler = (TIM11CLK / TIM11 counter clock) - 1

    The TIM11 is running at 36 KHz: TIM10 Frequency = TIM11 counter clock/(ARR + 1)
                                                  = 24 MHz / 666 = 36 KHz
    TIM11 Channel1 duty cycle = (TIM11_CCR1/ TIM11_ARR)* 100 = 37.5%
  ----------------------------------------------------------------------- */
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
	
uint16_t CCR1Val = 700;
uint16_t PrescalerValue = 0;
	   
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 12000000) - 1;
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period =3000; //665=18kHz;// 4k
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM11, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM11, ENABLE);

  /* TIM11 enable counter */
  TIM_Cmd(TIM11, ENABLE);
	
 
	
}
 
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif	
//////////////////////////////////////////////////////////////////////////////////////////////////////Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/* Configure all unused GPIO port pins in Analog Input mode (floating input
	 trigger OFF), this will reduce the power consumption and increase the device
	 immunity against EMI/EMC 
	/////////////////////////////////////////////////
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
	                        RCC_APB2Periph_GPIOD |
	                     RCC_APB2Periph_GPIOE, ENABLE);
//////////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
	                    RCC_APB2Periph_GPIOD |
	                     RCC_APB2Periph_GPIOE, DISABLE);
	*/	
