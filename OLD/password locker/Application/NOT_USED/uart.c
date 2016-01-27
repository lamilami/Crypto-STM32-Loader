/*Includes********************************************************/
#include "Includes.h"
#define MAX_STRLEN 12 // this is the maximum string length of our string in characters
#define BLUETOOTH_TASK_PRIORITY         ( tskIDLE_PRIORITY + 5 )

COLORS InputColors;
INPUT UsrInput;
volatile char received_string[13];
/* Public Functions ---------------------------------------------------------*/
void init_USART1(uint32_t baudrate);
void USART1_IRQHandler(void);
void USART_send(volatile char *string);
void BluetoothTask    ( void *pvParameters );

TaskHandle_t BluetoothTaskHandle     = NULL;
SemaphoreHandle_t User_Input_Mutex;
SemaphoreHandle_t Bluetooth_Semaphore;
/******************************************************************************
* init_USART1
*
*       Initializes USART1 to be used by the bluetooth communications
*
*       Parameters: baudrate- the baudrate at which the USART is supposed to operate
*******************************************************************************/
void init_USART1(uint32_t baudrate){
    GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
    USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
    NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)
/******* Initialize clock for USART1 ********************/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//enable clock for USART1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//enable clock to GPIO pins on portB
/******* Configure USART1 pins *******************************/
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pin 6 (TX) and Pin 7 (RX)
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;           // configured as alternate function to access them via USART1
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;      //IO speed, has nothing to do with baudrate
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         // set output as push pull mode
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;           // activate the pullup resistors on IO pins
    GPIO_Init(GPIOB, &GPIO_InitStruct);                 // set GPIO registers based on previous lines
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //specify the alternate function as USART1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1); //specify the alternate function as USART1
/******* Initialize USART1******************************/
    USART_InitStruct.USART_BaudRate = baudrate;             // the baudrate is set to the value we passed into this function
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;// 8-bit data frame size (standard)
    USART_InitStruct.USART_StopBits = USART_StopBits_1;     //1 stop bit (standard)
    USART_InitStruct.USART_Parity = USART_Parity_No;        // No parity bit (standard)
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // No flow control (standard)
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // Enable transmitter and receiver
    USART_Init(USART1, &USART_InitStruct);                  // Initialize USART1 with previous lines of code as settings
/******* Setup and Enable USART1 Interrupt***************/
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//Group_2); //changed interrupt priority group to cooperate with FreeRTOS
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;          // select the USART1 interrupt for configuration
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13; // set the priority group of the USART1 interrupts high to cooperate with FreeRTOS
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 13;        // set the subpriority also high to cooperate with FreeRTOS
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // Globally  enable USART1 interrupts
    NVIC_Init(&NVIC_InitStructure);                            // Pass in previous structure to Enable USART1 receive interrupt
/******** Enable USART1 Peripheral************************/
    USART_Cmd(USART1, ENABLE); //enable complete USART1 peripheral to allow modules to use it.
    xTaskCreate( BluetoothTask, "Bluetooth Task", configMINIMAL_STACK_SIZE, NULL, BLUETOOTH_TASK_PRIORITY, &BluetoothTaskHandle );
    User_Input_Mutex  = xSemaphoreCreateMutex();
    vSemaphoreCreateBinary( Bluetooth_Semaphore ); // Create the semaphore
    xSemaphoreTake(Bluetooth_Semaphore, 0);        // Take semaphore after creating it.
    UsrInput.InputColors.Red=0xa;
    UsrInput.InputColors.Green=0x05;
    UsrInput.InputColors.Blue=0x12;
}

/**************************************************************************************************
* USART_send
*
*       sends a string of characters via USART
*
*       Parameters: string- pointer to string to be transmitted, must be null terminated
***************************************************************************************************/
void USART_send(volatile char *string){
    uint8_t *tosend;
    tosend = string;
    while(*tosend != 0){
        while(!(USART1->SR & 0x00000040)); // wait until data register is empty
        USART_SendData(USART1, *tosend);        // send character via USART
        tosend++;                              // point to the next character to be transmitted
    }
}

