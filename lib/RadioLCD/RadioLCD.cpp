#include <Arduino.h>
#include "RadioLCD.h"

/*
 * MHz: 118-136 +/-1
 * KHz: +/- 50 or +/- 25
 * +0--3---4---B---F+   
 * |125.125  125.125|
 * |COM1 TR    Ch:32| Ch/Pg Pg - programming mode
 * +0--3---4---B---F+
 *
 * +0--3---4---B---F+   
 * |125.125  ---    | - not programmed, or select dashes to skip chanel
 * |COM1 TR    Pg: 5|
 * +0--3---4---B---F+
 *
 * <--> (COMM TRANSFER) for 2 seconds to switch to ACTIVE ENTRY (direc tune) mode.
 * 
 * A non-volatile memory stores the comm ACTIVE and STANDBY frequencies on 
 * power down. When the unit is turned on again, the COMM ACTIVE and STANDBY
 * windows will display the same ACTIVE and STANDBY frequencies that were
 * displayed before power down.
 * 
 * 2 sec ch - enter Pg
 * encoder to select Ch for Pg
 * <--> - to start tune SB
 * <--> - to switch back to Ch for Pg selection
 * Ch button to exit Pg mode or after 20 sec of inactivity
 * 
 * Short Ch push for Ch selection. Return to SB after 5 sec of incativity.
 * Select Ch, then <--> to move it to active
 * 
 * If manually entered SB match Ch, then display Ch number. --- no, it's mixing
 * 
 */

void RadioLCD::init( NeboLCD *lcd ) {
  this->_lcd = lcd;
  this->_frM = 118;
  this->_frK = 0;
  this->_sbM = 136;
  this->_sbK = 950;
  this->_frMmin = 118;
  this->_frMmax = 136;
  this->_radioMode = NEBO_RADIO_MODE_MHZ;
  this->_radioState = NEBO_RADIO_STATE_SB;
  this->_name = "";
  this->_lcd->setBlink(0,0);
  this->_lcd->setBlink(1,0);
  this->_lastCommand = "";
}

void RadioLCD::setName( String name ) {
  this->_name = name;
  if ( name.startsWith("COM") ) {
    this->_frM = 118;
    this->_sbM = 136;
    this->_frMmin = 118;
    this->_frMmax = 136;
    return;
  }
  if ( name.startsWith("NAV") ) {
    this->_frM = 108;
    this->_sbM = 117;
    this->_frMmin = 108;
    this->_frMmax = 117;
    return;
  }
  return;
}

String RadioLCD::_link2fsCmdPrfix() {
  if ( this->_name == "COM1" ) {
    return (String) "A05";
  }
  if ( this->_name == "COM2" ) {
    return (String) "A11";
  }
  if ( this->_name == "NAV1" ) {
    return (String) "A17";
  }
  if ( this->_name == "NAV2" ) {
    return (String) "A23";
  }
  return "ERROR_UNKNOWN_RADIO_NAME";
}

String RadioLCD::_textFrForLink2fs() {
  return String((uint32_t)(this->_frM-100)*(uint32_t)100+this->_frK/10);
}

void RadioLCD::link2fsSend() {
  //String command = this->_link2fsCmdPrfix();
  String command = this->_link2fsCmdPrfix()+this->_textFrForLink2fs();
  if ( command != this->_lastCommand ) {
    this->_lastCommand = command;
    Serial.println(command);
  }
}

void RadioLCD::swKM() {
  if( this->_radioMode == NEBO_RADIO_MODE_MHZ ) {
    this->_radioMode = NEBO_RADIO_MODE_KHZ;
  } else {
    this->_radioMode = NEBO_RADIO_MODE_MHZ;
  }
  this->show();
}

void RadioLCD::switchKhzStep() {
  if( this->_khzStep == 50 ) {
    this->_khzStep = 25;
  } else {
    this->_khzStep = 50;
  }
}

void RadioLCD::incrSbK() {
  this->_sbK += this->_khzStep;
  this->_sbK = this->_sbK / this->_khzStep * this->_khzStep;
  if ( this->_sbK >= 1000 ) {
    //this->incrSbM();
    this->_sbK = 0;
    this->incrSbM();
  }
  this->show();
}

void RadioLCD::decrSbK() {
  this->_sbK -= this->_khzStep;
  this->_sbK = this->_sbK / this->_khzStep * this->_khzStep;
  if ( this->_sbK < 0 ) {
    //this->decrSbM();
    this->_sbK = 1000-this->_khzStep;
    this->decrSbM();
  }
  this->show();
}

void RadioLCD::incrSbM() {
  this->_sbM += 1;
  if ( this->_sbM > this->_frMmax ) { this->_sbM = this->_frMmin; }
  this->show();
}

void RadioLCD::decrSbM() {
  this->_sbM -= 1;
  if ( this->_sbM < this->_frMmin ) { this->_sbM = this->_frMmax; }
  this->show();
}

void RadioLCD::incr() {
  if ( this->_radioState == NEBO_RADIO_STATE_SB ) {
    if ( this->_radioMode == NEBO_RADIO_MODE_MHZ ) {
      this->incrSbM();
    }
    if ( this->_radioMode == NEBO_RADIO_MODE_KHZ ) {
      this->incrSbK();
    }
  }
}

void RadioLCD::decr() {
  if ( this->_radioState == NEBO_RADIO_STATE_SB ) {
    if ( this->_radioMode == NEBO_RADIO_MODE_MHZ ) {
      this->decrSbM();
    }
    if ( this->_radioMode == NEBO_RADIO_MODE_KHZ ) {
      this->decrSbK();
    }
  }
}

void RadioLCD::swapFrAndSb(){
  unsigned int tmpM = this->_frM;
  unsigned int tmpK = this->_frK;
  this->_frM = this->_sbM;
  this->_frK = this->_sbK;
  this->_sbM = tmpM;
  this->_sbK = tmpK;
  this->show();
}

/*String RadioLCD::getFr( char delim = '.' ) {
  char fr[7], tmp[3];
  dtostrf(this->_frM, 3, 0, fr);
  strcat(fr,&delim);
  dtostrf(this->_frK, 3, 0, tmp);
  Serial.println(tmp);
  strcat(fr,tmp);
  Serial.println(fr);
  return (String(fr));
}*/

String RadioLCD::getFr( char delim = '.' ) {
  char sFrM[4], sFrK[4];
  sprintf(sFrM, "%03d", this->_frM);
  sprintf(sFrK, "%03d", this->_frK);
  return String(sFrM) + String(delim) + String(sFrK);
}

String RadioLCD::getSb( char delim = '.' ) {
  char sSbM[4], sSbK[4];
  sprintf(sSbM, "%03d", this->_sbM);
  sprintf(sSbK, "%03d", this->_sbK);
  return String(sSbM) + String(delim) + String(sSbK);
}

void RadioLCD::show() {
  if ( this->_radioMode == NEBO_RADIO_MODE_MHZ ) {
    this->blinkSbM();
  }
  if ( this->_radioMode == NEBO_RADIO_MODE_KHZ ) {
    this->blinkSbK();
  }
  this->_lcd->setLine(0,this->getFr()+String("  ")+this->getSb());
  this->_lcd->setLine(1,this->_name);
  this->link2fsSend();
}

void RadioLCD::blinkSbK() {
  //this->_lcd->blinkMask[0] = 7; // 0000000000000111
  this->_lcd->setBlink(0,7); // 0000000000000111
}

void RadioLCD::blinkSbM() {
  //this->_lcd->blinkMask[0] = 112; // 0000000001110000
  this->_lcd->setBlink(0,112); // 0000000001110000
}
