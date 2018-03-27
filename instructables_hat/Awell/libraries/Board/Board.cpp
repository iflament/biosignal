
#include "Board.h"


//
// occupy sram
//
char STR_Micro[]      = "Micro";
char STR_Uno[]        = "Uno";
char STR_Mega2560[]   = "Mega2560";
char STR_MegaADK[]    = "MegaADK";
char STR_LilyPad328[] = "LilyPad328";
char STR_Unknown[]    = "Board?";
//
char STR_type[]       = "type:";
char STR_id[]         = "  id:";
char STR_i2c[]        = " i2c:";
char STR_loop[]       = "loop:";
char STR_dur[]        = " dur:";
//
char STR_stop[]       = "stop";
char STR_error[]      = "err";
//
// end occupy sram.


#define CHAR_COLON     (char)58
#define CHAR_SPACE     (char)32

 
//
// Serial and i2c and other? background tasks can conflict.
// it's best not to do some things when hardware Serial in taking place. weird.
//
boolean Board::m_bSerialOpen = false;
boolean Board::m_bSerialBusy = false;
boolean Board::m_bStopped    = false;



//
// construction, harmless, doesnt do anything to the hardware
//
Board::Board()
: m_type((BoardType)-1),
  m_id(-1),
  m_i2c(-1),
  m_nloops(0),
  m_loopStartMicros(0),
  m_loopEndMicros(0),
  m_doHeartbeat(false),
  m_loopZeroTime(0)
{

} // ctor



//
// destruction, closes the serial port if I opened it.
//
Board::~Board() {
  if( isSerialOpen() ) {
    endSerial();
  }
} // dtor



//
// very important. get it right.
//
void Board::setup(
  BoardType     type,
  int           boardId,
  unsigned int  baudRate,
  unsigned char i2cAddr,
  boolean       doHeartbeat,
  boolean       debugVerbose)
{
  m_type = type;
  m_id = boardId;
  m_i2c = i2cAddr;
  m_nloops = 0;
  m_loopStartMicros = 0;
  m_loopEndMicros = 0;
  m_doHeartbeat = doHeartbeat;
  m_bStopped = false;
  
  beginSerial(baudRate);

  // been here, done that?
  if( !Flash::isUserMode(USERMODE_EEPROM_READY) ) {
    eepromPreload();
    Flash::maybeSetUserModeBit( USERMODE_EEPROM_READY, true );
  }

  if( m_doHeartbeat ) {
    pinMode(13, OUTPUT); // heartbeat led
  }

  // Enable or disable production debug messages.
  Flash::maybeSetDebugModeBit( DEBUG_VERBOSE, debugVerbose );

  // there is no need to wake up the led pins
  //  because analogWrite is sufficient to light the leds.
  
} // method


//
// diagnostic
//
void Board::printReport(Stream& os) {
  //os.println();
  for(int i=0; i<10; i++) { os.print("-"); } os.println();
  
  os.print(STR_type);   os.println( nameOfType(board()) );
  os.print(STR_id);     os.println( id()   );
  os.print(STR_i2c);    os.println( i2c() );
  os.print(STR_loop);   os.println( getLoopCount() );
  os.print(STR_dur);    os.println( getLoopElapsedMicros() );
  
  for(int i=0; i<10; i++) { os.print("-"); } os.println();
}


//
// diagnostic
//
void Board::printLoopTiming(Stream& os) {
  
  char buf[96];
  
  unsigned long ms  = millis();
  unsigned long nLoops  = getLoopCount();
  unsigned long idle = getLoopStartMicros() - getLoopEndMicros();
  unsigned long elapsedUs = getLoopElapsedMicros();
  unsigned long elapsedMs = elapsedUs / 1000;
  
  sprintf(buf, "%9lu  loops,  %9lu  millis, %4lu  idle (uS), %6lu elapsed (uS), %6lu elapsed (mS)", nLoops, ms, idle, elapsedUs, elapsedMs);
  Serial.println(buf);
  
} // method



//
// returns a human readable name for my BoardType
//
char* Board::nameOfType(BoardType type) {
  switch(type) {
    case Micro:       return STR_Micro;
    case Uno:         return STR_Uno;
    case Mega2560:    return STR_Mega2560;
    case MegaADK:     return STR_MegaADK;
    case LilyPad328:  return STR_LilyPad328;
    default:          return STR_Unknown;
  }
}



//
// initialize the Serial device, only once please.
//
void  Board::beginSerial(unsigned int baudRate) {
  if(!m_bSerialOpen && (baudRate != 0)) {
    Serial.begin(baudRate);
    //Serial.println();
    //Serial.flush();
    m_bSerialOpen = true;
    m_bSerialBusy = false;
  }
}

//
// Close the Serial device and remember it's closed.
//
void Board::endSerial() {
  if( m_bSerialOpen ) {
    Serial.end();
    m_bSerialOpen = false;
  }
}


//
// called from beginLoop() and endLoop()
// and is used to indicate 'aliveness' when enabled.
//
void Board::heartbeat(boolean state)    {
  if( m_doHeartbeat ) {
    digitalWrite(13, state);
  }
}





void Board::stop(Stream& os) {
  m_bStopped = true;
  os.println(STR_stop);
  os.flush();
  while(true) {}
}

void Board::stop(char* msgReason, Stream& os) {
  m_bStopped = true;
  os.print(STR_stop);
  os.print(CHAR_COLON);
  os.print(CHAR_SPACE);
  os.print(msgReason);
  os.flush();
  while(true) {}
}



boolean Board::isStopped() {
  return m_bStopped;
}


void Board::errmsg(int errCode, Stream& os) {
  os.print(STR_error);
  os.print(CHAR_COLON);
  os.print(CHAR_SPACE);
  os.println(errCode);
  os.flush();
}


void Board::errmsg(char* errMsg, Stream& os) {
  os.print(STR_error);
  os.print(CHAR_COLON);
  os.print(CHAR_SPACE);
  os.println(errMsg);
  os.flush();
}

void Board::errstop(int errCode,  Stream& os) {
  errmsg(errCode, os);
  stop(os);
}
void Board::errstop(char* errMsg, Stream& os) {
  errmsg(errMsg, os);
  stop(os);
}



void Board::stopIfNotFormatted(unsigned int baudRate) {
  if( !Flash::isFormatted() ) {
    if( baudRate != 0 ) {
      Serial.begin(baudRate);
      errmsg(ERR_EEPROM_FORMAT, Serial);
    }
    stop(Serial);
  }
}



void Board::debug(char* msg, Stream& os) {
  if( Flash::isDebugMode(DEBUG_VERBOSE) ) {
    os.print(msg);
  }
}

void Board::debug(char c, Stream& os) {
  if( Flash::isDebugMode(DEBUG_VERBOSE) ) {
    os.print(c);
  }
}

void Board::debugln(char* msg, Stream& os) {
  if( Flash::isDebugMode(DEBUG_VERBOSE) ) {
    os.println(msg);
  }
}

void Board::debugln(String msg, Stream& os) {
  if( Flash::isDebugMode(DEBUG_VERBOSE) ) {
    os.println(msg);
  }
}

void Board::debugln(char* msg, int value, Stream& os) {
  if( Flash::isDebugMode(DEBUG_VERBOSE) ) {
    os.print(msg);
    os.println(value);
  }
}

void Board::pulseD13(int nPulses) {
  pinMode(13, OUTPUT);
  for(int i = 0; i < nPulses * 2; i++ ) {
    digitalWrite(13, i % 2);
    delay(250);
  }
	digitalWrite(13, LOW);
}
