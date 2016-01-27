#ifndef _____INCLUDES_H___________
#define _____INCLUDES_H___________

#include <stm32f10x.h>

#ifndef INT8
#define INT8     signed char
#endif

#ifndef INT16
#define INT16    signed short int
#endif

#ifndef INT32
#define INT32    signed long  int
#endif

#ifndef INT8U
#define INT8U    unsigned char
#endif

#ifndef INT16U
#define INT16U   unsigned short int
#endif

#ifndef INT32U
#define INT32U   unsigned long  int
#endif

#ifndef NULL
#define NULL	((void*)0)
#endif

#ifndef BOOL
#define BOOL	INT8U
#endif



#ifndef TRUE
#define TRUE	(1 == 1)
#endif

#ifndef FALSE
#define FALSE	(0 == 1)
#endif	

#endif

