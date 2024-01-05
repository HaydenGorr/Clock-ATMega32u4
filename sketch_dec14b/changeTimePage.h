#ifndef CHANGETIMEPAGE_H
#define CHANGETIMEPAGE_H

#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <RotaryEncoder.h>

extern bool pmFlag;
extern RotaryEncoder encoder;
extern Adafruit_SSD1306 display;
extern int8_t rotaryPosition;
extern DS3231 myRTC;
extern uint8_t lastHour;

class ChangeTimePage {
private:
  inline uint8_t clampRotary();
  
public:

  // STATIC TIME DISPLAY
  byte hourBackup;
  byte minBackup;
  bool hr12backup;
  bool amBackup;
  bool storedTimeBackup;

  enum changeTimeStates { HOUR, MIN, SAVE, AMPM };
  changeTimeStates changeHourCursor;
  
  ChangeTimePage();

  void drawPage();

  void captureTimeToDisplay();
  
  void reset();

  bool moveHourCursor(bool);
};

#endif