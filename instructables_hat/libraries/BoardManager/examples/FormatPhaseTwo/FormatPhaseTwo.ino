
#include <EEPROM.h>
#include <Board.h>
#include <BoardManager.h>

//  Micro,
//  Uno,
//  Mega2560,
//  MegaADK,
//  LilyPad328,

const BoardType boardType     = LilyPad328;  // Uno,   LilyPad328
const int       boardId       = 1;    // 1=Uno, 2=Lily
const int       eepromVersion = 1;

void setup() {
  Serial.begin(9600);
  BoardManager::formatPhaseTwo(boardType, boardId, eepromVersion, Serial);
  
  // allow the developer to see this occured.
  Board::pulseD13(3);
}

void loop() {}

