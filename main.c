/*
 * Lab3 Digital2.c
 *
 * Created: 11/02/2025 13:54:28
 * Author : pablo
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "ADC/ADC.h"
#include "SPI/SPI.h"
#include "UART/UART.h"

char buffer[10];

void mostrarEnLEDS(uint8_t valor);

int main(void)
{
    uint8_t comando;
    spiInit(SPI_SLAVE);
    initADC();
    iniciarADC(0);

    // Habilitar Puerto D como salida
    DDRD = 0xFF;  // Configurar todos los pines del Puerto D como salida
    PORTD = 0x00; // Inicializar LEDs apagados

    sei(); // Habilitar interrupciones globales

    while (1) 
    {
        comando = spiRead();  // Leer comando del maestro

        if (comando == 1) {
            spiWrite(pot1);
        } 
        else if (comando == 2) {
            spiWrite(pot2);
        } 
        else if (comando == 3) {
            uint8_t mensajeSPI = spiRead(); // Recibir un solo byte por SPI
            mostrarEnLEDS(mensajeSPI);  // Mostrar el mensaje en los LEDs del Puerto D
        }
    }
}

void mostrarEnLEDS(uint8_t valor) {
    PORTD = valor;  // Enviar el valor al puerto D
}
