// STM32 USART IRQ TX/RX FIFO ECHO (USART1 Tx PA.9, Rx PA.10)
 
#include <stm32f10x.h>
 
//**************************************************************************************
 
// FIFO implementation from  http://coactionos.com/embedded%20design%20tips/2013/10/02/Tips-A-FIFO-Buffer-Implementation/
// Ivan's Thread  https://my.st.com/public/STe2ecommunities/mcu/Lists/STM32Discovery/Flat.aspx?RootFolder=%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fSTM32Discovery%2fUART%20example%20code%20for%20STM32F0&FolderCTID=0x01200200770978C69A1141439FE559EB459D75800084C20D8867EAD444A5987D47BE638E0F&TopicsView=https%3A%2F%2Fmy.st.com%2Fpublic%2FSTe2ecommunities%2Fmcu%2FLists%2FSTM32Discovery%2FAllItems.aspx¤tviews=10407
 
typedef struct {
     char * buf;
     int head;
     int tail;
     int size;
} fifo_t;
 
//This initializes the FIFO structure with the given buffer and size
void fifo_init(fifo_t * f, char * buf, int size){
     f->head = 0;
     f->tail = 0;
     f->size = size;
     f->buf = buf;
}
 
//This reads nbytes bytes from the FIFO
//The number of bytes read is returned
int fifo_read(fifo_t * f, void * buf, int nbytes){
     int i;
     char * p;
     p = buf;
     for(i=0; i < nbytes; i++){
          if( f->tail != f->head ){ //see if any data is available
               *p++ = f->buf[f->tail];  //grab a byte from the buffer
               f->tail++;  //increment the tail
               if( f->tail == f->size ){  //check for wrap-around
                    f->tail = 0;
               }
          } else {
               return i; //number of bytes read
          }
     }
     return nbytes;
}
 
//This writes up to nbytes bytes to the FIFO
//If the head runs in to the tail, not all bytes are written
//The number of bytes written is returned
int fifo_write(fifo_t * f, const void * buf, int nbytes){
     int i;
     const char * p;
     p = buf;
     for(i=0; i < nbytes; i++){
           //first check to see if there is space in the buffer
           if( (f->head + 1 == f->tail) ||
                ( (f->head + 1 == f->size) && (f->tail == 0) )){
                 return i; //no more room
           } else {
               f->buf[f->head] = *p++;
               f->head++;  //increment the head
               if( f->head == f->size ){  //check for wrap-around
                    f->head = 0;
               }
           }
     }
     return nbytes;
}
 
//**************************************************************************************
 
#define RXSIZE 1024
 
uint8_t RxBuffer[RXSIZE];
fifo_t RxFifo[1];
 
#define TXSIZE 512
 
uint8_t TxBuffer[TXSIZE];
fifo_t TxFifo[1];
 
//**************************************************************************************
 
void USART1_IRQHandler(void)
{
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // Received characters added to fifo
  {
    uint16_t rx;
 
    rx = USART_ReceiveData(USART1); // Receive the character
 
    fifo_write(RxFifo, &rx, 1); // Place in reception fifo
  }
 
  if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    uint8_t tx;
 
    if (fifo_read(TxFifo, &tx, 1) == 1) // Anything to send?
      USART_SendData(USART1, (uint16_t)tx); // Transmit the character
    else
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE); // Suppress interrupt when empty
  }
}
 
//**************************************************************************************