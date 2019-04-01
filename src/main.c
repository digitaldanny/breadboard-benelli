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

int shoot_gun;
int start_debounce_count;
int wait_for_timer;
int numberAliveBullets;

bullet_t bullet_array[BULLET_MAX_NUM];

// ISR for button input ---------------------------------------

// ISR for system time counter --------------------------------
#pragma vector=TIMERA0_VECTOR
__interrupt void TA0_ISR(void)
{
    wait_for_timer = 1; // transmit spi data
    updatePlayerCount++;
    updateEnemyCount++;
    updateLedCount++;
    updateBulletCount++;

    // counter to debounce the button
    if ( start_debounce_count == 1 )
        updateButtonDebounceCount++;

    TACCTL1 &= ~CCIFG;  // clear timer flag
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    DISABLE_BUTTON_INTERRUPT;
    shoot_gun = 1;
    CLEAR_BUTTON_FLAGS;
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

    shoot_gun                   = 0;
    start_debounce_count        = 0;
    wait_for_timer              = 0;
    numberAliveBullets          = 0;

    // initialize all bullets to be dead
    for (unsigned int i = 0; i < BULLET_MAX_NUM; i++)
    {
        bullet_array[i].body = BODY_BULLET;
        bullet_array[i].index = 1;
        bullet_array[i].line = LINE1;   // don't care...
        bullet_array[i].alive = DEAD;    // DEAD - CRITICAL
    }

    // init hardware modules -------------------------------
    button_init();
    adc_init();
    spi_init();
    timer_init();
    lcd_init();
    reset_screen(); // clear lcd

    // game initializations --------------------------------
    player_t player1;
    player1.body = BODY_PLAYER;
    player1.health = PLAYER_HEALTH;
    player1.index = 0;
    player1.line = LINE1;

    player_t enemy1;
    enemy1.body = BODY_ENEMY;
    enemy1.health = ENEMY_HEALTH;
    enemy1.index = 19;
    enemy1.line = LINE1;

    bullet_t b1;
    b1.body = '-';
    b1.index = 1;
    b1.line = LINE1;
    // ------------------------------------------------------

    __bis_SR_register(GIE);         // enable interrupts

    // initialize player and enemy ---------------------------
    DISABLE_TIMER_INTERRUPT;
    initDrawing(&player1.body, &player1.line, &player1.index);
    initDrawing(&enemy1.body, &enemy1.line, &enemy1.index);
    ENABLE_TIMER_INTERRUPT;

    // run main program --------------------------------------
    while(1)
    {
        // update bullet movements ----------------------------
        if (updateBulletCount >= TIMER_BULLET_UPDATE)
        {
            // update each bullet that is alive.
            // ignore all dead bullets.
            for (unsigned int i = 0; i < BULLET_MAX_NUM; i++)
            {
                bullet_t* tempBullet = &bullet_array[i];
                if ( tempBullet->alive == ALIVE )
                {
                    DISABLE_TIMER_INTERRUPT;
                    moveRight(&tempBullet->body, &tempBullet->line, &tempBullet->index);
                    ENABLE_TIMER_INTERRUPT;
                }
            }

            updateBulletCount = 0;
        }

        // player movements------------------------------------
        if (updatePlayerCount >= TIMER_PLAYER_UPDATE)
        {
            // moves player up or down
            photoresInput(&player1);

            // shoot gun from the player's current position
            if ( shoot_gun == 1 )
            {
                // only attempt to activate a bullet if there
                // are any more bullets available.
                if ( numberAliveBullets < BULLET_MAX_NUM )
                {
                    // find the first dead bullet and make it alive
                     bullet_t* tempBullet;
                     for (unsigned int i = 0; i < BULLET_MAX_NUM; i++)
                     {
                         if ( bullet_array[i].alive == DEAD )
                         {
                             // redefine the newly-alive bullet to start from
                             // the player's position
                             tempBullet = &bullet_array[i];
                             tempBullet->alive = ALIVE;
                             tempBullet->line = player1.line;
                             tempBullet->index = 1;

                             // start the bullet animation, but do not do any
                             // movement within this if-branch because this
                             // is only for player-related movements.
                             DISABLE_TIMER_INTERRUPT;
                             initDrawing(&(tempBullet->body), &(tempBullet->line), &(tempBullet->index));
                             ENABLE_TIMER_INTERRUPT;

                             // don't allow too many bullets to be added
                             numberAliveBullets++;
                             break;
                         }
                     }
                }

                start_debounce_count = 1;
                shoot_gun = 0;
            }

            updatePlayerCount = 0;
        }

        // enemy movements ------------------------------------
        if (updateEnemyCount >= TIMER_ENEMY_UPDATE)
        {

            updateEnemyCount = 0;
        }

        // LED update -----------------------------------------
        if (updateLedCount >= TIMER_LED_UPDATE)
        {

            updateLedCount = 0;
        }

        // button debounce update -----------------------------
        if (updateButtonDebounceCount >= TIMER_DEBOUNCE_UPDATE)
        {
            P1IFG &= ~BIT6; // P1.7 IFG clear
            start_debounce_count = 0;
            updateButtonDebounceCount = 0;
            ENABLE_BUTTON_INTERRUPT;
        }
    }
}
