#include "intervalTimer.h"
#include "xil_io.h"
#include "xparameters.h"

// Offset values to get registers
#define TCSR0_OFFSET 0x00
#define TLR0_OFFSET 0x04
#define TCR0_OFFSET 0x08
#define TCSR1_OFFSET 0x10
#define TLR1_OFFSET 0x14
#define TCR1_OFFSET 0x18
// Bits to turn on specific functions
#define ENABLE_CASCADE 0x800
#define ENABLE_TIMER 0x080
#define LOAD 0x020
#define COUNT_DOWN 0x812
#define TIME 100000000
#define ENABLE_INTERRUPT 0x040
#define ACKNOWLEDGE_INTERRUPT 0x100

#define BLANK 0
#define BIT_SHIFT 32

static uint32_t readRegister(uint8_t timerNumber, uint32_t offset);
static void writeRegister(uint8_t timerNumber, uint32_t offset, uint32_t value);
static uint32_t getAddress(uint8_t timerNumber);

// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts up
// 2. Initialize both LOAD registers with zeros
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountUp(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSR0_OFFSET, BLANK);
  writeRegister(timerNumber, TCSR1_OFFSET, BLANK);
  writeRegister(timerNumber, TLR0_OFFSET, BLANK);
  writeRegister(timerNumber, TLR1_OFFSET, BLANK);
  writeRegister(timerNumber, TCSR0_OFFSET, ENABLE_CASCADE);
  intervalTimer_reload(timerNumber);
}

// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts down
//  - The timer automatically reloads when reaching zero
// 2. Initialize LOAD registers with appropriate values, given the `period`.
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountDown(uint32_t timerNumber, double period) {
  writeRegister(timerNumber, TCSR0_OFFSET, COUNT_DOWN);
  writeRegister(timerNumber, TCSR1_OFFSET, COUNT_DOWN);
  uint64_t ticks = (double)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ * period;
  uint32_t upper = ticks >> BIT_SHIFT;
  uint32_t lower = ticks;
  writeRegister(timerNumber, TLR0_OFFSET, lower);
  writeRegister(timerNumber, TLR1_OFFSET, upper);
  intervalTimer_reload(timerNumber);
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
  uint32_t data = readRegister(timerNumber, TCSR0_OFFSET);
  data = data | ENABLE_TIMER;
  writeRegister(timerNumber, TCSR0_OFFSET, data);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  uint32_t data = readRegister(timerNumber, TCSR0_OFFSET);
  data = data & ~ENABLE_TIMER;
  writeRegister(timerNumber, TCSR0_OFFSET, data);
}

// This function is called whenever you want to reload the Counter values
// from the load registers.  For a count-up timer, this will reset the
// timer to zero.  For a count-down timer, this will reset the timer to
// its initial count-down value.
void intervalTimer_reload(uint32_t timerNumber) {
  uint32_t data = readRegister(timerNumber, TCSR0_OFFSET);
  data = data | LOAD;
  writeRegister(timerNumber, TCSR0_OFFSET, data);
  writeRegister(timerNumber, TCSR1_OFFSET, data);
  data = data & ~LOAD;
  writeRegister(timerNumber, TCSR0_OFFSET, data);
  writeRegister(timerNumber, TCSR1_OFFSET, data);
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
// In cascade mode you will need to read the upper and lower 32-bit registers,
// concatenate them into a 64-bit counter value, and then perform the conversion
// to a double seconds value.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint64_t upper = readRegister(timerNumber, TCR1_OFFSET);
  uint64_t lower = readRegister(timerNumber, TCR0_OFFSET);

  double seconds = (lower | (upper << BIT_SHIFT)) / (double)TIME;

  return seconds;
}

// Enable the interrupt output of the given timer.
void intervalTimer_enableInterrupt(uint8_t timerNumber) {
  uint32_t data = readRegister(timerNumber, TCSR0_OFFSET);
  data |= ENABLE_INTERRUPT;
  writeRegister(timerNumber, TCSR0_OFFSET, data);
}

// Disable the interrupt output of the given timer.
void intervalTimer_disableInterrupt(uint8_t timerNumber) {
  uint32_t data = readRegister(timerNumber, TCSR0_OFFSET);
  data = data & ~ENABLE_INTERRUPT;
  writeRegister(timerNumber, TCSR0_OFFSET, data);
}

// Acknowledge the rollover to clear the interrupt output.
void intervalTimer_ackInterrupt(uint8_t timerNumber) {
  uint32_t data = readRegister(timerNumber, TCSR0_OFFSET);
  data |= ACKNOWLEDGE_INTERRUPT;
  writeRegister(timerNumber, TCSR0_OFFSET, data);
}

// Helper function to read data in registers
static uint32_t readRegister(uint8_t timerNumber, uint32_t offset) {
  if (timerNumber == INTERVAL_TIMER_0)
    return Xil_In32(XPAR_AXI_TIMER_0_BASEADDR + offset);
  if (timerNumber == INTERVAL_TIMER_1)
    return Xil_In32(XPAR_AXI_TIMER_1_BASEADDR + offset);
  if (timerNumber == INTERVAL_TIMER_2)
    return Xil_In32(XPAR_AXI_TIMER_2_BASEADDR + offset);
}

// Helper function to write data in registers
static void writeRegister(uint8_t timerNumber, uint32_t offset,
                          uint32_t value) {
  Xil_Out32(getAddress(timerNumber) + offset, value);
}

// Helper function to return timer base
static uint32_t getAddress(uint8_t timerNumber) {
  if (timerNumber == INTERVAL_TIMER_0)
    return XPAR_AXI_TIMER_0_BASEADDR;
  if (timerNumber == INTERVAL_TIMER_1)
    return XPAR_AXI_TIMER_1_BASEADDR;
  if (timerNumber == INTERVAL_TIMER_2)
    return XPAR_AXI_TIMER_2_BASEADDR;
}
