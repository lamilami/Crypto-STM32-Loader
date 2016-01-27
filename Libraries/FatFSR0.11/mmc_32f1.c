/* date:2011-04-16 */
/* FatFs µÍ²ãIOÇý¶¯ */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "diskio.h"



/* Private define -----------------------------------------------------------*/
/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) *//* CMD1 - start initialization */


#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	 0x10    //(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */
 
#define  SD_SEND_STATUS		(0x40 + 13)			/* CMD13 - send card status */
#define  SD_LOCK_UNLOCK		(0x40 + 42)			/* CMD42 - lock/unlock card */  
// #define CMD_CRC_ON_OFF 0x3b
 
 
volatile  uint32_t SD_timer1, SD_timer2;
static volatile	 DSTATUS Stat = STA_NOINIT;	/* Disk status */
static BYTE CardType;

extern __IO uint16_t beep;

/* Private function prototypes -----------------------------------------------*/
static u8 SPI_ReadWrite_Byte(unsigned char byte);
static void SPI_LowSpeed(void);
static void SPI_HighSpeed(void);
static BYTE Wait_Ready (void);
static void SPI_Release(void);
static BYTE Send_Command(BYTE cmd, DWORD arg);
static bool Receive_DataBlock(BYTE *buff, UINT btr);
static bool Transmit_DataBlock(const BYTE *buff, BYTE token);

