#ifndef HANGMANDISPLAY
#define HANGMANDISPLAY

#include <stdbool.h>
#include <stdint.h>

#include "display.h"

typedef struct{
    int16_t x;
    int16_t y;
} hangman_location_t;


// Inits the hangman display, drawing the lines that form the board.
void hangmanDisplay_init();

void endScreen(int16_t guessses, int16_t timer, bool erase);

void displayWords(uint32_t buttonVal, char *first, char *second, char *third, bool erase);

// Performs the increment or decrement, given a touched point.
void changeKeyColor(display_point_t point);

void drawBody(int16_t incorrect, bool erase);

hangman_location_t
hangmanDisplay_getLocationFromPoint(display_point_t point);




#endif /* TICTACTOEDISPLAY */
