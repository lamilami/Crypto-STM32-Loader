
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"

#include   "diskio.h"
#include  "uart.h"

char PASS[40]; 



void Card_UTL(void)
{
	
		DSTATUS stat;
char CMD[40];
char  *p;  
	
	/*
	{
	printf("Command: \r\n");
	printf("u - UNLOCK\r\n");
	printf("l - LOCK\r\n");
	printf("? - INFO\r\n");
	printf("p - PASSWORD\r\n");
	}	
	*/
	
	UartGetStringEx(CMD, 40, 200,'-', '\r'); 
  
		
	switch(CMD[1])
	{
case 'u': 
                       p=strstr(CMD,"-u");
											 if(p)
											 { sprintf(PASS,"%s",p+3);
				                 PASS[strlen(PASS)-1]='\0';     
				            
											   printf("Trying to unlock with pass:%s\r\n",PASS);	
												 
												   stat = disk_initialize(0);
												 
	                         if (stat == STA_NOINIT) { printf("Error, check card type.\r\n");	  break; }
	                         if (stat == STA_PROTECT){ printf("Error, card is write protected.\r\n");	 break; }
													 if (stat == STA_NODISK) { printf("Error, check card in holder.\r\n");	 break; }
												 
													 
													if (ReadCardStatus()& 0x01)	//  unlocked ==0/ locked==1
													{
 
														if(ModifyPWD((BYTE*)PASS,MASK_CLR_PWD)==RES_OK)
														{
														
															if ((ReadCardStatus()& 0x01)) printf("Failed!  Card is still locked.\r\n");	
													   	else                         	printf("DONE !\r\n");	
																												
														}else printf("Error, check password: %s\r\n",PASS);	
										 

												   }else  printf("This card is not locked\r\n");	


													}else   printf("Error,password is blank: %s\r\n",PASS);	
break;
												case 'l': printf("l -OK\r\n");break;
   case 'p':

											 p=strstr(CMD,"-p");
											 if(p)
											 { sprintf(PASS,"%s",p+3);
				                 PASS[strlen(PASS)-1]='\0';
												 
												 printf("Trying to lock with pass:%s\r\n",PASS);	
												 
												 					   stat = disk_initialize(0);
												 
	                         if (stat == STA_NOINIT) { printf("Error, check card type.\r\n");	 break; }
	                         if (stat == STA_PROTECT){ printf("Error, card is write protected.\r\n");	 break; }
													 if (stat == STA_NODISK) { printf("Error, check card in holder.\r\n");	 break; }
												 
													if ((ReadCardStatus()& 0x01) == 0)	//  unlocked ==0/ locked==1
											    {
														if(ModifyPWD((BYTE*)PASS,MASK_SET_PWD)==RES_OK)
														{	
                                if((ReadCardStatus()& 0x01)){ printf("PASSWORD IS: %s\r\n",PASS);	 break; }
																else 
																{ printf("Card is still unlocked.Wait!\r\n");	
																  if(ModifyPWD((BYTE*)PASS,MASK_LOCK_UNLOCK)==RES_OK)
										         			{ 	
                                    if((ReadCardStatus()& 0x01)) printf("PASSWORD IS: %s\r\n",PASS);	
																	  else printf("Card locked failed!\r\n");	
																	}else  printf("Error,password [%s] is not set...\r\n",PASS);	
																	
																};  	
																	
															}else  printf("Error,pass is not set...\r\n");																									
													 				
													
													}else   printf("Card is locked...\r\n");	
													}else   printf("Error,password is blank: %s\r\n",PASS);	
 
};

};