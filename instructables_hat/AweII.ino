
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Illumino Project
// Io Flament, Kurt Olsen, Mael Flament
// @Date 6/14/2014
// @Version beta1
// This code is licensed under Creative Commons Attribution-Non-Commercial-Share Alike 3.0 and GNU GPL license.
//
// The goal of this application is to tap into the serial output 
// of the chip used by the Neurosky 'MindFlex' EEG headband and use
// the data provided to light up Adafruit NeoPixel RGB led devices
// which will be sewn into a hat. The hat will thus indicate
// the mental state (attention or meditation etc.) of the wearer.
//
// The cpu in the hat is the TinyLily-Mini which uses the same cpu/firmware
// as an Arduino Uno, which is what is used in development. The biggest
// difference as far as this app is concerned is that the TinyLily runs 
// at 8mhz instead of 16mhz. They both have 32k Flash, 2k Ram, and 1k of eeprom. 
// Goto tiny-circuits.com for more info.
//
// This app requires libraries or it won't compile. Check the #include's.
// They must be added to your /PathToYourSketchpad/libraries folder.
// Create the libraries directory (see above) and put the libraries in it.
// After that you *must* restart the Arduino IDE so it can find them.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


// Think of these as the 'master' list of #includes and keep Definitions.h synchronized with it.
#include <Arduino.h>
#include <EEPROM.h>
#include <Brain.h>
#include <Adafruit_NeoPixel.h>
#include <Timer.h>
#include <Common.h>
#include <Color.h>
#include <Board.h>
#include <BoardManager.h>
#include <NeoPixels.h>
#include <LowFreqOsc.h>
#include <Devices.h>
#include <LightController.h>
#include <ModeSwitch.h>
//
#include "Definitions.h"


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// The board does all the work.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

HatBoard
  board;


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void setup() {
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  //
  // Here we go...
  //
  const unsigned int    baudRate     = 9600;
  const unsigned char   i2cAddress   = 58;
  const boolean         doHeartbeat  = true;
  const boolean         debugVerbose = false;
  //
  // Has to halt if the eeprom hasn't been two-phase formatted.
  // This is because this app fetches the board type and id
  // from eeprom and uses them to adapt to the hardware environment.
  // The BoardManager library has examples that can do the formatting.
  // Remember to modify the boardType and boardId etc. before running them!
  //
  
  if( !Flash::isFormatted() ) {
    BoardManager::formatPhaseOne(Uno, 1, 1, Serial);
  }
  

  // Please ensure that when you do the two-phase formatting operation that you specify the same
  // thing in both phases. 
  //
  BoardType boardType    = Flash::getBoardType();
  int       boardId      = Flash::getBoardId();
  //
  // At last...
  //
  board.setup( boardType, boardId, baudRate, i2cAddress, doHeartbeat, debugVerbose );
  
  
  // Memory checking - take with a grain of salt. 
  const int memSize = 128;
 
  void* pMem = malloc(memSize);
 
  if( pMem != 0) {
    free(pMem);
    pMem = 0;
    Serial.println();
    Serial.print(CHAR_PLUS); Serial.println(memSize);
  } else {
    Serial.println();
    Serial.print(CHAR_MINUS); Serial.println(memSize);
  }
  */

} // setup


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// MAIN LOOP
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void loop() { 

  board.beginLoop();
  board.loop();
  board.endLoop();
  
  // Maybe do this to get a measure of the loops performance.
  /*if( board.isLoopMod(1000) ) {
    board.printLoopTiming(Serial);
    //board.printReport(Serial);
  }*/
  
} // loop



// the end.


