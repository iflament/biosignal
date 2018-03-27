
#include "LowFreqOsc.h"


//
// LowFreqOsc
//
LowFreqOsc::LowFreqOsc()
: m_period(1000),
  m_function(LFO_SIN),
  m_bInverted(false),
  m_modAmplitude(1.0f),
  m_startMillis(0)
{
}


//
// LowFreqOsc
//
LowFreqOsc::LowFreqOsc(unsigned long periodMs, LfoFunction function, bool inverted)
: m_period(periodMs),
  m_function(function),
  m_bInverted(inverted),
  m_modAmplitude(1.0f),
  m_startMillis(0)
{
    
}


//
// used internally
//
float LowFreqOsc::saw(float radians) {
  return radians/(M_PI * 2);
}

//
// used internally
//
float LowFreqOsc::sqw(float radians) {
	return (radians < M_PI) ? -1.0f : 1.0f;
}


//
// This is the short form of the LFO
// and must be indentical to readVerbose()
//
float LowFreqOsc::getMod(unsigned long atMs) {

  float
    retv = 0.0f;
  
  switch( m_function ) {

    case LFO_SIN:
      //Serial.println("*SIN@*");
      retv = sin( (float)(atMs % m_period) * ((2 * M_PI) / m_period) );
    break;

    case LFO_COS:
      //Serial.println("*COS@*");
      retv = cos( (float)(atMs % m_period) * ((2 * M_PI) / m_period) );
    break;

    case LFO_SAW:
      //Serial.println("*SAW@*");
      retv = saw( (float)(atMs % m_period) * ((2 * M_PI) / m_period) );
    break;

		case LFO_SQW:
			retv = sqw( (float)(atMs % m_period) * ((2 * M_PI) / m_period) );
		break;

  } // switch

  if( m_bInverted ) {
    return invertSign(retv);
  } else {
    return retv;
  }

} // method




//
// applies (adds) modulation to signal.
//
float LowFreqOsc::apply(float signalLevel, float modAmplitude, unsigned long atMs) {
  
  //Serial.print("   sig:"); Serial.print(signalLevel);
  //Serial.print(",  amp:"); Serial.print(modAmplitude);
  //Serial.print(", atMs:"); Serial.print(atMs);
  //Serial.print(", mod@:"); Serial.print(getMod(atMs));
  
  float retv = signalLevel + (getMod(atMs) * modAmplitude);
  
  //Serial.print(", retv:"); Serial.println(retv);
  
  return retv;
}

float LowFreqOsc::apply(float signalLevel) {
  
  float retv = signalLevel + (getMod(this->millis()) * this->getModAmplitude() );
  
  //Serial.print(", retv:"); Serial.println(retv);
  
  return retv;
}

//
// 1hz equals 1000ms, give 1, get 1000.
//
unsigned long LowFreqOsc::freqToPeriod(float hz) {
  return ((1.0f/hz) * 1000);
}



//
// Inverts the modulation's sign.
//
float LowFreqOsc::invertSign( float modulation ) {
  float mod180 = 0;
  if( modulation > 0 ) {
    mod180 = -1 * abs(modulation);
  } else if(modulation < 0) {
    mod180 =  1 * abs(modulation);
  }
  return mod180;
}



/*
//
// This is the long form of the LFO
// and must be identical to read()
//
float LowFreqOsc::readVerbose(unsigned long tNowMillis, unsigned long tCycleMs) {
  
  // iCycle is a zero-relative index into an imaginary
  // low frequence oscillator cycle. The value of iCycle
  // can be thought of as an index into a waveform sample
  // representing the state of an LFO whose period is tCycleMs.
  //
  // In use, you'd see iCycle rising until it recycles at tCycleMs.
  //
  unsigned long
  iCycle = tNowMillis % tCycleMs;
  
  // I want a modulation output ranging from -1 to +1
  // The sin() function conviently does that for me.
  // But sin() expects radians as input, zero thru 2pi.
  // Thus tCycleMs must be scaled to 2pi.
  //
  // Create my scalar. Time to pi, time to radians, whatevah.
  float
  timeToPiScaler = (2 * M_PI) / tCycleMs;
  
  
  // Apply the scaling to iCycle, yielding an argument
  //  suitable for use by the sin() function.
  float
  sinArg = (float)iCycle * timeToPiScaler;
  
  
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //
  // Modulation ranges from -1.0 thru +1.0 and indicates
  //  the amplitude of the modulation at the given point in time.
  //
  //   A sinewave is the core LFO waveform.
  //    Other periodic waveforms can be derived from it.
  //
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  float modulation = sin(sinArg);
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  
  return modulation;
  
} // method
*/

// the end






