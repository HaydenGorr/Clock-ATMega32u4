#include "changeDatePage.h"
#include "helper.h"

const uint8_t* ChangeDatePage::months[] = {"Jan", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const uint8_t* ChangeDatePage::monthsShort[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec"};
const uint8_t ChangeDatePage::monthsCode[] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

ChangeDatePage::ChangeDatePage(){
  changeDateCursor = DAY;
  storedDateBackup = false;
  dayBackup = -1;
  monthBackup = -1;
  yearBackup = -1;
}

void ChangeDatePage::captureDateToDisplay(){
  if (!storedDateBackup) {
    dayBackup = myRTC.getDate();
    monthBackup = myRTC.getMonth(century);
    yearBackup = myRTC.getYear();

    storedDateBackup = true;
    // I have this so the encoder doesn"t reset the hour backup time to 0
    // when changeTimePage() runs when the page first opens
    encoder.setPosition(dayBackup*-1);

    // We use this global var in drawPage, so we reset it here before we go to drawpage
    rotaryPosition = (int8_t)encoder.getPosition() * -1;
  }
}

void ChangeDatePage::drawPage(){
  // Draw title
  display.setTextSize(1);
  display.setCursor(1, 1);
  Helper::WriteCharArray("Set the date");

  char clampedRotaryPos = ChangeDatePage::clampRotary();

  if (changeDateCursor == DAY) dayBackup = clampedRotaryPos;
  else if (changeDateCursor == MONTH) monthBackup = clampedRotaryPos;
  else if (changeDateCursor == YEAR) yearBackup = clampedRotaryPos;

  display.setCursor(1, 20);

  display.setTextSize(2);
  Helper::printByteAsChars(dayBackup);
  display.write("-");
  Helper::WriteCharArray(monthsShort[monthBackup-1]);
  display.write("-");
  Helper::printByteAsChars(yearBackup);

  display.setTextSize(1);
  display.setCursor(1, 50);
  Helper::WriteCharArray("Save");

  if (changeDateCursor == DAY){
    display.fillRect(1, 40, 22, 2, 1);
  }
  else if (changeDateCursor == MONTH){
    int cursorWidth = 33;
    if(monthBackup == JULY || monthBackup == SEPTEMBER ){
      cursorWidth += 13;
    }
    display.fillRect(38, 40, cursorWidth, 2, 1);
  }
  else if (changeDateCursor == YEAR){
    int cursoroffset = 0;
    if(monthBackup == JULY || monthBackup == SEPTEMBER ){
      cursoroffset += 12;
    }
    display.fillRect(86+cursoroffset, 40, 22, 2, 1);
  }
  else if (changeDateCursor == SAVE){
    display.fillRect(1, 61, 24, 2, 1);
  }
}

void ChangeDatePage::reset(){
  storedDateBackup = false;
  changeDateCursor = DAY;
}

char ChangeDatePage::clampRotary(){
  // if the cursor is over 28 check if it should wrap back around to 1
  if (changeDateCursor == DAY && rotaryPosition > 28){
    switch(monthBackup) {
      case FEBRUARY:
        encoder.setPosition(-1);
        return 1;
      case APRIL:
      case JUNE:
      case SEPTEMBER:
      case NOVEMBER:
        if (rotaryPosition > 30){
          dayBackup = 1;
          encoder.setPosition(-1);
          return 1;
        }
        break;
      default:
        if (rotaryPosition > 31){
          encoder.setPosition(-1);
          return 1;
        }
    }
  }

  // if the cursor is under 1 it should wrap around
  else if (changeDateCursor == DAY && rotaryPosition < 1){
    switch(monthBackup) {
      case FEBRUARY:
        encoder.setPosition(-28);
        return 28;
      case APRIL:
      case JUNE:
      case SEPTEMBER:
      case NOVEMBER:
        encoder.setPosition(-30);
        return 30;
      default:
        encoder.setPosition(-31);
        return 31;
    }
  }

  if (changeDateCursor == MONTH && rotaryPosition > 12) {
    encoder.setPosition(-1);
    return 1;
  }
  else if (changeDateCursor == MONTH && rotaryPosition < 1) {
    encoder.setPosition(-12);
    return 12;
  }

  if (changeDateCursor == YEAR && rotaryPosition > 99) {
    encoder.setPosition(0);
    return 0;
  }
  else if (changeDateCursor == YEAR && rotaryPosition < 0) {
    encoder.setPosition(-99);
    return 99;
  }

  return rotaryPosition;
}

bool ChangeDatePage::moveDateCursor(bool moveUp){
  if (moveUp){
    if (changeDateCursor == DAY){
      changeDateCursor = MONTH;
      encoder.setPosition(monthBackup*-1);
    }
    else if (changeDateCursor == MONTH) {
      changeDateCursor = YEAR;
      encoder.setPosition(yearBackup*-1);
    }
    else if (changeDateCursor == YEAR) {
      changeDateCursor = SAVE;
    }
    else if (changeDateCursor == SAVE) {
      myRTC.setDate(dayBackup);
      myRTC.setMonth(monthBackup);
      myRTC.setYear(yearBackup);
      myRTC.setDoW(Helper::calculateDayOfWeek(yearBackup, monthBackup, dayBackup));
      reset();
      return true;
    }
  }
  else {
    if (changeDateCursor == SAVE){
      changeDateCursor = YEAR;
      encoder.setPosition(yearBackup*-1);
    }
    else if (changeDateCursor == YEAR){
      changeDateCursor = MONTH;
      encoder.setPosition(monthBackup*-1);
    }
    else if (changeDateCursor == MONTH) {
      changeDateCursor = DAY;
      encoder.setPosition(dayBackup*-1);
    }
    else if (changeDateCursor == DAY){
      reset();
      return true;
    }
  }
  return false;
}







