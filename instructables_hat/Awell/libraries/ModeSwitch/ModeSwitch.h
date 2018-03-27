
#ifndef ModeSwitch_h
#define ModeSwitch_h

#include <Arduino.h>
#include "../Timer/Timer.h"


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// Encloses mode switch crud.
//  setup() will initialize it's pin as an input.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
class ModeSwitch {
public:

  ModeSwitch();

  void      setup(byte pin, byte nModes);

  // when true is returned then curMode is saved in prevMode before incrementing it w/wraparound).
  boolean   checkModeSwitch();

  byte      getCurMode()  { return m_curMode;  }
  byte      getPrevMode() { return m_prevMode; }

  void      forceMode(int mode)	  { m_prevMode = m_curMode; m_curMode = mode; if( m_curMode >= m_nModes ) { m_curMode = 0; } }
  
private:
  
  byte
    m_pin;

  Timer
    t_modeSwitchDebounce;

  boolean 
    m_bSwitchState;
  
  byte
    m_nModes;
  
  byte
    m_curMode;
  
  byte
    m_prevMode;

}; // class

#endif

