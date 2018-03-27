//
//  Devices.h
//  AwesomeEEG
//
//  Created by Kurt Olsen on 6/30/13.
//
//

#ifndef RGBLed_h
#define RGBLed_h

#include <Arduino.h>
#include "../Board/Board.h"
#include "../Devices/Devices.h"


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class RGBLed : public Light {
public:

  RGBLed() {}
  virtual ~RGBLed() {}

  void setup(BoardType boardType) { m_type = boardType; }

  void show();

private:

  // These are legacy and should still work with my shields.
  // They are only useful when led's are attached to the correct pins (see the .cpp file).
  // I chose green/yellow/red because yellow is better than blue for indicating problems.
  // I find them useful during development so that I can change colors and see where the code is.
  //
  // These three methods are on/off (but supply legacy PWM values of only 1, which was bright enough.
  //
  //void          green    (boolean state); // if my LEDPIN is zero I do nothing.
  //void          yellow   (boolean state); // if my LEDPIN is zero I do nothing.
  //void          red      (boolean state); // if my LEDPIN is zero I do nothing.

private:

  BoardType
    m_type;

  void  rgbRed   (uint8_t brightness);
  void  rgbGreen (uint8_t brightness);
  void  rgbBlue  (uint8_t brightness);
  void  rgbLed   (uint32_t color);

}; // class


#endif // RGBLed_h




