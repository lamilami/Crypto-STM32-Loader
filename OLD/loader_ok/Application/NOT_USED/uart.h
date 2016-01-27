
/*******Public UART Functions*********************/
extern void init_USART1(uint32_t baudrate);
extern void USART1_IRQHandler(void);
void USART_send(volatile char *string);
extern volatile char received_string[13]; // this will hold the recieved string



