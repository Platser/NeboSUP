#include <Arduino.h>
#include <NeboLCD.h>
#include <RadioLCD.h>
#include <Bounce2.h>
#include <Encoder.h>
#include <AutopilotLCD.h>




#define PIN_ENC_LEFT_1 2
#define PIN_ENC_LEFT_2 4
#define PIN_ENC_LEFT_BTN 8

#define PIN_ENC_RIGHT_1 3
#define PIN_ENC_RIGHT_2 5
#define PIN_ENC_RIGHT_BTN 9

#define PIN_BTN_LEFT 6
#define PIN_BTN_RIGHT 7

#define ENC_STEP 4

void nextDisplay();
void prevDisplay();

enum hwEvents {
  EMPTY_HW_EVENT,
  BTN_L_PUSH,
  BTN_L_LPUSH,
  ENC_L_INC,
  ENC_L_DEC,
  ENC_L_PUSH,
  ENC_L_LPUSH,
  BTN_R_PUSH,
  BTN_R_LPUSH,
  ENC_R_INC,
  ENC_R_DEC,
  ENC_R_PUSH,
  ENC_R_LPUSH
};

enum btnEvents {
  EMPTY_BTN_EVENT,
  BTN_PUSH,
  BTN_LPUSH
};

enum myEncStates {
  EMPTY_ENC_EVENT,
  PLUS,
  MINUS,
  PUSH,
  PUSH_LONG
};

enum displays {
  COM1,
  NAV1,
  COM2,
  NAV2,
  AUTOPILOT
} display;

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
  return EMPTY_ENC_EVENT;
}

Bounce btn_left = Bounce();
Bounce btn_right = Bounce();
MyEnc enc_left(PIN_ENC_LEFT_1, PIN_ENC_LEFT_2, PIN_ENC_LEFT_BTN);
MyEnc enc_right(PIN_ENC_RIGHT_1, PIN_ENC_RIGHT_2, PIN_ENC_RIGHT_BTN);

uint32_t timer;

NeboLCD lcd;
RadioLCD com1;
RadioLCD nav1;
RadioLCD com2;
RadioLCD nav2;
AutopilotLCD autopilot;

void *radio;

byte counter;

btnEvents readBtnEvent( Bounce *btn ) {
  btn->update();
  if ( btn->fell() ) {
    return BTN_PUSH;
  } else {
    return EMPTY_BTN_EVENT;
  }
}

hwEvents readHwEvent() {
  switch( readBtnEvent( &btn_left ) ) {
    case BTN_PUSH:
      return BTN_L_PUSH;
      break;
    case BTN_LPUSH:
      return BTN_L_LPUSH;
      break;
    default:
      break;
  }
  switch( readBtnEvent( &btn_right ) ) {
    case BTN_PUSH:
      return BTN_R_PUSH;
      break;
    case BTN_LPUSH:
      return BTN_R_LPUSH;
      break;
    default:
      break;
  }
  switch( enc_left.read() ) {
    case PLUS:
      return ENC_L_INC;
      break;
    case MINUS:
      return ENC_L_DEC;
      break;
    case PUSH:
      return ENC_L_PUSH;
      break;
    case PUSH_LONG:
      return ENC_L_LPUSH;
      break;
    default:
      break;
  }
  switch( enc_right.read() ) {
    case PLUS:
      return ENC_R_INC;
      break;
    case MINUS:
      return ENC_R_DEC;
      break;
    case PUSH:
      return ENC_R_PUSH;
      break;
    case PUSH_LONG:
      return ENC_R_LPUSH;
      break;
    default:
      break;
  }
  return EMPTY_HW_EVENT;
}

void handleEventCom( RadioLCD *com, hwEvents event ) {
  switch( event ) {
    case BTN_L_PUSH:
      com->swapFrAndSb();
      break;
    case ENC_R_INC:
      com->incr();
      break;
    case ENC_R_DEC:
      com->decr();
      break;
    case ENC_R_PUSH:
      com->swKM();
      break;
    case ENC_R_LPUSH:
      com->switchKhzStep();
      break;
    default:
      break;
  }
}

void handleEvent( hwEvents event ) {
  switch( event ) {
    case ENC_L_INC:
      nextDisplay();
      break;
    case ENC_L_DEC:
      prevDisplay();
      break;
    default:
      break;
  }
  switch( display ) {
    case COM1:
      handleEventCom(&com1, event);
      break;
    case COM2:
      handleEventCom(&com2, event);
      break;
    case NAV1:
      handleEventCom(&nav1, event);
      break;
    case NAV2:
      handleEventCom(&nav2, event);
      break;
    case AUTOPILOT:
      switch( event ) {
        case BTN_L_PUSH:
          break;
        case ENC_R_INC:
          autopilot.incr();
          break;
        case ENC_R_DEC:
          autopilot.decr();
          break;
        case ENC_R_PUSH:
          autopilot.swEdit();
          break;
        case ENC_R_LPUSH:
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}


void nextDisplay() {
  switch (display) {
    case AUTOPILOT:
      display = COM1;
      break;
    case COM1:
      display = NAV1;
      break;
    case NAV1:
      display = COM2;
      break;
    case COM2:
      display = NAV2;
      break;
    case NAV2:
      display = AUTOPILOT;
      break;
  }
}

void prevDisplay() {
  switch (display) {
    case AUTOPILOT:
      display = NAV2;
      break;
    case COM1:
      display = AUTOPILOT;
      break;
    case NAV2:
      display = COM2;
      break;
    case COM2:
      display = NAV1;
      break;
    case NAV1:
      display = COM1;
      break;
  }
}

void showDisplay() {
  switch (display){
    case COM1:
      com1.show();
      break;
    case NAV1:
      nav1.show();
      break;
    case COM2:
      com2.show();
      break;
    case NAV2:
      nav2.show();
      break;
    case AUTOPILOT:
      autopilot.show();
      break;
  }
}



void setup() {
    // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();
  display = COM1;
  // COM1
  com1.init(&lcd);
  com1.setName("COM1");
  // NAV1
  nav1.init(&lcd);
  nav1.setName("NAV1");
  // COM2
  com2.init(&lcd);
  com2.setName("COM2");
  // NAV2
  nav2.init(&lcd);
  nav2.setName("NAV2");

  // Autopilot
  autopilot.init(&lcd);

  timer = millis();

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
  handleEvent(
    readHwEvent()
  );
  showDisplay();
  lcd.render();
}