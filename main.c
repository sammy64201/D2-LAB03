/*
 * D2-Lab-03.c
 *
 * Created: 2/11/2025 10:09:14 AM
 * Author : samue
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "SPI/SPI.h"
#include "UART/UART.h"
#define BUFFER_SIZE 32

volatile char uart_buffer[BUFFER_SIZE];
volatile uint8_t uart_head = 0;
volatile uint8_t uart_tail = 0;

volatile uint8_t p1 = 0;
uint8_t p2 = 0;
char buffer[30];
float volt1;
float volt2;

void Puertos(uint8_t ports) {
	//PORTD = (PORTD & 0b00000011) | ((ports & 0b00001100) << 2);
	//PORTB = (PORTB & 0b11111100) | ((ports & 0b11110000) >> 4) | (ports & 0b00000011);
	PORTD = (PORTD & 0x03) | ((ports & 0x3F) << 2);
	// D6-D7
	PORTB = (PORTB & 0xFC) | ((ports >> 6) & 0x03);
}

void setup() {
	cli();
	PORTB |= (1 << PORTB2);
	spiInit(SPI_MASTER_OSC_DIV16, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	initUART9600();
	DDRD |= 0b11111100; // PD2 - PD7 como salida
	DDRB |= 0b00000011; // PB0 - PB1 como salida
	UCSR0B |= (1 << RXCIE0); // Habilitar interrupción UART RX
	sei();
}

ISR(USART_RX_vect) {
	char c = UDR0;
	uint8_t next_head = (uart_head + 1) % BUFFER_SIZE;
	if (next_head != uart_tail) {
		uart_buffer[uart_head] = c;
		uart_head = next_head;
	}
}

char recieveUART_Buffered() {
	while (uart_head == uart_tail);
	char c = uart_buffer[uart_tail];
	uart_tail = (uart_tail + 1) % BUFFER_SIZE;
	return c;
}

void Menu_PostLab() {
	WriteTextUART("1. Leer el potenciometro\r\n");
	WriteTextUART("2. Enviar valor entre 0 - 256\r\n");
}



void Leer_Potenciometros() {
	PORTB &= ~(1 << PORTB2); // Activar esclavo SPI

	// POTENCIOMETRO 1
	spiWrite(1);
	p1 = spiRead();
	
	// POTENCIOMETRO 2
	spiWrite(2);
	p2 = spiRead();

	PORTB |= (1 << PORTB2); // Desactivar esclavo SPI

	// CONVERSION PARA VOLTAJE 
	volt1 = (p1 / 255.0) * 5.0;
	volt2 = (p2 / 255.0) * 5.0;

	// CONVERTIR PARA MANDAR EN UART
	char buffer1[30], buffer2[30];
	dtostrf(volt1, 5, 2, buffer1);
	dtostrf(volt2, 5, 2, buffer2);

	// MANDO POR UART 
	sprintf(buffer, "Potenciometro 1: %sV\r\nPotenciometro 2: %sV\r\n", buffer1, buffer2);
	WriteTextUART(buffer);
}

void Enviar_Valor_UART_SPI() {
	WriteTextUART("Ingrese un numero (0-256): ");
	char num_str[4];
	uint8_t i = 0;
	char c;
	do {
		c = recieveUART_Buffered();
		if (c >= '0' && c <= '9' && i < 3) {
			num_str[i++] = c;
			writeUART(c);
		}
	} while (c != '\r');
	num_str[i] = '\0';
	uint8_t numero = atoi(num_str);
	WriteTextUART("\r\n");

	Puertos(numero);
	WriteTextUART("Valor enviado al esclavo\r\n");
	PORTB &= ~(1 << PORTB2);
	spiWrite(3);
	spiWrite(numero);
	PORTB |= (1 << PORTB2);
}

int main(void) {
	setup();
	while (1) {
		Menu_PostLab();
		char opcion = recieveUART_Buffered();
		writeUART(opcion);
		WriteTextUART("\r\n");

		if (opcion == '1') {
			Leer_Potenciometros();
			} else if (opcion == '2') {
			Enviar_Valor_UART_SPI();
		}
	}
}
