/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"
#include "filter.h"
#include "histogram.h"
#include "utils.h"

#define TOP_LABEL_TEXT_SIZE 1
#define HISTOGRAM_DEFAULT_BAR_COUNT 10
static uint16_t histogram_barCount = HISTOGRAM_DEFAULT_BAR_COUNT;
static uint16_t
    histogram_barWidth; // May share this with other functions in this package.
static uint16_t topLabelMaxWidthInChars; // How many chars will be printed.
static histogram_data_t
    currentBarData[HISTOGRAM_MAX_BAR_COUNT]; // Current histogram data.
static histogram_data_t
    previousBarData[HISTOGRAM_MAX_BAR_COUNT]; // Keep the old stuff around so
                                              // you can erase things properly.
static char
    topLabel[HISTOGRAM_MAX_BAR_COUNT]
            [HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS]; // Labels at top of
                                                          // histogram bars.
static char
    oldTopLabel[HISTOGRAM_MAX_BAR_COUNT]
               [HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS]; // Old label so you
                                                             // only update as
                                                             // necessary.

#define ONE_HALF(x) ((x) / 2) // Integer divide by 2.

static bool initFlag =
    false; // Keep track whether histogram_init() has been called.
// These are the default colors for the bars.
const static uint16_t histogram_defaultBarColors[HISTOGRAM_MAX_BAR_COUNT] = {
    DISPLAY_BLUE,    DISPLAY_RED,    DISPLAY_GREEN,   DISPLAY_CYAN,
    DISPLAY_MAGENTA, DISPLAY_YELLOW, DISPLAY_WHITE,   DISPLAY_BLUE,
    DISPLAY_RED,     DISPLAY_GREEN,  DISPLAY_BLUE,    DISPLAY_RED,
    DISPLAY_GREEN,   DISPLAY_CYAN,   DISPLAY_MAGENTA, DISPLAY_YELLOW,
    DISPLAY_WHITE,   DISPLAY_BLUE,   DISPLAY_RED,     DISPLAY_GREEN,
    DISPLAY_BLUE,    DISPLAY_RED,    DISPLAY_GREEN,   DISPLAY_CYAN,
    DISPLAY_MAGENTA};
static uint16_t histogram_barColors[HISTOGRAM_MAX_BAR_COUNT];
// Default colors for the white dynamic labels.
const static uint16_t
    histogram_defaultBarTopLabelColors[HISTOGRAM_MAX_BAR_COUNT] = {
        DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE,
        DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE,
        DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE,
        DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE,
        DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE,
        DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE, DISPLAY_WHITE,
        DISPLAY_WHITE};
static uint16_t histogram_barTopLabelColors[HISTOGRAM_MAX_BAR_COUNT];
// Default labels for the histogram bars.
// These labels do not change during operation.
const static char histogram_defaultLabel[HISTOGRAM_MAX_BAR_COUNT]
                                        [HISTOGRAM_MAX_BAR_LABEL_WIDTH] = {
                                            {"0"}, {"1"}, {"2"}, {"3"}, {"4"},
                                            {"5"}, {"6"}, {"7"}, {"8"}, {"9"},
                                            {"A"}, {"B"}, {"C"}, {"D"}, {"E"},
                                            {"F"}, {"G"}, {"H"}, {"I"}, {"J"},
                                            {"K"}, {"L"}, {"M"}, {"N"}, {"O"}};
static char histogram_label[HISTOGRAM_MAX_BAR_COUNT]
                           [HISTOGRAM_MAX_BAR_LABEL_WIDTH];

// The bottom labels are drawn at the bottom of the bar and are static.
void histogram_drawBottomLabels() {
  uint16_t labelOffset =
      ONE_HALF(histogram_barWidth -
               (DISPLAY_CHAR_WIDTH *
                HISTOGRAM_BOTTOM_LABEL_TEXT_SIZE));      // Center the label.
  display_setTextSize(HISTOGRAM_BOTTOM_LABEL_TEXT_SIZE); // Set the text-size.
  for (int i = 0; i < histogram_barCount; i++) {         //
    display_setCursor(
        i * (histogram_barWidth + HISTOGRAM_BAR_X_GAP) + labelOffset,
        display_height() -
            (DISPLAY_CHAR_HEIGHT * HISTOGRAM_BOTTOM_LABEL_TEXT_SIZE));
    display_setTextColor(histogram_barColors[i]);
    display_print(histogram_label[i]);
  }
}

