#include <Timer.h>

#define LED_OUTPUT_PIN 13
#define DEFAULT_DELAY 200;

Timer blinkTimer;
boolean ledState = false;
int delayTime = DEFAULT_DELAY;

void setup() {
  
  pinMode(LED_OUTPUT_PIN, OUTPUT);

  blinkTimer.start(delayTime);
}

void loop() {
  
  if( blinkTimer.isExpired(false) ) {
    ledState = !ledState;
    if( !ledState ) { // when off, change the delay
      delayTime -= 15;
      if( delayTime <= 0 ) {
        delayTime = DEFAULT_DELAY;
      }
    }
    digitalWrite( LED_OUTPUT_PIN, ledState);
    blinkTimer.start(delayTime);    
  }
}


