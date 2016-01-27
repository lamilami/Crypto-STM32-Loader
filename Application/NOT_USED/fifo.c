#include "fifo.h"
#include <stdlib.h>
#include <stdio.h>

/* Allocate FIFO */
int fifo_alloc(struct TFIFO *fifo, int len)
{
  fifo->len = len;
  fifo->ring_buf = (char*)malloc(len);
  fifo->read_ptr = fifo->ring_buf;
  fifo->write_ptr = fifo->ring_buf;
  
  return (fifo->ring_buf != NULL);
}

/* Free FIFO */
void fifo_free(struct TFIFO *fifo)
{
  if(fifo)
    free(fifo->ring_buf);
}

/* Check is FIFO empty */
int fifo_empty(struct TFIFO *fifo)
{
  return (fifo->read_ptr == fifo->write_ptr);
}

/* Check is FIFO full */
int fifo_full(struct TFIFO *fifo)
{
  return  ((fifo->read_ptr - fifo->write_ptr == 1 
          || (fifo->write_ptr - fifo->read_ptr == fifo->len - 1)));
}

/* Put byte on FIFO */
char fifo_put(struct TFIFO *fifo, char byte)
{
  if(!fifo_full(fifo))
  {
    *fifo->write_ptr = byte;
    fifo->write_ptr++;
    if(fifo->write_ptr - fifo->ring_buf == fifo->len)
      fifo->write_ptr = fifo->ring_buf;
    return 1;
  }
  else
    return 0;
}

/* Get byte from FIFO */
char fifo_get(struct TFIFO *fifo)
{
  char byte;
  
  if(!fifo_empty(fifo))
  {
    byte = *fifo->read_ptr;
    fifo->read_ptr++;
    if(fifo->read_ptr - fifo->ring_buf == fifo->len)
      fifo->read_ptr = fifo->ring_buf;
    return byte;
  }
  else
    return EOF;
}

/* Clear FIFO */
void fifo_clear(struct TFIFO *fifo)
{
  fifo->read_ptr = fifo->ring_buf;
  fifo->write_ptr = fifo->ring_buf;
}