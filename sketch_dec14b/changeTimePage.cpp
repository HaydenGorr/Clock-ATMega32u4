#include "changeTimePage.h"
#include "helper.h"

ChangeTimePage::ChangeTimePage(){
  changeHourCursor = ChangeTimePage::HOUR;
  storedTimeBackup = false;
  minBackup = -1;
  hourBackup = -1;
}

void ChangeTimePage::captureTimeToDisplay(){
  if (!storedTimeBackup) {
    amBackup = Helper::getAMFlag();

    hourBackup = myRTC.getHour(h12Flag, pmFlag);
    minBackup = myRTC.getMinute();

    storedTimeBackup = true;
    
    // I have this so the encoder doesn't reset the hour backup time to 0
    // when changeTimePage() runs when the page first opens
    encoder.setPosition(Helper::getH12Flag() && hourBackup>12 ? (hourBackup-12)*-1  : hourBackup*-1);
  }
}

void ChangeTimePage::drawPage(){
  // Draw title
  display.setTextSize(1);
  display.setCursor(1, 1);
  Helper::WriteCharArray("Set the time");

  char clampedRotaryVal = ChangeTimePage::clampRotary();

  if (changeHourCursor == HOUR) hourBackup = clampedRotaryVal;
  else if (changeHourCursor == MIN) minBackup = clampedRotaryVal;
  else if (changeHourCursor == AMPM) amBackup = clampedRotaryVal;

  display.setCursor(1, 20);

  display.setTextSize(2);
  Helper::printByteAsChars(Helper::getH12Flag() && hourBackup > 12 ? hourBackup-12 : hourBackup);
  display.write(":");
  Helper::printByteAsChars(minBackup);
  
  if (Helper::getH12Flag()) Helper::WriteCharArray(amBackup ? " am" : " pm");

  display.setTextSize(1);
  display.setCursor(1, 50);
  Helper::WriteCharArray("Save");

  if (changeHourCursor == HOUR){
    display.fillRect(1, 40, 20, 2, 1);
  }
  else if (changeHourCursor == MIN){
    display.fillRect(37, 40, 20, 2, 1);
  }
  else if (Helper::getH12Flag() && changeHourCursor == AMPM) {
    display.fillRect(73, 40, 20, 2, 1);
  }
  else if (changeHourCursor == SAVE){
    display.fillRect(1, 61, 22, 2, 1);
  }
}


inline uint8_t ChangeTimePage::clampRotary(){

  if (changeHourCursor == HOUR){
    if (rotaryPosition > (Helper::getH12Flag() ? 12 : 23)) {
      hourBackup = 0;
      encoder.setPosition(0);
      return 0;
    }
    else if (rotaryPosition < 0) {
      encoder.setPosition((Helper::getH12Flag() ? -12 : -23));
      return (Helper::getH12Flag() ? 12 : 23);
    }
  }

  if (changeHourCursor == MIN){
    if (rotaryPosition > 59) {
      encoder.setPosition(0);
      return 0;
    }
    else if (rotaryPosition < 0) {
      encoder.setPosition(-59);
      return 59;
    }
  }

  if (changeHourCursor == AMPM){
    if (rotaryPosition > 1) {
      encoder.setPosition(0);
      return 0;
    }
    else if (rotaryPosition < 0) {
      encoder.setPosition(-1);
      return 1;
    }
  }

  return rotaryPosition;
}

void ChangeTimePage::reset(){
  storedTimeBackup = false;
  changeHourCursor = HOUR;
}

bool ChangeTimePage::moveHourCursor(bool moveUp){
  if (moveUp){

    if (changeHourCursor == HOUR){
      changeHourCursor = MIN;
      encoder.setPosition(minBackup*-1);
    }
    else if (changeHourCursor == MIN) {
      changeHourCursor = Helper::getH12Flag() ? AMPM : SAVE;
      if (Helper::getH12Flag()) encoder.setPosition(amBackup*-1);
    }
    else if (changeHourCursor == AMPM) changeHourCursor = SAVE;
    else if (changeHourCursor == SAVE) {
      lastHour = hourBackup;
      Helper::setAMFlag(amBackup);
      myRTC.setHour(hourBackup);
      myRTC.setMinute(minBackup);
      myRTC.setSecond(0);
      reset();
      return true;
    }
  }
  else {
    if (changeHourCursor == SAVE) {
      changeHourCursor = Helper::getH12Flag() ? AMPM : MIN;
      if (Helper::getH12Flag()) encoder.setPosition(amBackup*-1);
    }
     else if (changeHourCursor == AMPM) {
      changeHourCursor = MIN;
      encoder.setPosition(minBackup*-1);
    }
    else if (changeHourCursor == MIN) {
      changeHourCursor = HOUR;
      encoder.setPosition(hourBackup*-1);
    }
    else if (changeHourCursor == HOUR){
      reset();
      return true;
    }
  }
  return false;
}






