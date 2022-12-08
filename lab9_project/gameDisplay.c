#include "gameDisplay.h"
#include "config.h"
#include "buttons.h"
#include "stdio.h"


#define START_SCREEN                                                           \
  "Touch button0 to play easy\n         -or-\n press button1 to play med\n      -or-\n press button2 to play hard\n button3 to reset"

#define TOP_LINE_Y (CONFIG_BOX_KEYBOARD_Y_0)
#define BOT_LINE_Y (CONFIG_LINE_KEYBOARD)
#define LEFT_LINE_X (CONFIG_LINE_VERTICLE_LETTER_SPACE * 6)
#define RIGHT_LINE_X (CONFIG_LINE_VERTICLE_LETTER_SPACE * 7)


void hangmanDisplay_init()
{
    display_drawRect(CONFIG_BOX_KEYBOARD_X_0,CONFIG_BOX_KEYBOARD_Y_0,CONFIG_BOX_KEYBOARD_X_1,CONFIG_BOX_KEYBOARD_Y_1,CONFIG_BOX_COLOR); // keyboard box
    display_drawRect(CONFIG_BOX_GALLOWS_X_0,CONFIG_BOX_GALLOWS_Y_0,CONFIG_BOX_GALLOWS_X_1,CONFIG_BOX_GALLOWS_Y_1,CONFIG_BOX_COLOR); // hangman box
    for(uint16_t i = 1; i < 13; i++)
        display_drawLine(CONFIG_BOX_KEYBOARD_X_0 + CONFIG_LINE_VERTICLE_LETTER_SPACE * i,CONFIG_BOX_KEYBOARD_Y_0,CONFIG_BOX_KEYBOARD_X_0 + CONFIG_LINE_VERTICLE_LETTER_SPACE * i,CONFIG_LINE_KEYBOARD_Y, CONFIG_BOX_COLOR); // keyboard cut
    for(uint16_t i = 0; i < 13; i++){    
        display_drawChar(CONFIG_BOX_KEYBOARD_X_0 + 5 + CONFIG_LINE_VERTICLE_LETTER_SPACE * i,CONFIG_LETTER_Y_TOP, 65 + i, CONFIG_COLOR_TEXT, 1, 3);
        display_drawChar(CONFIG_BOX_KEYBOARD_X_0 + 5 + CONFIG_LINE_VERTICLE_LETTER_SPACE * i,CONFIG_LETTER_Y_BOTTOM, 78 + i, CONFIG_COLOR_TEXT, 1, 3);
    }
    display_drawLine(CONFIG_LENGTH_GALLOWS_BASE_X, CONFIG_LENGTH_GALLOWS_BASE_Y,DISPLAY_WIDTH, CONFIG_LENGTH_GALLOWS_BASE_Y,CONFIG_GALLOWS_COLOR); // base
    display_drawLine(CONFIG_LENGTH_GALLOWS_POLE_X,CONFIG_LENGTH_GALLOWS_BASE_Y,CONFIG_LENGTH_GALLOWS_POLE_X,CONFIG_LENGTH_GALLOWS_POLE_Y,CONFIG_GALLOWS_COLOR); // pole
    display_drawLine(CONFIG_LENGTH_GALLOWS_POLE_X,CONFIG_LENGTH_GALLOWS_POLE_Y,CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_GALLOWS_POLE_Y,CONFIG_GALLOWS_COLOR); // beam
    display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_GALLOWS_POLE_Y,CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_GALLOWS_ROPE_Y,CONFIG_GALLOWS_COLOR); // rope
    display_drawLine(CONFIG_BOX_KEYBOARD_X_0,CONFIG_LINE_KEYBOARD,CONFIG_BOX_KEYBOARD_X_1,CONFIG_LINE_KEYBOARD,CONFIG_BOX_COLOR);
}

void displayWords(uint32_t gameMode, char *first, char *second, char *third, bool erase)
{
  switch (gameMode)
  {
  case 1:

    display_setCursor(X_TEXT, Y_TEXT_0); // The middle of the boxes.
    if(erase)
      display_setTextColor(CONFIG_BACKGROUND_COLOR);
    else
      display_setTextColor(CONFIG_COLOR_TEXT); // Make the text black.
    display_setTextSize(TEXT_SIZE);  
    display_println(first);

    break;
  case 2:
    display_setCursor(X_TEXT, Y_TEXT_0); // The middle of the boxes.
    if(erase)
      display_setTextColor(CONFIG_BACKGROUND_COLOR);
    else
      display_setTextColor(CONFIG_COLOR_TEXT); // Make the text black.
    display_setTextSize(TEXT_SIZE);  
    display_println(first);
    if(erase)
      display_setTextColor(CONFIG_BACKGROUND_COLOR);
    else
      display_setTextColor(CONFIG_COLOR_TEXT);
    display_setCursor(X_TEXT, Y_TEXT_1);
    display_println(second);

    break;
  case 3:
    display_setCursor(X_TEXT, Y_TEXT_0); // The middle of the boxes.
    if(erase)
      display_setTextColor(CONFIG_BACKGROUND_COLOR);
    else
      display_setTextColor(CONFIG_COLOR_TEXT); // Make the text black.
    display_setTextSize(TEXT_SIZE);  
    display_println(first);
    if(erase)
      display_setTextColor(CONFIG_BACKGROUND_COLOR);
    else
      display_setTextColor(CONFIG_COLOR_TEXT);
    display_setCursor(X_TEXT, Y_TEXT_1);
    display_println(second);
    if(erase)
      display_setTextColor(CONFIG_BACKGROUND_COLOR);
    else
      display_setTextColor(CONFIG_COLOR_TEXT);
    display_setCursor(X_TEXT, Y_TEXT_2);
    display_println(third);

    break;
  default:

    break;
  }
}

