/*
  Timer.cpp - Implements a Timer object 
  Created by Kurt Olsen, August 9, 2012.
*/

#include "Timer.h"

Timer::Timer() {

    m_interval = 0;
    m_startTime = 0;
    m_bStarted = false;
  }

void Timer::start(long interval) {

  m_interval  = interval;
  m_startTime = millis();
  m_bStarted  = true;
}

void Timer::stop() {

  m_interval  = 0;
  m_startTime = 0;
  m_bStarted  = false;
}

boolean Timer::isStarted() {
  return m_bStarted;
}

boolean Timer::isExpired(boolean autoReset) {

  boolean retv = m_bStarted && ((millis() - m_startTime) >= m_interval);
  if( retv && autoReset ) {
    m_startTime = millis();
  }
  return retv;
}

unsigned long Timer::elapsed() {
  if( !m_bStarted ) {
    return 0;
  } else {
    return millis()-m_startTime;
  }
}
