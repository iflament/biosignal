
#ifndef Board_H
#define Board_H

#include <Arduino.h>
#include <avr/eeprom.h>


enum BoardType {
  Micro,
  Uno,
  Mega2560,
  MegaADK,
  LilyPad328,
};


enum BoardError {
  ERR_EEPROM_FORMAT,           // indicates the eeprom is not formatted.
  ERR_FLASH,                   // EEPROM formatting didn't work.
  ERR_MALLOC,                  // Indicates a malloc didn't work.
  ERR_INVALID_SWITCHSTATE,     // unused - indicates switch detector state machine is confused.
  ERR_USER_ERRORA,             // available for use by an app.
  ERR_USER_ERRORB,             // available for use by an app.
  ERR_USER_ERRORC,             // available for use by an app.
  ERR_USER_ERRORD,             // available for use by an app.
};


//
// I could have hidden these but I want you to see them, they consume ram.
//
extern  char STR_Micro[];
extern  char STR_Uno[];
extern  char STR_Mega2560[];
extern  char STR_MegaADK[];
extern  char STR_LilyPad328[];
extern  char STR_Unknown[];

extern  char STR_type[];
extern  char STR_id[];
extern  char STR_i2c[];
extern  char STR_loop[];
extern  char STR_dur[];

extern char STR_stop[];
extern char STR_error[];



class Board {

public:

  Board(); // if you alter this you will regret it.
  ~Board();
  
  
  // Initialize me carefully, and only once please.
  //  Also note that setup() will call virtual eepromPreload() on subclasses.
  //
  virtual void setup(
    BoardType     type,
    int           boardId,
    unsigned  int baudRate,
    unsigned char i2cAddr,
    boolean       doHeartbeat,
    boolean       debugVerbose
  );

  //
  // Who am I?, and in case it's needed - my i2c address.
  //
  BoardType       board()     { return m_type; } // used to adapt to different kinds of arduinos.
  unsigned int    id()        { return m_id;   } // useful for specifying my role in a system.
  unsigned char   i2c()       { return m_i2c;  } // an i2c address, useful elsewhere as well.
  
  // Overload this in your subclasses...
  //
  // If the state of USERMODE_EEPROM_READY is false then this method is called
  // on your subclass and it needs to preload any flash variables used by your app.
  // You should return true, in any case the return value is stored in USERMODE_EEPROM_READY
  // which should prevent this from being called more than once after the two-phase format is done.
  //
  virtual void    eepromPreload() {}


  //
  // returns true if the loop count is modulo N
  //
  boolean         isLoopMod(long n)       { return (m_nloops % n) == 0; }

  
  //
  // If true then setup was told to use heartbeats on dig 13.
  // Used to know if it's ok to add extra output for 'heartbeat telemetry' purposes.
  //
  boolean         isHeartbeatEnabled()    { return m_doHeartbeat; }
  
  
  //
  // You should call these first and last in the arduinos main loop().
  //
  // begin - logs the starting time in microseconds, and maybe sets heartbeat(true).
  // end   - clears serial 'busy', maybe sets heartbeat(false), and logs the ending time in microseconds.
  //
  void            beginLoop()             { m_loopStartMicros = micros(); heartbeat(true); if(m_nloops == 0) {m_loopZeroTime=millis();}}
  void            endLoop()               { m_nloops++;  m_bSerialBusy = false; heartbeat(false); m_loopEndMicros = micros(); }


  //
  // These provide nice timing information, almost free.
  //
  unsigned long   getLoopCount()          { return m_nloops; }
  unsigned long   getLoopStartMicros()    { return m_loopStartMicros; }
  unsigned long   getLoopEndMicros()      { return m_loopEndMicros; }
  unsigned long   getLoopElapsedMicros()  { return m_loopEndMicros - m_loopStartMicros; }
  
  //
  // new
  //
  unsigned long   getLoopZeroTime()       { return m_loopZeroTime; }
  unsigned long   loopMillis()            { return millis() - m_loopZeroTime; }
  
