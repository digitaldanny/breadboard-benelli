//----------------UF_LCD FOR MSP430----------------------
//
//
// 			PIN1:  Vss = GND
//		    PIN2:  Vdd = +5V
//		    PIN3:   Vo = Pot driven by +5V
//		    PIN4:   RS = 4.4
//		    PIN5:  R/W = GND
//		    PIN6:    E = 4.5
//          PIN7:  DB4 = 4.0
//          PIN8:  DB5 = 4.1
//          PIN9:  DB6 = 4.2
//          PIN10: DB7 = 4.3
//
//
//    *note: this code is written for 16 MHz,adjust delays accordingly.
//
//
//
//----------------CODE BY BRANDON CERGE--------------------

// UF_LCD.C DRIVERS DEVELOPED BY UNIVERSITY OF FLORIDA
// JUNIOR DESIGN TA'S

#include "msp430x22x2.h"
#include "UF_LCD.h"

#define CLOCK_FREQ_MHZ 16
#define DELAY_TIME CLOCK_FREQ_MHZ*22000/16

char uf_lcd_temp;
char uf_lcd_temp2;
char uf_lcd_x;

void lcd_command(char uf_lcd_x){
	P4DIR = 0xFF;
	uf_lcd_temp = uf_lcd_x;
	P4OUT = 0x00;
	__delay_cycles(DELAY_TIME);
	uf_lcd_x = uf_lcd_x >> 4;
	uf_lcd_x = uf_lcd_x & 0x0F;
	uf_lcd_x = uf_lcd_x | 0x20;
	P4OUT =  uf_lcd_x;
	__delay_cycles(DELAY_TIME);
	uf_lcd_x = uf_lcd_x & 0x0F;
	P4OUT =  uf_lcd_x;
	__delay_cycles(DELAY_TIME);
	P4OUT = 0x00;
	__delay_cycles(DELAY_TIME);
	uf_lcd_x = uf_lcd_temp;
	uf_lcd_x = uf_lcd_x & 0x0F;
	uf_lcd_x = uf_lcd_x | 0x20;
	P4OUT = uf_lcd_x;
	__delay_cycles(DELAY_TIME);
	uf_lcd_x = uf_lcd_x & 0x0F;
	P4OUT = uf_lcd_x;
	__delay_cycles(DELAY_TIME);
}

void lcd_init(void){
	lcd_command(0x33);
	lcd_command(0x32);
	lcd_command(0x2C);
	lcd_command(0x0C);
	lcd_command(0x01);
}

void lcd_char(char uf_lcd_x){
	P4DIR = 0xFF;
	uf_lcd_temp = uf_lcd_x;
	P4OUT = 0x10;
	__delay_cycles(DELAY_TIME);
	uf_lcd_x = uf_lcd_x >> 4;
	uf_lcd_x = uf_lcd_x & 0x0F;
	uf_lcd_x = uf_lcd_x | 0x30;
	P4OUT =  uf_lcd_x;
	__delay_cycles(DELAY_TIME);
	uf_lcd_x = uf_lcd_x & 0x1F;
	P4OUT =  uf_lcd_x;
	__delay_cycles(DELAY_TIME);
	P4OUT = 0x10;
	__delay_cycles(DELAY_TIME);
	uf_lcd_x = uf_lcd_temp;
	uf_lcd_x = uf_lcd_x & 0x0F;
	uf_lcd_x = uf_lcd_x | 0x30;
	P4OUT = uf_lcd_x;
	__delay_cycles(DELAY_TIME);
	uf_lcd_x = uf_lcd_x & 0x1F;
	P4OUT = uf_lcd_x;
	__delay_cycles(DELAY_TIME);
}
