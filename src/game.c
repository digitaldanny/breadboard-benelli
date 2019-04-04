/*
 * game.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Daniel Hamilton
 */

#include "game.h"

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
