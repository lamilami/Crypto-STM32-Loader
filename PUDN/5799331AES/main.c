#include "aes.h"
#include "LPC17xx.h"
#include "uart.h"
#include "config.h"
#include "padlock.h"


int main(void)
{  unsigned char input[16]="0123456789abcdef";
   unsigned char output[16];
   unsigned char K[32];
   int C_UART0=0;
   unsigned int keysize=128	;
   aes_context ctx;



	SystemInit();
	UART0_Init();
	UART2_Init();
	memset(K, 1, 32 );
	//aes_setkey_enc(&ctx, K , 128 );

    while (1)
	{ input[C_UART0++]=UART0_GetChar ();
	  if (C_UART0==16)
	{
	 //aes_crypt_ecb(&ctx,1,input,output );
	 UART0_SendString (input);	
	 UART0_SendString (output);	 
	 C_UART0=0;}
	 }


}