//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Illumino Project
// Io Flament, Kurt Olsen, Mael Flament
// This code is licensed under Creative Commons Attribution-Non-Commercial-Share Alike 3.0 and GNU GPL license.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/



#ifndef Definitions_h
#define Definitions_h

#include <Arduino.h>
#include <EEPROM.h>
#include <Brain.h>
#include <Adafruit_NeoPixel.h>
#include <Timer.h>
#include <Color.h>
#include <Board.h>
#include <NeoPixels.h>
#include <Devices.h>
#include <LowFreqOsc.h>
#include <LightController.h>
#include <ModeSwitch.h>
  

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// Constants, and some variables.
//
//  Using #define's may not consume memory like constants do.
//
//   * do not* put comments at the end of the #define's
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

#define FIRMWARE_VERSION "Illumino Firmware 2.0"

// the number of neopixels in the string.
#define NUM_NEOPIXELS 4

// You CAN change this, just don't set it too low or low, range is 0 to 255, default is 10.
#define MIN_FLASH_PWM_BRIGHTNESS 10


// If we're not receiving data from the eeg unit then we have to indicate that. 
// updates happen at ~1Hz, wait about twice that before indicating failure
#define UPDATE_FAIL_DELAYMS  2100
#define UPDATE_FAIL_BLINKMS  1000
#define UPDATE_FAIL_COLOR    Color::YELLOW


// if signal drops below threshold (see below) then we have to indicate that
// but we don't do it for 2100ms in case user is moving their head etc.
#define SIGNAL_FAIL_DELAYMS  2100
#define SIGNAL_FAIL_BLINKMS  500
#define SIGNAL_FAIL_COLOR    Color::RED


// if signal quality is below this number then we have a good signal, range is 0 - 100.
#define SIGNAL_FAIL_THRESHOLD 50

// used by mode 3 to decide if you're paying attention, or not, range is 0 - 100.
#define ATTENTION_ACHIEVED_THRESHOLD 60

// used by mode 5 to decide if you're meditating, or not, range is 0 - 100.
#define MEDITATION_ACHIEVED_THRESHOLD 60

#define CHAR_COMMA     (char)44


// Used when a null pointer is detected.
enum UserErrors {
  UserError_NullMode = ERR_USER_ERRORA
};



// convienent names for our modes.
// MODE_ENDSTOP is not an actual mode but is used to denote the end of the list.
// The ordering determines the modeSwitch.getMode() to a specific EEGMoide subclass instantiation.
enum EMODE {
  eMODE_RGB,
  eMODE_ATTENTION,
  eMODE_ATTENTION_ACHIEVED,
  eMODE_MEDITATION,
  eMODE_MEDITATION_ACHIEVED,
  eMODE_BRIGHTNESS,
  eMODE_ACCESSORY,
  eMODE_COLORSELECT,
  eMODE_ENDSTOP,
};



//
//  Need this forward declaration to help HatBoard and EEGMode resolve their
//  whose "who's the chicken and who's the egg?" difficulty.
//
class EEGMode;



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// EEGReceiver, encapsulates brain and its sim
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGReceiver {
public:

  EEGReceiver();	

  void            setup(HardwareSerial& _brainSerial);

  void            setSimMode(boolean state);
  boolean         getSimMode();
  boolean         setSimRange(int firstRecord, int lastRecord);
  boolean         setSimRecord(boolean state); // used to record new data to eeprom.
  void            setSimUseLFO(boolean state) { m_bSimUseLFO = state; }
  
  // reads data from the eeg headband or the simulator
  boolean         update();
  boolean         isSignalValid();

  // after an update you can grab the data
  //  if the signal isn't valid then neither are these.
  int             getSignalQuality();
  int             getAttention();
  int             getMeditation();
  unsigned long*  getPowerArray();
  unsigned long   getDelta();
  unsigned long   getTheta();
  unsigned long   getLowAlpha();
  unsigned long   getHighAlpha();
  unsigned long   getLowBeta();
  unsigned long   getHighBeta();
  unsigned long   getLowGamma();
  unsigned long   getMidGamma();

  unsigned char   getLatestError();

  void flashSimData();


private:
  // Note: This serially prints an 's' and the record number.
  void readNextSimRecord();
  void writeNextSimRecord(byte signalQuality, byte attention, byte meditation);

private: // keep out

  Brain  m_brain;

  bool   m_bSimMode;
  int    m_nextSimRecord;
  int    m_firstSimRecord;
  int    m_lastSimRecord;
  int    m_bSimRecording;
  int    m_signalQuality;
  int    m_attention;
  int    m_meditation;
  Timer  m_updateTimer;
  bool   m_bSimUseLFO;

}; // class


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// This is what allows the lights to modulate in-between
// eeg data updates, which occur only once per second.
// If you set a mode on the light controller then call loop()
// on it then it will keep the lights changing while waiting
// for new data, or in the case of an eeg hardware failure
// the lights will begin blinking continuously to tell
// the user that things aren't working.
//
// One of it's main functions is the implementation of MODE_RGB
// The goal is twofold, the first is to smoothly change the
// attention or meditation values over one second. And secondly
// to introduce some flicker which rides on top of the
// of the first signal, the smoothly changing value.
//
// The usage of the m_lfoAttention and m_lfoMeditation lfo's
// is what allows the eeg data values to smoothly change
// over one second. Then lfo's m_lfoR1 and m_lfoR2 are used
// to create the flickering effect.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class LightControllerX : public LightController {
public:
  
  enum DisplayMode { /*aweII*/
    MODE_RGB = LightController::FAIL_SIGNAL + 1,
    MODE_ATTENTION,
    MODE_ATTENTION_ACHIEVED,
    MODE_MEDITATION,
    MODE_MEDITATION_ACHIEVED,
  };
  
  //
  // construction, needs lights (pixels in this case)
  //
  LightControllerX(LedArray& pixels);


  // these two are tricky, look inside the source code.
  void  setAttention  (int attention);
  void  setMeditation (int meditation);

  int getRandomBipolar(int n);
  
