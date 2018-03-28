
#include "Color.h"

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Color
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


//
// returns a color, does not adjust for brightness. it's raw.
//
uint32_t Color::create(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}


uint32_t Color::scaleColor(uint32_t color, float brightness) {
  
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

  return Color::create(r,g,b);

} // method


uint32_t Color::brighter(uint32_t color, int more) {
  
  // break it into the constituant parts.
  int b = (color >>  0) & 0x0000FF;
  int g = (color >>  8) & 0x0000FF;
  int r = (color >> 16) & 0x0000FF;

  b += more; if(b>255) b=255;
  g += more; if(g>255) g=255;
  r += more; if(r>255) r=255;
  
  return Color::create(r,g,b);

} // method



//
//
//
uint32_t Color::invertColor( uint32_t color) {

    // now I calculate the color inversion
    byte r  = (color >> 16) & 0x000000FF;
    byte g  = (color >>  8) & 0x000000FF;
    byte b  =  color        & 0x000000FF;
    
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
    
    uint32_t
      color2 = Color::create( r, g, b );

    return color2;
  
}// method




//
// does not do brightness adjustments.
//
uint32_t Color::colorWheelValue(byte WheelPos) {
  
  uint32_t retv = 0;
  
  
  if(WheelPos < 85) {
    retv = Color::create(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    retv = Color::create(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    retv = Color::create(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  
  return retv;
  
} // method



//
// this performs 'color inversion' by using the color wheel
//
uint32_t Color::invertColorWheelValue( byte wheelPos ) {
  int i2Color = ((int)(wheelPos + 128)) & 0x00FF;
  return Color::colorWheelValue(i2Color);
}




//
// yuk. and split the diff
//
uint8_t Color::blendByte(uint8_t c1, uint8_t c2) {

  uint8_t
    retv = 0; // a color component, red green or blue, range 0-255
  
  uint8_t
    dR = abs( (int)c1 - (int)c2 );
  
  if( c1 == c2 ) {
    retv = c1;
    //Serial.print("dc=:"); Serial.println(retv);
  } else if (c1 < c2 ) { // add half the delta
    retv = constrain( c1 + (dR / 2), 0, 255);
    //Serial.print("dc<:"); Serial.println(retv);
  } else if (c1 > c2 ) { // subtract half the delta
    retv = constrain( c1 - (dR / 2), 0, 255);
    //Serial.print("dc>:"); Serial.println(retv);
  } // if
  
  return retv;
}


//
// simply do the math, brightness is *not* applied, the colors should be at max bright for best results?
//
uint32_t Color::blendColors(uint32_t color1, uint32_t color2 ) {
  
  uint8_t r1 = ((color1 >> 16) & 0x000000FF);
  uint8_t g1 = ((color1 >>  8) & 0x000000FF);
  uint8_t b1 = ((color1 >>  0) & 0x000000FF);
  
  uint8_t r2 = ((color2 >> 16) & 0x000000FF);
  uint8_t g2 = ((color2 >>  8) & 0x000000FF);
  uint8_t b2 = ((color2 >>  0) & 0x000000FF);
  
  // my results..
  uint8_t red = blendByte(r1, r2);
  uint8_t grn = blendByte(g1, g2);
  uint8_t blu = blendByte(b1, b2);

//  Serial.print("rgb");
//  Serial.print(red); Serial.print("/");
//  Serial.print(grn); Serial.print("/");
//  Serial.println(blu);
  

  uint32_t retv = Color::create(red, grn, blu);
  //Serial.print("mc:"); Serial.println(retv, HEX);
  return retv;

} // method






RgbColor Color::HsvToRgb(HsvColor hsv)
{
    RgbColor rgb;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6; 

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            rgb.r = hsv.v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = hsv.v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = hsv.v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = hsv.v;
            break;
        default:
            rgb.r = hsv.v; rgb.g = p; rgb.b = q;
            break;
    }

    return rgb;
}

HsvColor Color::RgbToHsv(RgbColor rgb)
{
    HsvColor hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}


// the end






