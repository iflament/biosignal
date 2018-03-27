/*
  Timer.h - Implements a Timer object 
  Created by Kurt Olsen, August 9, 2012.
*/

#ifndef Timer_h
#define Timer_h

#include "Arduino.h"

class Timer {
public:

  Timer();
  
  // control
  void    start(long interval);
  void    stop();
  
  boolean isStarted();
  boolean isExpired(boolean autoReset = false);

  unsigned long  getInterval() { return m_interval; }
  unsigned long  elapsed();

private:

  unsigned long m_interval;
  unsigned long m_startTime;
  boolean       m_bStarted;

}; // Timer

#endif
// the end.