  //
  // Useful diagnostics
  //
  void            printReport(Stream& os);
  void            printLoopTiming(Stream& os);
  //
  // This is how you report the actual time the loop took
  //  without also measuring the serial port printing time.
  //
  // unsigned long elapsed = arduino.getLoopElapsedMicros();
  // unsigned long emillis = elapsed / 1000;
  // Serial.print("tL (uS): "); Serial.println( elapsed );
  // Serial.print("tL (mS): "); Serial.println( emillis);
  // Serial.println();
  //
  
  
  // these are harmless and useful
  int readA0() { return analogRead(0); } 
  int readA1() { return analogRead(1); } 
  int readA2() { return analogRead(2); } 


  //
  // the human readable name of my BoardType.
  //
  static char*    nameOfType(BoardType type);

  
  //
  // serial port management, because Serial and i2c can fight each other.
  //
  // eg. The handler you write for processing incoming i2c messages
  //     is called by an interrupt routine. This means your msg handler
  //     just interrupted the arduino main loop() which might be using
  //     the Serial port when interrupted. What is needed is a way for
  //     the main loop() to indicate when it is using the Serial port.
  //     if you always called SetSerialBusy(state) in the main loop
  //     then your i2c message handler can use it for diagnostic messages.
  //     If you don't do this and both the i2c and main loop are using it
  //     then your situation gets pretty ugly in a hurry.
  //

  // does a 'smart' open/close on the Serial port.
  static void     beginSerial(unsigned int baudRate);
  static void     endSerial();
  
  
  // used to share the serial port beteen the main loop() and an i2c rcv handler.
  static void     setSerialBusy(boolean state) { m_bSerialBusy = state; }
  static boolean  isSerialOpen() { return m_bSerialOpen; }
  static boolean  isSerialBusy() { return m_bSerialBusy && m_bSerialOpen; }

   
  //
  // Sometimes you just can't run or bad things happen.
  //
  // Used to stop the main loop() in the event of problems.
  // Warning: This only stops the main loop - interrupts and i2c callbacks etc. might still occur.
  // To deal with that situation use the isStopped() method in your application.
  //
  // These never return, they enter endless loops. They try to print a message first though.
  static void          stop              (Stream& os);
  static void          stop              (char* msgReason, Stream& os);

  // Useful in interrupt routines to see if the main loop is halted.
  static boolean       isStopped         ();

  // Prints a numeric error in a standard fashion.
  static void          errmsg            (int errCode,  Stream& os);
  static void          errstop           (int errCode,  Stream& os);

  static void          errmsg            (char* errMsg, Stream& os);
  static void          errstop           (char* errMsg, Stream& os);
  
  // If the flash isn't formatted then halt by entering an endless loop. Doesn't return obviously.
  // If the baudRate is non-zero then initialize Serial and print/flush a message first.
  //
  static void          stopIfNotFormatted(unsigned int baudRate);

  // *** These are used to embed debug messages in production code. Use sparingly. ***
  //     They only print to the Stream if DEBUG_VERBOSE is flashed true.
  //
  //     (Use another mechanism for real-time diagnostics during development.)
  //
  static void          debug         (char* msg,            Stream& os);
  static void          debug         (char c,               Stream& os);
  static void          debugln       (char* msg,            Stream& os);
  static void          debugln       (String msg,           Stream& os);
  static void          debugln       (char* msg, int value, Stream& os);

  static void pulseD13(int nPulses=3);

protected:

  // If enabled this sets digital output 13 to the given state.
  // Its called by beginLoop() and endLoop(), and works if enabled.
  void heartbeat(boolean state);

private: // go away
  
  BoardType       m_type;
  unsigned int    m_id;
  unsigned char   m_i2c;
  unsigned long   m_nloops;
  unsigned long   m_loopStartMicros;
  unsigned long   m_loopEndMicros;
  boolean         m_doHeartbeat;
  unsigned long   m_loopZeroTime;
   
  static boolean  m_bStopped;
  static boolean  m_bSerialOpen;
  static boolean  m_bSerialBusy;
  
}; // class



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// EEPROM variables, are inherently static.
// (Yes, I call it eeprom here, because it really is)
//
// I have decided to treat the EEPROM as words not bytes.
// This uses up extra space but makes life a lot easier
// because the values I need to save and recall are
// usually 2 bytes (a word).
//
// Thus, the index given to get/setFlashWord must
// be multiplied by 2 to obtain the actual EEPROM address.
// Each word is stored in little-endian format.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

