#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <RotaryEncoder.h>
#include <ezButton.h>
#include "clockPage.h"
#include "Helper.h"
#include "settingsPage.h"
#include "changeTimePage.h"
#include "changeDatePage.h"
#include <EEPROM.h>
// #include <avr/sleep.h>
// #include <avr/wdt.h>

// PINS
#define CLK 1  // Rotary encoder clock
#define DT 0   // Rotary encoder digital input
#define SW 7   // Switch for the rotary encoder

// SCREEN CONFIG
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// CLOCK CONFIG
DS3231 myRTC;
bool h12Flag;
bool pmFlag;
bool century = false;
bool h12Display = false;

int8_t currentHour = 0;
int8_t currentMinute = 0;
int8_t currentSecond = 0;

// ROTARY ENCODER
int8_t rotaryPosition = 0; // Latest rotary position. Updates once per loop before calling the renderer.
RotaryEncoder encoder(CLK, DT, RotaryEncoder::LatchMode::FOUR3);

// Page Switch vars
enum pages { CLOCK_P, SETTINGS_PAGE, CHANGETIME_P, CHANGEDATE_P };
uint8_t currentPage = CLOCK_P;
pages pageStack[5] = {currentPage};
uint8_t stackptr = 0;

// Rotary Encoder click vars
ezButton p_btn(SW);
unsigned long ezpressedTime = 0;
unsigned long ezreleasedTime = 0;
const uint16_t SHORT_PRESS_TIME = 300;        // 1000 milliseconds
const uint16_t LONG_PRESS_TIME = 700;         // 1000 milliseconds
const uint16_t EXTRA_LONG_PRESS_TIME = 2000;  // 1000 milliseconds

// Used for switching clock faces
int8_t startRotNum = 0;
uint8_t currentRotNum = 0;
uint8_t SWTICH_CLOCK_FACE_TICKS = 3; // How many ticks of the rotart dial before clock face switches

// Ignore Click Flag
// Show Long Press Feedback Flag
// Count Rotations Flag
// Show Seconds
// hour12Clock
// amPM
// ShowAMPMIndicator
// MINUTE PROGESS BAR
uint8_t general_flags = 0; // THE DEFAULT FLAG POSITIONS ARE SET IN SETUP() FROM EEPROM

uint8_t lastHour;
uint8_t lastMin;
uint8_t lastSec;
uint8_t refresh = 1;

// SCREEN
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// PAGES
ClockPage clockPage;
SettingsPage settingsPage(false);
ChangeTimePage changeTimePage;
ChangeDatePage changeDatePage;

// ISR(WDT_vect) {
//   if (currentPage == CLOCK_P) sleep_disable();
// }


void setup() { 
  Wire.begin();
  
  Serial.begin(9600);

  //Start the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // set_sleep_mode(SLEEP_MODE_IDLE);

  // wdt_enable(WDTO_8S);
  // WDTCSR |= _BV(WDIE);

  // Set basic display configs
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.cp437(true);                  // Use full 256 char 'Code Page 437' font

  // Set pins
  // pinMode(SW, INPUT_PULLUP);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(
    digitalPinToInterrupt(DT), []() {
      refresh++;
      // if (currentPage == CLOCK_P) sleep_disable();
      encoder.tick();
    },
    CHANGE);
  attachInterrupt(
    digitalPinToInterrupt(CLK), []() {
      refresh++;
      // if (currentPage == CLOCK_P) sleep_disable();
      encoder.tick();
    },
    CHANGE);

  p_btn.setDebounceTime(50);
  myRTC.setClockMode(false);
  
  // If it's over 12 and our time flag is true (AM) toggle it to PM
  if (myRTC.getHour(h12Flag, pmFlag) > 12 && Helper::getAMFlag()) Helper::toggleAMFlag();
  lastHour = myRTC.getHour(h12Flag, pmFlag);

  // SETTINGS SET FROM EEPROM
  uint8_t b = EEPROM.read(0); // Clockface Pointer in clockFace.cpp
  clockPage.facePTR = (b == 0xFF) ? clockPage.TIMEONLY : b;

  b = EEPROM.read(settingsPage.SHOWSECONDS);
  Helper::setShowSecondsFlag(b == 0xFF ? false : b);

  b = EEPROM.read(settingsPage.TIMEFORMAT);
  Helper::setH12Flag(b == 0xFF ? false : b);

  b = EEPROM.read(settingsPage.AMPMINDICATOR);
  Helper::setAMPMIndicatorFlag(b == 0xFF ? false : b);

  b = EEPROM.read(settingsPage.MINPROG);
  Helper::setMinProgFlag(b == 0xFF ? false : b);

}

