/*
 * game.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Daniel Hamilton
 */

#include "game.h"

// SOUND EFFECT LOOKUP TABLE ----------------------------------------

char GunShot[LUT_SIZE] =
{
    0x80,0x88,0x8f,0x97,0x9f,0xa7,0xae,0xb6,
    0xbd,0xc4,0xca,0xd1,0xd7,0xdc,0xe2,0xe7,
    0xeb,0xef,0xf3,0xf6,0xf9,0xfb,0xfd,0xfe,
    0xff,0xff,0xff,0xfe,0xfd,0xfb,0xf9,0xf6,
    0xf3,0xef,0xeb,0xe7,0xe2,0xdc,0xd7,0xd1,
    0xca,0xc4,0xbd,0xb6,0xae,0xa7,0x9f,0x97,
    0x8f,0x88,0x80,0x77,0x70,0x68,0x60,0x58,
    0x51,0x49,0x42,0x3b,0x35,0x2e,0x28,0x23,
    0x1d,0x18,0x14,0x10,0xc,0x9,0x6,0x4,
    0x2,0x1,0x0,0x0,0x0,0x1,0x2,0x4,
    0x6,0x9,0xc,0x10,0x14,0x18,0x1d,0x23,
    0x28,0x2e,0x35,0x3b,0x42,0x49,0x51,0x58,
    0x60,0x68,0x70,0x77
};

char EnemyHit[LUT_SIZE] =
{
    0x5,0xa,0xf,0x14,0x1a,0x1f,0x24,0x29,0x2e,0x33,
    0x38,0x3d,0x42,0x47,0x4d,0x52,0x57,0x5c,0x61,0x66,
    0x6b,0x70,0x75,0x7a,0x80,0x85,0x8a,0x8f,0x94,0x99,
    0x9e,0xa3,0xa8,0xad,0xb3,0xb8,0xbd,0xc2,0xc7,0xcc,
    0xd1,0xd6,0xdb,0xe0,0xe6,0xeb,0xf0,0xf5,0xfa,0xff,
    0xfa,0xf5,0xf0,0xeb,0xe6,0xe0,0xdb,0xd6,0xd1,0xcc,
    0xc7,0xc2,0xbd,0xb8,0xb3,0xad,0xa8,0xa3,0x9e,0x99,
    0x94,0x8f,0x8a,0x85,0x80,0x7a,0x75,0x70,0x6b,0x66,
    0x61,0x5c,0x57,0x52,0x4d,0x47,0x42,0x3d,0x38,0x33,
    0x2e,0x29,0x24,0x1f,0x1a,0x14,0xf,0xa,0x5,0x0
};

char* LUTS[2] = { GunShot, EnemyHit };
char* lut;

// HELPER FUNCTIONS -------------------------------------------------
short line2addr ( line_t line_num )
{
    switch (line_num)
    {
    case LINE1  : return LINE_1_ST;
    case LINE2  : return LINE_2_ST;
    case LINE3  : return LINE_3_ST;
    case LINE4  : return LINE_4_ST;
    default     : return 0; // all cases handled... should never enter here
    }
}

// determines which LUT the sound effect
// should be playing from.
short playNewSound ( short soundId )
{

    switch (soundId)
    {
    case GUN: lut = LUTS[GUN]; break;
    case HIT: lut = LUTS[HIT]; break;
    }

    return 1;
}

// INPUT SCRIPTS ---------------------------------------------------
void photoresInput ( player_t * player )
{
    long buf = 0;
    short short_buf = 0;
    for (int i = 0; i < PHOTO_BUF_SIZE; i++)
    {
        while( !(ADC10CTL0 & ADC10IFG) ); // wait until a new ADC value comes in
        buf += ADC10MEM;
    }

    buf /= PHOTO_BUF_SIZE;

    // determine whether the player should be moving
    // up, down, or stay still.
    short_buf = (short)buf;

    DISABLE_TIMER_INTERRUPT;
    if ( short_buf > PHOTO_MOVE_UP )
        moveUp(&player->body, &player->line, &player->index);
    else if ( short_buf < PHOTO_MOVE_DOWN )
        moveDown(&player->body, &player->line, &player->index);
    ENABLE_TIMER_INTERRUPT;
}

// ENEMY AI --------------------------------------------------------
void updateEnemy ( player_t * enemy )
{

}

