
#include "Averager.h"


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Keeps track of an average, of type int.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
Averager::Averager(byte nAverages)
{
  m_size = nAverages;
  m_next = 0;
  m_ready = false;
  p_values = (int*)malloc( m_size * sizeof(int) );
}

Averager::~Averager() {
  if( p_values == 0 )
    return;
  
  free(p_values);
}

void Averager::reset() {
  m_next = 0;
  m_ready = false;
  if( p_values == 0 )
    return;
  for(int i = 0; i < m_size; i++ ) {
    p_values[i] = 0;
  }
}


int Averager::add(int value) {
  if( p_values == 0 )
    return value;
  
  p_values[m_next] = value;
  if( ++m_next == m_size ) {
    m_next = 0;
    m_ready = true;
  }
  return value;
}


boolean Averager::isReady() {
  return m_ready;
}


int Averager::average() {
  if( p_values == 0 )
    return 0;
  
  long accum = 0;
  for(int i = 0; i < m_size; i++ ) {
    accum += p_values[i];
  }
  int avg = accum / m_size;
  return avg;
}


