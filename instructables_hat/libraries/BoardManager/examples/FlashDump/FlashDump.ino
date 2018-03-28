
#include <EEPROM.h>
#include <Board.h>
#include <BoardManager.h>

void setup() {
  Serial.begin(9600);
  BoardManager::printReport(Serial);
}

void loop() {}
