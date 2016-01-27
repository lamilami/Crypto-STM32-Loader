/*****Includes********************************/
#include "Includes.h"

/* Private typedef -----------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
/*****Public Function Prototypes**************/
void PWM_Init(void);
void PWM_Config(uint16_t PWM);
void ToggleMotor(uint8_t run, uint16_t speed);
/* Private variables ---------------------------------------------------------*/
uint16_t PrescalerValue = 0;

/******************************************************************************
* PWM_Init
*
*       Initializes the PWM registers to automatically mantain PWM at
*       Port C pin 6
*
*       Parameters: data- the byte of data to be transmitted over SPI
*       returns   : none
*******************************************************************************/
void PWM_Init(void){
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
/******* Enable clocks used by PWM channel **************************/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//enable timer 3
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//enable Port C's clock
/****** Initialize Pins for PWM output *****************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //enable PWM output on Port C pin 6
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;// use pin's alternate function to allow PWM output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//set GPIO as push-pull type
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//enable Pin's pull up resistor
  GPIO_Init(GPIOC, &GPIO_InitStructure);   //initialize Port C with previous structure's contents
/****** Select Alternate Function *************************/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
/****** PWM setup *****************************************/
  PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 21000000) - 1; //compute the prescaler for the PWM channel
  TIM_TimeBaseStructure.TIM_Period =  1000;   //This is the "TIM3_ARR" Value-how many counts for 100% duty cycle
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;//what frequency should the PWM be running at (20KHz)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //No clock division needed-prescalervalue takes care of this
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //Initialize PWM based on structure settings above.
/****** PWM configuration for channel 1 ******************************/
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //detect PWM by comparing ARR value with current count value
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // toggle ouput depending on OC value
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //high when OC value below given CCR1 value
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//enable auto reload so the CPU keeps outputting a PWM
/****** Enable TIM3 counter ******************************/
  TIM_Cmd(TIM3, ENABLE);
}
/******************************************************************************
* PWM_Config
*
*       Sets the Duty cycle for the PWM channel on portC pin6
*       Parameters: duty_cycle - duty cycle for the PWM channel
*                   we can turn it off by setting duty cycle to zero
*       returns   : none
*******************************************************************************/
void PWM_Config(uint16_t duty_cycle){
          TIM3->CCR1 = duty_cycle;// - (brightness + 333/2) % 333; // set brightness
 }

/******************************************************************************
* ToggleMotor
*
*       Turns on the motor at the speed passed in
*       Parameters: run  - boolean to turn motor on or off
*                   speed-speed for motor to run at (in ratio of 'speed'/1000)
*                                   maximum speed is when 1000 passed in for 'speed'
*       returns   : none
*******************************************************************************/
void ToggleMotor(uint8_t run, uint16_t speed){
    if (run == FALSE){
        PWM_Config(0);
    }else if (run){
        PWM_Config(speed*10);
    }
}