void loop() {
  currentHour = myRTC.getHour(h12Flag, pmFlag);
  currentMinute = myRTC.getMinute();
  currentSecond = myRTC.getSecond();

  if (lastHour != currentHour) {
    if (currentHour == 0 || currentHour > 12) Helper::toggleAMFlag();
    lastHour = currentHour;
  }

  if (lastMin != currentMinute) {
    lastMin = currentMinute;
    refresh++;
  }
  
  if ((clockPage.facePTR == clockPage.TIMEONLY || clockPage.facePTR == clockPage.RECTANGLECLOCK) && lastSec != currentSecond){
    lastSec = currentSecond;
    refresh++;
  }

  handleClicks();

  if (refresh) {
    display.clearDisplay();

    handleClicks();

    // Poll the rotary encoder
    rotaryPosition = (int8_t)encoder.getPosition() * -1;

    switch(currentPage) {
    case CLOCK_P:
      clockPage.drawClockFace();
      break;
    case SETTINGS_PAGE:
      settingsPage.drawPage();
      break;
    case CHANGETIME_P:
      changeTimePage.captureTimeToDisplay();
      changeTimePage.drawPage();
      break;
    case CHANGEDATE_P:
      changeDatePage.captureDateToDisplay();
      changeDatePage.drawPage();
      break;
    default:
      break;
    }

    display.display();

    refresh--;
  }
}

void handleClicks() {

  p_btn.loop();
  
  // Called once when button is pressed down
  if (p_btn.isPressed()){
    refresh++;
    Helper::setShowLongPressFeedbackFlag(false);
    ezpressedTime = millis();
    startRotNum = (int8_t)encoder.getPosition() * -1;
  }
  else if (p_btn.isReleased()) {
    
    if (Helper::getIgnoreClickFlag()) {Helper::setIgnoreClickFlag(false); return;}
    ezreleasedTime = millis();

    refresh++;

    long pressDuration = ezreleasedTime - ezpressedTime;

    if (pressDuration >= EXTRA_LONG_PRESS_TIME) {
      if (currentPage == CHANGETIME_P) {
          changeTimePage.reset();
      } 
      if (currentPage == CHANGEDATE_P) {
          changeDatePage.reset();
      } 
      currentPage = CLOCK_P;
      stackptr = 0;
    } 
    else if (pressDuration >= LONG_PRESS_TIME) {
      if (currentPage == CHANGETIME_P) {
        if (!changeTimePage.moveHourCursor(false)) return;
      }
      if (currentPage == CHANGEDATE_P) {
        if (!changeDatePage.moveDateCursor(false)) return;
      }
      if (currentPage == SETTINGS_PAGE) {
        settingsPage.scroll = 0;
      }

      currentPage = pageStack[--stackptr];

    } 
    else if (pressDuration < LONG_PRESS_TIME) {
      if (currentPage == CLOCK_P) {
        currentPage = SETTINGS_PAGE;
        pageStack[++stackptr] = SETTINGS_PAGE;
      } 
      else if (currentPage == SETTINGS_PAGE) {
        if (rotaryPosition == SettingsPage::CHANGETIME) {
          currentPage = CHANGETIME_P;
          pageStack[++stackptr] = CHANGETIME_P;
        }
        else if (rotaryPosition == SettingsPage::CHANGEDATE) {
          currentPage = CHANGEDATE_P;
          pageStack[++stackptr] = CHANGEDATE_P;
        }
        else if (rotaryPosition == SettingsPage::TIMEFORMAT) {
          Helper::toggleH12Flag();
          EEPROM.write(settingsPage.TIMEFORMAT, Helper::getH12Flag());
          return; // Returning so we avoid the endoer position reset below
        }
        else if (rotaryPosition == SettingsPage::SHOWSECONDS) {
          Helper::toggleShowSecondsFlag();
          EEPROM.write(settingsPage.SHOWSECONDS, Helper::getShowSecondsFlag());
          return; // Returning so we avoid the endoer position reset below
        }
        else if (rotaryPosition == SettingsPage::AMPMINDICATOR) {
          Helper::toggleAMPMIndicatorFlag();
          EEPROM.write(settingsPage.AMPMINDICATOR, Helper::getAMPMIndicatorFlag());
          return; // Returning so we avoid the endoer position reset below
        }
        else if (rotaryPosition == SettingsPage::MINPROG){
          Helper::toggleMinProgFlag();
          EEPROM.write(settingsPage.MINPROG, Helper::getMinProgFlag());
          return; // Returning so we avoid the endoer position reset below
        }
      }
      else if (currentPage == CHANGETIME_P){
        
        // If true we've left this page
        if (changeTimePage.moveHourCursor(true)) {
          currentPage = CLOCK_P;
          stackptr = 0;
        }
        return;
      }
      else if (currentPage == CHANGEDATE_P){
        // If true we've left this page
        if (changeDatePage.moveDateCursor(true)) {
          currentPage = CLOCK_P;
          stackptr = 0;
        }
        return;
      }
    }

    encoder.setPosition(0);
  } 
  // If the button is currently being pressed
  else if (p_btn.getState() == 0){

    refresh++;

    int8_t rotNum = (int8_t)encoder.getPosition() * -1;

    drawClickFeedback();

    if (currentPage == CLOCK_P){
      if (rotNum - startRotNum >= SWTICH_CLOCK_FACE_TICKS){
        startRotNum = rotNum;
        Helper::setIgnoreClickFlag(true);
        clockPage.switchClockFace(true);
      } 
      else if (rotNum - startRotNum <= -SWTICH_CLOCK_FACE_TICKS){
        startRotNum = rotNum;
        Helper::setIgnoreClickFlag(true);
        clockPage.switchClockFace(false);
      }
      else if(!Helper::getIgnoreClickFlag() && abs(rotNum - startRotNum) > 0) {
        Helper::setIgnoreClickFlag(true);
      }
    }


  }

}

