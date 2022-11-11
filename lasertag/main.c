/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

// Uncomment to run tests, various Milestones
#define RUNNING_MODE_TESTS

// Uncomment to run Milestone 3, Task 2
// #define RUNNING_MODE_M3_T2

// Uncomment to run continuous/shooter mode, Milestone 3, Task 3
// #define RUNNING_MODE_M3_T3

// Uncomment to run two-player mode, Milestone 5
// #define RUNNING_MODE_M5

#include <assert.h>
#include <stdio.h>

#include "buttons.h"
#include "detector.h"
#include "filter.h"
#include "filterTest.h"
#include "hitLedTimer.h"
#include "interrupts.h"
#include "isr.h"
#include "leds.h"
#include "lockoutTimer.h"
#include "mio.h"
#include "runningModes.h"
#include "sound.h"
#include "switches.h"
#include "transmitter.h"
#include "trigger.h"

int main() {
  mio_init(false);  // true enables debug prints
  leds_init(false); // true enables debug prints
  buttons_init();
  switches_init();

#ifdef RUNNING_MODE_TESTS
  // interrupts not needed for these tests
  queue_runTest(); // M1
  // filterTest_runTest(); // M3 T1
  // transmitter_runTest(); // M3 T2
  // detector_runTest(); // M3 T3
  // sound_runTest(); // M4
#endif

#ifdef RUNNING_MODE_M3_T2
  // add transmitter, trigger, hitLedTimer, lockoutTimer,
  // and sound init functions to isr_init(), i.e. anything
  // with _tick() functions.
  isr_init();

  interrupts_initAll(true);           // main interrupt init function.
  interrupts_enableTimerGlobalInts(); // enable global interrupts.
  interrupts_startArmPrivateTimer();  // start the main timer.
  interrupts_enableArmInts(); // now the ARM processor can see interrupts.

  transmitter_runNoncontinuousTest();
  transmitter_runContinuousTest();
  trigger_runTest();
  hitLedTimer_runTest();
  lockoutTimer_runTest();
  while (1)
    ; // Forever-while loop. Modify as you see fit.
#endif

#ifdef RUNNING_MODE_M3_T3
  // The program comes up in continuous mode.
  // Hold BTN2 while the program starts to come up in shooter mode.
  if (buttons_read() &
      BUTTONS_BTN2_MASK) { // Read the buttons to see if BTN2 is depressed.
    printf("Starting shooter mode\n");
    runningModes_shooter(); // Run shooter mode if BTN2 is depressed.
  } else {
    printf("Starting continuous mode\n");
    runningModes_continuous(); // Otherwise, go to continuous mode.
  }
#endif

#ifdef RUNNING_MODE_M5
  printf("Starting two team mode\n");
  runningModes_twoTeams();
#endif

  return 0;
}
