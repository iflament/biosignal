
#ifndef BoardManager_h
#define BoardManager_h

//
// you may have to include all of these at your top levels too. I don't know why.
//

#include <Arduino.h>
#include "Board.h"


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// literal char's consume ram, these don't
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#define CHAR_SPACE     (char)32
#define CHAR_COMMA     (char)44
#define CHAR_COLON     (char)58
#define CHAR_EQUALS    (char)61
#define CHAR_LBRACKET  (char)91
#define CHAR_RBRACKET  (char)93
#define CHAR_a         (char)97
#define CHAR_b         (char)98
#define CHAR_c         (char)99
#define CHAR_d        (char)100

//#define CHAR_LPAREN    (char)40
//#define CHAR_RPAREN    (char)41
//#define CHAR_STAR      (char)42
//#define CHAR_PLUS      (char)43
//#define CHAR_COMMA     (char)44
//#define CHAR_MINUS     (char)45
//#define CHAR_FWDSLASH  (char)47
//#define CHAR_0         (char)48
//#define CHAR_1         (char)49
//#define CHAR_2         (char)50
//#define CHAR_3         (char)51
//#define CHAR_4         (char)52
//#define CHAR_5         (char)53
//#define CHAR_6         (char)54
//#define CHAR_7         (char)55
//#define CHAR_8         (char)56
//#define CHAR_9         (char)57
//#define CHAR_COLON     (char)58
//#define CHAR_EQUALS    (char)61
//#define CHAR_QUESTION  (char)63
//#define CHAR_A         (char)65
//#define CHAR_C         (char)67
//#define CHAR_I         (char)73
//#define CHAR_L         (char)76
//#define CHAR_P         (char)80
//#define CHAR_R         (char)82
//#define CHAR_S         (char)83
//#define CHAR_LBRACKET  (char)91
//#define CHAR_RBRACKET  (char)93
//#define CHAR_a         (char)97
//#define CHAR_b         (char)98
//#define CHAR_c         (char)99
//#define CHAR_d        (char)100
//#define CHAR_f        (char)102
//#define CHAR_k        (char)107
//#define CHAR_l        (char)108
//#define CHAR_m        (char)109
//#define CHAR_r        (char)114
//#define CHAR_s        (char)115
//#define CHAR_v        (char)118
//#define CHAR_w        (char)119
//#define CHAR_x        (char)120

extern char STR_magic[];
extern char STR_formatting[];
extern char STR_ver[];
extern char STR_verified[];
extern char STR_failed[];

class BoardManager {
public:

  static void dumpMagic(Stream& ostream) {
    ostream.print(STR_magic); ostream.print(CHAR_COLON); ostream.print(CHAR_SPACE);
    for(int i = 0; i < 4; i++ ) {
      ostream.print(CHAR_LBRACKET);
      ostream.print(i);
      ostream.print(CHAR_RBRACKET); ostream.print(CHAR_EQUALS);
      ostream.print(EEPROM.read(i));
      if( i < 3 ) {
        ostream.print(CHAR_COMMA); ostream.print(CHAR_SPACE);
      }
    }
    ostream.println();
  }
  
  static void eraseEEPROM() {
    //
    // Erase 1024 bytes of the eeprom.
    // This provides room for 512 eeprom variables (which are 2 bytes each).
    // I use a one-size-fits all approach, I think I'm sized for an Uno.
    //
    for (int i = 0; i < 1024; i++) {
      EEPROM.write(i, 0);
    }
  }