inline void drawClickFeedback(){
  int timeSincePress = millis() - ezpressedTime;
  if (!Helper::getIgnoreClickFlag()){

    if (!Helper::getShowLongPressFeedbackFlag() && timeSincePress >= (LONG_PRESS_TIME/4)) display.drawCircleHelper(128-10, 64-10, 4, 0x1, 1);
    if (!Helper::getShowLongPressFeedbackFlag() && timeSincePress >= (LONG_PRESS_TIME/4)*2) display.drawCircleHelper(128-10, 64-10, 4, 0x2, 1);
    if (!Helper::getShowLongPressFeedbackFlag() && timeSincePress >= (LONG_PRESS_TIME/4)*3) display.drawCircleHelper(128-10, 64-10, 4, 0x4, 1);
    if (timeSincePress >= LONG_PRESS_TIME){
      display.fillCircle(128-10, 64-10, 4, 1);
      Helper::setShowLongPressFeedbackFlag(true);
    }

    if (Helper::getShowLongPressFeedbackFlag()) {
      if (timeSincePress >= (LONG_PRESS_TIME + (EXTRA_LONG_PRESS_TIME-LONG_PRESS_TIME)/4)) display.drawCircleHelper(128-10, 64-10, 8, 0x1, 1);
      if (timeSincePress >= (LONG_PRESS_TIME + (EXTRA_LONG_PRESS_TIME-LONG_PRESS_TIME)/4*2)) display.drawCircleHelper(128-10, 64-10, 8, 0x2, 1);
      if (timeSincePress >=(LONG_PRESS_TIME + (EXTRA_LONG_PRESS_TIME-LONG_PRESS_TIME)/4*3))  display.drawCircleHelper(128-10, 64-10, 8, 0x4, 1);
      if (timeSincePress >= EXTRA_LONG_PRESS_TIME) {
        display.fillCircle(128-10, 64-10, 8, 1);
      }
    }
  }
}
