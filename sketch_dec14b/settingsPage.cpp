#include "settingsPage.h"
#include "helper.h"

const char* SettingsPage::selections[] = { "Change Time", "Change Date", "Time Format", "Show Seconds", "AM/PM ind.", "Min. line" };

SettingsPage::SettingsPage(bool inBlinkCursor){
  blinkCursor = inBlinkCursor;
  lastBlinkTime = millis();
  cursorColour = true; // Starts black
  scroll = 0;
  SettingsLength = sizeof(selections) / sizeof(selections[0])-1;
  scrollBarLength = (64/(SettingsLength+1)) * 4;
}

void SettingsPage::drawPage(){

  display.setTextSize(1);
  display.setCursor(1, 1);
  Helper::WriteCharArray("Settings");

  int8_t clampedRotaryPos = SettingsPage::clampCursor();

  uint8_t originY = (16 + (clampedRotaryPos * 12))-(scroll*12);
  uint8_t originX = 0;

  // BLINK AND DRAW THE CURSOR
  if (blinkCursor){
    // Flip the colour we use every half second
    if (millis() - lastBlinkTime >= blinkInterval) {
      cursorColour = !cursorColour;
      lastBlinkTime = millis();
    }
  }

  display.fillTriangle(
    originX, originY,
    originX, originY + 6,
    originX + 6, originY + 3, cursorColour);

  // Starting at scroll and ending at scroll + 4 means we only render the 4 options in view
  for (uint8_t i=scroll; i<scroll+4; i++){
    display.setCursor(15, 16+(12*i)-(scroll*12));
    Helper::WriteCharArray(selections[i]);
    
    // TIME FORMAT
    if (i == TIMEFORMAT){
      display.setCursor(99, 16+(12*i)-(scroll*12));
      Helper::WriteCharArray(Helper::getH12Flag() ? "12hr" : "24hr");
    }
    // SHOW SECONDS
    else if (i == SHOWSECONDS) {
      display.setCursor(99, 16+(12*i)-(scroll*12));
      Helper::WriteCharArray(Helper::getShowSecondsFlag() ? "yes" : "no");
    }
    // AM PM INDICATOR
    else if (i == AMPMINDICATOR) {
      display.setCursor(99, 16+(12*i)-(scroll*12));
      Helper::WriteCharArray(Helper::getH12Flag() ? Helper::getAMPMIndicatorFlag() ? "show" : "hide" : "N\\A");
    }
    else if (i == MINPROG) {
      display.setCursor(99, 16+(12*i)-(scroll*12));
      Helper::WriteCharArray(Helper::getMinProgFlag() ? "show" : "hide");
    }
  }

  display.drawFastVLine(127, ((64-scrollBarLength)/(SettingsLength+1-4))*scroll, scrollBarLength, 1);


  // DRAW TRIANGLE SCROLL INDICATORS
  // DEACTIVATED BECAUSE IT TOOK INSANE AMOUNTS OF MEMORY TO USE
  // if (scroll + 4 < SettingsLength+1 ){
  //   // Down Arrow
  //   display.fillTriangle(
  //     123, 1,
  //     125, 3,
  //     127, 1,
  //     1);
  // }

  // if (scroll > 0){
  //   // Up arrow
  //   display.fillTriangle(
  //     116, 3,
  //     118, 1,
  //     120, 3,
  //     1);
  // }

}

inline char SettingsPage::clampCursor(){

  if (rotaryPosition < 0) {
    scroll = SettingsLength-4;
    encoder.setPosition(-SettingsLength);
    return SettingsLength;
  }
  else if (rotaryPosition > SettingsLength) {
    scroll = 0;
    encoder.setPosition(0);
    return 0;
  }

  uint8_t start = scroll;
  uint8_t end = scroll + 4;

  if (rotaryPosition == end) scroll++;
  else if (rotaryPosition>0 && rotaryPosition == start) scroll--;

  return rotaryPosition;

}




