

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000UL

#pragma config FOSC = INTOSC
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config CLKOUTEN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config WRT = OFF
#pragma config PLLEN = OFF
#pragma config STVREN = ON
#pragma config BORV = LO
#pragma config LVP = ON

#define SPBRG_VALUE 25   // 9600 baud at 4 MHz with BRGH = 1

void UART_Init(void)
{
    OSCCON = 0x6A;   // 4 MHz internal oscillator

    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;

    TRISCbits.TRISC4 = 1;   // RX input
    TRISCbits.TRISC5 = 0;   // TX output

    APFCON0bits.RXDTSEL = 1;   // RX on RC4
    APFCON0bits.TXCKSEL = 1;   // TX on RC5

    SPBRG = SPBRG_VALUE;

    TXSTAbits.SYNC = 0;   // async mode
    TXSTAbits.BRGH = 1;   // high speed
    RCSTAbits.SPEN = 1;   // serial port enable
    TXSTAbits.TXEN = 1;   // enable transmitter
    RCSTAbits.CREN = 1;   // enable receiver
}

void UART_Write(char data)
{
    while (!PIR1bits.TXIF) {
        ;
    }
    TXREG = data;
}

void main(void)
{
    UART_Init();

    TRISBbits.TRISB7 = 1;     // RB7 = input button
    

    while (1)
    {
        // button pressed (active low)
        if (PORTBbits.RB7 == 0)
        {
            __delay_ms(20);   // debounce

            if (PORTBbits.RB7 == 0)
            {
                UART_Write('A');   // send command to Arduino once

                while (PORTBbits.RB7 == 0)
                {
                    ;   // wait until button released
                }

                __delay_ms(20);   // debounce release
            }
        }
    }
}