//Usart.h

#ifndef __USART_H__
#define __USART_H__

void USART1_IRQHandler (void);
void buffer_Init (void);
int SendChar (int c);
int GetKey (void);

#endif	