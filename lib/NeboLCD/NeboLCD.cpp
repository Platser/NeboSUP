#include <Arduino.h>
#include "NeboLCD.h"

void NeboLCD::init()
{
  this->_lcd = new LiquidCrystal_I2C(0x27, 16, 2);
  this->_lcd->init();
  this->_lcd->backlight();
  this->_timer = millis();
  this->resetBlink();
  this->_blinkFlag = false;
  this->_redrawNeeded = true;
  //                0123456789ABCDEF
  this->welcome();
}

void NeboLCD::resetBlink()
{
  this->blinkMask[0] = 0;
  this->blinkMask[1] = 0;
}

void NeboLCD::setBlink(byte line, byte pos, byte len) {
  this->blinkMask[line] |= ((unsigned int) (pow(2,len)-1)) << (16-len-pos);
}

void NeboLCD::setBlink(byte line, unsigned int mask) {
  this->blinkMask[line] = mask;
  this->_blinkFlag = false;
  this->_timer = millis() + 10000;
}

void NeboLCD::setLine(byte i, String str)
{
  this->_line[i] = str;
  this->_redrawNeeded = true;
  this->_blinkFlag = false;
  this->_timer = millis() + 10000;
}

void NeboLCD::_clearScreen() {
  for( byte l = 0; l < 2; l++ ) {
    this->_lcd->setCursor(0, l);
    this->_lcd->print("                ");
  }
}

void NeboLCD::welcome() {
  this->setLine(0, "----Nebo-LCD----");
  this->setLine(1, "-->>-Wazzap-<<<-");
  this->setBlink(1,2,2);
  this->setBlink(1,12,3);
}

void NeboLCD::render() {
  if( this->_redrawNeeded ) {
    for( byte l = 0; l < 2; l++ ) {
      this->_lcd->setCursor(0, l);
      this->_lcd->print(this->_line[l]);
    }
    this->_redrawNeeded = false;
  }
  if( millis() - this->_timer > 400 ) {
    this->_timer = millis();
    for( byte l = 0; l < 2; l++ ) {
      for( byte i = 0; i < 16; i++ ) {
        if( bitRead(this->blinkMask[l], 15-i) ) {
        //if( l == 1 && ( i == 1 || i == 14 ) ) {

          if( this->_blinkFlag ) {
            this->_lcd->setCursor(i, l);
            this->_lcd->print(" ");
          } else {
            this->_lcd->setCursor(i, l);
            this->_lcd->print(this->_line[l].charAt(i));
          }
        }
      }
    }
    this->_blinkFlag = !this->_blinkFlag;
  }
}