enum {
   EEPROM_MAGIC_LOW,   //  0 contains half the magic pattern if formatted
   EEPROM_MAGIC_HIGH,  //  1 contains half the magic pattern if formatted
   EEPROM_VERSION,     //  2 contains the eeprom format version number
   EEPROM_BOARDTYPE,   //  3 contains one of the BoardType values.
   EEPROM_BOARDID,     //  4 a user-defined value for use by the application
   EEPROM_DEBUG_MODE,  //  5 each bit corresponds to different diagnostics
   EEPROM_SYSTEM_MODE, //  6 the system mode bits
   EEPROM_USERMODE,    //  7 the applications modes bits if desired.
   EEPROM_USERAREA,    //  8 the first unassigned area, contiguous to the end.
}; // enum
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// These are bit definitions for EEPROM_DEBUG_MODE
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
enum {
   // the enumerations are bit indices - NOT bit masks!
   DEBUG_SERIAL,
   DEBUG_VERBOSE,
   DEBUG_BROADCAST,
   DEBUG_TELEMETRY,
   DEBUG_UNUSED4,
   DEBUG_UNUSED5,
   DEBUG_UNUSED6,
   DEBUG_UNUSED7
};

enum {
  // the enumerations are bit indices - NOT bit masks!
  USERMODE_EEPROM_READY,
};

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Note that Flash memory is inherently static.
// And yes, it's actualy eeprom but Flash is
// easier to type. I already tried eeprom. yuk.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class Flash {
public:
   
   static boolean isFormatted() {
      return
         eeprom_read_byte((unsigned char *) 0) == 'a'
      && eeprom_read_byte((unsigned char *) 1) == 'b'
      && eeprom_read_byte((unsigned char *) 2) == 'c'
      && eeprom_read_byte((unsigned char *) 3) == 'd';
   }
   
   
   // should probably be named 'fetch' but the i emphasizes we're index, not byte based.
   static word ifetch( byte index ) {
      word addr = index * 2 ;
      byte lo   = eeprom_read_byte((unsigned char *) addr);
      byte hi   = eeprom_read_byte((unsigned char *) addr+1);
      return word(hi,lo);
   }
   
   // should probably be named 'stash' but the i emphasizes we're index, not byte based.
   static void istash( byte index, word value ) {
      word addr = index * 2 ;
      eeprom_write_byte((unsigned char *)addr,   lowByte(value));
		  eeprom_write_byte((unsigned char *)addr+1, highByte(value));
   }
   
   
   
   static void      setVersion(word value)       { istash(EEPROM_VERSION, value); }
   static word      getVersion()                 { return ifetch(EEPROM_VERSION); }
   static boolean   isVersion(word version)      { return getVersion() == version; }
   
   static void      setBoardType(BoardType type) { istash(EEPROM_BOARDTYPE, (int)type); }
   static BoardType getBoardType()               { return (BoardType)ifetch(EEPROM_BOARDTYPE); }
   
   static void      setBoardId(int boardId)      { istash(EEPROM_BOARDID, boardId); }
   static BoardType getBoardId()                 { return (BoardType)ifetch(EEPROM_BOARDID); }
   
   static void      setDebugMode(word value)     { istash(EEPROM_DEBUG_MODE, value); }
   static word      getDebugMode()               { return ifetch(EEPROM_DEBUG_MODE); }
  
   static void      setSystemMode(word value)    { istash(EEPROM_SYSTEM_MODE, value); }
   static word      getSystemMode()              { return ifetch(EEPROM_SYSTEM_MODE); }
  
   static void      setUserMode(word value)      { istash(EEPROM_USERMODE, value); }
   static word      getUserMode()                { return ifetch(EEPROM_USERMODE); }

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// set or clear a specific debug mode bit
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  static void setDebugModeBit(int modeBit, boolean state) {
    word mode = getDebugMode();
    bitWrite(mode, modeBit, state);
    setDebugMode(mode);
  }
   
  // return true if a specific bit is set
  static boolean isDebugMode(int modeBit) {
    return bitRead( getDebugMode(), modeBit);
  }
   
  // toggle a specific bit
  static void toggleDebugModeBit(int modeBit) {
    word mode = getDebugMode();
    bitWrite(mode, modeBit, !isDebugMode(modeBit));
    setDebugMode(mode);
  }
   
  // this only writes to the flash if the state's are different.
  static void maybeSetDebugModeBit(int modeBit, boolean state) {
    if( !isDebugMode(modeBit) == state ) {
        setDebugModeBit(modeBit, state);
    }
  }
