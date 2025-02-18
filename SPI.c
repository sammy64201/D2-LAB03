/*
 * SPI.c
 *
 * Created: 2/11/2025 11:18:57 AM
 *  Author: samue
 */ 

#include "SPI.h"

void spiInit(Spi_Type sType, Spi_Data_Order sDataOrder, Spi_Clock_Polarity sClockPolarity, Spi_Clock_Phase sClockPhase){
	// PB2 --> SS
	// PB3 --> MOSI
	// PB4 --> MISO
	// PB5 --> SCK
	
	if(sType & 0b00010000) {
		DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2); // MOSI, SCK, SS
		DDRB &= ~(1 << DDB4);	// MISO
		SPCR |= (1 << MSTR);	// Master
		
		uint8_t temp = sType & 0b00000111;
		switch(temp){
			case 0:		// DIV2
				SPCR &= ~((1 << SPR1) | (1 << SPR0));
				SPSR |= (1 << SPI2X);
				break;
			case 1:		// DIV4
				SPCR &= ~((1 << SPR1) | (1 << SPR0));
				SPSR &= ~(1 << SPI2X);
				break;
			case 2:		// DIV8
				SPCR |= (1 << SPR0);
				SPCR &= ~(1 << SPR1);
				SPSR |= (1 << SPI2X);
				break;
			case 3:		// DIV16
				SPCR |= (1 << SPR0);
				SPCR &= ~(1 << SPR1);
				SPSR &= ~(1 << SPI2X);
				break;
			case 4:		// DIV32
				SPCR &= ~(1 << SPR0);
				SPCR |= (1 << SPR1);
				SPSR |= (1 << SPI2X);
				break;
			case 5:		// DIV64
				SPCR &= ~(1 << SPR0);
				SPCR |= (1 << SPR1);
				SPSR &= ~(1 << SPI2X);
				break;
			case 6:		// DIV128
				SPCR |= (1 << SPR0) | (1 << SPR1);
				SPSR &= ~(1 << SPI2X);
				break;
		}
		
		
	}
	else {		// SLAVE MODE 
		DDRB |= (1 << DDB4);									// MISO
		DDRB &= ~((1 << DDB3) | (1 << DDB5) | (1 << DDB2));		// MOSI, SCK, SS
		SPCR &= ~(1 << MSTR);
	}
	/*  Enable Data Order, Clock Polarity, Clock Phase	*/
	SPCR |= (1 <<SPE) | sDataOrder | sClockPolarity | sClockPhase;	
}

static void spiReceiveWait(){
	// ESPERA PARA QUE LA DATA SE RECIBA COMPLETA 
	while (!(SPSR & (1 << SPIF))); 
}

void spiWrite (uint8_t dat){
	// ESCRIBE DATA EN EL BUS
	SPDR = dat;
	while (!(SPSR & (1 << SPIF)));
}


unsigned spiDataReady(){
	// REVISA SI YA ESTA LISTO A LEER
	if (SPSR & (1 << SPIF))
	return 1;
	else 
	return 0;
}

char spiRead(){
	// LEE LO QUE RECIBIO
	while (!(SPSR & (1 << SPIF)));	// ESPERA QUE LA INFO SE RECIBA COMPLETA
	return(SPDR);	// LEE LO QUE RECIBIO DEL BUFFER
}

