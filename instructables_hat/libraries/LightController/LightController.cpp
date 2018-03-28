 
#include "LightController.h"



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// LightController
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

LightController::LightController(LedArray& pixels)
: m_leds(pixels),
  m_failMode(MODE_OFF),
  m_normalMode(MODE_OFF),

  m_bDisableFlicker(false),

  m_color1(Color::BLACK),
  m_color2(Color::BLACK),
  m_brightnessColor(Color::WHITE),

  m_animationTimer(),
  m_animationColor(Color::GREEN),
  m_gColorWheelValue(0),
  m_gCurPixel(0),
  m_animationDir(true),
  m_animationAutoRev(true)
{
} // ctor


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void LightController::doStartupBling() {

  //m_leds.fxColorWipe(Color::GREEN, 175);
  //m_leds.fxRainbowCycle(1);
  m_leds.fxRainbow(2);
  m_leds.setColor(0);
}


void LightController::nextChaseLed() {
  
  if( m_animationDir ) {
    if( ++m_gCurPixel >= m_leds.numLeds() ) {
      if( !m_animationAutoRev ) {
        m_gCurPixel = 0;
      } else {
        m_animationDir = !m_animationDir;
        m_gCurPixel = m_leds.numLeds()-1;
      }
    }
  } else {
    if( --m_gCurPixel < 0 ) {
      if( !m_animationAutoRev ) {
        m_gCurPixel = m_leds.numLeds()-1;
      } else {
        m_animationDir = !m_animationDir;
        m_gCurPixel = 0;
      }
    }
  }
} // method


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

boolean LightController::switchFailMode() {

  switch( m_failMode ) {
  
    case FAIL_UPDATE:
    {
      //
      // This modulates the color UPDATE_FAIL_COLOR through full brightness.
      // Then, the brightness level of m_leds may reduce it further.
      // It demonstrates full modulation within the current brightness
      //
      LowFreqOsc
        twoHz(LowFreqOsc::freqToPeriod(2), LowFreqOsc::LFO_SIN);
      
      uint32_t
        color = Color::scaleColor( UPDATE_FAIL_COLOR, twoHz.apply( .5, .5 ) );

      m_leds.setColor(color);
    } break;
      
    case FAIL_SIGNAL:
    {
      LowFreqOsc
        oneHz(LowFreqOsc::freqToPeriod(1), LowFreqOsc::LFO_SIN);
      
      uint32_t
        color = Color::scaleColor( SIGNAL_FAIL_COLOR, oneHz.apply(.5, .5) );

      m_leds.setColor(color);
    } break;

    default: {
      return false;
    } break;

  } // switch
  
  // if we get here one of the cases was handled so return true to indicate this.
  return true;
  
} // method




uint32_t PRICOLORS[] = {
  Color::RED,
  Color::GREEN, 
  Color::BLUE,
  Color::YELLOWRED,
//  Color::PURPLE,
//  Color::WHITE,
//  Color::TURQUOISE,
//  Color::ORANGE,
};

uint32_t INVPRICOLORS[] = {
  Color::GREEN,
  Color::RED,
  Color::WHITE,
  Color::TURQUOISE,
//  Color::PURPLE,
//  Color::BLUE,
//  Color::YELLOWRED,
//  Color::ORANGE,
};


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

