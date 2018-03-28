//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Illumino Project
// Io Flament, Kurt Olsen, Mael Flament
// This code is licensed under Creative Commons Attribution-Non-Commercial-Share Alike 3.0 and GNU GPL license.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

#include "Definitions.h"

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// An EEGMode is pretty much the brains of the app.
// This is the class that decides if the receiver
// is working, deals with the modes, handles
// common activities etc.
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

EEGMode::EEGMode(HatBoard* _pBoard) : m_pBoard(_pBoard) {
  t_initDelay.start(2000);
}


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// All the modes send the same serial data
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGMode::sendSerialData() {


  Serial.print( m_pBoard->eegReceiver.getSignalQuality() );
  Serial.print( CHAR_COMMA );

  Serial.print( m_pBoard->eegReceiver.getAttention() );
  Serial.print( CHAR_COMMA );

  Serial.print( m_pBoard->eegReceiver.getMeditation() );
  Serial.print( CHAR_COMMA );

  Serial.print( m_pBoard->getMode() );
  Serial.print( CHAR_COMMA );
  
  Serial.print( m_pBoard->fetchBrightness() );
  Serial.print( CHAR_COMMA );
  
  Serial.print( m_pBoard->fetchColor1(), DEC );
  Serial.print( CHAR_COMMA );

  Serial.print( m_pBoard->fetchColor2(), DEC );
  Serial.println();

} // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Useful as a diagnostic, not used in production mode.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGMode::brainDump() {
  
// Serial.print("err: "); Serial.println(m_pBoard->eegReceiver.getLatestError() );
//    Serial.println(m_board.eegReceiver.readCSV());
//    m_board.eegReceiver.printDebug();
  
  Serial.print  (m_pBoard->eegReceiver.getSignalQuality());
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getAttention()    );
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getMeditation()   );
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getDelta()        );
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getTheta()        );
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getLowAlpha()     );
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getHighAlpha()    );
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getLowBeta()      );
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getHighBeta()     );
  Serial.print  (CHAR_COMMA);
  Serial.print  (m_pBoard->eegReceiver.getLowGamma()     );
  Serial.print  (CHAR_COMMA);
  Serial.println(m_pBoard->eegReceiver.getMidGamma()     );

} // method  




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Indicates an eeg data update failure
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGMode::handleUpdateFailure() {
  if( !t_updateFailure.isStarted() ) {
    t_updateFailure.start(UPDATE_FAIL_DELAYMS);
    //Serial.println("-upTS");
  } else {
    if( t_updateFailure.isExpired(true) ) {
      // isExpired(false) == full loop rate, very fast.
      // isExpired(true)  == call rate is timer rate (~2hz)
      m_pBoard->lightController.setFailUpdate();
      //Serial.println("-up");
    }
  }
} // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// resets timers after an update failure
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGMode::resetAfterUpdateFailure() {
  t_updateFailure.stop();       
  m_pBoard->lightController.resetFailMode();
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Indicates poor quality signal
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGMode::handleSignalFailure() {
  if( !t_signalFailure.isStarted() ) {
    t_signalFailure.start(SIGNAL_FAIL_DELAYMS);
    //Serial.println("-sigTS");
  } else {
    if( t_signalFailure.isExpired(false) ) {
      // isExpired(false) == call rate is ~1hz, after a valid update.
      // isExpired(true)  == call rate is timer rate (~2hz),
      m_pBoard->lightController.setFailSignal();
      //Serial.println("-sig");
    }
  }
} // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Resets timers after a signal failure
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGMode::resetAfterSignalFailure() {
  t_signalFailure.stop();       
  m_pBoard->lightController.resetFailMode();
}





//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Called from HatBoard::loop()
// This is what does the majority of this apps work.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void EEGMode::loop() {
    
  // always do this, and remember what happened.
  boolean bNewData = m_pBoard->eegReceiver.update();
  //Serial.print("bNewData: "); Serial.println(bNewData);

  // Don't do anything right after construction when the pixels
  // which the constructor turned on are indicating the current mode.
  if( t_initDelay.isStarted() ) {
    if( t_initDelay.isExpired(false) ) {
      t_initDelay.stop();
    }
    // calling update helps keep me in sync.
    // when it's not called you get a checksum error after you change modes.
    // that's because of each mode having a startup delay (2sec) when update was being ignored.
    // calling this helps stop that checkum error. ok.
    
    return;
  } // init delay
  
  
  // Expect packets about once per second.
  // The .readCSV() function returns a string (well, char*) listing the most recent brain data, in the following format:
  // "signal strength, attention, meditation, delta, theta, low alpha, high alpha, low beta, high beta, low gamma, high gamma"	

  // brite or connection level modes?
  if((m_pBoard->getMode() == eMODE_BRIGHTNESS) 
  || (m_pBoard->getMode() == eMODE_COLORSELECT)
  || (m_pBoard->getMode() == eMODE_ACCESSORY))
  {
    m_pBoard->lightController.resetFailMode();
    indicateMode();
  
  } else {
  
    if( !bNewData /*m_pBoard->eegReceiver.update()*/ ) {
      handleUpdateFailure();
    } else {
      resetAfterUpdateFailure();

      if( !m_pBoard->eegReceiver.isSignalValid() ) {
        handleSignalFailure();
      } else {
        resetAfterSignalFailure();
        indicateMode();
      }

    } // update
    
  } // brite or connection level modes?

  if( bNewData ) {
    sendSerialData();
  }
} // eegLoop
  

// the end.



