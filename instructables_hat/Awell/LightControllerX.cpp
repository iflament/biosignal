//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Illumino Project
// Io Flament, Kurt Olsen, Mael Flament
// This code is licensed under Creative Commons Attribution-Non-Commercial-Share Alike 3.0 and GNU GPL license.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

#include "Definitions.h"



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// LightController
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

LightControllerX::LightControllerX(LedArray& pixels)
: LightController(pixels),

  m_lfoR1(1000,LowFreqOsc::LFO_SIN), // do NOT change this value.
  m_lfoR2(1500,LowFreqOsc::LFO_SIN), // do NOT change this value.

  m_lfoAttention(),
  m_eegAttention(0),
  m_modAttention(0),
  m_rndAttention(0),

  m_lfoMeditation(),
  m_eegMeditation(0),
  m_modMeditation(0),
  m_rndMeditation(0)

{
} // ctor


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void LightControllerX::resetModLfo( LowFreqOsc& lfo, float modAmplitude ) {

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //
  // The lfo is a 4hz, cosine, with inverted output and reset to 'now'.
  // the modulation starts at max inverted (-1), and rises upwards to zero.
  // In one second it should approach pi/2 - and shortly thereafter new eeg data arrives.
  //
  // The lfo setup seems backwards because the display loop has to work backwards
  // from the target of m_eegAttention, to where it must have started when the setAttention()
  // call was made. If you're trying to hit 51, and you started at say 35
  // then the display loop has to start near 35 and climb upwards to m_eegAttention of 51.
  //
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  lfo.setFunction(LowFreqOsc::LFO_COS);
  lfo.setInverted(true);
  lfo.setPeriod(4000);
  lfo.setModAmplitude( modAmplitude );
  lfo.setStartMillis();

} // method




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// This one is tricky
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void  LightControllerX::setAttention(int attention) {

  // the distance to travel, to get from where we were, to the new value.
  float
    modAmplitude = attention - m_eegAttention;

  resetModLfo(m_lfoAttention, modAmplitude);

  // save the new value last
  m_eegAttention = attention;

  //Serial.print("setAttn-m_attn:"); Serial.println(m_eegAttention);

} // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// This one is tricky
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void LightControllerX::setMeditation(int meditation) {

  // the distance to travel, to get from where we were, to the new value.
  float
    modAmplitude = meditation - m_eegMeditation;
  
  resetModLfo(m_lfoMeditation, modAmplitude);
  
  // save the new value last
  m_eegMeditation = meditation;

} // method
  


