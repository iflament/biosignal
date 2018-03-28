
#include "ModeSwitch.h"
#include <Timer.h>

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Detects mode change when user presses the mode
// switch, which is a toggle switch. Note that this
// won't work correctly with momentary pushbuttons.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

ModeSwitch::ModeSwitch()
  : m_pin(0),
    t_modeSwitchDebounce(),
    m_bSwitchState(false),
    m_nModes(1),
    m_curMode(0),
    m_prevMode(0)
{
} // ctor



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//e
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void ModeSwitch::setup(byte pin, byte nModes) {

  m_pin = pin;
  m_nModes = nModes;
  m_curMode = 0;
  m_prevMode = 0;
  
  pinMode(m_pin,  INPUT_PULLUP);
  //
  delay(20); // let the pin settle; just-in-case.
  //
  m_bSwitchState = digitalRead(m_pin);
  //Serial.print("DIO_IN_MODESW: "); Serial.println(DIO_IN_MODESW);

} // method




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

boolean ModeSwitch::checkModeSwitch() {
  
  //Serial.print("mswtitch "); Serial.println(DIO_IN_MODESW);

  if( t_modeSwitchDebounce.isStarted() ) {

    if( !t_modeSwitchDebounce.isExpired(false) ) {
      // we're waiting for debounce, come back later
      return false;

    } else {
      // timer has expired, see if anything changed
      t_modeSwitchDebounce.stop();
      
      boolean swState = digitalRead(m_pin);

      if( swState != m_bSwitchState ) {
        m_bSwitchState = swState;
        //Serial.println("MCHANGE");
        m_prevMode = m_curMode;
        if( ++m_curMode >= m_nModes ) {
          m_curMode = 0;
        }
        return true;
      } 
      else {
        return false;
      }
    } // timer expired
  } 
  else {
    // timer isn't running, not in debounce detect, switch changed?

    if( digitalRead(m_pin) == m_bSwitchState ) {
      // switch hasn't changed, we're done.
      return false;

    } 
    else {
      // switch has changed, start the debouce wait
      t_modeSwitchDebounce.start(30);
      return false;

    } // switch changed?

  } // debouce started? 

} // method



