#include "interrupts.h"
#include "xil_io.h"
#include "xparameters.h"

#include "armInterrupts.h"
#include <stdint.h>
#include <stdio.h>

#define IPR_OFFSET 0x04
#define IER_OFFSET 0x08
#define IAR_OFFSET 0x0C
#define SIE_OFFSET 0x10
#define CIE_OFFSET 0x14
#define MER_OFFSET 0x1C

#define FCN_SIZE 3

#define MER_MASK 0x03
#define BLANK 0

static uint32_t readRegister(uint32_t offset);
static void writeRegister(uint32_t offset, uint32_t value);
static void interrupts_isr();
static void (*isrFcnPtrs[FCN_SIZE])() = {NULL};

// Initialize interrupt hardware
// This function should:
// 1. Configure AXI INTC registers to:
//  - Enable interrupt output (see Master Enable Register)
//  - Disable all interrupt input lines.
// 2. Enable the Interrupt system on the ARM processor, and register an ISR
// handler function. This is done by calling:
//  - armInterrupts_init()
//  - armInterrupts_setupIntc(isr_fcn_ptr)
//  - armInterrupts_enable()
void interrupts_init() {
  writeRegister(MER_OFFSET, MER_MASK);
  writeRegister(IER_OFFSET, BLANK);
  armInterrupts_init();
  armInterrupts_setupIntc(interrupts_isr);
  armInterrupts_enable();
}

// Register a callback function (fcn is a function pointer to this callback
// function) for a given interrupt input number (irq).  When this interrupt
// input is active, fcn will be called.
void interrupts_register(uint8_t irq, void (*fcn)()) { isrFcnPtrs[irq] = fcn; }

// Enable single input interrupt line, given by irq number.
void interrupts_irq_enable(uint8_t irq) { writeRegister(SIE_OFFSET, 1 << irq); }

// Disable single input interrupt line, given by irq number.
void interrupts_irq_disable(uint8_t irq) {
  writeRegister(CIE_OFFSET, 1 << irq);
}

// Helper function to pull values from registers
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_AXI_INTC_0_BASEADDR + offset);
}

// Helper function to write values to registers
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + offset, value);
}

// Checks if there is an interrupt from a function and sends acknowledgement
static void interrupts_isr() {
  for (int i = 0; i < FCN_SIZE; i++) {
    if (readRegister(IPR_OFFSET) & (1 << i)) {
      if (isrFcnPtrs[i])
        isrFcnPtrs[i]();
      writeRegister(IAR_OFFSET, 1 << i);
    }
  }
}