int LightControllerX::getRandomBipolar(int n) {
  
  int r = random(n+1);
  //Serial.print("r:"); Serial.print(r);
  
  int mult = (random(2) == 0) ? -1 : 1;
  //Serial.print(", m:"); Serial.print(mult);
  
  int retv = r * mult;
  //Serial.print("rtv:"); Serial.println(retv);
  
  return retv;
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// Upon entry into the Mode cycle the hat assumes that setAttention(...) and setMeditation(...) have already been called.
// thus...
//  m_eegAttention,  range (int)0-100, usually obtained from EEGReceiver
//  m_eegMeditation, range (int)0-100, usually obtained from EEGReceiver
//
// Upon exit the following internal properties have been calculated and set:
//
//  m_modAttention,  range 0.0f - 100.0f, the result of the 'smooth' modulation being applied.
//  m_modMeditation, range 0.0f - 100.0f, the result of the 'smooth' modulation being applied.
//
//  m_rndAttention,    range 0.0f - 100.0f, the result of applying flicker.
//  m_rndMeditation,   range 0.0f - 100.0f, the result of applying flicker.
//
void LightControllerX::calculateFlicker( float signalDivisor ) {

  // the smoothing applied between updates.
  m_modAttention  = m_lfoAttention.apply ( m_eegAttention  );
  m_modMeditation = m_lfoMeditation.apply( m_eegMeditation );

  // Having one modulation source at 1hz and the other at 2hz works nicely.
  m_rndAttention  = m_lfoR1.apply( m_modAttention,  m_modAttention  / signalDivisor,  m_lfoR1.millis() );
  m_rndMeditation = m_lfoR2.apply( m_modMeditation, m_modMeditation / signalDivisor,  m_lfoR2.millis() );

  // Let's keep those within limits.
  m_rndAttention  = constrain(m_rndAttention,  0.0f, 100.0f);
  m_rndMeditation = constrain(m_rndMeditation, 0.0f, 100.0f);

} // method




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void LightControllerX::randomizeR1andR2Timebases( int newPeriodRandomness ) {

  int
    jitterMs = newPeriodRandomness;
    jitterMs = constrain(jitterMs, 0, 30);

  //
  // Make more flicker by adjusting m_lfoR1's period? Yikes...
  // m_lfoR1 is constructed at 1hz
  //
  unsigned int newPeriod1 = ((int)(m_lfoR1.getPeriod() + getRandomBipolar(jitterMs) )); // from 5 to 125 makes a big difference
  if( (newPeriod1 <= 800) || (newPeriod1 >= 1200) ) {
    newPeriod1 = 1000 + random(10);
    //Serial.println("per1Reset");
  }
  //Serial.println(newPeriod1);
  m_lfoR1.setPeriod( newPeriod1 );


  //
  // Make more flicker by adjusting m_lfoR2's period? Yikes...
  // m_lfoR2 is constructed at 2hz
  //
  unsigned int newPeriod2 = ((int)(m_lfoR2.getPeriod() + getRandomBipolar(jitterMs) )); // from 5 to 125 makes a big difference
  if( (newPeriod2 <= 1300) || (newPeriod2 >= 1700) ) {
    newPeriod2 = 1500 + random(10);
    //Serial.println("per2Reset");
  }
  //Serial.println(newPeriod2);
  m_lfoR2.setPeriod( newPeriod2 );
  
} // method


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

boolean LightControllerX::switchFailMode() {
  return LightController::switchFailMode();
}


boolean LightControllerX::switchNormalMode() {
  
  if( LightController::switchNormalMode() ) {
    return true;
  }

  // otherwise try one of these modes.
  switch( m_normalMode ) {
    
    case MODE_RGB:
    {
      calculateFlicker(4);
      randomizeR1andR2Timebases(1);

      /*
      // The following mappings are perfect, leave them alone.
      // #define MAXBRIGHTNESS 255 and never change it.
      byte aValue  = 0;
      byte mValue  = 0;
      byte gValue  = 0;

      //
      // This lets us see what's signal and what's not.
      //   If m_bDisableFlicker is true then ignore the modulation that was just calculated.
      //
      if( m_bDisableFlicker ) {
        aValue  = map( m_modAttention,  0, 100, 0, MAXBRIGHTNESS );
        mValue  = map( m_modMeditation, 0, 100, 0, MAXBRIGHTNESS );
      } else {
        aValue  = map( m_rndAttention,    0, 100, 0, MAXBRIGHTNESS );
        mValue  = map( m_rndMeditation,   0, 100, 0, MAXBRIGHTNESS );
      }
      //
      // Serial.print("m_rndAttention:");     Serial.print(m_rndAttention);
      // Serial.print(", m_rndMeditation: "); Serial.print(m_rndMeditation);
      // //Serial.print(", %aRV:");     Serial.print(m_rndAttention/100.0);
      // Serial.print(" >> pwm >> aValue:"); Serial.print(aValue);
      // Serial.print(", mValue:"); Serial.println(aValue);
      //
      //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
      //
      // Note that attnPlusMedValue and gValue have an inverse relationship.
      //
      float attnPlusMedValue = m_modAttention + m_modMeditation; // 0-200
      gValue = map((int)attnPlusMedValue, 0, 200,  16, 0); // it doesn't take much green to make it too white.
      //
      //
      // Warmer colors, without green
      m_leds.setColor( NeoPixels::color(aValue, mValue, 0), Pixels::all );
      */
      
      /*
      float aValue = m_rndAttention/100.0f;
      float mValue = m_rndMeditation/100.0f;
      
      if( m_bDisableFlicker ) {
        aValue = m_modAttention/100.0f;
        mValue = m_modMeditation/100.0f;
      }
      
      m_leds.setColor( NeoPixels::scaleColor( getColor1(), aValue),  NEOPIXEL_N4 );
      m_leds.setColor( NeoPixels::scaleColor( getColor1(), aValue),  NEOPIXEL_N3 );
      m_leds.setColor( NeoPixels::scaleColor( getColor2(), mValue),  NEOPIXEL_N2 );
      m_leds.setColor( NeoPixels::scaleColor( getColor2(), mValue),  NEOPIXEL_N1 );
      */


      
      float aValue = m_rndAttention/100.0f;
      float mValue = m_rndMeditation/100.0f;
      
      if( m_bDisableFlicker ) {
        aValue = m_modAttention/100.0f;
        mValue = m_modMeditation/100.0f;
      }
      
      //Serial.print("lc1:"); Serial.print(getColor1()); Serial.print(", lc2:"); Serial.println(getColor2());
      //Serial.print("av:"); Serial.print(aValue); Serial.print(", mv:"); Serial.println(mValue);
      
      uint32_t
        scaledAttnColor = Color::scaleColor( getColor1(), aValue );
      
      uint32_t
        scaledMedColor = Color::scaleColor( getColor2(), mValue );

      uint32_t
        actualColor = Color::blendColors( scaledAttnColor, scaledMedColor );
        //Serial.print("ac:"); Serial.println(actualColor,HEX);

      m_leds.setColor( actualColor );
      
    } break; // MODE_RGB
    
    
    
    case MODE_ATTENTION:
    {
      calculateFlicker();
      randomizeR1andR2Timebases(1);
      
      float aValue = m_rndAttention/100.0f;
      
      if( m_bDisableFlicker ) {
        aValue = m_modAttention/100.0f;
      }

      uint32_t
        actualColor = Color::scaleColor( getColor1(), aValue );

      m_leds.setColor( actualColor );
      
    }
    break; // MODE_ATTENTION
    
    
    
    case MODE_ATTENTION_ACHIEVED:
    {
      calculateFlicker(10); // makes for an interesting transition.
      randomizeR1andR2Timebases();

      uint32_t
        actualColor = Color::scaleColor( getColor1(), m_modAttention/100.0f);

      if( m_rndAttention >= ATTENTION_ACHIEVED_THRESHOLD ) {

        m_leds.setColor( actualColor );
        m_leds.setColor( Color::WHITE, NEOPIXEL_N1 );
        
      } else {
        m_leds.setColor( Color::WHITE );
      }
    } break; // MODE_ATTENTION_ACHIEVED

    
    
    case MODE_MEDITATION:
    {
      calculateFlicker();
      randomizeR1andR2Timebases(1);
      
      float mValue = m_rndMeditation/100.0f;
      
      if( m_bDisableFlicker ) {
        mValue = m_modMeditation/100.0f;
      }
      
      uint32_t
        actualColor = Color::scaleColor( getColor2(), mValue );

      m_leds.setColor( actualColor );
    
    } break; // MODE_MEDITATION
    
    

    case MODE_MEDITATION_ACHIEVED:
    {
      calculateFlicker(10); // makes for an interesting transition.
      randomizeR1andR2Timebases();

      uint32_t
        actualColor = Color::scaleColor( getColor2(), m_modMeditation/100.0f);

      if( m_rndMeditation >= MEDITATION_ACHIEVED_THRESHOLD ) {
        
        m_leds.setColor( actualColor );
        m_leds.setColor( Color::WHITE, NEOPIXEL_N1 );
        
      } else {
        m_leds.setColor( Color::WHITE );
      }
    } break; // MODE_MEDITATION_ACHIEVED


    default: {
      return false;
    } break;
    
  } // switch
  
  // if we get here one of the cases was handled so return true to indicate this.
  return true;
  
} // method



// the end.



