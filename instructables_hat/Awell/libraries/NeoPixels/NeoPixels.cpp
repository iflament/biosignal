
#include "NeoPixels.h"


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// NeoPixels
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


//
// after this I'm ready, but I'm dark.
//
void NeoPixels::setup(uint16_t nPixels, uint8_t digitalPin, uint8_t pinType /*= NEO_GRB + NEO_KHZ800*/) {
  
  LedArray::setup(nPixels);
  
  m_leds.setup( nPixels, digitalPin, pinType);
  m_leds.begin();

  setColor(Color::BLACK);
  show(true);

  //Serial.print("npix:"); Serial.println(m_leds.numPixels());
  
} // method


//
// you won't see color changes unless you call show(). just sayin. you can force the issue.
//
void NeoPixels::show(boolean bForceShow) {

  if( m_bNeedsShowing || bForceShow ) {
    //
    // and...has it actually changed?
    //
    bool bChanged = 0;
    for(int i = 0; i < (int)m_leds.numPixels(); i++ ) {
      
      uint32_t 
        scaledColor = Color::scaleColor( (uint32_t)m_pColors[i], (float)m_brightness );
    
      if( scaledColor != m_leds.getPixelColor(i) ) {
        m_leds.setPixelColor(i, scaledColor );
        //Serial.print("c:"); Serial.println(m_pColors[i], HEX);
        bChanged |= 1;
      }
    }
    if( bChanged || bForceShow) {
      //Serial.print(".");
      m_leds.show(); // this is expensive, avoid if nobody called setColor(...)
    }
    m_bNeedsShowing = false;
  } else {
    // Uncomment this every now and then to make sure we skip sometimes. Skipping is usually good.
    //Serial.println("show()skip");
  }
  
} // method


/*
uint32_t NeoPixels::create(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

//
// Multiplies each component color by the brightness.
//
uint32_t NeoPixels::scaleColor(uint32_t color, float brightness) {
  
  // break it into the constituant parts.
  float b = (color >>  0) & 0x0000FF;
  float g = (color >>  8) & 0x0000FF;
  float r = (color >> 16) & 0x0000FF;
  
  //
  // stop brighening when r, g, or b hits maximum.
  //  to brigten beyond this results in severe color distortion.
  //   this this happens at high power it's very visible.
  //
  // this next stuff will find the maximum brightness and not go beyond that.
  // 
  float
    rgbMax = 0;
    rgbMax = max( rgbMax, r);
    rgbMax = max( rgbMax, g);
    rgbMax = max( rgbMax, b);
  
  float
    maxBrightness = 255.0f / rgbMax;
  
  float
    colorScaler = min(brightness, maxBrightness);
  
  //  Serial.print(" ms:"); Serial.print(maxScale);
  //  Serial.print(" cs:"); Serial.print(colorScaler);
  //  Serial.print(" br:"); Serial.print(brightness);
  //  Serial.print(", r:"); Serial.print(r);
  //  Serial.print(", g:"); Serial.print(g);
  //  Serial.print(", b:"); Serial.print(b);
  //  Serial.print(", m:"); Serial.println(rgbMax);
  
  //
  // I'll accept uneven flooring at zero though because the low power color changes
  // aren't quite as noticeable. I think it could be smoother but it's plenty good for now.
  //
  r = max(0, (r * colorScaler) );
  g = max(0, (g * colorScaler) );
  b = max(0, (b * colorScaler) );
  
  
  //Serial.print(  "*r:"); Serial.print(r);
  //Serial.print(", *g:"); Serial.print(g);
  //Serial.print(", *b:"); Serial.println(b);
  
  return create(r,g,b);
  
} // method
*/


// the end






