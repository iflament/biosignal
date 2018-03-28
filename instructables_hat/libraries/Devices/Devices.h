//
//  Devices.h
//  AwesomeEEG
//
//  Created by Kurt Olsen on 6/30/13.
//
//

#ifndef Devices_h
#define Devices_h

#include <Arduino.h>

 
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// A virtual class, you must implement show()
//
// An LedArray remembers how many led's are in the array
// and allocates memory to store the color for each led.
// It also remembers the 'brightness' as a float in the range
// of 0.0 thru 1.0.
//
// The setColor() methods do NOT apply the brightness value
// they simply store the color in the array. The special effects
// methods WILL apply the brightness however.
//
// When you subclass LedArray an implement the show() method
// you'll have to decide how you want to deal with brightness
// yourself.
//
// The 'needsShowing' flag 
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class LedArray {
public:

  static const int all; // this value (-1) means 'select all leds in the strip'

  LedArray();
  ~LedArray();

  virtual void show(boolean bForceShow = false) {}

  
  void    setup(uint16_t nLeds);

  int     numLeds() {return m_nLeds; }

  void    setBrightness (float brightness)  { m_brightness = constrain(brightness, 0.0, 1.0); m_bNeedsShowing = true; /*Serial.print("pxBrite:"); Serial.println(m_brightness);*/ }
  float   getBrightness ()                  { return m_brightness; }

  // Sets color, of the one, or the many. and applies (master) brightness.
  void    setColor(uint8_t r, uint8_t g, uint8_t b, int n = all);

  // Sets the color, which is NOT adjusted for brightness.
  void    setColor(uint32_t c, int n = all);

  // returns the color for the given pixel# in the hardware string.
  uint32_t getColor(uint16_t n) { return m_pColors[n]; }


  // bling
  void    fxRainbow		(uint8_t delayTimeMs);
  void    fxColorWipe	(uint32_t c, uint8_t delayTimeMs);
  void	  fxColorChase	(uint8_t delayTimeMs);
  void    fxRainbowCycle(uint8_t  delayTimeMs);


protected:

  int
    m_nLeds;

  float
    m_brightness;

  bool
    m_bNeedsShowing; // setColor() alters this and show() pays attention to it.
  
  uint32_t
    *m_pColors;        // Holds LED color values (3 bytes  each), used by show() to see if anything changed.
  
}; // class

#endif /* defined(Devices_h) */




