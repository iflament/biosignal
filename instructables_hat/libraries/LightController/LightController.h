
#ifndef LightController_h
#define LightController_h

#include <Arduino.h>
#include <Color.h>
#include <AdaFruit_NeoPixel.h>
#include <NeoPixels.h>
#include <Devices.h>
#include <LowFreqOsc.h>
#include <Timer.h>



//#define UPDATE_FAIL_DELAYMS  2100
//#define UPDATE_FAIL_BLINKMS  1000
#define UPDATE_FAIL_COLOR    Color::YELLOW

 
// if signal drops below threshold (see below) then we have to indicate that
// but we don't do it for 2100ms in case user is moving their head etc.
//#define SIGNAL_FAIL_DELAYMS  2100
//#define SIGNAL_FAIL_BLINKMS  500
#define SIGNAL_FAIL_COLOR    Color::RED


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

class LightController {
public:
  
  enum DisplayMode {
    MODE_BYPASS,
    MODE_OFF,
    MODE_BRIGHTNESS,
    MODE_COLORSELECT,
    MODE_ACCESSORY,
    MODE_BLING1,
    MODE_RAINBOW,
    MODE_RAINBOW_CYCLE,
    MODE_COLORCHASE,
    MODE_COLORSWEEP,
    MODE_COLORSELECT2,
    FAIL_UPDATE,
    FAIL_SIGNAL,
  };
   
  //
  // construction, I need lights (LedArray in this case)
  //
  LightController(LedArray& pixels);

  // call this on every pass thru the Arduino's main loop()
  void      loop();

  // pretty lights, right now.
  void      doStartupBling();


  // control methods
  void      setNormalMode (int mode)            { m_normalMode = mode; }
  void      resetNormalMode()                   { m_normalMode = MODE_OFF; }

  void      setFailMode   (int mode)            { m_failMode   = mode; }
  void      resetFailMode ()                    { m_failMode = MODE_OFF;    }

  void      setFailSignal ()                    { m_failMode = FAIL_SIGNAL; }
  void      setFailUpdate ()                    { m_failMode = FAIL_UPDATE; }

  void      reset()                             { resetFailMode(); resetNormalMode(); m_gColorWheelValue = 0; m_gCurPixel=0; }

  // Used in dev to turn off the modulation (flicker) making debugging the algorithm easier.
  void      setDisableFlicker(bool state)		{ m_bDisableFlicker = state; }

  void      setBrightness(float brightness)		{ m_leds.setBrightness(brightness); }
  void		setBrightnessColor(uint32_t color)	{ m_brightnessColor = color; }
  
  // used for attention, this is set by color select mode.
  void      setColor1(uint32_t color)			{ m_color1 = color; }
  uint32_t  getColor1()							{ return m_color1;  }


  // used for meditation, this is set by color select mode.
  void      setColor2(uint32_t color)			{ m_color2 = color; }
  uint32_t  getColor2()							{ return m_color2;  }


  //
  // used by the bling, rainbow and color wipe/sweeps.
  // call setAnimationTimer just once (not in the loop) or you'll keep resetting the timer!
  //
  void      setAnimationTimer(unsigned long interval) { m_animationTimer.start(interval); }
  void      setAnimationColor(uint32_t color)         { m_animationColor   = color;      }
  void      setAnimationDirection(bool bForward)      { m_animationDir     = bForward;   }
  void      setAnimationAutoReverse(bool state)       { m_animationAutoRev = state;      }
  
  int       hzToMs(float hz) { return (1.0/hz) * 1000; }
  
protected: // used internally

  virtual boolean switchNormalMode();
  virtual boolean switchFailMode();
  
  void  nextChaseLed();
  
  LedArray&   m_leds;
  
  int         m_failMode;
  int         m_normalMode;
  
  bool        m_bDisableFlicker;
  
  uint32_t    m_color1;
  uint32_t    m_color2;
  uint32_t	  m_brightnessColor;
  
  Timer       m_animationTimer;
  uint32_t    m_animationColor;
  
  uint16_t    m_gColorWheelValue;
  int         m_gCurPixel;
  
  bool        m_animationDir;
  bool        m_animationAutoRev;
  
}; // class

#endif // LightController_h



