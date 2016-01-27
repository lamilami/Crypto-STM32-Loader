/**
  * @date  2011-04-17
  * @brief application.
  **/
#include "app.h"

FATFS fs; /* Work area (file system object) for logical drives */
BYTE w_buffer[81];   /* file copy buffer */
FRESULT res;  /* FatFs function common result code */
UINT bw; /* File read/write count */

//FATFS fs; /* Work area (file system object) for logical drives */
//BYTE w_buffer[81];   /* file copy buffer */
//FRESULT res;  /* FatFs function common result code */
//UINT bw; /* File read/write count */

void Create_File(void)
{
	uint16_t i;
	FIL f;

	f_mount(0, &fs);
	res = f_open(&f, "dd.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK) return; 
 

	w_buffer[0] = 'T';
	w_buffer[1] = 'T';
	for(i=2; i<81; i++)	//Ìî³ä8192¸öÊý
		w_buffer[i] = 0x5a;
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	
	for(i=125; i>0; i--)	//100M
	{
		res = f_write(&f, w_buffer, 81, &bw);
		if(bw < 81)
			break;
	}
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	res = f_close(&f);
	if(res != FR_OK)
		return;

	f_mount(0, NULL);
}
