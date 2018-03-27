
#include <EEPROM.h>
#include <Board.h>
#include <BoardManager.h>

void setup() {
  Serial.begin(9600);
  BoardManager::eraseEEPROM(); // zap
  Board::pulseD13(3);
}

void loop() {}

