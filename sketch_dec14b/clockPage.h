#ifndef CLOCKPAGE_H
#define CLOCKPAGE_H

#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <EEPROM.h>

extern bool century;
extern int8_t currentHour;
extern int8_t currentMinute;
extern int8_t currentSecond;
extern Adafruit_SSD1306 display;
extern int8_t rotaryPosition;
extern DS3231 myRTC;

class ClockPage {
private:
  static const char* const DOW[];
  static const int8_t DAYMARGIN[];

public:
  
  enum clockFacesENUM {
      TIMEONLY=0,
      BIG=1,
      RECTANGLECLOCK=2,
      ROUNDCLOCK=3,
  };

  enum HandType {
    HOUR_HAND,
    MINUTE_HAND,
    SECOND_HAND
};

  static const clockFacesENUM clockFaces[];

  void drawClockFace();

  void drawTimeDateClockFace(clockFacesENUM);
  void drawTimeOnlyClockFace();

  void drawClockHandRect(HandType);
  double calculateHandLength(double, int);

  void drawTime();
  void drawDOW();
  void drawDate();

  void switchClockFace(bool);

  void drawMinProg();

  const char* getCurrentDayString();
  const uint8_t getCurrentDayIndex();
  const uint8_t GetTimeLength();

  // CLOCK FACE
  int8_t facePTR;
};

#endif // CLOCKPAGE_H