void histogram_init(uint16_t barCount) {
  histogram_barCount = barCount;
  if (histogram_barCount > HISTOGRAM_MAX_BAR_COUNT) {
    printf("Error: histogram_init(): histogram_barCount (%d) is larger than "
           "the max allowed (%d)",
           histogram_barCount, HISTOGRAM_MAX_BAR_COUNT);
    exit(0);
  }
  display_init(); // Init the display package.
  histogram_barWidth =
      (display_width() / histogram_barCount) - HISTOGRAM_BAR_X_GAP;
  topLabelMaxWidthInChars =
      (histogram_barWidth /
       DISPLAY_CHAR_WIDTH); // The top-label can be this wide.
  // But, double-check to make sure that it will fit in the memory allocated for
  // the label array. Set to fit allocated area in any case. The -1 allows space
  // for the 0 that ends the string.
  topLabelMaxWidthInChars =
      (topLabelMaxWidthInChars <
       (HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS - 1))
          ? topLabelMaxWidthInChars
          : HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS - 1;
  for (int i = 0; i < histogram_barCount; i++) {
    currentBarData[i] = 0;
    previousBarData[i] = 0;
    topLabel[i][0] = 0;    // Start out with empty strings.
    oldTopLabel[i][0] = 0; // Start out with empty strings.
  }
  for (int i = 0; i < HISTOGRAM_MAX_BAR_COUNT; i++) {
    strncpy(histogram_label[i], histogram_defaultLabel[i],
            HISTOGRAM_MAX_BAR_LABEL_WIDTH);
    histogram_barColors[i] = histogram_defaultBarColors[i];
    histogram_barTopLabelColors[i] = histogram_defaultBarTopLabelColors[i];
  }
  display_fillScreen(DISPLAY_BLACK);
  histogram_drawBottomLabels();
  initFlag = true;
}

// Simply erases all of the pixels in the label area under the histogram bars
// and redraws the labels.
void histogram_redrawBottomLabels() {
  display_fillRect(0,
                   display_height() -
                       (DISPLAY_CHAR_HEIGHT * HISTOGRAM_BOTTOM_LABEL_TEXT_SIZE),
                   display_width(), display_height(), DISPLAY_BLACK);
  histogram_drawBottomLabels();
}

// This function only updates the data for the histogram.
// histogram_updateDisplay() will do the actual drawing. barIndex is the index
// of the histogram bar, 0 is left, larger indices to the right. data is the
// value of the bar in PIXELS. barTopLabel is a dynamic label that will be drawn
// at the top of the bar. It will be redrawn whenever a change is detected.
bool histogram_setBarData(histogram_index_t barIndex, histogram_data_t data,
                          const char barTopLabel[]) {
  if (!initFlag) {
    printf("Error! histogram_setBarData(): must call histogram_init() before "
           "calling this function.\n");
    return false;
  }
  // Error checking.
  if (barIndex > histogram_barCount) {
    printf("Error! histogram_setBarData(): barIndex(%d) is greater than "
           "maximum (%d)\n",
           barIndex, histogram_barCount - 1);
    return false;
  }
  // Error checking.
  if (data > HISTOGRAM_MAX_BAR_DATA_IN_PIXELS) {
    printf("Error! histogram_setBarData(): data (%d) is greater than maximum "
           "(%d) for index(%d)\n",
           data, HISTOGRAM_MAX_BAR_DATA_IN_PIXELS - 1, barIndex);
    return false;
  }
  // If it has changed, update the data in the array but don't render anything
  // on the display.
  if (data !=
      currentBarData[barIndex]) { // Only modify the data if it has changed.
    previousBarData[barIndex] =
        currentBarData[barIndex]; // Save the old data so you render things
                                  // properly.
    // Copy the old string so that you can render things properly.
    currentBarData[barIndex] = data; // Store the data because it changed.
  }
  // If the label has changed, move the previous data to old, new data to
  // current. Labels are handled separately from data because the label may
  // change even if the underlying bar data does not. This allows the top label
  // to change and to be redrawn even if the bars stay the same height.
  if (strncmp(barTopLabel, topLabel[barIndex],
              HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS)) {
    // If you get here, the new label is different from the last one.
    strncpy(oldTopLabel[barIndex], topLabel[barIndex],
            HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS);
    // Copy the current label to become the new old label.
    strncpy(topLabel[barIndex], barTopLabel,
            HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS);
    // Copy the new label to become the current label.
    uint16_t barTopLabelLength =
        strlen(barTopLabel); // Get the length of the label.
    // Only copy as many characters as will fit in the available screen space.
    uint16_t charCopyLimit = (barTopLabelLength < topLabelMaxWidthInChars)
                                 ? barTopLabelLength
                                 : topLabelMaxWidthInChars;
    for (uint16_t i = 0; i < charCopyLimit; i++)
      topLabel[barIndex][i] = barTopLabel[i];
    // Null terminate the string in any case.
    topLabel[barIndex][charCopyLimit] = 0;
  }
  return true; // Everything is OK.
}

