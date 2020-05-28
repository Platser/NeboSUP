#include <Arduino.h>
#include "AutopilotLCD.h"

/*
 * +0--3---4---B---F+   
 * |HDG 359  SPD 350|
 * |ALT 255  VS  -18| 
 * +0--3---4---B---F+
 * ALT, VS - *100ft
 * 
 */

void AutopilotLCD::init( NeboLCD *lcd ) {
  this->_lcd = lcd;
  this->_hdg = 359;
  this->_spd = 120;
  this->_alt = 40;
  this->_vs  = 7;
  this->_mode = AUTOPILOT_MODE_SELECT;
  this->_activeField = AUTOPILOT_FIELD_HDG;
}

String AutopilotLCD::getHdg() {
    char hdg[4];
    sprintf(hdg, "%03d", this->_hdg);
    return String(hdg);
}

String AutopilotLCD::getSpd() {
    char spd[4];
    sprintf(spd, "%03d", this->_spd);
    return String(spd);
}

String AutopilotLCD::getAlt() {
    char alt[4];
    sprintf(alt, "%03d", this->_alt);
    return String(alt);
}

String AutopilotLCD::getVs() {
    char vs[4];
    sprintf(vs, "%02d", this->_vs);
    return String(vs);
}

void AutopilotLCD::incr() {
  switch ( this->_mode ) {
  case AUTOPILOT_MODE_SELECT:
    switch ( this->_activeField ) {
    case AUTOPILOT_FIELD_HDG:
      this->_activeField = AUTOPILOT_FIELD_ALT;
      break;
    case AUTOPILOT_FIELD_ALT:
      this->_activeField = AUTOPILOT_FIELD_SPD;
      break;
    case AUTOPILOT_FIELD_SPD:
      this->_activeField = AUTOPILOT_FIELD_VS;
      break;
    case AUTOPILOT_FIELD_VS:
      this->_activeField = AUTOPILOT_FIELD_HDG;
      break;
    default:
      break;
    }
    break;
  case AUTOPILOT_MODE_EDIT:
    /* code */
    break;
  default:
    break;
  }
  this->show();
}

void AutopilotLCD::decr() {
  switch ( this->_mode ) {
  case AUTOPILOT_MODE_SELECT:
    switch ( this->_activeField ) {
    case AUTOPILOT_FIELD_HDG:
      this->_activeField = AUTOPILOT_FIELD_VS;
      break;
    case AUTOPILOT_FIELD_ALT:
      this->_activeField = AUTOPILOT_FIELD_HDG;
      break;
    case AUTOPILOT_FIELD_SPD:
      this->_activeField = AUTOPILOT_FIELD_ALT;
      break;
    case AUTOPILOT_FIELD_VS:
      this->_activeField = AUTOPILOT_FIELD_SPD;
      break;
    default:
      break;
    }
    break;
  case AUTOPILOT_MODE_EDIT:
    /* code */
    break;
  default:
    break;
  }
  this->show();
}

void AutopilotLCD::swEdit() {
  switch ( this->_mode ) {
  case AUTOPILOT_MODE_SELECT:
    this->_mode = AUTOPILOT_MODE_EDIT;
    break;
  case AUTOPILOT_MODE_EDIT:
    this->_mode = AUTOPILOT_MODE_SELECT;
    break;
  default:
    break;
  this->show();
  }
}



void AutopilotLCD::show() {
  String sel1;
  String sel2;
  switch( this->_activeField ) {
    case AUTOPILOT_FIELD_HDG:
      sel1 = "< ";
      sel2 = "  ";
      break;
    case AUTOPILOT_FIELD_ALT:
      sel1 = "  ";
      sel2 = "< ";
      break;
    case AUTOPILOT_FIELD_SPD:
      sel1 = " >";
      sel2 = "  ";
      break;
    case AUTOPILOT_FIELD_VS:
      sel1 = "  ";
      sel2 = " >";
      break;
  }
  
  switch ( this->_mode ) {
    case AUTOPILOT_MODE_SELECT:
      this->_lcd->setBlink(0,0);
      this->_lcd->setBlink(1,0);
      break;
    case AUTOPILOT_MODE_EDIT:
      this->_lcd->setBlink(0,384); // 0000000110000000
      this->_lcd->setBlink(1,384); // 0000000110000000
      break;
    default:
      break;
  }

  this->_lcd->setLine(0,String("HDG ")+this->getHdg()+sel1+String("SPD ")+this->getSpd());
  this->_lcd->setLine(1,String("ALT ")+this->getAlt()+sel2+String("VS  ")+this->getVs());
  //this->link2fsSend();
}