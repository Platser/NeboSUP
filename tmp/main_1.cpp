#include <Arduino.h>
/*#include "GyverTimer.h"
#include "NeboLCD.h"*/

#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcdAp(0x27, 16, 2);

//NeboLCD com1Lcd(1);

class MyLcd {
  public:
    //MyLcd();
    void init();
  private:
    LiquidCrystal_I2C *_lcd;
};

/*MyLcd::MyLcd() {
      LiquidCrystal_I2C lcdAp(0x27, 16, 2);
      lcdAp.init();
      lcdAp.backlight();
      lcdAp.setCursor(0, 0);
      lcdAp.print("    Nebo LCD    ");
      lcdAp.setCursor(0, 1);
      lcdAp.print("   Constructor  ");
    }*/

void MyLcd::init() {
  this->_lcd = new LiquidCrystal_I2C(0x27, 16, 2);
  this->_lcd->init();
  this->_lcd->backlight();
  this->_lcd->setCursor(0, 0);
  this->_lcd->print("    Nebo LCD    ");
  this->_lcd->setCursor(0, 1);
  this->_lcd->print("      Init1     ");
}

MyLcd myLcd;

void setup1() {
  //myLcd = MyLcd();
  myLcd.init();
  /*lcdAp.init();
  lcdAp.backlight();
  lcdAp.setCursor(0, 0);
  lcdAp.print("    Nebo LCD    ");*/
  // put your setup code here, to run once:
}

void loop1() {
  // put your main code here, to run repeatedly:
  //com1Lcd.render();
  delay(2000);
}