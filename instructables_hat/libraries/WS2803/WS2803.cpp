
#include "WS2803.h"


  
void WS2803::setup(uint16_t nPixels) {
  
  
  LedArray::setup( min(nPixels, MAXLEDS) );

  SPI.begin();

  setColor(Color::BLACK);
  show(true);

  //Serial.print("npix:"); Serial.println(m_leds.numPixels());
  
}




//
// you won't see color changes unless you call show(). just sayin. you can force the issue.
//
void WS2803::show(boolean bForceShow) {

  if( m_bNeedsShowing || bForceShow ) {
    //Serial.print(".");
    
    for(int i = 0; i < numLeds(); i++ ) {
      
      uint32_t
        color = Color::scaleColor( m_pColors[i], m_brightness );
      
      uint8_t r = ((color >> 16) & 0x000000FF);
      uint8_t g = ((color >>  8) & 0x000000FF);
      uint8_t b = ((color >>  0) & 0x000000FF);
      
      SPI.transfer(r);
      SPI.transfer(g);
      SPI.transfer(b);

      //Serial.print("c["); Serial.print(i); Serial.println("]");
    }
    int nBlack = MAXLEDS - numLeds();
    if( nBlack > 0 ) {
      for(int i = 0; i < nBlack; i++ ) {
      SPI.transfer(0);
      SPI.transfer(0);
      SPI.transfer(0);
      }
    }
    
    if( m_delayMicros != 0) {
      delayMicroseconds(m_delayMicros);
    }
    
    m_bNeedsShowing = false;
    
  } else {
    // Uncomment this every now and then to make sure we skip sometimes. Skipping is usually good.
    //Serial.println("show()skip");
  }
} // method





