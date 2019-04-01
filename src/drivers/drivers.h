/*
 * drivers.h
 *
 *  Created on: Mar 31, 2019
 *      Author: Daniel Hamilton
 */

#ifndef DRIVERS_H_
#define DRIVERS_H_

#include <msp430.h>
#include "UF_LCD.h"

// spi
void spi_init(void);
void dac_spi_write(char data);

// timer
void timer_init(void);

// adc
void adc_init( void );

// lcd
void write( char * string, int length );
void placeCursor( unsigned char pos );
void reset_screen();
void nextLine();
void prevLine();

#endif /* DRIVERS_H_ */