// done \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// set or clear a specific system mode bit
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  static void setSystemModeBit(int modeBit, boolean state) {
    word mode = getSystemMode();
    bitWrite(mode, modeBit, state);
    setSystemMode(mode);
  }
   
  // return true if a specific bit is set
  static boolean isSystemMode(int modeBit) {
    return bitRead( getSystemMode(), modeBit);
  }
   
  // toggle a specific bit
  static void toggleSystemModeBit(int modeBit) {
    word mode = getSystemMode();
    bitWrite(mode, modeBit, !isSystemMode(modeBit));
    setSystemMode(mode);
  }
   
   // this only writes to the flash if the state's are different.
   static void maybeSetSystemModeBit(int modeBit, boolean state) {
      if( !isSystemMode(modeBit) == state ) {
         setSystemModeBit(modeBit, state);
      }
   }
  // done \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


  
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// set or clear a specific user mode bit
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  static void setUserModeBit(int modeBit, boolean state) {
    word mode = getUserMode();
    bitWrite(mode, modeBit, state);
    setUserMode(mode);
  }
   
  // return true if a specific bit is set
  static boolean isUserMode(int modeBit) {
    return bitRead( getUserMode(), modeBit);
  }
   
  // toggle a specific bit
  static void toggleUserModeBit(int modeBit) {
    word mode = getUserMode();
    bitWrite(mode, modeBit, !isUserMode(modeBit));
    setUserMode(mode);
  }
   
   // this only writes to the flash if the state's are different.
   static void maybeSetUserModeBit(int modeBit, boolean state) {
      if( !isUserMode(modeBit) == state ) {
         setUserModeBit(modeBit, state);
      }
   }
  // done \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  
  
private: // flash/eeprom simply embodies the concept of static yes?

  Flash();
  ~Flash();
  
}; // class



#endif
// the end.


