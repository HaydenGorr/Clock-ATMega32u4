#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <RotaryEncoder.h>
#include "helper.h"
#include "clockPage.h"
#include <avr/pgmspace.h>
#include "changeDatePage.h"

const char mon[] PROGMEM = {"Monday"}; // 6
const char tue[] PROGMEM = {"Tuesday"}; //7
const char wed[] PROGMEM = {"Wednesday"}; // 9
const char thu[] PROGMEM = {"Thursday"};// 8
const char fri[] PROGMEM = {"Friday"}; // 6
const char sat[] PROGMEM = {"Saturday"}; // 8
const char sun[] PROGMEM = {"Sunday"}; // 6
const char* const ClockPage::DOW[7] PROGMEM = { mon, tue, wed, thu, fri, sat, sun };
// int8_t ClockPage::facePTR = 0;
int8_t const ClockPage::DAYMARGIN[7] PROGMEM = { 28, 22, 10, 16, 28, 16, 28 };

void ClockPage::drawClockFace() {
  switch(facePTR) {
    case TIMEONLY:
      drawTimeDateClockFace(TIMEONLY);
      break;
    case BIG:
      drawTimeDateClockFace(BIG);
      break;
    case RECTANGLECLOCK:
      display.drawRoundRect(0, 0, 128, 64, 5, 1);
      ClockPage::drawClockHandRect(HOUR_HAND); // For hour hand
      ClockPage::drawClockHandRect(MINUTE_HAND); // For minute hand
      if (Helper::getShowSecondsFlag()) ClockPage::drawClockHandRect(SECOND_HAND); // For minute hand
      display.fillCircle(63, 31, 2, 1);
      break;
    default:
      break;
  }


  // if (Helper::getMinProgFlag()) drawMinProg();

}

void ClockPage::drawMinProg(){
    // Assuming display is already initialized and is globally accessible
  const int margin = 7;
  const int verticalLineHeight = 5;
  const int screenWidth = 128;
  const int screenHeight = 64;
  const int barHeight = 1; // Height of the progress bar
  const int yOffset = screenHeight - barHeight; // Y position of the progress bar
  if (facePTR != RECTANGLECLOCK){
    // Calculate the width of the progress bar based on the current minute
    uint16_t temp = static_cast<uint16_t>(currentMinute) * screenWidth;
    uint8_t progressBarWidth = static_cast<uint8_t>(temp / 60);

    // Draw the progress bar
    display.drawFastHLine(0, yOffset, progressBarWidth, INVERSE);
  }
  else {
    // Calculate the width of the progress bar based on the current minute
    uint16_t temp = static_cast<uint16_t>(currentMinute) * (screenWidth - 2 * margin);
    uint8_t progressBarWidth = static_cast<uint8_t>(temp / 60);

    // Draw the horizontal progress bar with margins
    display.drawFastHLine(margin, yOffset, progressBarWidth, BLACK);

    // Draw the vertical lines at the ends of the progress bar
    display.drawFastVLine(margin, screenHeight - verticalLineHeight - 1, verticalLineHeight, WHITE);
    display.drawFastVLine(margin + progressBarWidth, screenHeight - verticalLineHeight - 1, verticalLineHeight, WHITE);
  }
}

void ClockPage::drawTimeDateClockFace(clockFacesENUM Face){
  // Draw Time
  uint8_t STRINGSIZE = GetTimeLength();

  uint8_t textSize = 0;

  if (STRINGSIZE * (textSize*6) > 128){
    // Error this means we can't show the text without it wrapping around the screen
    return;
  }

  while((STRINGSIZE * (++textSize*6) < 128) && (textSize < 5));  // Increase the text size to max
  textSize--;

  uint8_t cursorPosX = (128-(STRINGSIZE*(textSize*6)))/2;
  uint8_t cursorPosY = (64-(textSize*6))/2;

  display.setTextSize(textSize);
  display.setCursor(cursorPosX, Face == TIMEONLY ? cursorPosY : 1);

  // Show Hour
  if (!Helper::getH12Flag()) Helper::printByteAsChars(currentHour);
  else if (Helper::getH12Flag() && currentHour > 9 && currentHour < 13) {
    Helper::printByteAsChars(currentHour);
  }
  else{
    Helper::printByteAsChars((currentHour > 12) ? currentHour-12 : currentHour, false);
  }

  display.write(":");

  // Show Minute
  Helper::printByteAsChars(currentMinute);

  if (Face == TIMEONLY && Helper::getShowSecondsFlag()){
    display.write(":");
    Helper::printByteAsChars(currentSecond);
  }

  if (Helper::getH12Flag() && Helper::getAMPMIndicatorFlag()) Helper::WriteCharArray(Helper::getAMFlag() ? "am" : "pm");

  // Stop here if we're showing the TIMEONLY face
  if (Face == TIMEONLY) return;


  // Day of week
  char buffer[10];
  // Reads the pointer to the string from progmem
  const char* strPtr = getCurrentDayString();

  strcpy_P(buffer, strPtr);

  bool largeString = false;

  buffer[3] = ' ';
  buffer[4] = ' ';

  uint8_t monthIndex = (myRTC.getMonth(century)-1) % 12;
  strcpy(&buffer[5], ChangeDatePage::monthsShort[monthIndex]);

  if (monthIndex == 6 || monthIndex == 8){
    largeString = true;
  }

  display.setTextSize(2);
  display.setCursor(largeString ? 10 : 16, 40);
  
  Helper::WriteCharArray(buffer);

  display.setTextSize(1);
  display.setCursor(largeString ? 52 : 58, 45);

  Helper::printByteAsChars(myRTC.getDate());
}



