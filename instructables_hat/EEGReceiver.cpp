//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Illumino Project
// Io Flament, Kurt Olsen, Mael Flament
// This code is licensed under Creative Commons Attribution-Non-Commercial-Share Alike 3.0 and GNU GPL license.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

#include "Definitions.h"

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// EEGReceiver
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

EEGReceiver::EEGReceiver()
: m_brain(),
  m_bSimMode(false),
  m_nextSimRecord(0),
  m_firstSimRecord(0),
  m_lastSimRecord(330),
  m_signalQuality(0),
  m_attention(0),
  m_meditation(0),
  m_updateTimer(),
  m_bSimUseLFO(false)
{
} // ctor



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// only do this once please
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGReceiver::setup(HardwareSerial& brainSerial) {
  m_brain.setup(brainSerial);
  m_updateTimer.start(1000);
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// simulation mechanics
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGReceiver::setSimMode(boolean state) {
  m_bSimMode = state;
}

boolean EEGReceiver::getSimMode() {
  return m_bSimMode;
}

boolean EEGReceiver::setSimRange(int firstRecord, int lastRecord) {
  m_nextSimRecord  = firstRecord;
  m_firstSimRecord = firstRecord;
  m_lastSimRecord  = lastRecord;
}

boolean EEGReceiver::setSimRecord(boolean state) {
  m_bSimRecording = state;
  if( m_bSimRecording ) {
    m_firstSimRecord = 0;
    m_lastSimRecord  = 330;
  }
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Note: this serially prints an 's' and the record number.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGReceiver::readNextSimRecord() {
  
  if( m_nextSimRecord > m_lastSimRecord) {
    m_nextSimRecord = m_firstSimRecord;
  }

  unsigned long
    addr = (EEPROM_USERAREA * 2) + (m_nextSimRecord * 3);
    
  //Serial.print(CHAR_s); Serial.print(m_nextSimRecord); Serial.print(CHAR_SPACE); //Serial.print(", addr: "); Serial.println( addr );
  
  if( addr >= 1020 ) {
    // TODO good place for boards stop() and debug
    return;
  }
  
  m_signalQuality = EEPROM.read( addr++ );
  m_attention     = EEPROM.read( addr++ );
  m_meditation    = EEPROM.read( addr++ );

  m_nextSimRecord++;
  if( m_nextSimRecord > m_lastSimRecord) {
    m_nextSimRecord = m_firstSimRecord;
  }

  
  // This replaces the sim data with data generated from the lfo's when enabled
  if( m_bSimUseLFO ) {
  
    LowFreqOsc
      lfo1( 22750, LowFreqOsc::LFO_SIN),
      lfo2( 30632, LowFreqOsc::LFO_SIN);

    float bias = 50.0f;

    m_attention =  lfo1.apply(bias, 100.0 - bias);
    m_meditation = lfo2.apply(bias, 100.0 - bias);
    m_signalQuality = 5; // a nice tag saying we're using the lfo's for data.

  } // use sim lfo?
  
} // method


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Note: This serially prints rec# and flash address
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGReceiver::writeNextSimRecord(byte signalQuality, byte attention, byte meditation) {

//  if( m_nextSimRecord > m_lastSimRecord) {
//    Serial.print("fLastRec: "); Serial.println(m_nextSimRecord);
//    m_nextSimRecord = m_firstSimRecord;
//  }

  unsigned long
    addr = (EEPROM_USERAREA * 2) + (m_nextSimRecord * 3);
  
  if( addr > 1020 ) {
    // TODO good place for boards stop() and debug
    return;
  }

  Serial.print("sRec:"); Serial.print( m_nextSimRecord );
  Serial.print(", flash:"); Serial.println( addr );

  EEPROM.write(addr++, signalQuality );
  EEPROM.write(addr++, attention );
  EEPROM.write(addr++, meditation  );

  
  m_nextSimRecord++;
//  if( m_nextSimRecord > m_lastSimRecord) {
//    m_nextSimRecord = m_firstSimRecord;
//  }

} // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// data retrieval methods
// from brain, or from sim
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

boolean EEGReceiver::update() {

  //simulate here
  if( !m_bSimMode ) {
    boolean gotUpdate = m_brain.update();
    if( gotUpdate && m_bSimRecording) {
      writeNextSimRecord( m_brain.readSignalQuality(), m_brain.readAttention(), m_brain.readMeditation() );
    }
    return gotUpdate;
  }
  
  // othewise, simulate data.
  if( m_updateTimer.isExpired(true) ) {
    readNextSimRecord();
    return true;
  } else {
    return false;
  }

} // method




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

boolean EEGReceiver::isSignalValid() {
  
    boolean retv = getSignalQuality() <= SIGNAL_FAIL_THRESHOLD;  
//    Serial.print("sigQ: "); Serial.println(retv);

  return retv;
}
  
  

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

int EEGReceiver::getSignalQuality() {
  if( !m_bSimMode ) {
    return m_brain.readSignalQuality();
  }
  // otherwise
  return m_signalQuality;
}


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

int EEGReceiver::getAttention() {
  if( !m_bSimMode ) {
    return m_brain.readAttention();
  }
  // otherwise
  return m_attention;
 }


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

int EEGReceiver::getMeditation() {
  if( !m_bSimMode ) {
    return m_brain.readMeditation();
  }
  // otherwise
  return m_meditation;
}
 



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// and get all the rest too. not sim'd yet.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

unsigned long* EEGReceiver::getPowerArray() {
  return m_brain.readPowerArray();
}
unsigned long  EEGReceiver::getDelta() {
  return m_brain.readDelta();
}
unsigned long  EEGReceiver::getTheta() {
  return m_brain.readTheta();
}
unsigned long  EEGReceiver::getLowAlpha() {
  return m_brain.readLowAlpha();
}
unsigned long  EEGReceiver::getHighAlpha() {
  return m_brain.readHighAlpha();
}
unsigned long  EEGReceiver::getLowBeta() {
  return m_brain.readLowBeta();
}
unsigned long  EEGReceiver::getHighBeta() {
  return m_brain.readHighBeta();
}
unsigned long  EEGReceiver::getLowGamma() {
  return m_brain.readLowGamma();
}
unsigned long  EEGReceiver::getMidGamma() {
  return m_brain.readMidGamma();
}

unsigned char EEGReceiver::getLatestError() {
  return m_brain.getLatestError();
}


const byte simdata[] = {
  0,50,40,
  0,83,60,
  0,97,60,
  0,91,66,
  0,84,75,
  0,67,69,
  0,40,74,
  0,40,43,
  0,30,30,
  0,37,29,
  0,47,37,
  0,67,57,
  0,70,53,
  0,83,56,
  0,91,53,
  0,75,53,
  0,77,66,
  0,74,77,
  0,77,74,
  0,83,67,
  0,70,47,
  0,60,40,
  0,53,47,
  0,48,63,
  0,66,77,
  0,78,74,
  0,78,63,
  0,80,61,
  0,66,64,
  0,60,66,
  0,56,80,
  0,57,67,
  0,70,60,
  0,75,70,
  0,75,63,
  0,69,67,
  0,64,70,
  0,64,56,
  0,61,51,
  0,66,40,
  0,66,51,
  0,64,61,
  0,69,61,
  0,77,67,
  0,69,50,
  0,70,43,
  0,84,50,
  0,80,44,
  0,74,47,
  0,75,57,
  0,66,47,
  0,57,47,
  0,66,57,
  0,66,47,
  0,66,48,
  0,66,66,
  0,69,66,
  0,87,61,
  0,70,60,
  0,80,57,
  0,87,57,
  0,81,69,
  0,96,77,
  0,94,75,
  0,77,78,
  0,54,60,
  0,48,70,
  0,50,74,
  0,57,63,
  0,60,81,
  0,64,70,
  0,63,74,
  0,66,69,
  0,61,51,
  0,54,37,
  0,57,34,
  0,70,30,
  0,74,34,
  0,67,43,
  0,64,41,
  0,56,53,
  0,64,53,
  0,67,61,
  0,53,57,
  0,27,43,
  0,40,54,
  0,40,51,
  0,27,38,
  0,54,48,
  0,63,53,
  0,60,56,
  0,93,75,
  0,80,84,
  0,56,81,
  0,57,75,
  0,57,74,
  0,66,60,
  0,74,60,
  0,78,56,
  0,80,51,
  0,81,44,
  0,81,43,
  0,88,41,
  0,77,44,
  0,78,47,
  0,70,56,
  0,64,60,
  0,66,57,
  0,60,77,
  0,67,66,
  0,51,74,
  0,64,77,
  0,67,56,
  0,69,54,
  0,75,50,
  0,60,56,
  0,53,60,
  0,56,54,
  0,74,57,
  0,78,60,
  0,90,61,
  0,100,66,
  0,100,69,
  0,100,61,
  0,100,80,
  0,100,77,
  0,100,69,
  0,100,66,
  0,97,56,
  0,81,63,
  0,75,66,
  0,67,77,
  0,77,66,
  0,66,66,
  0,56,77,
  0,64,67,
  0,57,87,
  0,67,83,
  0,67,77,
  0,77,69,
  0,83,61,
  0,84,57,
  0,81,54,
  0,75,63,
  0,74,57,
  0,74,57,
  0,87,60,
  0,100,40,
  0,100,41,
  0,100,41,
  0,100,44,
  0,83,64,
  0,64,70,
  0,70,69,
  0,61,67,
  0,61,67,
  0,81,54,
  0,77,57,
  0,91,53,
  0,93,54,
  0,91,61,
  0,94,61,
  0,83,70,
  0,87,64,
  0,77,77,
  0,53,66,
  0,53,63,
  0,34,56,
  0,38,44,
  0,44,50,
  0,63,43,
  0,78,50,
  0,75,50,
  0,81,54,
  0,56,51,
  0,53,53,
  0,60,47,
  0,67,41,
  0,77,53,
  0,87,47,
  0,78,60,
  0,70,66,
  0,70,57,
  0,69,64,
  0,64,60,
  0,66,56,
  0,61,57,
  0,63,56,
  0,66,64,
  0,64,75,
  0,74,78,
  0,64,81,
  0,61,70,
  0,60,60,
  0,63,54,
  0,66,47,
  0,70,48,
  0,69,66,
  0,57,74,
  0,51,80,
  0,38,78,
  0,37,63,
  0,30,54,
  0,26,48,
  0,26,50,
  0,27,41,
  0,40,47,
  0,38,53,
  0,41,57,
  0,48,60,
  0,44,61,
  0,56,66,
  0,67,56,
  0,66,63,
  0,64,66,
  0,66,54,
  0,63,63,
  0,70,64,
  0,78,56,
  0,80,63,
  0,84,66,
  0,75,74,
  0,56,75,
  0,64,67,
  0,66,66,
  0,53,60,
  0,67,64,
  0,57,69,
  0,54,70,
  0,54,74,
  0,63,66,
  0,54,60,
  0,50,56,
  0,70,51,
  0,50,63,
  0,57,74,
  0,54,70,
  0,44,75,
  0,51,70,
  0,50,74,
  0,47,69,
  0,47,77,
  0,41,64,
  0,26,61,
  0,40,70,
  0,34,61,
  0,38,75,
  0,38,80,
  0,30,80,
  0,26,74,
  0,27,75,
  0,30,77,
  0,38,78,
  0,48,84,
  0,51,78,
  0,47,60,
  0,43,48,
  0,40,53,
  0,35,57,
  0,43,63,
  0,47,77,
  0,50,83,
  0,50,81,
  0,50,90,
  0,48,81,
  0,44,63,
  0,41,56,
  0,40,48,
  0,44,41,
  0,48,53,
  0,44,61,
  0,37,61,
  0,26,64,
  0,23,61,
  0,27,53,
  0,41,63,
  0,50,66, 
};

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// The best place to call this from is from the very end of HatBoard::setup()
// Otherwise the EEGReceiver may not be ready for this.
// This looks like it clobbers brightness, color1 and color2 in the eeprom.
// Those are user fixable though so I'm not doing antyhing about it yet.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGReceiver::flashSimData() {
  for(int i = 0; i < 277; i++ ) {
    int addr = i * 3;
    writeNextSimRecord(simdata[addr+0], simdata[addr+1], simdata[addr+2]);
  }
}


// the end.


