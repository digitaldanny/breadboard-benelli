/*
 * drivers.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Daniel Hamilton
 */

/*
 *
 * TODO:
 * ~ Implement enemy AI
 */

#include <msp430.h> 
#include "drivers.h"
#include "UF_LCD.h"
#include "game.h"

// MACROS -----------------------------------------------------

// GLOBALS ----------------------------------------------------
int updatePlayerCount;
int updateEnemyCount;
int updateButtonDebounceCount;
int updateBulletCount;
short enemy_collision;
short lut_count;
short play_new_sound;
short updateSoundEffect;

int shoot_gun;
int start_debounce_count;
int numberAliveBullets;
int sound_iterations;

bullet_t bullet_array[BULLET_MAX_NUM];

// ISR for button input ---------------------------------------

// ISR for system time counter --------------------------------
#pragma vector=TIMERA0_VECTOR
__interrupt void TA0_ISR(void)
{
    updatePlayerCount++;
    updateEnemyCount++;
    updateBulletCount++;

    // counter to play new sound
    if (play_new_sound == 1)
        updateSoundEffect++;

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

    int number_games_played = 1;

    // init hardware modules -------------------------------
    lcd_init();
    increase_clock_speed();
    button_init();
    led_init();
    adc_init();
    spi_init();
    timer_init();

    reset_screen(); // clear lcd
    ENABLE_TIMER_INTERRUPT;

    while (1)
    {
        // init globals ----------------------------------------
        updatePlayerCount           = TIMER_PLAYER_UPDATE;
        updateEnemyCount            = TIMER_ENEMY_UPDATE;
        updateButtonDebounceCount   = TIMER_DEBOUNCE_UPDATE;
        updateBulletCount           = TIMER_BULLET_UPDATE;
        updateSoundEffect           = 0;

        shoot_gun                   = 0;
        start_debounce_count        = 0;
        numberAliveBullets          = 0;
        enemy_collision             = 0;
        lut_count                   = 0;
        play_new_sound              = 0;
        sound_iterations            = 0;

        reset_screen(); // clear lcd

        // local variable init ----------------------------------
        int enemy_movement_div = 0;

        // initialize all bullets to be dead
        for (unsigned int i = 0; i < BULLET_MAX_NUM; i++)
        {
            bullet_array[i].body = BODY_BULLET;
            bullet_array[i].index = 1;
            bullet_array[i].line = LINE1;   // don't care...
            bullet_array[i].alive = DEAD;    // DEAD - CRITICAL
        }

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
        enemy1.dir = MOVE_DOWN;

        lut = LUTS[GUN];

        // ------------------------------------------------------

        srand(time(NULL));
        dac_spi_write(0x0);
        // __bis_SR_register(GIE);         // enable interrupts
        __enable_interrupt();

        // initialize player and enemy ---------------------------
        DISABLE_TIMER_INTERRUPT;
        initDrawing(&player1.body, &player1.line, &player1.index);
        initDrawing(&enemy1.body, &enemy1.line, &enemy1.index);
        ENABLE_TIMER_INTERRUPT;

        // run main program -------------------------------------------------------
        while(1)
        {
            // update bullet movements --------------------------------------------
            if (updateBulletCount >= TIMER_BULLET_UPDATE)
            {
                // update each bullet that is alive.
                // ignore all dead bullets.
                for (unsigned int i = 0; i < BULLET_MAX_NUM; i++)
                {
                    bullet_t* tempBullet = &bullet_array[i];
                    if ( tempBullet->alive == ALIVE )
                    {
                        // if the bullet hasn't reached the screen size
                        // yet, continue drawing.
                        if ( tempBullet->index < SCREEN_WIDTH - 1 )
                        {
                            DISABLE_TIMER_INTERRUPT;
                            moveRight(&tempBullet->body, &tempBullet->line, &tempBullet->index);
                            ENABLE_TIMER_INTERRUPT;
                        }

                        // if the bullet has reached the screen size, delete ball
                        // and decide whether to kill enemy.
                        else if ( tempBullet->index == SCREEN_WIDTH - 1 )
                        {
                            // determine whether a collision is happening with the enemy.
                            // If it is colliding, set boolean to true.
                            if ( enemy1.line == tempBullet->line )
                            {
                                playNewSound( HIT, &play_new_sound );
                                enemy_collision = 1;
                            }

                            // always delete bullet if it is past the screen
                            // and decrement bullet count so more bullets can
                            // be shot.
                            deleteDrawing(&tempBullet->line, &tempBullet->index);
                            numberAliveBullets--;
                            tempBullet->alive = DEAD;
                        }
                    }
                }

                updateBulletCount = 0;
            }

            // player movements-----------------------------------------------------
            if (updatePlayerCount >= TIMER_PLAYER_UPDATE)
            {
                // moves player up or down
                photoresInput(&player1);

                // shoot gun from the player's current position
                if ( shoot_gun == 1 )
                {
                    playNewSound( GUN, &play_new_sound );

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

            // enemy movements -------------------------------------------------------
            if (updateEnemyCount >= TIMER_ENEMY_UPDATE)
            {
                // if there was a bullet+enemy collision, animate the enemy
                // and decrement health.
                if ( enemy_collision == 1 )
                {
                    initDrawing(&enemy1.body, &enemy1.line, &enemy1.index);
                    // play enemy_damage sound here...
                    enemy1.health--;
                    enemy_collision = 0;
                }

                // update number of lives on led display
                enemyLives( enemy1.health );

                // end the game here .... (add animation later)
                if ( enemy1.health == 0 ) break; // end game...

                // enemy AI goes here ...
                else
                {
                    // randomly decide which direction to AI will move this time
                    int random = rand() % 3;
                    if      ( random == 0 )     enemy1.dir = MOVE_UP;
                    else if ( random == 1 )     enemy1.dir = MOVE_DOWN;
                    else                        enemy1.dir = CHARGE;

                    // enemy movement model with movement speed counter
                    if ( enemy_movement_div >= ENEMY_MOVEMENT_DIV )
                    {
                        if ( enemy1.dir == MOVE_UP )
                        {
                            moveUp(&enemy1.body, &enemy1.line, &enemy1.index);
                        }
                        else if ( enemy1.dir == MOVE_DOWN )
                        {
                            moveDown(&enemy1.body, &enemy1.line, &enemy1.index);
                        }
                        else
                        {
                            // Implement logic for enemy charge here...
                        }

                        enemy_movement_div = 0;
                    }
                }

                enemy_movement_div++;
                updateEnemyCount = 0;
            }

            // button debounce update ------------------------------------------------
            if (updateButtonDebounceCount >= TIMER_DEBOUNCE_UPDATE)
            {
                CLEAR_BUTTON_FLAGS;
                start_debounce_count = 0;
                updateButtonDebounceCount = 0;
                ENABLE_BUTTON_INTERRUPT;
            }

            // output a sample of the sound effect -----------------------------------
            if (updateSoundEffect >= TIMER_SOUND_FX && play_new_sound == 1)
            {
                __disable_interrupt();             // disable interrupts
                dac_spi_write( lut[lut_count] );    // this is slowing down the frequency
                __enable_interrupt();             // enable interrupts

                // display next value of the LUT
                lut_count++;

                // if the sound has played all the way,
                // turn off the player.
                if ( lut_count >= LUT_SIZE )
                {
                    lut_count = 0;
                    sound_iterations++;

                    // number of cycles to repeat the
                    // sound wave
                    if (sound_iterations >= SOUND_ITERATIONS )
                    {
                        play_new_sound = 0;
                        dac_spi_write(0x0);
                        sound_iterations = 0;
                    }
                }

                updateSoundEffect = 0;
            }
        }

        // GAME WIN PROTOCOL ---------------------

        DISABLE_TIMER_INTERRUPT;
        reset_screen();
        int num_char = 0;
        char tempPtr[3] = {0, 0, 0};
        short temp_addr = line2addr(LINE2);
        placeCursor(temp_addr + 2);
        write("Games Played: ", 15);

        // write the number of games won by the user
        if ( number_games_played < 10 ) num_char = 2;
        else                            num_char = 3;
        snprintf(tempPtr, num_char, "%d", number_games_played);
        write(&tempPtr[0], num_char);

        temp_addr = line2addr(LINE3);
        placeCursor(temp_addr + 3);
        write("Restart Game?", 14);

        // user input delay
        for (int i = 0; i < 20; i++ )
        {
            temp_addr = line2addr(LINE4);  // draw bottom line
            placeCursor(temp_addr + i);    // draw bottom line
            write(".", 2);                 // draw bottom line

            temp_addr = line2addr(LINE1);  // draw top line
            placeCursor(temp_addr + i);    // draw top line
            write(".", 2);                 // draw top line

            for (long long j = 0; j < 100000; j++); // delay to make it pretty
        }

        ENABLE_TIMER_INTERRUPT;

        // wait for button input from the user to restart the game
        // from the beginning. After receieving input from user,
        // debounce so it doesn't affect the game when it begins.
        shoot_gun = 0;
        CLEAR_BUTTON_FLAGS;
        ENABLE_BUTTON_INTERRUPT;
        while ( !shoot_gun );
        start_debounce_count = 1;

        if (updateButtonDebounceCount >= TIMER_DEBOUNCE_UPDATE)
        {
            CLEAR_BUTTON_FLAGS;
            start_debounce_count = 0;
            ENABLE_BUTTON_INTERRUPT;
        }

        __disable_interrupt();         // disable interrupts

        // returns to menu and remembers number of games played
        number_games_played++;
    }
}
