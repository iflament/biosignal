#include <Timer.h>

#define LED_OUTPUT_PIN 13

Timer blinkTimer;
boolean ledState = false;

void setup() {
  
  pinMode(LED_OUTPUT_PIN, OUTPUT);

  blinkTimer.start(250/*ms*/);
}

void loop() {
  
  if( blinkTimer.isExpired(true) ) {
    ledState = !ledState;
    digitalWrite( LED_OUTPUT_PIN, ledState);
  }
  
}
