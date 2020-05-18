#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <GyverTimer.h>

class NeboLCD {
  public:
    //NeboLCD(byte i);
    void init(); // init should be used instead of constructor
                 // https://forum.arduino.cc/index.php?topic=68854.0
    void resetBlink();
    void setBlink(byte line, byte pos, byte len);
    void setBlink(byte line, unsigned int mask);
    void setLine(byte i, String str);
    void render();
    void welcome();
    unsigned int blinkMask[2];

  private:
    LiquidCrystal_I2C *_lcd;
    String _line[2];
    String _blink[2];
    bool _blinkFlag;
    //GTimer *_timer;
    uint32_t _timer;
    bool _redrawNeeded;
    void _clearScreen();
};
