#include "touchscreen.h"
#include <stdio.h>

#define INIT_ST_MSG "init state\n"
#define WAITING_ST_MSG "waiting_st\n"
#define SETTLING_ST_MSG "settling_st\n"
#define PRESSED_ST_MSG "pressed_st\n"
#define SECONDS_TO_TICKS 20
#define EMPTY 0

enum touchscreen_st {
  waitingState,
  pressedState,
  settlingState,
};

static display_point_t myPoint;
static enum touchscreen_st currentState;
static uint64_t adc_settle_ticks;
static bool pressed;
static uint32_t adc_timer;
// Touchscreen has been released, but not acknowledged

static touchscreen_status_t currentStatus;
// currentstate

// Initialize the touchscreen driver state machine, with a given tick period (in
// seconds).
void touchscreen_init(double period_seconds) {
  currentState = waitingState;
  currentStatus = TOUCHSCREEN_IDLE;
  adc_settle_ticks = period_seconds * SECONDS_TO_TICKS;
  pressed = false;
  adc_timer = EMPTY;
}

void debugStatePrint() {
  static enum touchscreen_st previousState;
  static bool firstPass = true;
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case waitingState:
      printf(WAITING_ST_MSG);
      break;
    case settlingState:
      printf(SETTLING_ST_MSG);
      break;
    case pressedState:
      printf(PRESSED_ST_MSG);
      break;
    }
  }
}

// Tick the touchscreen driver state machine
void touchscreen_tick() {
  debugStatePrint();
  switch (currentState) {
  case waitingState:
    if (display_isTouched()) {
      display_clearOldTouchData();
      currentState = settlingState;
    }

    break;
  case settlingState:
    if (display_isTouched() && adc_timer == adc_settle_ticks) {
      uint8_t z;
      display_getTouchedPoint(&myPoint.x, &myPoint.y, &z);
      touchscreen_get_location();
      currentState = pressedState;
      currentStatus = TOUCHSCREEN_PRESSED;
    } else if (!display_isTouched()) {
      currentState = waitingState;
      currentStatus = TOUCHSCREEN_IDLE;
    }
    break;
  case pressedState:
    if (!display_isTouched()) {
      currentStatus = TOUCHSCREEN_RELEASED;
      currentState = waitingState;
    }

    break;
  default:
    break;
  }

  switch (currentState) {
  case waitingState:
    adc_timer = EMPTY;
    break;
  case settlingState:
    adc_timer++;
    break;
  case pressedState:
    pressed = true;
    break;
  default:
    break;
  }
}

// Return the current status of the touchscreen
touchscreen_status_t touchscreen_get_status() { return currentStatus; }

// Acknowledge the touchscreen touch.  This function will only have effect when
// the touchscreen is in the TOUCHSCREEN_RELEASED status, and will cause it to
// switch to the TOUCHSCREEN_IDLE status.
void touchscreen_ack_touch() {
  switch (touchscreen_get_status()) {
  case TOUCHSCREEN_RELEASED:
    if (pressed == true) {
      pressed = false;
      currentStatus = TOUCHSCREEN_IDLE;
    }
    break;
  default:
    break;
  }
}

// Get the (x,y) location of the last touchscreen touch
display_point_t touchscreen_get_location() { return myPoint; }
