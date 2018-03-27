
#ifndef Color_h
#define Color_h

#include <Arduino.h>


typedef struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

typedef struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;


class Color {
public:
  
  
  //
  // Creates a color, does not apply brightness.
  // I strongly recommend creating colors at full brightness.
  // Otherwise, at full brightness they might still be dim.
  //
  static uint32_t create( uint8_t r,  uint8_t g,  uint8_t b);
  

  //
  // Individually multiplies R, G & B by the brightness.
  // x1.0 = no change,  x2.0 = twice as bright etc.
  // I only brighten until an r, g or b value would saturate.
  // I stop there, otherwise you'd see the color change.
  // Asking for a brightness change and seeing a color change
  // is too mind boggling to be allowed to continue.
  //
  // I don't bother to scale when going too dim because
  // you can't see the color change very much, because it's dim.
  //
  static uint32_t scaleColor(uint32_t color, float brightness);

  static uint32_t brighter(uint32_t color, int more);



  // returns the result of subtracting each color from 127.
  static uint32_t invertColor( uint32_t color);

   
  //
  // creates a color, does not apply brightness.
  //  returns an old-fashioned color-wheel value.
  //   it cycles from green to green at the extremes.
  //
  static uint32_t   colorWheelValue (byte WheelPos);


  // returns the result of adding 128 to WheelPos, with rollover.
  static uint32_t invertColorWheelValue(byte WheelPos);


  // splits the difference between two components of a color, byte r/g/b values, 0-255
  static uint8_t blendByte(uint8_t c1, uint8_t c2);


  // merge the colors? split the difference kind? See above.
  static uint32_t blendColors(uint32_t color1, uint32_t color2);


  static RgbColor HsvToRgb(HsvColor hsv);
  static HsvColor RgbToHsv(RgbColor rgb);


  
  static const uint32_t BLACK     = 0x000000;
  static const uint32_t BLUE      = 0x0000FF;
  static const uint32_t GREEN     = 0x00FF00;
  static const uint32_t TURQUOISE = 0x00FFFF;
  static const uint32_t RED       = 0xFF0000;
  static const uint32_t PURPLE    = 0xFF00FF;
  static const uint32_t YELLOW    = 0xFFFF00;
  static const uint32_t YELLOWRED = 0xFFC000;
  static const uint32_t WHITE     = 0xFFFFFF;
  static const uint32_t ORANGE    = 0xF62200;

private:
  Color() {}


}; // class


#endif // Color_h
// the end



