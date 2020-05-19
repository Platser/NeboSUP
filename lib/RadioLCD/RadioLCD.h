#include <Arduino.h>
#include <NeboLCD.h>

enum neboRadioModes {
  NEBO_RADIO_MODE_MHZ,  // 0
  NEBO_RADIO_MODE_KHZ   // 1
};


enum neboRadioStates {
  NEBO_RADIO_STATE_SB,           // 0
  NEBO_RADIO_STATE_FR,           // 1
  NEBO_RADIO_STATE_CH,           // 2
  NEBO_RADIO_STATE_CH_SEL,       // 3
  NEBO_RADIO_STATE_CH_PG_MHZ,    // 4
  NEBO_RADIO_STATE_CH_PG_KHZ     // 5
};

class RadioLCD
{
  public:
    void init(NeboLCD *lcd);
    void setFreq(String freq);
    String getFr( char delim );
    String getSb( char delim );
    void show();
    void blinkSbK();
    void blinkSbM();
    void switchKhzStep();
    void swapFrAndSb();
    void incrSbK();
    void decrSbK();
    void incrSbM();
    void decrSbM();
    void incr();
    void decr();
    void swKM(); // switch between KHz and MHz input
    String name;
  private:
    NeboLCD *_lcd;
    int _frM;
    int _frK;
    int _sbM;
    int _sbK;
    byte _khzStep = 50; // 50 or 25
    neboRadioModes _radioMode;
    neboRadioStates _radioState;
};