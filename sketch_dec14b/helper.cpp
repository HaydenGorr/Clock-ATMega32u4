// Helper.cpp
#include "Helper.h"

void Helper::WriteCharArray(char* cs) {
  for (uint8_t i = 0; cs[i] != '\0'; i++) {
    display.write(cs[i]);
  }
}

void Helper::printByteAsChars(byte value, bool showTens = true) {
  char tens = '0' + (value / 10);  // Convert tens digit to char
  char ones = '0' + (value % 10);  // Convert ones digit to char

  // Print characters individually
  if (showTens) display.write(tens);
  display.write(ones);
}

int Helper::calculateDayOfWeek(int inyearBackup, int inmonthBackup, int indayBackup) {
    // Adjust month and year for January and February
    if (inmonthBackup < 3) {
        inmonthBackup += 12;
        inyearBackup--;
    }

    // Since yearBackup is the last two digits of a year in the 21st century
    int fullYear = 2000 + inyearBackup;

    int k = fullYear % 100; // Year of the century
    int j = fullYear / 100; // Zero-based century

    // Zeller's Congruence
    int dayOfWeek = (indayBackup + ((13 * (inmonthBackup + 1)) / 5) + k + (k / 4) + (j / 4) - (2 * j)) % 7;

    // Adjusting the result to make 1 = Monday, 2 = Tuesday, ..., 7 = Sunday
    dayOfWeek = (dayOfWeek + 5) % 7 + 1;

    return dayOfWeek;
}

static bool Helper::getCountRotationsFlag() {
  return general_flags & 0b1;
}

static bool Helper::getShowLongPressFeedbackFlag() {
  return (general_flags >> 1) & 0b1;
}

static bool Helper::getIgnoreClickFlag() {
  return (general_flags >> 2) & 0b1;
}

static bool Helper::getShowSecondsFlag() {
  return (general_flags >> 3) & 0b1;
}

static bool Helper::getH12Flag() {
  return (general_flags >> 4) & 0b1;
}

static bool Helper::getAMFlag() {
  return (general_flags >> 5) & 0b1;
}

static bool Helper::getAMPMIndicatorFlag() {
  return (general_flags >> 6) & 0b1;
}

static bool Helper::getMinProgFlag() {
   return (general_flags >> 7) & 0b1;
}



static void Helper::setCountRotationsFlag(bool in) {
  if (in) general_flags |= COUNTROTATIONS;
  else general_flags &= ~COUNTROTATIONS;
}

static void Helper::setShowLongPressFeedbackFlag(bool in) {
  if (in) general_flags |= SHOWLONGPRESSFEEDBACK;
  else general_flags &= ~SHOWLONGPRESSFEEDBACK;
}

static void Helper::setIgnoreClickFlag(bool in) {
  if (in) general_flags |= IGNORECLICK;
  else general_flags &= ~IGNORECLICK;
}

static void Helper::setShowSecondsFlag(bool in) {
  if (in) general_flags |= SHOWSECONDS;
  else general_flags &= ~SHOWSECONDS;
}

static void Helper::setAMFlag(bool in) {
  if (in) general_flags |= AMPM;
  else general_flags &= ~AMPM;
}

static void Helper::setAMPMIndicatorFlag(bool in) {
  if (in) general_flags |= AMPMINDICATOR;
  else general_flags &= ~AMPMINDICATOR;
}

static void Helper::setH12Flag(bool in) {
  if (in) general_flags |= H12;
  else general_flags &= ~H12;
}

static void Helper::setMinProgFlag(bool in) {
  if (in) general_flags |= MINPROG;
  else general_flags &= ~MINPROG;
}



static void Helper::toggleAMFlag() {
  general_flags ^= AMPM;
}

static void Helper::toggleH12Flag() {
  general_flags ^= H12;
}

static void Helper::toggleShowSecondsFlag() {
  general_flags ^= SHOWSECONDS;
}

static void Helper::toggleAMPMIndicatorFlag() {
  general_flags ^= AMPMINDICATOR;
}

static void Helper::toggleMinProgFlag() {
  general_flags ^= MINPROG;
}