// uint8_t hour = currentHour - 3 % 12;
// uint8_t minute = currentMinute - 15 % 60;

void ClockPage::drawClockHandRect(HandType handType) {
    uint8_t hour = currentHour - 7 % 12;
    uint8_t minute = currentMinute - 15 % 60;
    uint8_t second = currentSecond - 15 % 60; // Assuming currentSecond is defined

    const int SCREEN_WIDTH = 127;
    const int SCREEN_HEIGHT = 63;
    const int CENTER_X = SCREEN_WIDTH / 2;
    const int CENTER_Y = SCREEN_HEIGHT / 2;
    const int GAP = 10;
    const int MAX_HOUR_HAND_LENGTH = 15;
    const int MAX_MINUTE_HAND_LENGTH = 20; // Adjusted for better visibility
    const int MAX_SECOND_HAND_LENGTH = 100; // Length for second hand

    // Calculate the angle in degrees
    double angle;
    int maxHandLength;
    switch (handType) {
        case HOUR_HAND:
            angle = (hour + minute / 60.0) * 30; // 30 degrees per hour
            maxHandLength = MAX_HOUR_HAND_LENGTH;
            break;
        case MINUTE_HAND:
            angle = minute * 6; // 6 degrees per minute
            maxHandLength = MAX_MINUTE_HAND_LENGTH;
            break;
        case SECOND_HAND:
            angle = second * 6; // 6 degrees per second
            maxHandLength = MAX_SECOND_HAND_LENGTH;
            break;
    }

    // Calculate the dynamic length of the hand
    double handLength = ClockPage::calculateHandLength(angle, maxHandLength);

    // Convert angle to radians
    double angleRad = angle * M_PI / 180;

    // Calculate end points
    uint16_t x1 = static_cast<uint16_t>(CENTER_X + handLength * cos(angleRad));
    uint16_t y1 = static_cast<uint16_t>(CENTER_Y + handLength * sin(angleRad));

    // Draw the line (assuming 'color' is predefined or passed as an argument)
    display.drawLine(CENTER_X, CENTER_Y, x1, y1, 1);
}

// Function to calculate hand length based on angle
double ClockPage::calculateHandLength(double angle, int maxHandLength) {
  const int SCREEN_WIDTH = 128;
  const int SCREEN_HEIGHT = 64;
  const int CENTER_X = SCREEN_WIDTH / 2;
  const int CENTER_Y = SCREEN_HEIGHT / 2;
  const int GAP = 10;

  // Convert angle to radians
  double angleRad = angle * M_PI / 180.0;

  // Calculate x and y components
  double x = maxHandLength * cos(angleRad);
  double y = maxHandLength * sin(angleRad);

  // Adjust length based on screen boundaries
  double xBound = CENTER_X - GAP;
  double yBound = CENTER_Y - GAP;

  double xRatio = xBound / fabs(x);
  double yRatio = yBound / fabs(y);

  double ratio = min(xRatio, yRatio);

  return maxHandLength * min(1.0, ratio);
}

void ClockPage::switchClockFace(bool right){

  if (right) facePTR++;
  else facePTR--;

  if (facePTR > 2) facePTR = 0;
  else if (facePTR < 0) facePTR = 2;

  EEPROM.write(0, facePTR);

}

const char* ClockPage::getCurrentDayString(){
  return (const char*)pgm_read_word(&(ClockPage::DOW[getCurrentDayIndex()]));
}

// returns num in range of 0-6
const inline uint8_t ClockPage::getCurrentDayIndex(){
  return (myRTC.getDoW())%6;
}

const uint8_t ClockPage::GetTimeLength(){
  uint8_t STRINGSIZE = 1+1+2; // min time string size - 1 hr char + 1 colon + 2 min chars

  if (!Helper::getH12Flag() || (Helper::getH12Flag() && currentHour > 9 && currentHour < 13)) STRINGSIZE++;
  
  if (Helper::getH12Flag() && Helper::getAMPMIndicatorFlag()) STRINGSIZE+=2;

  if (facePTR!=BIG && Helper::getShowSecondsFlag()) STRINGSIZE+=3;

  return STRINGSIZE;
}
