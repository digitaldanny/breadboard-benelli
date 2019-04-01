/*
 * drivers.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Daniel Hamilton
 */
#include "drivers.h"
#include "game.h"

// ========================================== ANALOG DIGITAL CONVERSION DRIVERS ===============================================
void adc_init( void )
{
    // DCOCTL = CALDCO_16MHZ; // set internal oscillator at 16MHz
    // BCSCTL1 = CALBC1_16MHZ; // set internal oscillator at 16MHz
    ADC10CTL1 |= CONSEQ1; //continuous sample mode, MUST BE SET FIRST!
    ADC10CTL0 |= ADC10SHT_2 | ADC10ON | MSC | SREF_0; //sample and hold time, adc on, cont. sample, Ref+ = Vcc, Ref- = Vss
    ADC10AE0 |= 0x01; // select channel A0
    ADC10CTL0 |= ADC10SC | ENC; // start conversions

    // P2SEL doesn't matter for this port because ADC10AE0
    P2DIR &= ~BIT0;  // set direction for port calculating voltages and voltage references
}

// ================================================= TIMER COUNTER DRIVERS =====================================================
// timer interrupts at a frequency from 10 Hz to 100 Hz
void timer_init(void)
{
    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TACTL = TASSEL_2 + MC_1 + ID_0;           // SMCLK, upmode
    TACCR0 =  TIMER_INTERRUPT_PERIOD;                          // 10 Hz
    TACCTL0 = OUTMOD_4 + CCIE;
}

// ================================================= ON BOARD SPI DRIVERS ======================================================
void spi_init(void)
{
    P3SEL |= 0x11;                                  // P3.0,4 USCI_A0 option select
    UCB0CTL1 |= UCSWRST;
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;    // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2 + ID_0;                           // SMCLK 1MHz
    UCB0BR0 |= 0x02;
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

    P3DIR |= BIT1 | BIT3;
    P3SEL |= BIT1 | BIT3; //select spi module for P3.1 MOSI and P3.3 CLK
    P2DIR |= 0x04; //set direction register for CS
    P2OUT |= 0x04; // set enable high
}

void dac_spi_write(char data)
{
    int output = 0x0000;
    char temp;

    temp = data; // format data
    data >>= 4;
    data &= 0x0F;
    data |= 0xF0;
    output = data;
    output <<= 8;
    data = temp;
    data <<= 4;
    data &= 0xF0;
    output |= data;

    // send out 16 bits of data starting from msb...
    P2OUT &= ~0x04; // set enable low
    while (!(IFG2 & UCB0TXIFG));            // USCI_A0 TX buffer ready
    UCB0TXBUF = (output >> 8) & 0xFF;        // MSB to SPI TXBUF (Register Address)

    while (!(IFG2 & UCB0TXIFG));            // USCI_A0 TX buffer ready
    UCB0TXBUF = (output >> 0) & 0xFF;        // LSB to SPI TXBUF (Data)

    while( !(IFG2 & UCB0TXIFG) );
    for (int i = 0; i < 0x02; i++);
    P2OUT |= 0x04; // set enable high
}

// ==================================== CUSTOM LCD DRIVERS =====================================
#define NEXT_LINE       0xC0
#define CLEAR_DISPLAY   0x01
#define CURSOR_HOME     0x02
#define CURSOR_RIGHT    0x06

// pass character array in for string and
// sizeof(array) into length
void write( char * string, int length )
{
    char transmit = 'X';

    for (unsigned int i = 0; i < length-1; i++ )
    {
        transmit = string[i];
        lcd_char(transmit);
    }
}

// index of where to place the cursor to type
// the next character
void placeCursor( unsigned char pos )
{
    lcd_command(0x80 | pos);
}

// clear the screen and place cursor at the home
void reset_screen()
{
    lcd_command(CLEAR_DISPLAY); // clear display
    lcd_command(CURSOR_HOME);   // cursor home
    lcd_command(CURSOR_RIGHT);  // move cursor over to the right
}

// next line
void nextLine()
{

}

// previous line
void prevLine()
{

}