/**************************************************************************************************
* USART_send
*
*       sends a string of characters via USART
*
*       Parameters: string- pointer to string to be transmitted, must be null terminated
***************************************************************************************************/
void USART1_IRQHandler(void){
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE; //FreeRTOS pointer to task woken up by semaphor post
    char receivedchar = 0; // the character from the USART1 data register is saved in t
    static int8_t receivednum = 0; // this counter is used to determine the string length
    receivedchar = USART_ReceiveData(USART1);
    received_string[receivednum] = receivedchar;
    receivednum++;
    if (receivednum == MAX_STRLEN || receivedchar == '\n'){
        received_string[receivednum] = 0;
        receivednum = 0;
        xSemaphoreGiveFromISR(Bluetooth_Semaphore, &xHigherPriorityTaskWoken);
    }
    USART_GetFlagStatus(USART1, USART_FLAG_RXNE);
}

void BluetoothTask( void *pvParameters ) {
    COLORS starcol;
    starcol.Red =0;
    starcol.Green =0;
    starcol.Blue =0;
    while(1) {
        switch (received_string[0]){
            case ('B'):         ///Change Background color behind seconds hand
                UsrInput.InputColors.Red = received_string[1];
                UsrInput.InputColors.Green = received_string[2];
                UsrInput.InputColors.Blue = received_string[3];
                received_string[0] = 0;
                SetColor(&UsrInput.InputColors);
            break;
            case ('F'):         ///Change background color fast
                UsrInput.InputColors.Red = received_string[1];
                UsrInput.InputColors.Green = received_string[2];
                UsrInput.InputColors.Blue = received_string[3];
                received_string[0] = 0;
                SetColor(&UsrInput.InputColors);
                UsrInput.Time= GetTime();
                ChangeClkColor(UsrInput.InputColors,UsrInput.Time);
            break;
            case ('P'):         ///Power on or Power off
                if ((received_string[1]=='N') && UsrInput.Run == FALSE){//turned on command received and motor is off
                    UsrInput.Run = TRUE;//~UsrInput.Run;
                    received_string[0] = 0;
                    EnableLEDOut();
                    vTaskDelay(8*ONEHUNDREDMILISEC);
                    ToggleMotor(UsrInput.Run,UsrInput.MotorSpeed);
                }else if((received_string[1]=='F') && UsrInput.Run == TRUE){
                    UsrInput.Run = FALSE;
                    received_string[0] = 0;
                    ToggleMotor(UsrInput.Run,UsrInput.MotorSpeed);
                    vTaskDelay(ONESECOND);
                    DisableLEDOut();
                }else{}
            break;
            case ('V'):             ///set motorspeed
                UsrInput.MotorSpeed = received_string[1];
                received_string[0] = 0;
            break;
            case ('T'):             ///adjust time
                UsrInput.Time.hour = received_string[1];
                UsrInput.Time.minute = received_string[2];
                ChangeClkTime(UsrInput.Time.hour,UsrInput.Time.minute);
                received_string[0] = 0;
            break;
            case ('R'):             ///reset the seconds counter
                ResetSeconds();
                received_string[0] = 0;
            break;
            case ('S'):             ///display a star
                DisplayStar(starcol);
                received_string[0] = 0;
            break;
            case ('L'):         ///Change Background color behind seconds hand
                UsrInput.LineColor.Red = received_string[1];
                UsrInput.LineColor.Green = received_string[2];
                UsrInput.LineColor.Blue = received_string[3];
                received_string[0] = 0;
                DisplayStar(UsrInput.LineColor);
            break;
            case ('G'):             ///show RGB POV example
                received_string[0] = 0;
            break;
            case ('D'):             /// Show vertical color stripes
                received_string[0] = 0;
            break;
            default:
            break;
        }
        vTaskDelay(TENMILISEC);
   }
}
