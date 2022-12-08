#include "display.h"
#include "config.h"
#include "gameDisplay.h"
#include "buttons.h"
#include "string.h"
#include "touchscreen.h"
#include "time.h"

#define TIMER (timer_ticks * CONFIG_GAME_TIMER_PERIOD)

typedef enum {
  initSt,
  gameModeSt,
  chooseSt,
  wrongSt,
  correctSt,
  endSt,
} hangman_st_t;

static int32_t seed; 

hangman_st_t currentState;
uint32_t buttonVal;
int16_t letterChoice[26];
int16_t guesses;
uint16_t timer_ticks;
uint16_t wrongChoice;
uint16_t rightChoice;
bool letterExists;
bool notUsed;
int16_t gameMode;

char firstWord[10];
char secondWord[10];
char thirdWord[10];
char firstWordBlank[10];
char secondWordBlank[10];
char thirdWordBlank[10];
hangman_location_t point;

void setWord();
int16_t getWord();
void locationToLetter();
void checkLetter();
void printIntructions(bool erase);
void modifyKeyboard(bool correct);

void words_init()
{
  printIntructions(0);
  currentState = initSt;
}
 
void words_tick()
{
  switch (currentState)
  {
  case initSt:
    timer_ticks = 0;
    guesses = 0;
    wrongChoice = 0;
    rightChoice = 0;
    memset(letterChoice, 0,sizeof(letterChoice));
    memset(firstWordBlank, 0,sizeof(firstWordBlank));
    memset(secondWordBlank, 0,sizeof(secondWordBlank));
    memset(thirdWordBlank, 0,sizeof(thirdWordBlank));
    memset(firstWordBlank, 0,sizeof(firstWord));
    memset(secondWordBlank, 0,sizeof(secondWord));
    memset(thirdWordBlank, 0,sizeof(thirdWord));
    letterExists = false;
    notUsed = true;
    currentState = gameModeSt;
    gameMode = 0;
    break;
  case gameModeSt:
    buttonVal = buttons_read();

    if((buttonVal & BUTTONS_BTN3_MASK) == BUTTONS_BTN3_MASK)
    {
        printIntructions(true);
        hangmanDisplay_init();
        setWord();
        gameMode = 3;
        timer_ticks = 0;
        displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank, false);
        currentState = chooseSt;
        
    }
    if((buttonVal & BUTTONS_BTN2_MASK) == BUTTONS_BTN2_MASK)
    {
        printIntructions(true);
        hangmanDisplay_init();
        setWord();
        gameMode = 2;
        timer_ticks = 0;
        displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank, false);
        currentState = chooseSt;
    }
    if((buttonVal & BUTTONS_BTN1_MASK) == BUTTONS_BTN1_MASK)
    {
        printIntructions(true);
        hangmanDisplay_init();
        setWord();
        gameMode = 1;
        timer_ticks = 0;
        displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank, false);
        currentState = chooseSt;
    }

    break;
  case chooseSt:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED)
    {
      point = hangmanDisplay_getLocationFromPoint(touchscreen_get_location());
      if(point.y != 26)
      {
      touchscreen_ack_touch();
      locationToLetter();
      checkLetter();
      if(letterExists && notUsed)
        currentState = correctSt;
      else if(!letterExists && notUsed)
        currentState = wrongSt;
      }
    }
    break;
  case wrongSt:
    drawBody(wrongChoice, false);
    modifyKeyboard(false);
    if(wrongChoice == 6)
    {
      displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank, true);
      displayWords(gameMode, firstWord, secondWord, thirdWord, false);
      endScreen(guesses,TIMER,false);
      currentState = endSt;
    
    }
    else
      currentState = chooseSt;
    break;
  case correctSt:
    modifyKeyboard(true);
    if(gameMode == 3){
    if(rightChoice == (strlen(firstWord) +strlen(secondWord) +strlen(thirdWord)))
      {
      endScreen(guesses,TIMER,false);
      currentState = endSt;
    }
    else 
      currentState = chooseSt;
  }
    if(gameMode == 2){
    if(rightChoice == (strlen(firstWord) +strlen(secondWord)))
      {
      endScreen(guesses,TIMER,false);
      currentState = endSt;
    }
    else 
      currentState = chooseSt;
  }
    if(gameMode == 1){
    if(rightChoice == (strlen(firstWord)))
    {
      endScreen(guesses,TIMER,false);
      currentState = endSt;
    }
    else 
      currentState = chooseSt;
  }
    break;
  case endSt:
    buttonVal = buttons_read();
    if((buttonVal & BUTTONS_BTN0_MASK) == BUTTONS_BTN0_MASK)
    {
      for(int16_t i = 0; i <= wrongChoice; i++)
        drawBody(i, true);
      currentState = initSt;
      endScreen(guesses,TIMER,true);
      displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank, true);
      displayWords(gameMode, firstWord, secondWord, thirdWord, true);
      hangmanDisplay_init();
      touchscreen_ack_touch();
    }
    break;
  default:
    break;
  }

  switch (currentState)
  {
    case gameModeSt:
    timer_ticks++;
    break;
  case chooseSt:
    timer_ticks++;
    notUsed = true;
    letterExists = false;
    break;
  case wrongSt:
    wrongChoice++;
    guesses++;
    break;
  case correctSt:
    guesses++; 
    break;
  default:
    break;
  }
}