/* Private functions ---------------------------------------------------------*/
static u8 SPI_ReadWrite_Byte(unsigned char byte)
{
    /* Loop while DR register in not emplty */
//    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while((SPI2->SR & SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI2 peripheral */
  //SPI_I2S_SendData(SPI2, byte);
 SPI2->DR = byte;
    /* Wait to receive a byte */
    // while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
   while((SPI2->SR & SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
//    return SPI_I2S_ReceiveData(SPI2); 
	return (SPI2->DR); 
}

static void SPI_LowSpeed(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    /* SPI2 configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI2, &SPI_InitStructure);

    /* SPI2 enable */
    SPI_Cmd(SPI2, ENABLE);
}

static void SPI_HighSpeed(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    /* SPI2 configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI2, &SPI_InitStructure);
    /* SPI2 enable */
    SPI_Cmd(SPI2, ENABLE);
}

/**
  * @brief  Wait for card ready.
  **/
static BYTE Wait_Ready (void)
{
    BYTE res;

    SD_timer2 = 500;	/* Wait for ready in timeout of 500ms */
    SPI_ReadWrite_Byte(0xff);;
    do
	{
        res = SPI_ReadWrite_Byte(0xff);
    }while((res != 0xFF) && SD_timer2);

    return res;
}

/**
  * @brief  Deselect the card and release SPI bus.
  **/
static void SPI_Release(void)
{
    SD_DESELECT();
    SPI_ReadWrite_Byte(0xff);
}

/**
  * @brief  Send a command packet to MMC.
  * @Output: R1, R2, R3 response                                           
             In idle mode R1 Response is 0x01                              
             When the card is initialized successfuly, R1 response is 0x00. 
  **/
static BYTE Send_Command(BYTE cmd,		/* Command byte */
        				 DWORD arg)		/* Argument */
{
    BYTE n, res;

    /* Is a ACMD<n>? */
    if(cmd & 0x80) /* ACMD<n> is the command sequense of CMD55-CMD<n> */
	{	
        cmd &= 0x7F;
        res = Send_Command(CMD55, 0);
        if(res > 1) return res;
    }

    /* Select the card and wait for ready */
    SD_DESELECT();
    SD_SELECT();
		
    if(Wait_Ready() != 0xFF) return 0xFF;

    /* Send command packet */
    SPI_ReadWrite_Byte(cmd);
    SPI_ReadWrite_Byte((BYTE)(arg >> 24));		/* Argument[31..24] */
    SPI_ReadWrite_Byte((BYTE)(arg >> 16));		/* Argument[23..16] */
    SPI_ReadWrite_Byte((BYTE)(arg >> 8));		/* Argument[15..8] */
    SPI_ReadWrite_Byte((BYTE)arg);				/* Argument[7..0] */
    n = 0x01;							        /* Dummy CRC + Stop */
    if(cmd == CMD0) n = 0x95;			        /* Valid CRC for CMD0(0) */
    if(cmd == CMD8) n = 0x87;			        /* Valid CRC for CMD8(0x1AA) */
    SPI_ReadWrite_Byte(n);

    /* Receive command response */
    if(cmd == CMD12) 
	{ 
        /* The received byte immediataly following 
         * CMD12 is a stuff byte, it should be discarded
         * before receive the response of the CMD12. */
        SPI_ReadWrite_Byte(0xff);		    
    }

    /* Wait for a valid response in timeout of 10 attempts */
    n = 10;								
    do
	{
        res = SPI_ReadWrite_Byte(0xff);
    }while((res & 0x80) && --n);

    return res;			/* Return with the response value */
}

/**
  * @brief  Receive a data packet from SD.
  * @param  buff: Data buffer to store received data.
            btr: Byte count (must be multiple of 4).
  * @return FALSE: fail.
  			TURE: success.
  **/
static bool Receive_DataBlock(BYTE *buff, UINT btr)			
{
    BYTE token;
    
    SD_timer1 = 200;
    do /* Wait for data packet in timeout of 100ms */
	{							
        token = SPI_ReadWrite_Byte(0xff);
    } while((token == 0xFF) && SD_timer1);
    if(token != 0xFE) return false;	/* If not valid data token, retutn with error */

    do 	 /* Receive the data block into buffer */
	{						
        *buff++ = SPI_ReadWrite_Byte(0xff);
    } while(btr--);
    SPI_ReadWrite_Byte(0xff);		/* Discard CRC */
    SPI_ReadWrite_Byte(0xff);

    return true;	 /* Return with success */
}

/**
  * @brief  Send a data packet to SD.
  * @param  buff: 512 byte data block to be transmitted.
            token: Data/Stop token.
  * @return FALSE: fail.
			TURE: success.
  **/
#if !_READONLY

static bool Transmit_DataBlock(const BYTE *buff, BYTE token)	
{
    BYTE resp;
    UINT wc;

    if(Wait_Ready() != 0xFF) return false;

    SPI_ReadWrite_Byte(token);	 /* Transmit data token */
    if(token != 0xFD)  /* Is data token, 0xFD for stop token */
	{	            
        wc = 512;
        do 
		{							/* Transmit the 512 byte data block to MMC */
            SPI_ReadWrite_Byte(*buff++);
        } while(--wc);
        SPI_ReadWrite_Byte(0xFF);		/* CRC (Dummy) */
        SPI_ReadWrite_Byte(0xFF);
        resp = SPI_ReadWrite_Byte(0xff);/* Reveive data response */
        if((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
            return false;
    }
    return true;
}
#endif /* !_READONLY */

/**
  * @brief  Initialize disk drive.
  * @param  drv: Physical drive nmuber,must be '0'.
  * @return	STA_NOINIT: no initial.
  			0: success.
  * @other  called by FF.C.
  **/
DSTATUS disk_initialize(BYTE drv)
{
	BYTE n, type, cmd, ocr[4];

	if(drv) return STA_NOINIT;			    /* Supports only single drive */
	if(Stat & STA_NODISK) return Stat;	    /* No card in the socket */

   for(SD_timer1=800; SD_timer1>0; ); //delay 500ms for power on.
	

	SPI_LowSpeed(); //low speed, 72000/256=281.25KHz
	SD_DESELECT();

	/* Wait for enter Idle state in timeout of 5000ms */
    SD_timer1 = 5000;
    do
	{
        for(n=10; n>0; n--) SPI_ReadWrite_Byte(0xff);	    /* 80 dummy clocks */
    } /*R1 response, return 0x01,with In Idle State bit (0x01). */
    while((Send_Command(CMD0,0) != 1) && SD_timer1);

    type = 0;  //card type.
    SD_timer1 = 2000;	/* Initialization timeout of 2000 msec */
		
    if(Send_Command(CMD8, 0x1AA) == 1) /* check whether support SDC Ver2 or not*/
	{	    
        for(n = 0; n < 4; n++) 
            ocr[n] = SPI_ReadWrite_Byte(0xff);	/* Get trailing return value of R7 resp */
        if(ocr[2] == 0x01 && ocr[3] == 0xAA) 
		{			
            /* The card can work at vdd range of 2.7-3.6V */
            /* Wait for leaving idle state (ACMD41 with HCS bit) */
            while(SD_timer1 && Send_Command(ACMD41, 1UL << 30));  				
            if(SD_timer1 && Send_Command(CMD58, 0) == 0) 
			{		
                /* Check CCS bit in the OCR */
                for(n = 0; n < 4; n++)
                    ocr[n] = SPI_ReadWrite_Byte(0xff);
                /* When CCS bit is set  R/W in block address insted of byte address */
                type = (ocr[0] & 0x40) ? 12 : 4;
            }
        }
    } 
	else  /* SDV1 or MMC */
	{							
  		/* initialize successful will response 0x00 */
        if(Send_Command(ACMD41, 0) <= 1) 	
		{  
            type = 2; cmd = ACMD41;	/* SDv1 */
        } 
		else 
		{
            type = 1; cmd = CMD1;		/* MMC */
        }
        while(SD_timer1 && Send_Command(cmd, 0));			/* Wait for leaving idle state */
				
        if(!SD_timer1 || Send_Command(CMD16, 512) != 0)	/* Set R/W block length to 512 */
            type = 0;
    }

    CardType = type;	
    SPI_HighSpeed();
    SPI_Release();

    if(type) 
	{
        /* Initialization succeded, stat=0*/
        Stat &= ~STA_NOINIT;	
    } 
    return Stat;
}
/**
  * @brief  Get Disk Status.
  **/
DSTATUS disk_status(BYTE drv)	/* Physical drive nmuber (0) */
{
    if(drv) return STA_NOINIT;		/* Supports only single drive */
    return Stat;
}

/**
  * @brief  Read Sector(s).
  * @param  drv: Physical drive nmuber (0).
  *         buff: Pointer to the data buffer to store read data.
  *         sector: Start sector number (LBA).
  *         count: Sector count (1..255).
  * @return RES_PARERR: invalid parameter.
  			RES_NOTRDY: not ready.
			RES_ERROR: read fail.
  			RES_OK: read success.                                                         
  **/
DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)			
{
	if(drv || !count) return RES_PARERR;
	if(Stat & STA_NOINIT) return RES_NOTRDY;

    if(!(CardType & 8)) sector *= 512;	/* Convert to byte address if needed */

    if(count == 1) /* Single block read */
	{	
        if((Send_Command(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
             && Receive_DataBlock(buff, 512))
            count = 0;
    }
    else 	/* Multiple block read */
	{			
        if(Send_Command(CMD18, sector) == 0) 
		{	
            do 
			{
                if(!Receive_DataBlock(buff, 512)) break;
                buff += 512;
            } while(--count);
            Send_Command(CMD12, 0);	/* STOP_TRANSMISSION */
        }
    }
    SPI_Release();

    return count ? RES_ERROR : RES_OK;
}

/**
  * @brief  Write Sector(s).
  * @param  drv: Physical drive nmuber(0).
            buff: Pointer to the data to be written.
            sector: Start sector number (LBA).
            count: Sector count (1..255).
  * @return RES_PARERR: invalid parameter.
  			RES_NOTRDY: not ready.
			RES_WRPRT: write protected.
			RES_ERROR: fail.
			RES_OK: success.
  **/
#if !_READONLY

BYTE  ReadCardStatus(void)
{
	 BYTE re=99;
	   /* Select the card and wait for ready */
	re = Send_Command(SD_SEND_STATUS, 0);
	
	re = SPI_ReadWrite_Byte(0xff);
	SPI_ReadWrite_Byte(0xff);
	return  re;
}
//////////////////////////////////////////////////////////////////////////////////////////////ModifyPWD(char *pwd)
DRESULT  ModifyPWD(const BYTE *pwd,BYTE mask)
{
 
	uint16_t	i;
	int8_t    pwd_len;
	
	pwd_len=sizeof(pwd);
	mask = mask;					// top five bits MUST be 0, do not allow forced-erase!
  if(Send_Command(SD_LOCK_UNLOCK, 0)!=0)return  RES_ERROR;
 
  SPI_ReadWrite_Byte(0xFE);								// send data token marking start of data block

	SPI_ReadWrite_Byte(mask);		
	SPI_ReadWrite_Byte(pwd_len);						// then send the password length
	for (i=0; i<512; i++)				// need to send one full block for CMD42
	{
		if (i < pwd_len)
		{
    		SPI_ReadWrite_Byte(pwd[i]);					// send each byte via SPI
		}
		else
		{
			SPI_ReadWrite_Byte(0xff);
		}
	}

	SPI_ReadWrite_Byte(0xff);							// ignore dummy checksum
	SPI_ReadWrite_Byte(0xff);							// ignore dummy checksum


	if (Wait_Ready())  return  RES_OK;			// return success
 	else  return  RES_ERROR;		// nope, didn't work
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)	
{
    if(drv || !count)      return RES_PARERR;
    if(Stat & STA_NOINIT)  return RES_NOTRDY;
    if(Stat & STA_PROTECT) return RES_WRPRT;

    if(!(CardType & 8)) sector *= 512;	/* Convert to byte address if needed */

    if(count == 1) /* Single block write */
	{	
        if((Send_Command(CMD24, sector) == 0)	/* WRITE_BLOCK */
             && Transmit_DataBlock(buff, 0xFE))
            count = 0;
    }
    else  /* Multiple block write */
	{				
        if(CardType & 6) Send_Command(ACMD23, count); //for SDC.
        if(Send_Command(CMD25, sector) == 0) 
		{	
            do 
			{
                if(!Transmit_DataBlock(buff, 0xFC)) break;
                buff += 512;
            } while(--count);
            if(!Transmit_DataBlock(0, 0xFD))	/* STOP_TRAN token */
                count = 1;   /* faild */
        }
    }
    SPI_Release();

    return count ? RES_ERROR : RES_OK;
}
#endif /* !_READONLY */

/**
  * @brief  Miscellaneous Functions.
  * @param  drv: Physical drive nmuber(0).
            ctrl: Control code.
            buff: Buffer to send/receive control data.
  * @return RES_PARERR: invalid parameter.
  			RES_NOTRDY: not ready.
			RES_WRPRT: write protected.
			RES_ERROR: fail.
			RES_OK: success.
  **/
#if _USE_IOCTL
DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
    DRESULT res;
    BYTE n, csd[16], *ptr = buff;
    WORD csize;

	if(drv) return RES_PARERR;
	if(Stat & STA_NOINIT) return RES_NOTRDY;

	switch(ctrl) 
	{	/* Make sure that no pending write process */
		case CTRL_SYNC : 
			SD_SELECT();		
	    	if(Wait_Ready() == 0xFF)
	        	res = RES_OK;
	    	break;
		/* Get number of sectors on the disk (DWORD) */
        case GET_SECTOR_COUNT :	
            if((Send_Command(CMD9, 0) == 0) && Receive_DataBlock(csd, 16)) 
			{
                if((csd[0] >> 6) == 1) /* SDC ver 2.00 */
				{	
                    csize = csd[9] + ((WORD)csd[8] << 8) + 1;
                    *(DWORD*)buff = (DWORD)csize << 10;
                } 
				else  /* SDC ver 1.XX or MMC*/
				{					
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                    *(DWORD*)buff = (DWORD)csize << (n - 9);
                }
                res = RES_OK;
            }
            break;
		/* Get R/W sector size (WORD) */
        case GET_SECTOR_SIZE :	
            *(WORD*)buff = 512;
            res = RES_OK;
            break;
		/* Get erase block size in unit of sector (DWORD) */
        case GET_BLOCK_SIZE :	
            if(CardType & 4) /* SDC ver 2.00 */
			{			
                if(Send_Command(ACMD13, 0) == 0) /* Read SD status */
				{		
                    SPI_ReadWrite_Byte(0xff);
                    if(Receive_DataBlock(csd, 16))  /* Read partial block */
					{				
                        for(n = 64 - 16; n; n--) 
							SPI_ReadWrite_Byte(0xff);	/* Purge trailing data */
                        *(DWORD*)buff = 16UL << (csd[10] >> 4);
                        res = RES_OK;
                    }
                }
            } 
			else  /* SDC ver 1.XX or MMC */
			{	/* Read CSD */				
                if((Send_Command(CMD9, 0) == 0) && Receive_DataBlock(csd, 16)) 
				{	
                    if(CardType & 2) /* SDC ver 1.XX */
					{			
                        *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
                    } 
					else  /* MMC */
					{					
                        *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
                    }
                    res = RES_OK;
                }
            }
            break;
		/* Get card type flags (1 byte) */
        case MMC_GET_TYPE :		
            *ptr = CardType;
            res = RES_OK;
            break;
		/* Receive CSD as a data block (16 bytes) */
        case MMC_GET_CSD :		
            if(Send_Command(CMD9, 0) == 0		/* READ_CSD */
                && Receive_DataBlock(ptr, 16))
                res = RES_OK;
            break;
		/* Receive CID as a data block (16 bytes) */
        case MMC_GET_CID :		
            if (Send_Command(CMD10, 0) == 0		/* READ_CID */
                && Receive_DataBlock(ptr, 16))
                res = RES_OK;
            break;
		/* Receive OCR as an R3 resp (4 bytes) */
        case MMC_GET_OCR :		
            if(Send_Command(CMD58, 0) == 0) /* READ_OCR */
			{	
                for(n = 4; n; n--) *ptr++ = SPI_ReadWrite_Byte(0xff);
                res = RES_OK;
            }
            break;
		/* Receive SD statsu as a data block (64 bytes) */
        case MMC_GET_SDSTAT :	
            if(Send_Command(ACMD13, 0) == 0) /* SD_STATUS */
			{	
                SPI_ReadWrite_Byte(0xff);
                if(Receive_DataBlock(ptr, 64))
                    res = RES_OK;
            }
            break;

        default : res = RES_PARERR;
    }

    SPI_Release();
    return res;
}
#endif /* _USE_IOCTL */

DWORD get_fattime (void)
{
    return 0;  
}
