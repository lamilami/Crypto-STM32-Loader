#ifndef __FIFO__
#define __FIFO__

/* FIFO structure */
struct TFIFO
{
  char *ring_buf;
  char *write_ptr;
  char *read_ptr;
  int len;
};

/* Allocate FIFO */
int fifo_alloc(struct TFIFO *fifo, int len);

/* Free FIFO */
void fifo_free(struct TFIFO *fifo);

/* Check is FIFO empty */
int fifo_empty(struct TFIFO *fifo);

/* Check is FIFO full */
int fifo_full(struct TFIFO *fifo);

/* Put byte on FIFO */
char fifo_put(struct TFIFO *fifo, char byte);

/* Get byte from FIFO */
char fifo_get(struct TFIFO *fifo);

/* Clear FIFO */
void fifo_clear(struct TFIFO *fifo);

#endif

