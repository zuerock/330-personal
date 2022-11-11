/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef WAMDISPLAY_H_
#define WAMDISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

// The index used to tell the display which mole is active/inactive.
typedef int16_t wamDisplay_moleIndex_t;
// Mole coordinates represented by this.
typedef int16_t wamDisplay_coord_t;
// Mole origins, whack coordinates, etc. are represented by this.
typedef struct {
  wamDisplay_coord_t x, y;
} wamDisplay_point_t;
// Represents a mole clock interval (asleep or awake).
typedef uint32_t wamDisplay_moleTickCount_t;

// Make it possible to select games with different numbers of moles.
// The display will determine the actual layout. Moles are always indexed by a
// single integer.
typedef enum {
  wamDisplay_moleCount_9,
  wamDisplay_moleCount_6,
  wamDisplay_moleCount_4
} wamDisplay_moleCount_e;

// This keeps track of all mole information.
typedef struct {
  wamDisplay_point_t origin; // This is the origin of the hole for this mole.
  // A mole is active if either of the tick counts are non-zero. The mole is
  // dormant otherwise. During operation, non-zero tick counts are decremented
  // at a regular rate by the control state machine. The mole remains in his
  // hole until ticksUntilAwake decrements to zero and then he pops out. The
  // mole remains popped out of his hole until ticksUntilDormant decrements to
  // zero. Once ticksUntilDomant goes to zero, the mole hides in his hole and
  // remains dormant until activated again.
  wamDisplay_moleTickCount_t
      ticksUntilAwake; // Mole will wake up (pop out of hole) when this goes
                       // from 1 -> 0.
  wamDisplay_moleTickCount_t ticksUntilDormant; // Mole will go dormant (back in
                                                // hole) this goes 1 -> 0.
} wamDisplay_moleInfo_t;

// Provide support to set games with varying numbers of moles. This function
// would be called prior to calling wamDisplay_init();
void wamDisplay_selectMoleCount(wamDisplay_moleCount_e moleCount);

// Call this before using any wamDisplay_ functions.
void wamDisplay_init();

// Draw the game display with a background and mole holes.
void wamDisplay_drawMoleBoard();

// Draw the initial splash (instruction) screen.
void wamDisplay_drawSplashScreen();

// Draw the game-over screen.
void wamDisplay_drawGameOverScreen();

// Selects a random mole and activates it.
// Activating a mole means that the ticksUntilAwake and ticksUntilDormant counts
// are initialized. See the comments for wamDisplay_moleInfo_t for details.
// Returns true if a mole was successfully activated. False otherwise. You can
// use the return value for error checking as this function should always be
// successful unless you have a bug somewhere.
bool wamDisplay_activateRandomMole();

// This takes the provided coordinates and attempts to whack a mole. If a
// mole is successfully whacked, all internal data structures are updated and
// the display and score is updated. You can only whack a mole if the mole is
// awake (visible). The return value can be used during testing (you could just
// print which mole is whacked without having to implement the entire game).
wamDisplay_moleIndex_t wamDisplay_whackMole(wamDisplay_point_t *whackOrigin);

// This updates the ticksUntilAwake/ticksUntilDormant clocks for all of the
// moles.
void wamDisplay_updateAllMoleTickCounts();

// Returns the count of currently active moles.
// A mole is active if it is not dormant, if:
// ticksUntilAwake or ticksUntilDormant are non-zero (in the moleInfo_t struct).
uint16_t wamDisplay_getActiveMoleCount();

// Sets the hit value in the score window.
void wamDisplay_setHitScore(uint16_t hits);

// Gets the current hit value.
uint16_t wamDisplay_getHitScore();

// Sets the miss value in the score window.
void wamDisplay_setMissScore(uint16_t misses);

// Gets the miss value.
// Can be used for testing and other functions.
uint16_t wamDisplay_getMissScore();

// Sets the level value on the score board.
void wamDisplay_incrementLevel();

// Retrieves the current level value.
// Can be used for testing and other functions.
uint16_t wamDisplay_getLevel();

// Completely draws the score screen.
// This function renders all fields, including the text fields for "Hits" and
// "Misses". Usually only called once when you are initializing the game.
void wamDisplay_drawScoreScreen();

// Reset the scores and level to restart the game.
void wamDisplay_resetAllScoresAndLevel();

// Test function that can be called from main() to demonstrate milestone 1.
// Invoking this function should provide the same behavior as shown in the
// Milestone 1 video.
void wamDisplay_runMilestone1_test();

#endif /* WAMDISPLAY_H_ */
