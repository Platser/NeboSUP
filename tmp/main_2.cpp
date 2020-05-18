#include <Arduino.h>
#include "NeboLCD.h"
#include "RadioLCD.h"
#include <Bounce2.h>
#include <Encoder.h>



#define ENC_LEFT_PIN_1 2
#define ENC_LEFT_PIN_2 4
#define ENC_RIGHT_PIN_1 3
#define ENC_RIGHT_PIN_2 5
#define BUTTON_LEFT_PIN 6
#define BUTTON_RIGHT_PIN 7
#define BUTTON_ENC_LEFT_PIN 8
#define BUTTON_ENC_RIGHT_PIN 9

/*#define MYENC_NONE  0
#define MYENC_PLUS  1
#define MYENC_MINUS 2
#define MYENC_PUSH  3*/



//   avoid using pins with LEDs attached

long oldPositionLeft  = -999;
long oldPositionRight  = -999;

enum myEncStates {
  NONE,
  PLUS,
  MINUS,
  PUSH,
  PUSH_LONG
};

#define ENC_STEP 2

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
    if ( this->_push_btn->previousDuration > 2000 ) {
      return PUSH_LONG;
    } else {
      return PUSH;
    }
  }
  long newPos = this->_enc->read();
  if ( newPos != this->_oldPos ) {
    if ( newPos - this->_oldPos >= ENC_STEP ) {
      return PLUS;
    }
    if ( newPos - this->_oldPos <= -ENC_STEP ) {
      return MINUS;
    }
  }
  return NONE;
}

Bounce debouncer_left = Bounce();
Encoder encLeft(ENC_LEFT_PIN_1, ENC_LEFT_PIN_2);
Encoder encRight(ENC_RIGHT_PIN_1, ENC_RIGHT_PIN_2);

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
  debouncer_left.attach(BUTTON_LEFT_PIN, INPUT_PULLUP);
  debouncer_left.interval(25);
  counter = 0;
}

bool flag = false;

void loop() {
  // put your main code here, to run repeatedly:
  debouncer_left.update();
  if ( debouncer_left.fell() ) {
    counter++;
    Serial.println(counter);
    com1Lcd.swapFrAndSb();
  }

  long newPositionRight = encRight.read();
  if (newPositionRight != oldPositionRight) {
    if( newPositionRight > oldPositionRight ) {
      com1Lcd.incrSbK();
    } else {
      com1Lcd.decrSbK();
    }
    oldPositionRight = newPositionRight;
  }

  lcd.render();
  //com1Lcd.show();
  /*if( millis() - timer > 1000 ) {
    timer = millis();
    Serial.println("----------------");
    Serial.println(com1Lcd.blinkMask[0], BIN);
    Serial.println(com1Lcd.blinkMask[1], BIN);
  }*/

  if( millis() - timer > 5000 ) {
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
  }


}