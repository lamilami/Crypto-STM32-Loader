#ifndef _____UART_H___________
#define _____UART_H___________

#include "includes.h"


#if defined(DFU_UART_RS485_MODE)
#define UART_RX485_TX_ON()		DFU_UART_RS485_DIR_PORT->ODR |= DFU_UART_RS485_DIR_PIN;TimerDelayMs(1)							
#define UART_RX485_TX_OFF()		DFU_UART_RS485_DIR_PORT->ODR &= (~DFU_UART_RS485_DIR_PIN);TimerDelayMs(1)	
#else
#define UART_RX485_TX_ON()
#define UART_RX485_TX_OFF()
#endif


void UartDisable(void);
void UartEnable(void);
BOOL UartPutChar(char c);
BOOL UartPutString(const char * s);
BOOL UartPutData(void* hData, INT32U DataLen);
char UartGetChar(INT32U Timeout);
INT32U UartGetStringEx(void* RxBuff, INT32U MaxLength, INT32U Timeout, char StatrChar, char EndChar);
void UartFix(void);

#define CON_ESC	(0xFFFFFFFF)
INT32U  ConsoleGetLineEx(char* RxBuff, INT32U Length, INT32U Timeout, char* Prompt, BOOL Echo, BOOL EscDetect);
#define ConsoleGetLine(RxBuff, Prompt, Echo, EscDetect)	ConsoleGetLineEx((RxBuff), DFU_UART_RX_BUFF_SIZE-1, 0, (Prompt), (Echo), (EscDetect))


#endif