// pass in the number of lives an enemy currently has (with a max of 5)
// and turn on the correct leds on board to represent the number of lives
// remaining.
void enemyLives ( short num_enemy_lives )
{
    if ( num_enemy_lives == 5 )
    {
        P1OUT |= (BIT5 | BIT4 | BIT3 | BIT2 | BIT1);
    }
    else if ( num_enemy_lives == 4 )
    {
        P1OUT |= (BIT4 | BIT3 | BIT2 | BIT1);
        P1OUT &= ~(BIT5);
    }
    else if ( num_enemy_lives == 3 )
    {
        P1OUT |= (BIT3 | BIT2 | BIT1);
        P1OUT &= ~(BIT5 | BIT4);
    }
    else if ( num_enemy_lives == 2 )
    {
        P1OUT |= (BIT2 | BIT1);
        P1OUT &= ~(BIT5 | BIT4 | BIT3);
    }
    else if ( num_enemy_lives == 1 )
    {
        P1OUT |= (BIT1);
        P1OUT &= ~(BIT5 | BIT4 | BIT3 | BIT2);
    }
    else
    {
        P1OUT &= ~(BIT5 | BIT4 | BIT3 | BIT2 | BIT1);
    }
}

// LCD DRIVERS -----------------------------------------------------
// EXP) moveRight(&b1.body, &b1.line, &b1.index);
void moveRight ( char* body, line_t* line_num, short* current_index )
{
    // erase the original character
    short addr = line2addr( *line_num );


    placeCursor(addr + *current_index);
    lcd_char(' ');

    (*current_index)++; // move to the right

    // only draw the character if it is
    // on the same line
    if (*current_index < 20)
    {
        placeCursor(addr + *current_index); // debug to see if this is required??
        lcd_char(*body);
    }
}

void moveLeft ( char* body, line_t* line_num, short* current_index )
{
    // erase the original character
    short addr = line2addr( *line_num );

    placeCursor(addr + *current_index);
    lcd_char(' ');

    (*current_index)--; // move to the right

    // only draw the character if it is
    // on the same line
    if (*current_index >= 0)
    {
        placeCursor(addr + *current_index); // debug to see if this is required??
        lcd_char(*body);
    }
}

void moveUp ( char* body, line_t* line_num, short* current_index )
{
    short addr;
    line_t new_line_num = *line_num;

    // find the new line to move to
    // NOTE: Can't move higher than LINE1
    if      ( *line_num == LINE4 )   new_line_num = LINE3;
    else if ( *line_num == LINE3 )   new_line_num = LINE2;
    else if ( *line_num == LINE2 )   new_line_num = LINE1;
    else                             new_line_num = LINE1;

    if ( new_line_num != *line_num )
    {
        addr = line2addr( *line_num );

        // only erase the old character if it is
        // on a different line
        placeCursor(addr + *current_index);
        lcd_char(' ');

        // find the new address
        addr = line2addr( new_line_num );

        // only draw the character if it is
        // on a different line
        placeCursor(addr + *current_index); // debug to see if this is required??
        lcd_char(*body);

        *line_num = new_line_num;
    }
}

void moveDown ( char* body, line_t* line_num, short* current_index )
{
    short addr;
    line_t new_line_num = *line_num;

    // find the new line to move to
    // NOTE: Can't move higher than LINE1
    if      ( *line_num == LINE1 )   new_line_num = LINE2;
    else if ( *line_num == LINE2 )   new_line_num = LINE3;
    else if ( *line_num == LINE3 )   new_line_num = LINE4;
    else                             new_line_num = LINE4;

    if ( new_line_num != *line_num )
    {
        addr = line2addr( *line_num );

        // only erase the old character if it is
        // on a different line
        placeCursor(addr + *current_index);
        lcd_char(' ');

        // find the new address
        addr = line2addr( new_line_num );

        // only draw the character if it is
        // on a different line
        placeCursor(addr + *current_index); // debug to see if this is required??
        lcd_char(*body);

        *line_num = new_line_num;
    }
}

void deleteDrawing ( line_t* line_num, short* current_index )
{
    short addr = line2addr( *line_num );

    // only erase the old character if it is
    // on a different line
    placeCursor(addr + *current_index);
    lcd_char(' ');
}

void initDrawing ( char* body, line_t* line_num, short* current_index )
{
    short addr = line2addr(*line_num);
    placeCursor(addr + *current_index);
    lcd_char(*body);
}
