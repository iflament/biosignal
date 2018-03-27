//
// You must #include <EEPROM.h> in your sketch before #include <Common.h>
//

#include "Common.h"

/*
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



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// converts floats to ascii in the given buffer
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}



void printFloat(char* msg, float value, int precision) {

    int  len = 32;   char buf[len];   for(int i = 0; i < len; i++) {buf[i]=0;}
    Serial.print(msg);
    ftoa(buf, value, precision);
    Serial.print(buf);
}



void printlnFloat(char* msg, float value, int precision) {

    int  len = 32;   char buf[len];   for(int i = 0; i < len; i++) {buf[i]=0;}
    Serial.print(msg);
    ftoa(buf, value, precision);
    Serial.println(buf);
}


int getRandomBipolar(int n) {

  int r = random(n+1);
  //Serial.print("r:"); Serial.print(r);
  
  int mult = (random(2) == 0) ? -1 : 1;
  //Serial.print(", m:"); Serial.print(mult);

  int retv = r * mult;
  //Serial.print("rtv:"); Serial.println(retv);
  
  return retv;
}
*/


// the end.

