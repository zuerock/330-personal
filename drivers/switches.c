#include "switches.h"
#include "leds.h"
#include "xil_io.h"
#include "xparameters.h"

#define OFFSET_DATA 0x00
#define OFFSET_TRI 0x04
#define TRI_WRITE_I 0xF
#define ALL_SWITCHES_ON 0xF
#define OFF 0

uint32_t switches_readGpioRegister(int32_t offset);
void switches_writeGpioRegister(int32_t offset, int32_t value);

void switches_init() { switches_writeGpioRegister(OFFSET_TRI, TRI_WRITE_I); }

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
uint8_t switches_read() { return switches_readGpioRegister(OFFSET_DATA); }

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.

uint32_t switches_readGpioRegister(int32_t offset) {
  return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

void switches_writeGpioRegister(int32_t offset, int32_t value) {
  Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}