// Internal helper function.
// Erases the old text (using a fillRect because it is small and fast) to erase
// the old label, if required. Finds the position for the label, just above the
// top of the bar.
void histogram_drawTopLabel(uint16_t barIndex, histogram_data_t data,
                            const char topLabel[], bool eraseOldLabel) {
  if (eraseOldLabel) {
    // Erase with a fillRect because the rect is small and should be faster than
    // hitting individual label pixels.
    display_fillRect(barIndex * (histogram_barWidth + HISTOGRAM_BAR_X_GAP),
                     display_height() - data - HISTOGRAM_BAR_Y_GAP -
                         DISPLAY_CHAR_HEIGHT - 1,
                     histogram_barWidth, DISPLAY_CHAR_HEIGHT, DISPLAY_BLACK);
  }
  uint16_t topLabelXOffset = ONE_HALF(
      histogram_barWidth -
      (strlen(topLabel) *
       DISPLAY_CHAR_WIDTH)); // This helps to center the label over the bar.
  display_setCursor(
      barIndex * (histogram_barWidth + HISTOGRAM_BAR_X_GAP) +
          topLabelXOffset, // This is the location of the top label.
      display_height() - data - HISTOGRAM_BAR_Y_GAP - DISPLAY_CHAR_HEIGHT - 1);
  display_setTextSize(TOP_LABEL_TEXT_SIZE); // Use tiny text to pack more
                                            // characters into the label.
  display_setTextColor(
      histogram_barTopLabelColors[barIndex]); // Set the color of the label.
  display_print(topLabel);                    // Draw the label.
}

// This updates the display.
// It loops across all bars, checking:
// If the height of the bar has changed, redraw both the bar and the top label.
// If the height of the bar has not changed, but the top label has changed,
// update the label.
void histogram_updateDisplay() {
  if (!initFlag) {
    printf("Error! histogram_displayUpdate(): must call histogram_init() "
           "before calling this function.\n");
    return;
  }
  for (int i = 0; i < histogram_barCount; i++) {
    histogram_data_t oldData = previousBarData[i]; // Get the previous data.
    histogram_data_t data = currentBarData[i];     // Get the current bar data.
    if (oldData !=
        data) { // If the are not equal, redraw the bar and the top-label.
      // Erase the old bar and extend the erase rectangle to include the
      // top-label so that everything is erased at once. Also, redraw the top
      // label.
      display_fillRect(i * (histogram_barWidth + HISTOGRAM_BAR_X_GAP),
                       display_height() - oldData - HISTOGRAM_BAR_Y_GAP -
                           DISPLAY_CHAR_HEIGHT - 1,
                       histogram_barWidth, oldData + DISPLAY_CHAR_HEIGHT + 1,
                       DISPLAY_BLACK);
      // Draw the new bar.
      display_fillRect(i * (histogram_barWidth + HISTOGRAM_BAR_X_GAP),
                       display_height() - data - HISTOGRAM_BAR_Y_GAP,
                       histogram_barWidth, data - 1, histogram_barColors[i]);
      if (data != 0) { // Only draw the top label if the bar-data != 0.
        histogram_drawTopLabel(i, data, topLabel[i],
                               false); // false means that the old label does
                                       // not need to be erased.
        previousBarData[i] = currentBarData[i]; // Old data and new data are the
                                                // same after the update.
        // Old label and new label are the same after the update.
        strncpy(oldTopLabel[i], topLabel[i],
                HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS);
      }
    } else if ((data != 0) &&
               strncmp(topLabel[i], oldTopLabel[i],
                       HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS)) {
      histogram_drawTopLabel(
          i, data, topLabel[i],
          true); // True means that the old label needs to be erased.
      // After the update, copy the label to old data so that it won't reupdate
      // until the next change.
      strncpy(oldTopLabel[i], topLabel[i],
              HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS);
    }
  }
}

