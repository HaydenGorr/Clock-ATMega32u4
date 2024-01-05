// Helper.h
#ifndef HELPER_H
#define HELPER_H

#include <Adafruit_SSD1306.h>

extern bool h12Flag;
extern bool century;
extern uint8_t general_flags;
extern Adafruit_SSD1306 display;

class Helper {
public:

  enum FLAGNUMS {
    COUNTROTATIONS=0b1,
    SHOWLONGPRESSFEEDBACK=0b10, // Var for showing click down visual user feedback
    IGNORECLICK=0b100, // When we want to ignore the click happening
    SHOWSECONDS=0b1000,
    H12=0b10000,
    AMPM=0b100000,
    AMPMINDICATOR=0b1000000,
    MINPROG=0b10000000

  };

  // Static methods, if needed
  static void printByteAsChars(byte, bool=true);
  static void WriteCharArray(char* cs);
  static int calculateDayOfWeek(int, int, int);

  static inline uint8_t convertTimeTo12Hour(uint8_t);

  static inline bool getCountRotationsFlagInline() {
    return general_flags & COUNTROTATIONS;
  }

  static inline bool getShowLongPressFeedbackFlagInline() {
    return (general_flags >> 1) & SHOWLONGPRESSFEEDBACK;
  }

  static inline bool getIgnoreClickInline() {
    return (general_flags >> 2) & IGNORECLICK;
  }

  static bool getCountRotationsFlag();
  static bool getShowLongPressFeedbackFlag();
  static bool getIgnoreClickFlag();
  static bool getShowSecondsFlag();
  static bool getH12Flag();
  static bool getAMFlag();
  static bool getAMPMIndicatorFlag();
  static bool getMinProgFlag();


  
  static void setCountRotationsFlag(bool);
  static void setShowLongPressFeedbackFlag(bool);
  static void setIgnoreClickFlag(bool);
  static void setAMFlag(bool);
  static void setShowSecondsFlag(bool);
  static void setAMPMIndicatorFlag(bool);
  static void setH12Flag(bool);
  static void setMinProgFlag(bool);

  static void toggleShowSecondsFlag();
  static void toggleAMFlag();
  static void toggleH12Flag();
  static void toggleAMPMIndicatorFlag();
  static void toggleMinProgFlag();
};

#endif