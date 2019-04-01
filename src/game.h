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

// GAME CONFIGURATION MACROS ---------------------
#define PHOTO_MOVE_UP           500
#define PHOTO_MOVE_DOWN         300

#define TIMER_PLAYER_UPDATE     20
#define TIMER_ENEMY_UPDATE      10
#define TIMER_LED_UPDATE        5
#define TIMER_DEBOUNCE_UPDATE   5
#define TIMER_BULLET_UPDATE     5
#define TIMER_INTERRUPT_PERIOD  10000

#define BULLET_SPEED_CNT        1

// MACROS ----------------------------------------
#define LINE_1_ST       0
#define LINE_1_END      19
#define LINE_2_ST       64
#define LINE_2_END      83
#define LINE_3_ST       20
#define LINE_3_END      39
#define LINE_4_ST       84
#define LINE_4_END      103

#define PHOTO_BUF_SIZE  30

// TYPEDEFS --------------------------------------

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
} player_t;

typedef struct bullet
{
    char body;
    line_t line;
    short index;
} bullet_t;

// PROTOTYPES -------------------------------------
void updateEnemy ( player_t * enemy );
void photoresInput ( player_t * player );
short line2addr ( line_t line_num );
void moveRight  ( char* body, line_t* line_num, short* current_index );
void moveLeft   ( char* body, line_t* line_num, short* current_index );
void moveUp     ( char* body, line_t* line_num, short* current_index );
void moveDown   ( char* body, line_t* line_num, short* current_index );
void initDrawing ( char* body, line_t* line_num, short* current_index );

#endif /* GAME_H_ */