protected: // used internally

  boolean switchNormalMode();
  boolean switchFailMode();

  void resetModLfo( LowFreqOsc& lfo, float modAmplitude );

  // increasing signalDivisor results in decreasing flicker.
  void calculateFlicker( float signalDivisor = 4 );

  //
  // newPeriodRandomness is jitter(in ms), range 0-30 (internally constrained)
  // Note: Any amount causes jitter, even zero.
  // Note: 50 is too high, timebase resets are happening frequently by then.
  //
  void randomizeR1andR2Timebases(int newPeriodRandomness = 4 );

private: // keep out
  
  LowFreqOsc  m_lfoR1;
  LowFreqOsc  m_lfoR2;

  LowFreqOsc  m_lfoAttention;  // used for smooth transitions in between updates
  int         m_eegAttention;     // attention, 0-100 as obtained from EEGReceiver 
  float       m_modAttention;  // attention, after the lfo has been applied
  float       m_rndAttention;    // attention, with flicker applied.
  
  LowFreqOsc  m_lfoMeditation; // used for smooth transitions in between updates
  int         m_eegMeditation;    // meditation, 0-100 as obtained from EEGReceiver 
  float       m_modMeditation; // meditation, after the lfo has been applied
  float       m_rndMeditation;   // meditation, with flicker applied.
  
}; // class




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// A HatBoard encapsulates the hardware, and initializes it.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class HatBoard : public Board {
public:

  // constructor, but useless before calling setup().
  HatBoard();

  virtual void setup(
    BoardType     type,
    int           boardId,
    unsigned  int baudRate,
    unsigned char i2cAddr,
    bool          doHeartbeat,
    bool          debugVerbose
  ); // setup


  //
  // This is called from the baseclass Board::setup()
  //
  void eepromPreload();
  
  //
  // These must be called in the arduino main loop()
  //
  void beginLoop() { Board::beginLoop(); }
  void loop();
  void endLoop()   { lightController.loop(); /*pixels.show();*/ Board::endLoop(); }

  
  // lighting test patterns
  void  rgb1();
  void  rgb2();
  void  neo1();
  
  void  setMode(int eMode);
  int   getMode() { return modeSwitch.getCurMode(); }

public: // members

  //
  // These are meant to controlled by the EEGMode (the brain activity mode)
  // It presents the neopixels, the uno's rgb led, an eeg receiver
  // and a light controller.
  //
  ModeSwitch
    modeSwitch;

  NeoPixels
    pixels;

  EEGReceiver
    eegReceiver;

  LightControllerX
    lightController;