/*
  All of the boards I'm using expect to have 3 leds attached to PWM pins.
  The different boards require the leds be attached to different pins.
  I've created class Board to determine which pins to use automatically.
  If desired it can also initialize the serial port and maintain a loop counter.
  It also collect loop timeing information.
 
  Extend as required in order to isolate sketch's from the physical board it's running on.
  
  You need to call Board::setup() once, from the arduinos main setup().
  Specifying a baudRate of 0 will completely skip any serial port initialization.
  Enabling 'heartbeat' allows beginLoop() and endLoop() to indicate 'running' on digital output 13.
  
  You need to call beginLoop() and endLoop() and the beginning and end of the main arduino loop().
  I gather timing information when you use them too.
 
  Also note, if the led pins are 0 then the green, yellow and red() methods don't do anything.
  The led's are controlled with analogWrite for PWM.
  Use current limiting resisters (but sometimes I don't and it works) on your leds even though it's PWM.

  The code has just been enhanced with rgbLed(r,g,b) if you want to attach a tri-color led
  in place of the green/yellow/red leds. They can't be used at the same time.
 
  ---------------------------------------------------------------------------------------------------------
   Writes an analog value (PWM wave) to a pin. Can be used to light a LED at varying brightnesses 
   or drive a motor at various speeds. After a call to analogWrite(), the pin will generate 
   a steady square wave of the specified duty cycle until the next call to analogWrite() 
   (or a call to digitalRead() or digitalWrite() on the same pin). 
   
   The frequency of the PWM signal is approximately 490 Hz.
 
   On most Arduino boards (those with the ATmega168 or ATmega328), 
     this function works on pins 3, 5, 6, 9, 10, and 11. 
     On the Arduino Mega, it works on pins 2 through 13. 
     Older Arduino boards with an ATmega8 only support analogWrite() on pins 9, 10, and 11. 
     You do not need to call pinMode() to set the pin as an output before calling analogWrite().
     
   The analogWrite function has nothing whatsoever to do with the analog pins or the analogRead function.
 ---------------------------------------------------------------------------------------------------------

  
  I'm not trying to encapsulate all your boards functionality.
  If you want to do that in a 'family' of boards then just sublass Board.
  Also note that if you dont enable heartbeat, and provide a baud rate of zero
  then absoslutely no hardware initialization of any kind is performed.
  Enabling heartbeat causes Board to init and use digital pin 13 as a loop running indicator.
  A non-zero baud rate results in Serial.begin(rate) being called, if not already open.
 
  If you have a family of similar boards then you should probably subclass Board
  and then conglomorate different device control classes into the Board in a public way.
  Then your add code has one convienent non-intrusive and easily changed hardware access point.

  ...requires editing..
 
 
 A Board helps you adapt to and control hardware that varies.
 It uses digital pin 13 as an output to drive an led indicator light.
 You can also put a scope on pin 13 and measure loop freq and duration directly.
 
 type() is useful for adapting to a kind of Arduino, an Uno, a Lily etc.
 id()   is useful for indicating what application should run.
 i2c()  is useful for establishing my i2c address. Oh, the name addr() was too ambiguous. a synonym perhaps.
 
 
 My constructor does absolutely nothing. Leave it that way.
 You simply must call setup(...) before using me. It's where I init pin 13 btw.
 
 A baudRate of zero tells setup(...) to ignore the serial port.
 
 setup() does not mess with the analog reference either.
 Use setAnalogRefExt() and getAnalogRefVoltage() for that.
 These are ugly, but I don't want setup() messing with aref either.
 
 A board and its eeprom (I call it Flash) memory are meant to work together.
 Thus I have included class Flash right after class Board.
 They are meant to work together in that Flash intrinsically understands BoardType.
 
 I want a subclass of Board to be dynamically adapt the application to the hardware.
 This requires I pre-format the eeprom and assign boardType, boardId and i2c address
 along with some other magic numbers to indicate the eeprom is ready for use.
 
 So, Board and Flash work together tightly, but are separate subsystems.
 Gathered here in one place for your convienence.
 Get use to the 'system', use the system.
 
 Note that another libary named <BoardManager.h> contains the formatting code.
 I split it out becuase a runtime system doesn't need it. Or the space it consumes.
 In order to use this system you have to do the one-time, two-phase eeprom formatting sequence.
 It's weird but you have to do it. After that, boards are 'stamped' and the software
 can read the eeprom and adapt accordingly.
 
 All this magic is accomplished by:
 
 1. Performing the one-time, two-phase Flash (eeprom) init.
 
 2. Writing another sketch.
 This sketch declares and uses exactly one Board (or subclass).
 This sketch reads Flash:: to supply arguments to Board::setup(...)
 I want you to see the Flash:: reading and arguments to Board::setup()
 because things become very confusing if you hide that information somewhere.
 
 3. Uploading the sketch to the newly formatted board.
 At which point you're probably trying to prove to yourself it did adapt. It did.
 
 ** The new sketch setup() & loop() must look like this:
 
 Board arduino;
 void setup() {
 Flash::maybeSetDebugModeBit( DEBUG_VERBOSE, false );
 arduino.setup( Flash::getBoardType(), Flash::getBoardId(), 9600, 0 );
 // your initialization here, aref? digitals? analogs?
 arduino.printReport(Serial);
 } // end of setup
 
 void loop() {
 arduino.beginLoop();
 // your code goes here
 //arduino.printReport(Serial);
 arduino.endLoop(); //Serial.print("tL: "); Serial.println(arduino.getLoopDuration() );
 } // end of loop
 
 
 You're good to go at this point and can call (using the Board arduino; example above):
 
 arduino.type() - figure out what hardware you're running on.
 arduino.id()   - what job you should do, while paying attention to type() while doing it.
 arduion.i2c()  - perhaps this is your i2c address, perhaps not. it's up to you then.
 
 // end needs editing.
 
*/


