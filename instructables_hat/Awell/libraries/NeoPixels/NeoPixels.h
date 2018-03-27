
#ifndef NeoPixels_h
#define NeoPixels_h

#include <Arduino.h>

#include "../Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#include "../Devices/Devices.h"
#include "../Color/Color.h"


// DO NOT CHANGE max brightness. it is 255 and that's final.
#define MAXBRIGHTNESS 255

// convienent id's for the adafruit neopixels
enum {
  NEOPIXEL_N1,
  NEOPIXEL_N2,
  NEOPIXEL_N3, 
  NEOPIXEL_N4
};



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// Encapsulates an Adafruit_NeoPixel, my way.
// Note that my way is color, then pixel number.
// Adafruit's way is pixel nunber then color.
// I like mine better because I can omit the pixel number.
// When that happens it defaults to 'all'.
//
// Conceptually, Pixels is capable of showing 1 color at a master brightness level.
// Don't thing ink r/g/b mode, instead merge those into concept 'color'.
// The color that is set has a master brightness value that is applied.
// And, the brightness that is applied is tuned to prevent color saturation whiteouts.
// So, you pass around a color (which sort of has an intrinsic brightness)
// which will then be modulated such that when a given r/g/b value would saturate
// then brightness increases are usless and not applied. it's maxed out (color wise).
// To go further would just bring up the other two values in a weird way until it's all white.
// And visually, it is *very* confusing!!!
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class NeoPixels : public LedArray {
public:
  
  NeoPixels() {}

  // Call me only once please, after this I'm ready - and dark.
  void setup(uint16_t nPixels, uint8_t digitalPin, uint8_t pinType = NEO_GRB + NEO_KHZ800);

  // You won't see any setColor(..) changes until you call show(), which commands the hardware.
  void show(boolean bForceShow = false);

private:
  
  // these two methods are already static's in class Color
  // but for reasons unexplainable I MUST define my own copies here
  // or the compiler can't find them... or any statics.
  // Bottom line? This fucker can't access statics???
  //
  // More info, The includes at the top? They also MUST be in your sketch
  // and before you include NeoPixels. This is very strange because the RGBLed class
  // also needs Devices.h and I DONT have to include that in my sketch. Very very strange.
  //
  //
//  uint32_t create( uint8_t r,  uint8_t g,  uint8_t b);
//  uint32_t scaleColor(uint32_t color, float brightness);

  Adafruit_NeoPixel
    m_leds;
  
}; // class


#endif // NeoPixels_h
// the end



