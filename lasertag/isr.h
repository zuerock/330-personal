/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef ISR_H_
#define ISR_H_

#include <stdint.h>

typedef uint32_t
    isr_AdcValue_t; // Used to represent ADC values in the ADC buffer.

// isr provides the isr_function() where you will place functions that require
// accurate timing. A buffer for storing values from the Analog to Digital
// Converter (ADC) is implemented in isr.c Values are added to this buffer by
// the code in isr.c. Values are removed from this buffer by code in detector.c

// Performs inits for anything in isr.c
void isr_init();

// This function is invoked by the timer interrupt at 100 kHz.
void isr_function();

// This adds data to the ADC buffer.
void isr_addDataToAdcBuffer(isr_AdcValue_t value);

// This removes a value from the ADC buffer.
isr_AdcValue_t isr_removeDataFromAdcBuffer();

// This returns the number of values in the ADC buffer.
uint32_t isr_adcBufferElementCount();

#endif /* ISR_H_ */
