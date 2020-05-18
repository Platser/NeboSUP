#include <Arduino.h>
#include "NeboLCD.h"
#include "RadioLCD.h"
#include <Bounce2.h>
#include <Encoder.h>



#define PIN_ENC_LEFT_1 2
#define PIN_ENC_LEFT_2 4
#define PIN_ENC_LEFT_BTN 8

#define PIN_ENC_RIGHT_1 3
#define PIN_ENC_RIGHT_2 5
#define PIN_ENC_RIGHT_BTN 9

#define PIN_BTN_LEFT 6
#define PIN_BTN_RIGHT 7


enum myEncStates {
  NONE,
  PLUS,
  MINUS,
  PUSH,
  PUSH_LONG
};

#define ENC_STEP 4

class MyEnc {
  public:
    MyEnc(uint8_t pin_enc_1, uint8_t pin_enc_2, uint8_t pin_push);
    //void init(uint8_t pin_enc_1, uint8_t pin_enc_2, uint8_t pin_push);
    void init();
    myEncStates read();
  private:
    Encoder *_enc;
    uint8_t _pin_enc_1;
    uint8_t _pin_enc_2;
    uint8_t _pin_push;
    Bounce *_push_btn;
    long _oldPos;
};

MyEnc::MyEnc(uint8_t pin_enc_1, uint8_t pin_enc_2, uint8_t pin_push) {
  this->_pin_enc_1 = pin_enc_1;
  this->_pin_enc_2 = pin_enc_2;
  this->_pin_push = pin_push;
  this->_oldPos = -999;
}

void MyEnc::init() {
  this->_enc = new Encoder(this->_pin_enc_1, this->_pin_enc_2);
  this->_push_btn = new Bounce();
  this->_push_btn->attach(this->_pin_push, INPUT_PULLUP);
}

myEncStates MyEnc::read() {
  this->_push_btn->update();
  if ( this->_push_btn->rose() ) {
    if ( this->_push_btn->previousDuration() > 1000 ) {
      return PUSH_LONG;
    } else {
      return PUSH;
    }
  }
  long newPos = this->_enc->read();
  if ( newPos != this->_oldPos ) {
    if ( newPos - this->_oldPos >= ENC_STEP ) {
      this->_oldPos = newPos;
      return PLUS;
    }
    if ( newPos - this->_oldPos <= -ENC_STEP ) {
      this->_oldPos = newPos;
      return MINUS;
    }
  }
  return NONE;
}

Bounce btn_left = Bounce();
Bounce btn_right = Bounce();
MyEnc enc_left(PIN_ENC_LEFT_1, PIN_ENC_LEFT_2, PIN_ENC_LEFT_BTN);
MyEnc enc_right(PIN_ENC_RIGHT_1, PIN_ENC_RIGHT_2, PIN_ENC_RIGHT_BTN);

uint32_t timer;

NeboLCD lcd;
RadioLCD com1Lcd;

byte counter;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();
  com1Lcd.init(&lcd);
  timer = millis();
  com1Lcd.show();
  btn_left.attach(PIN_BTN_LEFT, INPUT_PULLUP);
  btn_left.interval(25);
  btn_right.attach(PIN_BTN_RIGHT, INPUT_PULLUP);
  btn_right.interval(25);
  counter = 0;
  enc_left.init();
  enc_right.init();
}

bool flag = false;

void loop() {
  // put your main code here, to run repeatedly:
  btn_left.update();
  if ( btn_left.fell() ) {
    counter++;
    Serial.println(counter);
    com1Lcd.swapFrAndSb();
  }
  myEncStates enc_right_state = enc_right.read();
  switch (enc_right_state)
  {
  case PLUS:
    com1Lcd.incr();
    break;
  case MINUS:
    com1Lcd.decr();
    break;
  case PUSH:
    com1Lcd.swKM();
    break;
  case PUSH_LONG:
    com1Lcd.switchKhzStep();
    break;
  default:
    break;
  }

  lcd.render();

 /* if( millis() - timer > 5000 ) {
    Serial.println("----------------");
    Serial.println(lcd.blinkMask[0]);
    Serial.println(lcd.blinkMask[1]);

    timer = millis();
    if( flag ) {
      com1Lcd.blinkSbK();
      flag = false;
      Serial.println("blinkSbK");
    } else {
     com1Lcd.blinkSbM();
      flag = true;
      Serial.println("blinkSbM");
    }
  }*/


}