// Set the bar-color for each bar. This overwrites the defaults. Call
// histogram_init() to restore the defaults.
void histogram_setBarColor(histogram_index_t barIndex, uint16_t color) {
  if (barIndex < 0 || barIndex > HISTOGRAM_MAX_BAR_COUNT) {
    printf("Error!!! histogram_setBarColor: barIndex(%d) not in range.\n",
           barIndex);
    return;
  }
  histogram_barColors[barIndex] = color;
}

// Set the bottom label drawn under each bar. This overwrites the default. Call
// histogram_init() to restore the defaults.
void histogram_setBarLabel(histogram_index_t barIndex, const char *label) {
  if (barIndex < 0 || barIndex > HISTOGRAM_MAX_BAR_COUNT) {
    printf("Error!!! histogram_setBarColor: barIndex(%d) not in range.\n",
           barIndex);
    return;
  }
  strncpy(histogram_label[barIndex], label, HISTOGRAM_MAX_BAR_LABEL_WIDTH);
}

// Sets the size of the characters used in the bottom labels.
void histogram_setBottomLabelTextSize(uint16_t size) {}

// Runs a short test that writes random values to the histogram bar-values as
// specified by the #defines below.
#define RANDOM_LABEL "99"
#define HISTOGRAM_RUN_TEST_ITERATION_COUNT 10
#define HISTOGRAM_RUN_TEST_LOOP_DELAY_MS 500
void histogram_runTest() {
  histogram_init(
      HISTOGRAM_DEFAULT_BAR_COUNT); // Must init the histogram data structures.
  for (int i = 0; i < HISTOGRAM_RUN_TEST_ITERATION_COUNT;
       i++) { // Loop as required.
    for (int j = 0; j < histogram_barCount;
         j++) { // set each of the bar values.
      histogram_setBarData(j, rand() % HISTOGRAM_MAX_BAR_DATA_IN_PIXELS,
                           RANDOM_LABEL); // set the bar data to a random value.
    }
    histogram_updateDisplay();                       // update the display.
    utils_msDelay(HISTOGRAM_RUN_TEST_LOOP_DELAY_MS); // Slow the update so you
                                                     // can see it happen.
  }
}

// Tries to squeeze a little more into 4 characters by removing the e part of
// the exponent.
#define EXPONENT_CHARACTER 'e'
void trimLabel(char label[]) {
  uint16_t len = strlen(label); // Get the length of the label.
  bool found_e = false;         // You looking for the exponent character.
  uint16_t e_index = 0; // will point to the exponent character when completed.
  // Look for the e and keeps its index.
  for (int i = 0; i < len; i++) {
    if (label[i] == EXPONENT_CHARACTER) {
      found_e = true; // found the exponent character.
      e_index = i;    // Note its position.
      break;
    }
  }
  // If you found the "e", just copy over it.
  if (found_e) {
    for (int j = e_index; j < len; j++) {
      label[j] = label[j + 1];
    }
  }
}

// Used to normalize values prior to plotting.
void histogram_normalizePowerValues(double normalizedValues[],
                                    double origValues[], uint16_t size) {
  // First, find the indicies of the min. and max. value in the
  // currentPowerValue array.
  uint16_t maxIndex = 0;
  for (int i = 0; i < size; i++) {
    if (origValues[i] > origValues[maxIndex])
      maxIndex = i;
  }
  double maxValue = origValues[maxIndex];
  // Normalize everything between 0.0 and 1.0.
  for (int i = 0; i < size; i++)
    normalizedValues[i] = origValues[i] / maxValue;
}

