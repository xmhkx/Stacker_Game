

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
    // Turn all pins on ports A, B, and C into digital mode.
    // If a pin is left as analog, digital features like UART or button reading may not work right.


    TRISCbits.TRISC4 = 1;   // RC4 = RX pin, so it must be an INPUT because data comes into the PIC here.

    TRISCbits.TRISC5 = 0;   // RC5 = TX pin, so it must be an OUTPUT because the PIC sends data out from here.


    APFCON0bits.RXDTSEL = 1;
    // Select the alternate RX pin location.
    // This routes UART receive (RX) to RC4 instead of the default pin.

    APFCON0bits.TXCKSEL = 1;
    // Select the alternate TX pin location.
    // This routes UART transmit (TX) to RC5 instead of the default pin.

    SPBRG = SPBRG_VALUE;
    // Load the baud rate generator with the value needed for your chosen baud rate.
    // Example: for 9600 baud, SPBRG_VALUE would be calculated from Fosc.

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
    // Wait until the transmit register is empty and ready for a new byte.
    // TXIF = 1 means TXREG can accept new data.

    TXREG = data;
    
    // Put the byte into the transmit register.
    // UART hardware then shifts it out automatically on the TX pin.
}

char UART_Data_Ready(void)
{
    return PIR1bits.RCIF;
    // Return whether received data is available.
    // RCIF = 1 means a byte has arrived and is waiting in RCREG
}

char UART_Read(void)
{
    if (RCSTAbits.OERR) {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    // Check for overrun error.
    // OERR happens if received data piles up and the UART buffer gets stuck.
    // Toggling CREN off and back on resets the receiver.
    }

    while (!PIR1bits.RCIF) {
        ;
    }
    // Wait until a byte has been received.

    return RCREG;
    // Return the received byte from the receive register.
}

void main(void)
{
    char receivedChar;

    UART_Init();

    TRISBbits.TRISB7 = 1;     // RB7 = input button
    TRISCbits.TRISC6 = 0;   // Red LED output
    LATCbits.LATC6 = 1;
    

    while (1)
    {
        if (PORTBbits.RB7 == 0)
        {
            __delay_ms(20);

            if (PORTBbits.RB7 == 0)
            {
                UART_Write('A');   // send to Arduino

                receivedChar = UART_Read();   // wait for Arduino reply

                // Example: do something with returned char
                if (receivedChar == 'B')
                {
                    // put your PIC action here later
                    // example: Reading the UART from arduino chip to PIC
                    LATCbits.LATC6 = 0;   // turn on LED
                    __delay_ms(100);
                    LATCbits.LATC6 = 1;   // turn off LED

                    
                }

                while (PORTBbits.RB7 == 0)
                {
                    ;
                }

                __delay_ms(20);
            }
        }
    }
}