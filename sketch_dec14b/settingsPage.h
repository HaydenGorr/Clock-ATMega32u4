#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <RotaryEncoder.h>

extern RotaryEncoder encoder;
extern Adafruit_SSD1306 display;
extern int8_t rotaryPosition;

class SettingsPage {
private:
  // Cursor Blinking vars
  unsigned long lastBlinkTime;
  const int blinkInterval = 500; // ms
  bool cursorColour;  // This boolean flips to set the colour
  bool blinkCursor; // If true will blink

  // List data
  static const char* selections[];
  static const uint8_t selectionsPOS[][2]; 

  inline char clampCursor();

  uint8_t SettingsLength;

  uint8_t scrollBarLength;
  
public:

  enum SETTINGS {
    CHANGETIME,
    CHANGEDATE,
    TIMEFORMAT,
    SHOWSECONDS,
    AMPMINDICATOR,
    MINPROG,
  };
  
  int8_t scroll;

  SettingsPage(bool);

  void drawPage();
  void DrawSettingsPage();

};

#endif