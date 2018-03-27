
#ifndef WS2803_h
#define WS2803_h

#include <Arduino.h>
#include <Color.h>
#include <Devices.h>
#include <SPI.h>

class WS2803 : public LedArray {
public:

  WS2803() : MAXLEDS(6), m_delayMicros(400) {}
  
  void setup(uint16_t nPixels);

  void show(boolean bForceShow = false);

protected:

  const int MAXLEDS;

  // The data sheet says wait 600us after the spi transfers.
  // Since other things in the arduino loop take time I've
  // found that using 400us is working reliably. Adjust as needed.
  uint16_t
    m_delayMicros;
  
}; // class

#endif // WS2803_h
// the end.