  //
  // Testing the eeprom formatting can be tricky. More than once I've thought I formatted it
  // only to have the system report that it's NOT formatted. I think what's happening is that
  // in my sketches I'd sometimes put an EEPROM.write(0,0) to trash the 'magic', which is detected
  // and then formats. Then...I comment out the EEPROM.write(0,0), and formatEEPROM() and just go
  // with the detection phase. BUT - when you download the code, the arduino can reset before uploading
  // and I believe this is causing the EEPROM.write(0,0) to happen one last time before the new code
  // is uploaded, which causes it to think the eeprom needs formatting...because it does...
  //
  static boolean format(BoardType boardType, int boardId, int eepromVersion, Stream& ostream) {

    ostream.println(STR_formatting);
    
    //
    // Erase 512 bytes of the eeprom.
    // This provides room for 256 eeprom variables (which are 2 bytes each).
    // I use a one-size-fits all approach, I think I'm sized for an Uno.
    //
    eraseEEPROM();
    //
    // write the magic bit pattern "abcd" into the front
    // of the EEPROM to indicate it has been formatted
    //
    EEPROM.write(0, CHAR_a);
    EEPROM.write(1, CHAR_b);
    EEPROM.write(2, CHAR_c);
    EEPROM.write(3, CHAR_d);
    
    Flash::setVersion(eepromVersion);
    Flash::setBoardType(boardType);
    Flash::setBoardId(boardId);
    
    dumpMagic(ostream);
    ostream.print(STR_ver);  ostream.print(CHAR_COLON); ostream.println( Flash::getVersion() );
    ostream.print(STR_type); ostream.print(CHAR_COLON); ostream.println( Board::nameOfType(Flash::getBoardType()) );
    ostream.print(STR_id);   ostream.print(CHAR_COLON); ostream.println( Flash::getBoardId() );

    boolean
        bVerified = Flash::isFormatted();
    if( bVerified ) {
      ostream.println(STR_verified);
    } else {
      ostream.println(STR_failed);
    }
    ostream.flush();
    return bVerified;
    
  } // method
  

// I don't like these anymore.
//  static void formatMicro(int boardId)    { format(Micro,    boardId); }
//  static void formatUno(int boardId)      { format(Uno,      boardId); }
//  static void formatMega2560(int boardId) { format(Mega2560, boardId); }
//  static void formatMegaADK(int boardId)  { format(MegaADK,  boardId); }
  
  //
  // Follow the following procedure to initialize the eeprom as unbeleivable as it sounds.
  //
  // 1. create and upload a sketch contining formatPhaseOne(YourBoardType) in setup() - and nothing else.
  // 2. create and upload a sketch contining formatPhaseTwo(YourBoardType) in setup() - and nothing else.
  // 3. upload your main sketch - it should work and have a properly formatted eeprom.
  //
  // This must be done as a two phase procedure because of a weirdness caused by boards being reset
  // when the serial port connects (on some boards). This seems to cause the previous sketch
  // to run a second time before uploading the new sketch. I don't understand this.
  // It is happening however trying to do this in one sketch was getting me nowhere.
  //
  // I've chosen to write a BoardType into the eeprom so that my sketches can read it
  // and dynamically adapt to the different i/o pinouts on the various arduinos.
  // I seriously depend on that BoardType in the eeprom. It has to be right.
  // The app only checks to verify the eeprom's formatted, not that the boardcode is correct.
  // It assumes it is...
  //
  static void formatPhaseOne(BoardType type, int boardId, int eepromVersion, Stream& ostream) {
    ostream.println();
    format(type, boardId, eepromVersion, ostream);
  }
  
  static void formatPhaseTwo(BoardType type, int boardId, int eepromVersion, Stream& ostream) {
    
    ostream.println();
    
    if( !Flash::isFormatted() ) {
      formatPhaseOne(type, boardId, eepromVersion, ostream);
    } else {
      ostream.println(STR_verified);
    }
  }
  
  
   static void printReport(Stream& ostream) {
      ostream.println();
      ostream.println("-----------------------------------------------------");
      ostream.print("boardType: "); ostream.println( Board::nameOfType(Flash::getBoardType()) );
      ostream.print("  boardId: "); ostream.println( Flash::getBoardId() );
      ostream.print("eepromVer: "); ostream.println( Flash::getVersion() );
      ostream.print("    ");
      dumpMagic(ostream);
      ostream.println("-----------------------------------------------------");
      ostream.println();
   }
   
   
private:

  BoardManager() {}
  ~BoardManager() {}

}; // class



#endif



