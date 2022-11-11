#include "buttons.h"
#include "display.h"
#include "xil_io.h"
#include "xparameters.h"

#define OFFSET_TRI 0x04
#define OFFSET_DATA 0x00
#define TRI_WRITE_I 0x0F
#define ALL_BUTTONS_PUSHED 0xF
#define Y_COORDINATE 0
#define Y_TEXT 50
#define TEXT_SIZE 2
#define WIDTH 80
#define Height 120
#define X_3 0
#define X_2 80
#define X_1 160
#define X_0 240
#define X_TEXT_3 20
#define X_TEXT_2 100
#define X_TEXT_1 180
#define X_TEXT_0 260
#define BTN0_NAME "BTN0"
#define BTN1_NAME "BTN1"
#define BTN2_NAME "BTN2"
#define BTN3_NAME "BTN3"

uint32_t buttons_readGpioRegister(int32_t offset);
void buttons_writeGpioRegister(int32_t offset, int32_t value);

void buttons_init() { buttons_writeGpioRegister(OFFSET_TRI, TRI_WRITE_I); }

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
uint8_t buttons_read() { return buttons_readGpioRegister(OFFSET_DATA); }

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test wille until all 4 pushbuttons are
// simultaneously pressed.

uint32_t buttons_readGpioRegister(int32_t offset) {
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

void buttons_writeGpioRegister(int32_t offset, int32_t value) {
  Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}