private:  // used internally

  //
  // Constant pin assignments (they are the same on the uno and lily at least).
  //
  const int DIO_OUT_PIXELS;
  const int DIO_IN_DISABLE_FLICKER;
  const int DIO_IN_ENABLE_SIM;
  //
  // Dynamic pin assignments (the vary from board to board)
  //
  int DIO_IN_MODESW;

  //
  // Indexes for flash memory variables.
  //
  enum FLASHVAR {
    FLASH_BRIGHTNESS = EEPROM_USERAREA,
    FLASH_COLOR1R,
    FLASH_COLOR1GB,
    FLASH_COLOR2R,
    FLASH_COLOR2GB
  };
  
  //
  // This notices and handles somebody pressing the mode switch button.
  //
  void handleModeSwitch();

// terminology, fetch from flash or, stash in flash.
public:

  float     fetchBrightness();
  uint32_t  fetchColor1();
  uint32_t  fetchColor2();

private:

  void      stashBrightness(float brightness);
  void      stashColor1(uint32_t value);
  void      stashColor2(uint32_t value);



private: // keep out.

  EEGMode*
    m_pMode;

//  int
//    m_iMode;
  
}; // class










//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// An EEGMode handles the work that's common to all modes. This includes
// detecting noisy or no data from the eeg headband. In other words,
// this class is a heavyweight and somewhat complicated. Treat gently.
// 
// Meant to be subclassed. The loop() method does most of the apps work.
// Override the constructor, indicateMode() and sendSerialData() as needed.
//  and sendSerialData() methods in subclasses.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGMode {
public: 

  EEGMode(HatBoard* _pBoard);

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  // Call this from the arduinos main loop().
  // This is what does the majority of the work.
  // Never override, and remember to call it.
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  void loop();

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  // Override the virtuals in a subclass
  // to allow each mode to behave differently.
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  virtual void indicateMode() {}  // *must* be implemented by subclasses.
  virtual void sendSerialData();  // the subclasses inherit this one and nobodys overriding it at this time.

private:

  // useful as a diagnostic, not used in production mode.
  void brainDump();



private:

  // the mechanics, ignore them if possible

  void     handleUpdateFailure();
  void     resetAfterUpdateFailure();

  void     handleSignalFailure();
  void     resetAfterSignalFailure();


protected:

  HatBoard*  m_pBoard;

private:

  Timer     t_initDelay;
  Timer     t_updateFailure;
  Timer     t_signalFailure;

}; // class






