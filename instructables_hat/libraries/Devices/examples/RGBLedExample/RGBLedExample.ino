
#include <Common.h>
#include <Devices.h>
#include <EEPROM.h>
#include <Board.h>
#include <Timer.h>
#include <Color.h>


RGBLed
  rgb;

//
// Slowly brightens to red, then turns green and dims,
// then turns blue and brightens and finished fully blue

void testBasicColorAndBrightness() {
  
  int dly = 25;
  
  // brighten it
  for(float br = 0; br <= 1.0f; br+=.01 ) {
    rgb.setBrightness(br);
    rgb.setColor(Color::RED);
    rgb.show(true);
    delay(dly);
  }
  // dim it
  for(float br = 1.0f; br >= 0.0f; br-=.01 ) {
    rgb.setBrightness(br);
    rgb.setColor(Color::RED);
    rgb.show(true);
    delay(dly);
  }
  
  
  // brighten it
  for(float br = 0; br <= 1.0f; br+=.01 ) {
    rgb.setBrightness(br);
    rgb.setColor(Color::GREEN);
    rgb.show(true);
    delay(dly);
  }
  // dim it
  for(float br = 1.0f; br >= 0.0f; br-=.01 ) {
    rgb.setBrightness(br);
    rgb.setColor(Color::GREEN);
    rgb.show(true);
    delay(dly);
  }
  
  
  // brighten it
  for(float br = 0; br <= 1.0f; br+=.01 ) {
    rgb.setBrightness(br);
    rgb.setColor(Color::BLUE);
    rgb.show(true);
    delay(dly);
  }
  // dim it
  for(float br = 1.0f; br >= 0.0f; br-=.01 ) {
    rgb.setBrightness(br);
    rgb.setColor(Color::BLUE);
    rgb.show(true);
    delay(dly);
  }

} // method



void testRainbowWithDimming() {

  static float br = 1.0f;
  static boolean bDimming = true;
  
  if( bDimming ) {
    br -= 0.1f;
    if(br <= .2f ) {
      bDimming = false;
    }
  } else { // brightening
    br += 0.1f;
    if( br == 1.0) {
      bDimming = true;
    }
  }
  rgb.setBrightness(br);
  rgb.fxRainbow(3);
}


void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  rgb.setup(Uno);
}




void loop() {
  
  // this changes which test is run.
  static boolean bTestRainbow = false;

  if( bTestRainbow ) {
    testRainbowWithDimming();
  } else {
    testBasicColorAndBrightness();
  }
  
} // method

