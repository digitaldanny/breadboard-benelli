/*
 * drivers.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Daniel Hamilton
 */

#include <msp430.h> 
#include "drivers.h"
#include "UF_LCD.h"
#include "game.h"

// MACROS -----------------------------------------------------

// GLOBALS ----------------------------------------------------
int updatePlayerCount;
int updateEnemyCount;
int updateLedCount;
int updateButtonDebounceCount;
int updateBulletCount;

int start_debounce_count;
int wait_for_timer;

// ISR for button input ---------------------------------------

// ISR for system time counter --------------------------------
#pragma vector=TIMERA0_VECTOR
__interrupt void TA0_ISR(void)
{
    wait_for_timer = 1; // transmit spi data
    updatePlayerCount++;
    updateEnemyCount++;
    updateLedCount++;

    // counter to debounce the button
    if ( start_debounce_count == 1 )
        updateButtonDebounceCount++;
}

// MAIN PROGRAM -----------------------------------------------
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // BCSCTL1 = CALBC1_16MHZ;     // Raise master clock speed to 16 MHz
    // DCOCTL = CALDCO_16MHZ;      // Raise master clock speed to 16 MHz

    // init globals ----------------------------------------
    updatePlayerCount           = TIMER_PLAYER_UPDATE;
    updateEnemyCount            = TIMER_ENEMY_UPDATE;
    updateLedCount              = TIMER_LED_UPDATE;
    updateButtonDebounceCount   = TIMER_DEBOUNCE_UPDATE;
    updateBulletCount           = TIMER_BULLET_UPDATE;

    start_debounce_count        = 0;
    wait_for_timer              = 0;

    // init hardware modules -------------------------------
    adc_init();
    spi_init();
    timer_init();
    lcd_init();
    reset_screen(); // clear lcd

    // game initializations --------------------------------
    player_t player1;
    player1.body = '>';
    player1.health = 5;
    player1.index = 0;
    player1.line = LINE1;

    player_t enemy1;
    enemy1.body = '<';
    enemy1.health = 5;
    enemy1.index = 19;
    enemy1.line = LINE1;
    // ------------------------------------------------------

    bullet_t b1;
    b1.body = '-';
    b1.index = 0;
    b1.line = LINE1;

    __bis_SR_register(GIE);         // enable interrupts

    // initialize player and enemy ---------------------------
    initDrawing(&player1.body, &player1.line, &player1.index);
    initDrawing(&enemy1.body, &enemy1.line, &enemy1.index);

    // run main program --------------------------------------
    while(1)
    {
        // update bullet movements ----------------------------
        if (updateBulletCount >= TIMER_BULLET_UPDATE)
        {

        }

        // player movements------------------------------------
        if (updatePlayerCount >= TIMER_PLAYER_UPDATE)
        {
            photoresInput(&player1); // moves player up or down
            updatePlayerCount = 0;
        }

        // enemy movements ------------------------------------
        if (updateEnemyCount >= TIMER_ENEMY_UPDATE)
        {

        }

        // LED update -----------------------------------------
        if (updateLedCount >= TIMER_LED_UPDATE)
        {

        }

        // button debounce update -----------------------------
        if (updateButtonDebounceCount >= TIMER_DEBOUNCE_UPDATE)
        {
            // enable port interrupt goes here...
            start_debounce_count = 0;
            updateButtonDebounceCount = 0;
        }
    }
}