boolean LightController::switchNormalMode() {

  switch( m_normalMode ) {
    
    case MODE_BYPASS:
    {
      // do nothing, this allows something else to set the pixel colors.
    }
    break;
  
    case MODE_OFF:
    {
      m_leds.setColor(0);
    }
    break;
      
    
    case MODE_BRIGHTNESS:
    {
      // The LFO values are carefully chosen to set a minimum 'visible' brightness.
      LowFreqOsc lfoBrightness(3000, LowFreqOsc::LFO_SIN);

      float bias = 0.55f;

      setBrightness( lfoBrightness.apply(bias, 1.0 - bias) );

      m_leds.setColor( m_brightnessColor );
    
    } break; // MODE_BRIGHTNESS

    
    
    case MODE_COLORSELECT:
    {
      
      int
        iColor = LowFreqOsc(6000, LowFreqOsc::LFO_SIN).apply(128, 128);
      
      setColor1( Color::colorWheelValue(iColor) );
      setColor2( Color::invertColorWheelValue(iColor) );
      
      m_leds.setColor( getColor1() );

    } break; // MODE_COLORSELECT
    
     
    
    case MODE_COLORSELECT2: {
    
      // you have to call setAnimationTimer() and setAnimationColor() before this will work.

      if( !m_animationTimer.isExpired(true) ) {
        break;
      }
      
      int
        nColors = sizeof(PRICOLORS) / sizeof(uint32_t);

      if( ++m_gColorWheelValue >= nColors ) {
        m_gColorWheelValue = 0;
      }

      uint32_t
        actualColor = PRICOLORS[m_gColorWheelValue];
      
      setColor1( actualColor );
   // setColor2( Color::invertColor(actualColor) );
      setColor2( INVPRICOLORS[m_gColorWheelValue] );
      
      //Serial.print("c1:");   Serial.print( getColor1(), HEX );
      //Serial.print(", c2:"); Serial.print( getColor2(), HEX );
      //Serial.print(", i:");  Serial.println( m_gColorWheelValue );

      m_leds.setColor( getColor1() );
    
    } break;



    case MODE_ACCESSORY:
    {
      m_leds.setColor( getColor1() );

    } break; // MODE_ACCESSORY

    
    
    case MODE_BLING1:
    {
      LowFreqOsc   
        lfo1( 1000, LowFreqOsc::LFO_SIN, ::millis() ),
        lfo2(  750, LowFreqOsc::LFO_SIN),
        lfo3(  500, LowFreqOsc::LFO_SIN),
        lfo4(  250, LowFreqOsc::LFO_SIN);


      uint32_t
        c1, c2, c3, c4;
        
      float
        b = 1.0; //m_leds.getBrightness();
      
      // Four different pulsing lights on the NeoPixels by using four LFO's
      // 
      // Note the applyAt for RED (and use of board.loopMillis()). 
      //  It's designed to match up with doLightCycle1() which also uses applyAt().
      //  Since they're both at 1hz, and both using applyAt, they sync.
      //  But replace the applyAt() with apply() on either one and you lose sync.
      //
      // Note the nice lfo timing spread, 1000, 750, 500, 250.
      //  But because lfo1 specifices the millis,  and the others don't,
      //   then the nice even spacing is distorted in time.
      // 
      // If everything used the same technique we'd all be sync'd.
      //  However, applyAt() is used to simulate starting or syncing an Lfo to some arbitrary point in time.
      //
      // In any case, remember this when you can't understand why synchronized lights - are not synchronized!
      //
      c1 = Color::scaleColor(Color::Color::RED, lfo1.apply( b/2, b / 2.150, ::millis() ) );
      c2 = Color::scaleColor(Color::GREEN,      lfo2.apply( b/2, b / 2.150 ) );  
      c3 = Color::scaleColor(Color::BLUE,       lfo3.apply( b/2, b / 2.150 ) );  
      c4 = Color::scaleColor(Color::TURQUOISE,  lfo4.apply( b/2, b / 2.150 ) );  
      
      
      m_leds.setColor(c1, 0);
      m_leds.setColor(c2, 1);
      m_leds.setColor(c3, 2);
      m_leds.setColor(c4, 3);
      
    } break; // MODE_BLING1



    case MODE_RAINBOW: {
      // you have to call setAnimationTimer() before this will work.

      if( m_animationTimer.getInterval() != 0 && !m_animationTimer.isExpired(true) ) {
        break;
      }

      if( ++m_gCurPixel >= m_leds.numLeds() ) {
        m_gCurPixel = 0;
        if( ++m_gColorWheelValue >= 256 ) {
          m_gColorWheelValue = 0;
        }
      }
      
      uint32_t color = Color::colorWheelValue((m_gCurPixel + m_gColorWheelValue) & 255);
      m_leds.setColor(color, m_gCurPixel);
      
    } break; // MODE_RAINBOW
    


    case MODE_RAINBOW_CYCLE: {
      // you have to call setAnimationTimer() before this will work.
        
      if( m_animationTimer.getInterval() != 0 && !m_animationTimer.isExpired(true) ) {
        break;
      }

      if( ++m_gColorWheelValue >= 256 * 4 ) {
        m_gColorWheelValue = 0;
      }
      for(int i = 0; i < m_leds.numLeds(); i++ ) {
        uint32_t color = Color::colorWheelValue(((i * 256 / 4) + m_gColorWheelValue) & 255);
        m_leds.setColor(color, i);
      }
    
    } break; // MODE_RAINBOW_CYCLE

    
    
    case MODE_COLORCHASE: {
      // you have to call setAnimationTimer() and setAnimationColor() before this will work.

      if(  m_animationTimer.getInterval() != 0 && !m_animationTimer.isExpired(true) ) {
        break;
      }
      
      nextChaseLed();
      
      for(int i = 0; i < m_leds.numLeds(); i++ ) {
        if( i == m_gCurPixel ) {
          m_leds.setColor(m_animationColor, m_gCurPixel);
        } else {
          m_leds.setColor(Color::BLACK, i);
        }
      }

    } break; // MODE_COLORCHASE


    
    case MODE_COLORSWEEP: {
      // you have to call setAnimationTimer() and setAnimationColor() before this will work.

      if(  m_animationTimer.getInterval() != 0 && !m_animationTimer.isExpired(true) ) {
        break;
      }
      
      if( ++m_gCurPixel >= m_leds.numLeds() ) {
        m_gCurPixel = 0;
      }
      if( ++m_gColorWheelValue >= 256 ) {
        m_gColorWheelValue = 0;
      }	
      
      for(int i = 0; i < m_leds.numLeds(); i++ ) {
        if( i == m_gCurPixel ) {
          m_leds.setColor(Color::colorWheelValue(m_gColorWheelValue), m_gCurPixel);
        } else {
          m_leds.setColor(Color::BLACK, i);
        }
      }

    } break; // MODE_COLORSWEEP

    default: {
      return false;
    } break;
    
  } // switch

  // if we get here one of the cases was handled so return true to indicate this.
  return true;
  
} // method


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// Fail modes take precedence over, and exclude normal mode.
//  This allows a fail mode to be set and start indiciating.
//    Then later, when fail mode is set to MODE_OFF we revert
//     to normal mode handling. So fail modes take over
//     and stay in effect until turned off at which point
//     the previous normal mode resumes.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void LightController::loop() {
  
  if( m_failMode != MODE_OFF ) {
    switchFailMode();
  } else {
    switchNormalMode();
  }
  m_leds.show();
} // method

// the end.



