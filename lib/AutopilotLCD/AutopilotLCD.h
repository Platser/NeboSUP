#include <Arduino.h>
#include <NeboLCD.h>


enum autopilotModes {
  AUTOPILOT_MODE_SELECT,
  AUTOPILOT_MODE_EDIT
};

enum autopilotFields {
  AUTOPILOT_FIELD_HDG,
  AUTOPILOT_FIELD_ALT,
  AUTOPILOT_FIELD_SPD,
  AUTOPILOT_FIELD_VS
};

class AutopilotLCD {
  public:
    void init(NeboLCD *lcd);
    void show();
    String getHdg();
    String getSpd();
    String getAlt();
    String getVs();
    void incr();
    void decr();
    void swEdit(); // Enter/exit field editing
  private:
    NeboLCD *_lcd;
    uint16_t _hdg;
    uint16_t _spd;
    uint8_t  _alt;
    int8_t   _vs;
    autopilotModes _mode;
    autopilotFields _activeField;
};