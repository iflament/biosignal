//
//  Devices.cpp
//  AwesomeEEG
//
//  Created by Kurt Olsen on 6/30/13.
//
//

#include "RGBLed.h"


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// This is legacy code that supports my first few shields.
// I often added green, yellow and red leds as diagnostics.
// This code should be eliminated ASAP.
//
// The LEDPINS array indicates which pins per-cpu
//  were used for each color.
//
// LEDPINS[BOARDTYPE][0=green,1=yellow,2=red]
//
//int LEDPINS[5][3] = {
//  g    y   r
//  { 13, 11,  5 },  // Micro
//  {  3,  5,  6 },  // Uno
//  {  3,  5,  6 },  // Mega2560
//  {  3,  5,  6 },  // MegaADK
//  { -1, -1, -1 },  // LilyPad328
//};



int LEDPINS[5][3] = {
//  r   g  b/y
  { 5, 13, 11 },  // Micro
  { 6,  3,  5 },  // Uno
  { 6,  3,  5 },  // Mega2560
  { 6,  3,  5 },  // MegaADK
  {-1, -1, -1 },  // LilyPad328
};





/*
// indexes for the second element of LEDPINS. board type is the first.
enum {
  iPIN_RED,
  iPIN_GREEN,
  iPIN_YELLOW
};
void RGBLed::green (boolean state)    {
  int pin = LEDPINS[m_type][iPIN_GREEN];
  if( pin != -1  ) { analogWrite( pin, state ? 1 : 0 ); }
}
//
void RGBLed::yellow (boolean state)    {
  int pin = LEDPINS[m_type][iPIN_YELLOW];
  if( pin != -1  ) { analogWrite( pin, state ? 1 : 0 ); }
}
//
void RGBLed::red (boolean state)    {
  int pin = LEDPINS[m_type][iPIN_RED];
  if( pin != -1  ) { analogWrite( pin, state ? 1 : 0 ); }
}
*/
// end legacy code





//
// Tri-Color led support
//
// If you attach one, use the LEDPINS array to figure out which pins
// to attach to which color. The only difference is yellow is called blue.
//
//
// enum iRGB supports using an actual tri-color led.
// it just renames yellow as blue, and still allows use
// of the LEDPINS array without too many changes.
//
// if you do attach one tri-color instead of three discrete leds
// then don't use the green(), yellow() and red() methods.
// Use the rgbLed() method instead.
//
// indexes for the second element of LEDPINS. board type is the first.
//
enum iRGB {
  iRGB_RED,
  iRGB_GREEN,
  iRGB_BLUE
};
void RGBLed::rgbRed (uint8_t brightness)    {
  int pin = LEDPINS[m_type][iRGB_RED];
  if( pin != -1  ) { analogWrite( pin, 255-brightness); }
}

void RGBLed::rgbGreen (uint8_t brightness)    {
  int pin = LEDPINS[m_type][iRGB_GREEN];
  if( pin != -1  ) { analogWrite( pin, 255-brightness); }
}
 
void RGBLed::rgbBlue (uint8_t brightness)    {
  int pin = LEDPINS[m_type][iRGB_BLUE];
  if( pin != -1  ) { analogWrite( pin, 255-brightness); }
}
 

// sets all three colors on the tri-color led.
void RGBLed::rgbLed(uint32_t color) {
  rgbRed  ( (color >> 16) & (uint32_t)0x0000FF );
  rgbGreen( (color >>  8) & (uint32_t)0x0000FF );
  rgbBlue ( (color >>  0) & (uint32_t)0x0000FF );
}

void RGBLed::show() {
  rgbLed( getColor() );
}



// the end






