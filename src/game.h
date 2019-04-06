/*
 * game.h
 *
 *  Created on: Mar 31, 2019
 *      Author: Daniel Hamilton
 */

#ifndef GAME_H_
#define GAME_H_

#include <msp430.h>
#include "drivers.h"
#include "UF_LCD.h"
#include <stdlib.h>
#include <stdio.h>
#include "time.h"

// GAME CONFIGURATION MACROS ---------------------
#define PHOTO_MOVE_UP           500
#define PHOTO_MOVE_DOWN         300

#define TIMER_PLAYER_UPDATE     20000
#define TIMER_BULLET_UPDATE     10000
#define TIMER_ENEMY_UPDATE      TIMER_BULLET_UPDATE
#define TIMER_LED_UPDATE        5000
#define TIMER_DEBOUNCE_UPDATE   20000
#define TIMER_INTERRUPT_PERIOD  100
#define TIMER_SOUND_FX          1
#define SOUND_ITERATIONS        20

#define BULLET_MAX_NUM          5

#define PLAYER_HEALTH           5

#define ENEMY_HEALTH            5
#define ENEMY_MOVEMENT_DIV      5

#define BODY_BULLET             '-'
#define BODY_PLAYER             '>'
#define BODY_ENEMY              '<'
#define OOF                     'X'

// MACROS ----------------------------------------
#define LINE_1_ST           0
#define LINE_1_END          19
#define LINE_2_ST           64
#define LINE_2_END          83
#define LINE_3_ST           20
#define LINE_3_END          39
#define LINE_4_ST           84
#define LINE_4_END          103
#define SCREEN_WIDTH        20

#define LUT_SIZE            100
#define GUN                 0
#define HIT                 1

#define PHOTO_BUF_SIZE  30

#define DEAD            'd'
#define ALIVE           'a'

#define ENABLE_BUTTON_INTERRUPT     P1IE |= (BIT6)
#define DISABLE_BUTTON_INTERRUPT    P1IE &= ~(BIT6)
#define CLEAR_BUTTON_FLAGS          P1IFG &= ~(BIT6 | BIT7)  // P1.7 IFG clear
#define ENABLE_TIMER_INTERRUPT      TACCTL0 |= CCIE
#define DISABLE_TIMER_INTERRUPT     TACCTL0 &= ~CCIE

// EXTERNS ---------------------------------------

extern char GunShot[LUT_SIZE];
extern char EnemyHit[LUT_SIZE];
extern char* LUTS[2];
extern char* lut;

// TYPEDEFS --------------------------------------

typedef enum dir_decision
{
    MOVE_UP,
    MOVE_DOWN,
    CHARGE
} dir_decision_t;

typedef enum line
{
    LINE1,
    LINE2,
    LINE3,
    LINE4
} line_t;

typedef struct player
{
    line_t line;
    short index;
    char body;
    short health;
    dir_decision_t dir;
} player_t;

typedef struct bullet
{
    char body;
    line_t line;
    short index;
    char alive;
} bullet_t;

// PROTOTYPES -------------------------------------
void playNewSound ( short soundId, short * play_new_sound );
void increase_clock_speed ( void );
void updateEnemy ( player_t * enemy );
void enemyLives ( short num_enemy_lives );
void photoresInput ( player_t * player );
short line2addr ( line_t line_num );
void moveRight  ( char* body, line_t* line_num, short* current_index );
void moveLeft   ( char* body, line_t* line_num, short* current_index );
void moveUp     ( char* body, line_t* line_num, short* current_index );
void moveDown   ( char* body, line_t* line_num, short* current_index );
void deleteDrawing ( line_t* line_num, short* current_index );
void initDrawing ( char* body, line_t* line_num, short* current_index );

#endif /* GAME_H_ */
