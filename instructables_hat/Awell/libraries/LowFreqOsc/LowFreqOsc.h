
//
//  Author: Kurt Olsen, kurt6string@gmail.com
//
//

#ifndef LowFreqOsc_h
#define LowFreqOsc_h

#include <Arduino.h>
//#include <Common.h>
/*

  // 1hz osc, using sin()
  LowFreqOsc   
    lfo( 1000, LowFreqOsc::LFO_SIN );
 
  When using lfo.applyAt(signalLevel, modAmplitude, atMs)
  
  and starting atMs of zero, the value returned will start at signalLevel
  and successive values will travel towards the sign of the modAmplitude.
  At maximum modulation the delta from signalLevel is no more than modAmplitude
  in either direction.
  
  We see from the above that LFO_SIN begins modulation at min modulation and goes up.

  If we had chosen LFO_COS then the difference is that the very first value returned
  will be at max modulation (atMs = 0), and successive values the modulation decreases
  until reaching signalLevel at which point it reverses.
 
 
  // Called from the loop(), returns 0 to 1.0f in sequence. A sin increase.
  //  using positive modAmplitude
  //
  lfo1.applyAt( 0.0f, 1.0f, board.loopMillis() )
  
  // Called from the loop(), returns 0 to -1.0f in sequence. A sin decrease.
  // Using a negative modAmplitude.
  lfo1.applyAt( 0.0f, -1.0f, board.loopMillis() )
  
  
  Note to self:
    Given I have SIN, and I can invert the modulation..why do I need COS?
 
*/


class LowFreqOsc {
public:
  
  enum LfoFunction {
    LFO_SIN,
    LFO_COS,
    LFO_SAW,
		LFO_SQW,
  };
  
  // defaults to 1hz, sin, non-inverted
  LowFreqOsc();

  // specify everything, change later if desired
  LowFreqOsc(unsigned long periodMs, LfoFunction function, bool inverted = false);
  
  //
  // configuration, can be done on the fly, which is an advanced usage - be careful with that.
  //
  void          setPeriod        (unsigned long periodMs)  { m_period = periodMs; }
  unsigned long getPeriod        ()                        { return m_period; }
  
  void          setFunction      (LfoFunction function)    { m_function = function; }
//LfoFunction   getFunction      ()                        { return m_function; }
  
  void          setInverted      (bool state)              { m_bInverted = state; }
//bool          isInverted       ()                        { return m_bInverted;  }
  
  void          setModAmplitude  (float modAmplitude)      { m_modAmplitude = modAmplitude; }
  float         getModAmplitude  ()                        { return m_modAmplitude; }

  //
  // These two are tricky. They're used to abstractly set an lfo to restart to 'now'.
  // if you call setStartMillis, and then use LowFreqOsc::millis() in your application
  // then you get the modulation as it should be since your setStartMillis() call.
  // If you just use the arduino's ::millis() in your getMod() calls (the default)
  // then the LFO's are giving you the modulation as it should be since the arduino
  // was restarted. Also note that at loop #0, that ::millis() might return something
  // like 500ms - which might be the boot sequence delay time. It's like the arduino
  // resets it's millis immediately on powerup, then the boot seq delay occurs,
  // then finally the arduino loop begins running, so the first ::millis() call isn't zero.
  // thus, all your LFO's start when ::millis() does - or - you use these.
  //
  void          setStartMillis   ()                        { m_startMillis = ::millis(); }
  unsigned long millis           ()                        { return ::millis() - m_startMillis; }
  
  
  
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  // This is the short form of the algorithm.
  //  and must be indentical to readVerbose()
  // It defaults to using ::millis()
  //  remember that I also have millis()
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  float getMod(unsigned long atMs = ::millis() );


  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  // apply (add) modulation at specific time.
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  float apply(float signalLevel, float modAmplitude, unsigned long atMs = ::millis() );
  // the next one cheats by using onboard values.
  float apply(float signalLevel);
  

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  // Inverts the sign of the modulation. 
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  float invertSign( float modulation );


  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  // 1hz equals 1000ms, give 1, get 1000.
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  static unsigned long freqToPeriod(float hz);
  
  
private:

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  // This is the long form of the LFO and must
  //  be functionally indentical to getMod()
  // I keep it so that I can remember how
  //  this thing works.
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //float readVerbose(unsigned long tNowMillis, unsigned long tCycleMs);
  
	float saw(float radians);
	float sqw(float radians);
	
private:

  unsigned long m_period;
  LfoFunction   m_function;
  bool          m_bInverted;
  float         m_modAmplitude;
  unsigned long m_startMillis;

}; // class


#endif // LowFreqOsc_h



