
#ifndef Common_h
#define Common_h

//
// You must #include <EEPROM.h> in your sketch before #include <Common.h>
//

//#include <Arduino.h>
//#include <EEPROM.h>
//#include <Timer.h>
//#include <Board.h>



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// literal char's consume ram, these don't
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// #define CHAR_SPACE     (char)32
// #define CHAR_COMMA     (char)44
// #define CHAR_COLON     (char)58

// #define CHAR_SPACE     (char)32
// #define CHAR_LPAREN    (char)40
// #define CHAR_RPAREN    (char)41
// #define CHAR_ASTERISK  (char)42
// #define CHAR_PLUS      (char)43
// #define CHAR_COMMA     (char)44
// #define CHAR_MINUS     (char)45
// #define CHAR_FWDSLASH  (char)47
// #define CHAR_0         (char)48
// #define CHAR_1         (char)49
// #define CHAR_2         (char)50
// #define CHAR_3         (char)51
// #define CHAR_4         (char)52
// #define CHAR_5         (char)53
// #define CHAR_6         (char)54
// #define CHAR_7         (char)55
// #define CHAR_8         (char)56
// #define CHAR_9         (char)57
// #define CHAR_COLON     (char)58
// #define CHAR_EQUALS    (char)61
// #define CHAR_QUESTION  (char)63
// #define CHAR_A         (char)65
// #define CHAR_C         (char)67
// #define CHAR_I         (char)73
// #define CHAR_L         (char)76
// #define CHAR_P         (char)80
// #define CHAR_R         (char)82
// #define CHAR_S         (char)83
// #define CHAR_LBRACKET  (char)91
// #define CHAR_RBRACKET  (char)93
// #define CHAR_a         (char)97
// #define CHAR_b         (char)98
// #define CHAR_c         (char)99
// #define CHAR_d        (char)100
// #define CHAR_f        (char)102
// #define CHAR_k        (char)107
// #define CHAR_l        (char)108
// #define CHAR_m        (char)109
// #define CHAR_r        (char)114
// #define CHAR_s        (char)115
// #define CHAR_v        (char)118
// #define CHAR_w        (char)119
// #define CHAR_x        (char)120


/*

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// converts floats to ascii in the given buffer
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


char *ftoa(char *a, double f, int precision);
void printFloat(char* msg, float value, int precision);
void printlnFloat(char* msg, float value, int precision);



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Keeps track of an average, of type int.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
class Averager {
  
public:
  
  Averager(byte nAverages);
  ~Averager();
  
  void    reset();
  int     add(int value);
  boolean isReady();
  
  int average();
  
private:
  
  byte    m_size;
  byte    m_next;
  boolean m_ready;
  int*    p_values;
  
}; // class


int getRandomBipolar(int n);
*/


#endif // Common_h



