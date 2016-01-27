#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <string.h>
#include <stdio.h>

#include "serial.h"
#include "delay.h"
#include "fifo.h"

#define SEND_BUF_SIZE 200
#define RECV_BUF_SIZE 50

/* Serial port structure */
struct TSerial
{
  /* Hardware registers */
  USART_TypeDef *USART;
  /* Transmit and recieve queues */
  struct TFIFO send_buf;
  struct TFIFO recv_buf;  
};

/* Serial ports 1 and 2 */
struct TSerial serial_port1, serial_port2;
struct TSerial *serial1 = &serial_port1;
struct TSerial *serial2 = &serial_port2;

int rd_timeout = 0;
int wr_timeout = 0;

/* USART hardware configuration: baudrate from arg, 8 bit, 1 stop, no parity, no flow control*/
void USART_Configuration(USART_TypeDef* USARTx, int baudrate)
{
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure USARTx */
  USART_Init(USARTx, &USART_InitStructure);
  
  /* Enable the USARTx */
  USART_Cmd(USARTx, ENABLE);
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
}

/* Handler of intterupts from USART1 */
void USART1_IRQHandler(void)
{
  char byte;
  
  if(USART_GetITStatus(serial_port1.USART, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register
     * and if we have empty space place it to the buffer
     */
    byte = USART_ReceiveData(serial_port1.USART);
    fifo_put(&serial_port1.recv_buf, byte);
  }
  
  if(USART_GetITStatus(serial_port1.USART, USART_IT_TXE) != RESET)
  {
    /* Place one byte from send buffer to USART
     * if we have one
     */
    if(!fifo_empty(&serial_port1.send_buf))
    {
      byte = fifo_get(&serial_port1.send_buf);
      USART_SendData(serial_port1.USART, byte);
    }
    else
      USART_ITConfig(serial_port1.USART, USART_IT_TXE, DISABLE);
  }
}

/* Handler of intterupts from USART2 */
void USART2_IRQHandler(void)
{
  char byte;
  
  if(USART_GetITStatus(serial_port2.USART, USART_IT_RXNE) != RESET)
  {
    /* REDIRECTION: read one byte from the receive data register
     * and if we have empty space in the send buffer of SERIAL 1 place it there
     */
    byte = USART_ReceiveData(serial_port2.USART);
    fifo_put(&serial_port1.send_buf, byte);
  }
  
  if(USART_GetITStatus(serial2->USART, USART_IT_TXE) != RESET)
  {
    /* Place one byte from send buffer to USART
     * if we have one
     */
    if(!fifo_empty(&serial_port2.send_buf))
    {
      byte = fifo_get(&serial_port2.send_buf);
      USART_SendData(serial_port2.USART, fifo_get(&serial_port2.send_buf));
    }
    else
      USART_ITConfig(serial_port2.USART, USART_IT_TXE, DISABLE);
  }
}

void Serial_Initialize(int baudrate, int read_timeout, int write_timeout)
{ 
  rd_timeout = read_timeout;
  wr_timeout = write_timeout;
  
  NVIC_Configuration();
  
  /* Initialize serial ports structures */
  serial_port1.USART = USART1;
  serial_port2.USART = USART2;
  /* Allocate queues */
  fifo_alloc(&serial_port1.send_buf, SEND_BUF_SIZE);
  fifo_alloc(&serial_port1.recv_buf, RECV_BUF_SIZE);
  fifo_alloc(&serial_port2.send_buf, SEND_BUF_SIZE);
  
  /* Enable the USARTx receive interrupt */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  
  USART_Configuration(USART1, baudrate);
  USART_Configuration(USART2, baudrate);
}

/* FIFO not full predicate adapter */
int fifo_not_full(void *arg)
{
  return !fifo_full((struct TFIFO*)arg);
}

/* Putchar for specified serial */
int putchar_serial(int ch, struct TSerial *serial) 
{ 
  /* Wait for empty space in transmit buffer */
  if(WaitForSingleEvent(wr_timeout, fifo_not_full, &serial->send_buf) != -1)
  {
    fifo_put(&serial->send_buf, ch);
    USART_ITConfig(serial->USART, USART_IT_TXE, ENABLE);
    return ch;
  }
  else
    return EOF;
}

/* System wide default putchar (Serial 1) */
int putchar(int ch)
{
  return putchar_serial(ch, serial1);
}

/* FIFO not empty predicate adapter */
int fifo_not_empty(void *arg)
{
  return !fifo_empty((struct TFIFO*)arg);
}

/* Getchar from specified serial */
int getchar_serial(struct TSerial *serial) 
{
  char byte = EOF;
  
  /* Wait the end of reception */
  if(WaitForSingleEvent(rd_timeout, fifo_not_empty, &serial->recv_buf) != -1)
    byte = fifo_get(&serial->recv_buf);
  
  return byte;
}

/* System wide default getchar (Serial 1) */
int getchar()
{
  return getchar_serial(serial1);
}

/* Clear all buffered bytes */
void Serial_Clear()
{
  USART_ClearFlag(serial_port1.USART, USART_FLAG_RXNE);
  fifo_clear(&serial_port1.recv_buf);
}

/* Put all symbols of string until terminator to specified serial */
void putstring(const char *string, struct TSerial *serial)
{
  int i = 0;
  
  while(string[i])
  {
    putchar_serial(string[i], serial);
    i++;
  }
}

/* Get string from stdin until line feed 
 * without full feeds at the start
 */
char *gets_trim(char *string)
{
  int ch = '\n';
  int i = 0;
  
  /* skip all returns and newlines */
  while (ch == '\n' || ch == '\r')
  {
    ch = getchar();
  }
  /* store all symbols until newline */
  while (ch != '\n' && ch != EOF)
  {
    string[i] = ch;
    i++;
    ch = getchar();
  }
  /* place termination zero (removing return) */
  if(i > 0)
  {
    if(string[i-1] == '\r')
      string[i-1] = 0;
    else
      string[i] = 0;
  }
  else
    string[0] = 0;
  
  return string;
}