//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Mode 0
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGModeRGB : public EEGMode {
public:

  EEGModeRGB(HatBoard* board) : EEGMode(board)
  {
//    m_pBoard->pixels.setColor(Color::RED);
//    m_pBoard->lightController.setNormalMode(LightController::MODE_BYPASS);

    m_pBoard->lightController.setAnimationTimer(75);
    m_pBoard->lightController.setAnimationColor(Color::RED);
    m_pBoard->lightController.setNormalMode(LightController::MODE_COLORCHASE);
  }

  
  // EEGMode.loop() will only call this when new data arrives.
  virtual void indicateMode()
  {
    m_pBoard->lightController.setAttention ( m_pBoard->eegReceiver.getAttention()  );
    m_pBoard->lightController.setMeditation( m_pBoard->eegReceiver.getMeditation() );

    m_pBoard->lightController.setNormalMode(LightControllerX::MODE_RGB);
    
  } // method

}; // class




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Mode 1
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGModeAttention : public EEGMode {
public:
  
  EEGModeAttention(HatBoard* board) : EEGMode(board)
  {
    m_pBoard->pixels.setColor( m_pBoard->fetchColor1() );
    m_pBoard->lightController.setNormalMode(LightController::MODE_BYPASS);
  }
  
  
  // EEGMode.loop() will only call this when new data arrives.
  void indicateMode()
  {
    m_pBoard->lightController.setAttention ( m_pBoard->eegReceiver.getAttention()  );
    m_pBoard->lightController.setNormalMode(LightControllerX::MODE_ATTENTION);
  }
  
}; // class



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Mode 2
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGModeAttentionAchieved : public EEGMode {
public:
  
  EEGModeAttentionAchieved(HatBoard* board) : EEGMode(board)
  {
    m_pBoard->pixels.setColor( m_pBoard->fetchColor1() );
    m_pBoard->pixels.setColor(Color::WHITE, NEOPIXEL_N1);
    m_pBoard->lightController.setNormalMode( LightController::MODE_BYPASS );
  }
  
  // EEGMode.loop() will only call this when new data arrives.
  void indicateMode()
  {
    m_pBoard->lightController.setAttention ( m_pBoard->eegReceiver.getAttention()  );
    
    m_pBoard->lightController.setNormalMode( LightControllerX::MODE_ATTENTION_ACHIEVED );
  } 
  
}; // class



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Mode 3
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGModeMeditation : public EEGMode {
public:
  
  EEGModeMeditation(HatBoard* board) : EEGMode(board)
  {
    m_pBoard->pixels.setColor( m_pBoard->fetchColor2() );
    m_pBoard->lightController.setNormalMode(LightController::MODE_BYPASS);
  }
  
  // EEGMode.loop() will only call this when new data arrives.
  void indicateMode()
  {
    m_pBoard->lightController.setMeditation ( m_pBoard->eegReceiver.getMeditation()  );
    
    m_pBoard->lightController.setNormalMode(LightControllerX::MODE_MEDITATION);
  }
  
}; // class



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Mode 4
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGModeMeditationAchieved : public EEGMode {
public:
  
  EEGModeMeditationAchieved(HatBoard* board) : EEGMode(board)
  {
    m_pBoard->pixels.setColor( m_pBoard->fetchColor2() );
    m_pBoard->pixels.setColor(Color::WHITE, NEOPIXEL_N1);
    m_pBoard->lightController.setNormalMode( LightController::MODE_BYPASS );
  }
  
  // EEGMode.loop() will only call this when new data arrives.
  void indicateMode()
  {
    m_pBoard->lightController.setMeditation ( m_pBoard->eegReceiver.getMeditation()  );
    m_pBoard->lightController.setNormalMode( LightControllerX::MODE_MEDITATION_ACHIEVED );
  }
  
}; // class



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Mode 5
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGModeBrightness : public EEGMode {
public:
  
  EEGModeBrightness(HatBoard* board) : EEGMode(board)
  {
    m_pBoard->pixels.setColor( Color::WHITE );
    m_pBoard->lightController.setNormalMode(LightController::MODE_BYPASS);
  }
  
  // the EEGMode.loop() gives special handling to this mode and will call indicateMode() frequently.
  void indicateMode()
  {
    m_pBoard->lightController.setNormalMode(LightController::MODE_BRIGHTNESS);
  }
  
}; // class



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Mode 6
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGModeAccessory: public EEGMode {
public:
  
  EEGModeAccessory(HatBoard* board) : EEGMode(board)
  {
    m_pBoard->pixels.setColor(Color::PURPLE);
    m_pBoard->lightController.setNormalMode(LightController::MODE_BYPASS);
  }
  
  // the EEGMode.loop() gives special handling to this mode and will call indicateMode() frequently.
  void indicateMode()
  {
    m_pBoard->lightController.setNormalMode(LightController::MODE_ACCESSORY);
  }
  
}; // class



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Mode 7
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class EEGModeColorSelect : public EEGMode {
public:
  
  EEGModeColorSelect(HatBoard* board) : EEGMode(board), m_timer(), m_step(0), m_bFirstTime(true)
  {
    m_pBoard->pixels.setColor(Color::WHITE, NEOPIXEL_N1);
    m_pBoard->pixels.setColor(Color::BLUE,  NEOPIXEL_N2);
    m_pBoard->pixels.setColor(Color::GREEN, NEOPIXEL_N3);
    m_pBoard->pixels.setColor(Color::RED,   NEOPIXEL_N4);
    m_pBoard->lightController.setNormalMode(LightController::MODE_BYPASS);

  }  // ctor

  
  // the EEGMode.loop() gives special handling to this mode and will call indicateMode() frequently.
  void indicateMode()
  {
    if( m_bFirstTime ) {
      m_bFirstTime = false;
      m_timer.start(6000);
      m_pBoard->lightController.setAnimationTimer( 750 ); //m_timer.getInterval() / 8);
      m_pBoard->lightController.setNormalMode(LightController::MODE_COLORSELECT2);
    }
  
    if( m_timer.isExpired(true) ) {
    
      if( ++m_step > 1 ) {
        m_step = 0;
      }

      switch( m_step ) {

        case 0: {
          //Serial.println("seq0");
          m_pBoard->lightController.setAnimationTimer( 750 ); //m_timer.getInterval() / 8);
          m_pBoard->lightController.setNormalMode(LightController::MODE_COLORSELECT2);
        } break;

        case 1: {
          //Serial.println("seq1");
          m_pBoard->lightController.setNormalMode(LightController::MODE_COLORSELECT);
        } break;

        default: {
          //Serial.println("wtf?");
        } break;
        
      }; // switch
      
      
    } 
    
  } // method
  
private:

  Timer
    m_timer;
  
  int
    m_step;
  
  boolean
    m_bFirstTime;
  
}; // class

#endif
// the end.