void drawBody(int16_t incorrect, bool erase)
{
    switch (incorrect)
    {
    case 1:
        if(erase)
          display_drawCircle(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_MAN_HEAD_Y,CONFIG_MAN_HEAD,CONFIG_BACKGROUND_COLOR);
        else
          display_drawCircle(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_MAN_HEAD_Y,CONFIG_MAN_HEAD,CONFIG_COLOR_MAN);
        break;
    case 2:
        if(erase)
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_BODY_Y_0, CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_BODY_Y_1,CONFIG_BACKGROUND_COLOR);
        else
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_BODY_Y_0, CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_BODY_Y_1,CONFIG_COLOR_MAN);
        break;
    case 3:
        if(erase)
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_ARM_Y_0,CONFIG_LENGTH_MAN_ARM_0_X_1,CONFIG_LENGTH_MAN_ARM_Y_1,CONFIG_BACKGROUND_COLOR);
        else
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_ARM_Y_0,CONFIG_LENGTH_MAN_ARM_0_X_1,CONFIG_LENGTH_MAN_ARM_Y_1,CONFIG_COLOR_MAN);
        break;
    case 4:
        if(erase)
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_ARM_Y_0,CONFIG_LENGTH_MAN_ARM_1_X_1,CONFIG_LENGTH_MAN_ARM_Y_1,CONFIG_BACKGROUND_COLOR);
        else
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_ARM_Y_0,CONFIG_LENGTH_MAN_ARM_1_X_1,CONFIG_LENGTH_MAN_ARM_Y_1,CONFIG_COLOR_MAN);
        break;
    case 5:
        if(erase)
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_BODY_Y_1,CONFIG_LENGTH_MAN_LEG_0_X_1,CONFIG_LENGTH_MAN_LEG_Y_1, CONFIG_BACKGROUND_COLOR);
        else
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_BODY_Y_1,CONFIG_LENGTH_MAN_LEG_0_X_1,CONFIG_LENGTH_MAN_LEG_Y_1, CONFIG_COLOR_MAN);
        break;
    case 6:
        if(erase)
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_BODY_Y_1,CONFIG_LENGTH_MAN_LEG_1_X_1,CONFIG_LENGTH_MAN_LEG_Y_1, CONFIG_BACKGROUND_COLOR);
        else
          display_drawLine(CONFIG_LENGTH_GALLOWS_BEAM_X,CONFIG_LENGTH_MAN_BODY_Y_1,CONFIG_LENGTH_MAN_LEG_1_X_1,CONFIG_LENGTH_MAN_LEG_Y_1, CONFIG_COLOR_MAN);
        break;
    default:
        break;
    }
}

void printIntructions(bool erase) {
  if (!erase) {
    display_fillScreen(DISPLAY_BLACK);
    display_setTextColor(DISPLAY_WHITE); // Make the text white.
    display_setTextSize(2);
    display_setCursor(DISPLAY_WIDTH / 8, (DISPLAY_HEIGHT / 3));
    display_println(START_SCREEN);
  }
  if (erase) {
    display_setTextColor(CONFIG_BACKGROUND_COLOR); // Make the text black.
    display_setTextSize(2);
    display_setCursor(DISPLAY_WIDTH / 8, (DISPLAY_HEIGHT / 3));
    display_println(START_SCREEN);
  }
}

#define IGNORE 26
#define TOP 0
#define BOTTOM 13


hangman_location_t
hangmanDisplay_getLocationFromPoint(display_point_t point) {

  hangman_location_t location;
  int16_t quadrant;

  int16_t x = point.x;
  int16_t y = point.y;

  uint16_t y1 = TOP_LINE_Y;
  uint16_t y2 = BOT_LINE_Y;

  
    location.x = (point.x + 24) / 24;

  if (y < y1)
    location.y = IGNORE;
  else if (y < y2)
    location.y = TOP;
  else
    location.y = BOTTOM;

  return location;
}