// Used to plot the power response for user frequencies 0-9.
void histogram_plotUserFrequencyPower(double powerValues[]) {
  double normalizedPowerValues[FILTER_FREQUENCY_COUNT];
  histogram_normalizePowerValues(normalizedPowerValues, powerValues,
                                 FILTER_FREQUENCY_COUNT);
  for (int i = 0; i < FILTER_FREQUENCY_COUNT;
       i++) { // Update across all filters.
    // The height of the histogram bar depends upon the normalized value.
    histogram_data_t histogramBarValue =
        ((double)(HISTOGRAM_MAX_BAR_DATA_IN_PIXELS)) * normalizedPowerValues[i];
    // You can have a dynamic label at the top of the bar.
    char label[HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS]; // Get a buffer for
                                                            // the label.
    // Create the label, based upon the actual power value.
    if (snprintf(label, HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS, "%0.0e",
                 powerValues[i]) == -1)
      printf("Error: snprintf encountered an error during conversion.\n");
    // Pull out the 'e' from the exponent to make better use of your characters.
    trimLabel(label);
    // Have the bar value and the label, send the data to the histogram.
    if (!histogram_setBarData(i, histogramBarValue, label)) {
      // If returns false, histogram_setBarData() is not happy. Print out some
      // information.
      printf("Error:histogram_setBarData() histogramBarValue(%d) out of "
             "range.\n",
             histogramBarValue);
      printf("Provided normalizedPowerValue[%d]:%lf\n", i,
             normalizedPowerValues[i]);
      printf("Dumping current and normalized power values.\n");
      for (int tmp_i = 0; tmp_i < FILTER_FREQUENCY_COUNT; tmp_i++) {
        printf("currentPowerValue[%d]:%lf\n", tmp_i,
               filter_getCurrentPowerValue(tmp_i));
        printf("normalizedPowerValue[%d]:%lf\n", tmp_i,
               normalizedPowerValues[tmp_i]);
      }
    }
  }
  histogram_updateDisplay();
}

// Used to display hit-counts in shooter mode.
void histogram_computeNormalizedHitValues(double normalizedHitValues[],
                                          uint16_t hitArray[]) {
  // First, find the indicies of the min. and max. value in the
  // currentPowerValue array.
  uint16_t maxIndex = 0;
  for (int i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
    if (hitArray[i] > hitArray[maxIndex])
      maxIndex = i;
  }
  double maxHitValue = (double)hitArray[maxIndex];
  // Normalize everything between 0.0 and 1.0.
  for (int i = 0; i < FILTER_FREQUENCY_COUNT; i++)
    normalizedHitValues[i] = (double)hitArray[i] / maxHitValue;
}

// Used to plot hits for frequencies 0-9.
void histogram_plotUserHits(uint16_t hitCounts[]) {
  double normalizedHitValues[FILTER_FREQUENCY_COUNT]; // Store normalized values
                                                      // here for the histogram.
  histogram_computeNormalizedHitValues(
      normalizedHitValues, hitCounts); // Get the normalized hit values.
  for (int i = 0; i < FILTER_FREQUENCY_COUNT;
       i++) { // Iterate through the results for each channel.
    char label[HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS]; // Get a buffer for
                                                            // the label.
    // Create the label, based upon the actual power value.
    if (snprintf(label, HISTOGRAM_BAR_TOP_MAX_LABEL_WIDTH_IN_CHARS, "%d",
                 hitCounts[i]) == -1)
      printf("Error: snprintf encountered an error during conversion.\n");
    histogram_setBarData(
        i, normalizedHitValues[i] * HISTOGRAM_MAX_BAR_DATA_IN_PIXELS, label);
    histogram_updateDisplay(); // Redraw the histogram.
  }
}

// Normalizes the values in the array argument.
void histogram_normalizeArrayValues(double *array, uint16_t size) {
  // Find the maximum value
  uint16_t maxIndex = 0;
  // Find the index of the max value in the array.
  for (int i = 0; i < size; i++) {
    if (array[i] > array[maxIndex])
      maxIndex = i;
  }
  double maxPowerValue = array[maxIndex];
  // Normalize everything between 0.0 and 1.0, based upon the max value.
  for (int i = 0; i < size; i++)
    array[i] = array[i] / maxPowerValue;
}
