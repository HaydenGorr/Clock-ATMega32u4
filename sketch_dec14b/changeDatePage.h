#ifndef CHANGEDATEPAGE_H
#define CHANGEDATEPAGE_H

#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <RotaryEncoder.h>
#include "clockPage.h"

extern int8_t rotaryPosition;
extern RotaryEncoder encoder;
extern Adafruit_SSD1306 display;
extern DS3231 myRTC;
extern ClockPage clockPage;


class ChangeDatePage {
public:
  char clampRotary();

  static const uint8_t* months[];
  static const uint8_t* monthsShort[];
  static const uint8_t monthsCode[];
  static const uint8_t daysInMonth[];
  enum MonthEnum {
        JANUARY = 1,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER
    };

  
public:

  // STATIC TIME DISPLAY
  byte dayBackup;
  byte monthBackup;
  byte yearBackup;
  bool storedDateBackup;

  enum changeDateStates { DAY, MONTH, YEAR, SAVE };
  changeDateStates changeDateCursor;
  
  ChangeDatePage();

  void drawPage();

  void captureDateToDisplay();
  
  void reset();

  bool moveDateCursor(bool);
};

#endif