//
//  Devices.cpp
//  AwesomeEEG
//
//  Created by Kurt Olsen on 6/30/13.
//
//

#include "Devices.h"
#include "../Color/Color.h"



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// LedArray
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

const int LedArray::all = -1;

LedArray::LedArray()
: m_nLeds(0),
  m_brightness(1.0f),
  m_pColors(0)
{
}

LedArray::~LedArray() {
  if( m_pColors != 0 ) {
    free(m_pColors);
  }
}

void LedArray::setup(uint16_t nLeds) {
  
  m_nLeds = nLeds;
  m_brightness = 1.0f,
  m_pColors = 0;

  int numBytes = nLeds * sizeof(uint32_t);
	
  if((m_pColors = (uint32_t *)malloc(numBytes))) {
    memset(m_pColors, 0, numBytes);
  } else {
    //Serial.println("LASMALLOC"); Serial.flush();
		//while(true) {}
  }

} // method



// sets color, of the one, or the many. and applies (master) brightness.
void LedArray::setColor(uint8_t r, uint8_t g, uint8_t b, int n) {
  setColor( Color::scaleColor( Color::create(r,g,b), m_brightness ), n );
}



// Sets the color, which is NOT adjusted for brightness.
void LedArray::setColor(uint32_t c, int n) {
  //Serial.println("SC2");
  
  // I shouldn't be adjusting for brightness here yet.
  uint32_t color = c; //Color::scaleColor( c, m_brightness );

  if( n == all) {
    for(int i=0; i < m_nLeds; i++) {
      m_pColors[i] = color;
    }
  } else {
    if( n < m_nLeds ) {
      m_pColors[n] = color;
    }
  }
  
  m_bNeedsShowing = true;

} // method



void LedArray::fxRainbow(uint8_t delayTimeMs) {

  uint16_t i, j;
  
  for(j=0; j<256; j++) {
    for(i=0; i < m_nLeds; i++) {
      uint32_t adjustedColor = Color::scaleColor( Color::colorWheelValue((i+j) & 255), m_brightness );
      setColor(adjustedColor, i);
    }
    show();
    delay(delayTimeMs);
  }
}

void LedArray::fxColorWipe(uint32_t c, uint8_t delayTimeMs) {
  for(uint16_t i=0; i < m_nLeds; i++) {
    uint32_t adjustedColor = Color::scaleColor( c, m_brightness );
    setColor(adjustedColor, i);
    show();
    delay(delayTimeMs);
  }
}


void LedArray::fxColorChase(uint8_t delayTimeMs) {
	
//	setColor(0);
//	int colorInc = 256 / m_nLeds;
//	for(uint16_t i=0; i < m_nLeds; i++) {
//		uint32_t adjustedColor = Color::scaleColor( Color::colorWheelValue(i*colorInc), 1.0f /* m_brightness */ );
//		setColor(adjustedColor, i);
//		if( i > 0 ) {
//			setColor(0, i-1);
//		}
//		show();
//		delay(delayTimeMs);
//	}
	
	setColor(0);
	int colorInc = 256 / m_nLeds;
	for(uint16_t i=0; i < m_nLeds/2; i++) {
		uint32_t adjustedColor = Color::scaleColor( Color::colorWheelValue(i*colorInc), 1.0f /* m_brightness */ );
		setColor(adjustedColor, i);
		setColor(adjustedColor, i + m_nLeds/2);
		if( i > 0 ) {
			setColor(0, i-1);
			setColor(0, (i+m_nLeds/2)-1);
		}
		show();
		delay(delayTimeMs);
	}
	
} // method



void LedArray::fxRainbowCycle(uint8_t delayTimeMs) {

  uint16_t i, j;
  
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i < m_nLeds; i++) {
      uint32_t adjustedColor = Color::scaleColor( Color::colorWheelValue(((i * 256 / m_nLeds) + j) & 255), m_brightness );
      setColor(adjustedColor, i);
    }
    show();
    delay(delayTimeMs);
  }
}


// the end






