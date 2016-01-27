

/*Includes********************************************************/
#include "Includes.h"
/****************************************************************/
/*Function Prototypes********************************************/
void init_LEDSPI(void);
uint8_t LEDSPI_send(uint8_t data);
/**************************************************************/

void init_LEDSPI(void){
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;
/******* Initialize clock for SPI1 ********************/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // enable clock for used IO pins
/******* Configure SPI1 pins **************************/
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5; //Pins Used by SPI1-(PA5=SCK,PA6=MISO,PA7=MOSI)
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;  //Select alternate function so that SPI1 can use these pins
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//Select push pull configuration
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//50 MHz GPIO speed for SPI pins
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // Disable pull up resistor
    GPIO_Init(GPIOA, &GPIO_InitStruct);         // pass in previous structure to initialize SPI1 pins
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);//connect PA5 with its SPI1 clk function
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);//connect PA6 with its SPI1 MISO function
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);//connect PA7 with its SPI1 MOSI funtion
/******* Enable clock used by SPI1 **************************/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
/******* Configure SPI1 peripheral **************************/
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // SPI frequency is APB2 frequency / 16 =(5MHz)
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
    SPI_Init(SPI1, &SPI_InitStruct);
/******* Enable Entire SPI1 peripheral **********************/
    SPI_Cmd(SPI1, ENABLE); // enable SPI1, From now on any module can use SPI1
}

/******************************************************************************
* LEDSPI_send
*
*       sends one byte over SPI and returns one byte of data received from
*       at SPI port
*
*       Parameters: data- the byte of data to be transmitted over SPI
*       returns   : returns a byte of data shifted into MISO line
*******************************************************************************/
uint8_t LEDSPI_send(uint8_t data){

    SPI1->DR = data; // write data to be transmitted to the SPI data register
    while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
    while( !(SPI1->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
    while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
    return SPI1->DR; // return received data from SPI data register
}

