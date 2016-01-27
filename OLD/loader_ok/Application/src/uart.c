#include <stm32f10x.h>
#include "uart.h"

__IO uint32_t PreTick;

//用于字符检查的宏
//#define isgraph(ch)	((ch >= 0x21) && (ch <= 0x7E))    //是否为可打印字符(不包含空格)
#define isprint(ch)	((ch >= 0x20) && (ch <= 0x7E))    //是否为可打印字符(包含空格)

 #define  UartPort USART1


#if defined(DFU_UART_RS485_MODE)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DFU_UART_RS485_DIR_APB_PERIPH, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DFU_UART_RS485_DIR_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DFU_UART_RS485_DIR_PORT, &GPIO_InitStructure);
}
#endif

 
BOOL UartPutChar(char c)
{
	if(NULL == UartPort){
		return FALSE;
	}
	else{
		while((~UartPort->SR) & USART_SR_TXE);
		UartPort->DR = c;
		while(!(UartPort->SR & USART_SR_TC));
		return TRUE;		
	}
}

BOOL UartPutString(const char * s)
{
	if(NULL == UartPort){
		return FALSE;
	}
	else{
		while(*s != '\0')
		{
			while(!(UartPort->SR & USART_SR_TXE));
			UartPort->DR = *s++;
			while(!(UartPort->SR & USART_SR_TC));
		}
		return TRUE;		
	}
}

BOOL UartPutData(void* hData, INT32U DataLen)
{
	char* pData;
	pData = (char*)hData;
	if(NULL == UartPort){
		return FALSE;
	}
	else{
		while(DataLen--)
		{
			while((~UartPort->SR) & USART_SR_TXE);
			UartPort->DR = *pData++;
			while(!(UartPort->SR & USART_SR_TC));
		}
		return TRUE;		
	}
}

char UartGetChar(INT32U Timeout)
{
	if(NULL == UartPort){
		return 0;
	}
	else{
	 	char rtn;
		PreTick = Timeout;
		while(0 == (UartPort->SR & USART_SR_RXNE)){	
			if(!PreTick){
				UartPort->SR ^= ~(USART_SR_RXNE);
				return 0;
			}
		}
		rtn = UartPort->DR;		   
		UartPort->SR ^= ~(USART_SR_RXNE);
		return rtn;
	}
	
}

INT32U ConsoleGetLineEx( 
	char* RxBuff, 
	INT32U Length, 
	INT32U Timeout, 
	char* Prompt,
	BOOL Echo, 
	BOOL EscDetect
)
{
	if(NULL == UartPort){
		return 0;
	}
	else{
		INT8U* pRxBuff = (INT8U*)RxBuff;
		INT32U LengthX;	
		//INT32U Skip = 0;
		INT8U  RxTemp;
		
		//清除现有数据
		RxTemp = UartPort->DR;
		UartPort->SR ^= ~(USART_SR_RXNE);

		if(Echo){
			if(Prompt)UartPutString(Prompt);		
		}

		LengthX = Length;
		
		while(Length-- || (LengthX == 0)){
			PreTick = Timeout;
			while(0 == (UartPort->SR & USART_SR_RXNE)){	
				if(!PreTick)
				{
					UartPort->SR ^= ~(USART_SR_RXNE);
					return pRxBuff - (INT8U*)RxBuff;
				}
			}
			RxTemp = UartPort->DR;
			UartPort->SR ^= ~(USART_SR_RXNE);
			switch(RxTemp){
				case '\r':{ //Enter
					INT32U CmdLineLen;
					*pRxBuff = '\0';
					CmdLineLen = pRxBuff - (INT8U*)RxBuff;
					if(Echo){
						UartPutChar('\r');UartPutChar('\n');
					} 
					return CmdLineLen;
					//break;
				}

				case '\b':	//backspace	
					if((pRxBuff > (INT8U*)RxBuff) && Echo){
						UartPutChar('\b'); 
						UartPutChar(' ');
						UartPutChar('\b');
						pRxBuff--;
					}
					break;

				case 0x1b:{ //终端方向键 ESC
 					if(Echo){
						//TimerDelayMs(10);
						if(UartPort->SR & USART_SR_RXNE){
							RxTemp = UartPort->DR;
							UartPort->SR ^= ~(USART_SR_RXNE);
							//这里可以判断方向按键
						}
						else{
							if(EscDetect){	
								return CON_ESC;	
							}							
						}
					}
					break;
				}
				default:
					if(isprint(RxTemp)){ //可打印字符
						if(Echo) {
							UartPutChar(RxTemp);
						}
						*pRxBuff++ = RxTemp;
					}
					break;					
			}	
								   
		}
		(*pRxBuff) = '\0';
		return pRxBuff - (INT8U*)RxBuff;
	}
	
}

INT32U UartGetStringEx(void* RxBuff, INT32U MaxLength, INT32U Timeout, char StatrChar, char EndChar)
{
	if(NULL == UartPort){
		return 0;
	}
	else{
		INT8U* pRxBuff = (INT8U*)RxBuff;
		INT8U  RxTemp;
		BOOL   Start = FALSE;
		while(MaxLength){
			PreTick = Timeout;
		 
			while(0 == (UartPort->SR & USART_SR_RXNE))
			{	
				if(!PreTick)
					{
					UartPort->SR ^= ~(USART_SR_RXNE);
					*pRxBuff = '\0'; return pRxBuff - (INT8U*)RxBuff;
				  }
			}
			RxTemp = UartPort->DR;
			UartPort->SR ^= ~(USART_SR_RXNE);
			if(Start)
			{
				if(RxTemp == EndChar){
					*pRxBuff++ = RxTemp;
					*pRxBuff = '\0'; return pRxBuff - (INT8U*)RxBuff;
				}
				else{
					*pRxBuff++ = RxTemp;
					MaxLength--;				
				}
			}
			else{
				if(RxTemp == StatrChar){
					Start = TRUE;
					*pRxBuff++ = RxTemp;
					MaxLength--;
				}
			}		   	
		}
		*pRxBuff = '\0'; return pRxBuff - (INT8U*)RxBuff;
	}
}

INT32U UartGetData(void* RxBuff, INT32U Length, INT32U Timeout)
{
	if(NULL == UartPort){
		return 0;
	}
	else{
		INT8U* pRxBuff = (INT8U*)RxBuff;
		INT32U PreTick;
		while(Length--){
			PreTick = Timeout;
			while(0 == (UartPort->SR & USART_SR_RXNE))
		 {	
				if(!PreTick){
					UartPort->SR ^= ~(USART_SR_RXNE);
					return pRxBuff - (INT8U*)RxBuff;
				}
			}		   
			*pRxBuff++ = UartPort->DR;
			UartPort->SR ^= ~(USART_SR_RXNE);
		}
		return pRxBuff - (INT8U*)RxBuff;
	}
}

void UartFix(void)
{
	if(UartPort){
	    USART_ClearFlag(UartPort,USART_FLAG_ORE);	//读SR
	    USART_ReceiveData(UartPort);//读DR	
	}
	return;
}

void UartDisable(void)
{
	if(UartPort){
	    USART_Cmd(UartPort, DISABLE);	
	}
	return;
}

void UartEnable(void)
{
	if(UartPort){
	    USART_Cmd(UartPort, ENABLE);	
	}
	return;
}				