void setWord()
{
  srand(timer_ticks);
  int16_t random = rand() % 11;
  char wordfirst[][11] = {"ARMCHAIRS", "AUNTS", "BALLS", "BERMUDAS","BEANS","BALLOONS","BEARS","BLOUSES","BEDS","BABIES","PSST"};
  char wordsecond[][10] = {"AFFORD","AGREE","AIM","APPEAR","ATTEMPT","ASK","ARRANGE","BEG","BEGIN","CARE"};
  char wordthird[][11] = {"ARMCHAIRS", "AUNTS", "BALLS", "BERMUDAS","BEANS","BALLOONS","BEARS","BLOUSES","BEDS","BABIES","PSST"};
  strcpy(firstWord,wordfirst[random]);
  random = rand() % 10;
  strcpy(secondWord,wordsecond[random]);
  random = rand() % 11;
  strcpy(thirdWord,wordthird[random]);

  for(int16_t i = 0; i != strlen(firstWord); i++)
  {
    firstWordBlank[i] = '_';
  }
  for(int16_t i = 0; i != strlen(secondWord); i++)
  {
    secondWordBlank[i] = '_';
  }
  for(int16_t i = 0; i != strlen(thirdWord); i++)
  {
    thirdWordBlank[i] = '_';
  }
}

void locationToLetter()
{
  letterChoice[guesses] = 64 + point.x + point.y;
}

void checkLetter()
{

  for(int16_t z = 0; z < guesses; z++)
    if(letterChoice[z] == letterChoice[guesses])
      notUsed = false;
  if(notUsed)
  {
  switch (gameMode)
  {
  case 1:
  displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank, true);
    for(int16_t i = 0; i < strlen(firstWord); i++)
    {
      if(letterChoice[guesses] == firstWord[i])
      {
        firstWordBlank[i] = letterChoice[guesses];
        letterExists = true;
        rightChoice++;
      }

    }
    displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank, false);
    break;
  case 2:
  displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank,true);
    for(int16_t i = 0; i < strlen(firstWord); i++)
    {
      if(letterChoice[guesses] == firstWord[i])
      {
        firstWordBlank[i] = letterChoice[guesses];
        letterExists = true;
        rightChoice++;
      }
      if(letterChoice[guesses] == secondWord[i])
      {
        secondWordBlank[i] = letterChoice[guesses];
        letterExists = true;
        rightChoice++;
      }

    }
    displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank,false);
    break;
    case 3:
    displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank,true);
    for(int16_t i = 0; i < strlen(firstWord); i++)
    {
      if(letterChoice[guesses] == firstWord[i])
      {
        firstWordBlank[i] = letterChoice[guesses];
        letterExists = true;
        rightChoice++;
      }
      if(letterChoice[guesses] == secondWord[i])
      {
        secondWordBlank[i] = letterChoice[guesses];
        letterExists = true;
        rightChoice++;
      }
      if(letterChoice[guesses] == thirdWord[i])
      {
        thirdWordBlank[i] = letterChoice[guesses];
        letterExists = true;
        rightChoice++;
      }

    }
    displayWords(gameMode, firstWordBlank, secondWordBlank, thirdWordBlank,false);
    break;

  default:
    break;
   }
  }
}

void modifyKeyboard(bool correct)
{
  if(point.y == 0)
  {
    if(correct)
    display_drawChar(CONFIG_BOX_KEYBOARD_X_0 + 5 + CONFIG_LINE_VERTICLE_LETTER_SPACE * (point.x -1),CONFIG_LETTER_Y_TOP, 64 + point.x + point.y, CONFIG_COLOR_USED_TEXT, 1, 3);
    else
      display_drawChar(CONFIG_BOX_KEYBOARD_X_0 + 5 + CONFIG_LINE_VERTICLE_LETTER_SPACE * (point.x -1),CONFIG_LETTER_Y_TOP, 64 + point.x + point.y, CONFIG_COLOR_MAN, 1, 3);
  }
  else
    if(correct)
    display_drawChar(CONFIG_BOX_KEYBOARD_X_0 + 5 + CONFIG_LINE_VERTICLE_LETTER_SPACE * (point.x -1),CONFIG_LETTER_Y_BOTTOM, 64 + point.x + point.y, CONFIG_COLOR_USED_TEXT, 1, 3);
    else
      display_drawChar(CONFIG_BOX_KEYBOARD_X_0 + 5 + CONFIG_LINE_VERTICLE_LETTER_SPACE * (point.x -1),CONFIG_LETTER_Y_BOTTOM, 64 + point.x + point.y, CONFIG_COLOR_MAN, 1, 3);
}
