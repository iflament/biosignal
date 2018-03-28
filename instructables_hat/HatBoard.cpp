//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Illumino Project
// Io Flament, Kurt Olsen, Mael Flament
// This code is licensed under Creative Commons Attribution-Non-Commercial-Share Alike 3.0 and GNU GPL license.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

#include "Definitions.h"


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// HatBoard 
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

HatBoard::HatBoard() 
  : Board(),
    // constant pin assignments
    DIO_OUT_PIXELS(2),
    DIO_IN_DISABLE_FLICKER(9),
    DIO_IN_ENABLE_SIM(10),
    // dynamic pin assignments
    DIO_IN_MODESW(0),
    // other big pieces of the puzzle
    modeSwitch(),
    pixels(),
    eegReceiver(),
    lightController(pixels),
    // misc properties
    m_pMode(0)
{
} // ctor


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void HatBoard::setup(
  BoardType     type,
  int           boardId,
  unsigned  int baudRate,
  unsigned char i2cAddr,
  bool          doHeartbeat,
  bool          debugVerbose ) 
{

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Initialize the hardware first.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


  // *critical*
  Board::setup(type, boardId, baudRate, i2cAddr, doHeartbeat, debugVerbose);

  // The uno and lily use different pins so you may have to adapt this;
  switch( (int)type ) {
    case Uno: 
      DIO_IN_MODESW = 12;
    break;
    case LilyPad328: 
      DIO_IN_MODESW = 3;
    break;
  }

  modeSwitch.setup(DIO_IN_MODESW, eMODE_ENDSTOP);

  // Only on an Uno: Grounding this pin disables the flicker.
  pinMode(DIO_IN_DISABLE_FLICKER, INPUT_PULLUP);

  // Only on an Uno: Grounding this pin enables the eeg simulator.
  pinMode(DIO_IN_ENABLE_SIM, INPUT_PULLUP);

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Random number seeding
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  // i hope this is noise from an unused analog input.
  int rseed = ::analogRead(3);
  randomSeed( rseed );
  //Serial.print("r:"); Serial.println(rseed);


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Initialize the lights and light controller
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  
  //Serial.print("brightness:"); Serial.println(fetchBrightness());
  //Serial.print("color1:"); Serial.println(fetchColor1());
  //Serial.print("color2:"); Serial.println(fetchColor2());

  pixels.setup(NUM_NEOPIXELS, DIO_OUT_PIXELS, NEO_GRB + NEO_KHZ800);
  pixels.setBrightness( fetchBrightness() );
  
  lightController.setColor1( fetchColor1() );
  lightController.setColor2( fetchColor2() );
  
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Initialize the eeg receiver
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  
  eegReceiver.setup(Serial);

  
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// eeg simulator setup
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  eegReceiver.setSimRange ( 0, 277 );
//eegReceiver.loadSimDataAndStop();
//eegReceiver.setSimRecord( true   );
  eegReceiver.setSimMode  ( true  ); // this turns on and off simulaion mode. Ture is ON, false is OFF.
  eegReceiver.setSimUseLFO( true   );
  
  
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Final setup
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  
  lightController.doStartupBling();
  Serial.println(FIRMWARE_VERSION);
  setMode( eMODE_RGB );
  
  
} // method







//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// This is called by baseclass Board::setup()
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void HatBoard::eepromPreload() {

  //Serial.println("preload");

  // let's do this first since it clobbers brightness, color1 and color2
  eegReceiver.flashSimData();

  // these overwrite sim data but so what?
  stashBrightness((float)MAXBRIGHTNESS);
  stashColor1(Color::GREEN);
  stashColor2(Color::RED);
  
  // allow the developer (using an Uno) to see this occured.
  pulseD13(8);

} // method




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Changes mode, usually in response to button pushes
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void HatBoard::setMode(int eMode) {

  lightController.reset();

  delete m_pMode;
  m_pMode = NULL;

  //Serial.print("M:"); Serial.println(m_iMode);
  switch( eMode ) {
  case eMODE_RGB: 
    m_pMode = new EEGModeRGB(this);
    //Serial.println("mRGB");
    break;
  case eMODE_ATTENTION: 
    m_pMode = new EEGModeAttention(this); 
    break;
  case eMODE_ATTENTION_ACHIEVED: 
    m_pMode = new EEGModeAttentionAchieved(this); 
    break;
  case eMODE_MEDITATION: 
    m_pMode = new EEGModeMeditation(this); 
    break;
  case eMODE_MEDITATION_ACHIEVED: 
    m_pMode = new EEGModeMeditationAchieved(this); 
    break;
  case eMODE_BRIGHTNESS:
    m_pMode = new EEGModeBrightness(this);
    break;
  case eMODE_COLORSELECT:
    m_pMode = new EEGModeColorSelect(this);
    break;
  case eMODE_ACCESSORY:
    m_pMode = new EEGModeAccessory(this);
    break;
  } //switch
	
	
} // method


 

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

 float pwmToBrightness(word pwm) {
   return pwm / (float)MAXBRIGHTNESS;
 }
 
 
 //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
 //
 //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
 
 word brightnessToPwm(float brightness) {
   return ((float)MAXBRIGHTNESS) / brightness;
 }


 //
 // Has two constraints, one is MIN_FLASH_PWM_BRIGHTNESS and 255.
 //  The other is that it reads flash first and only writes when different.
 //
 void HatBoard::stashBrightness(float brightness)  {
   
   word newFlashVal = constrain(brightness * MAXBRIGHTNESS, MIN_FLASH_PWM_BRIGHTNESS, MAXBRIGHTNESS);
   word curFlashVal = Flash::ifetch(FLASH_BRIGHTNESS);
   
   //char* str_flash = "flash";
   
   if( newFlashVal != curFlashVal ) {
     Flash::istash(FLASH_BRIGHTNESS, newFlashVal); 
     //Serial.print(CHAR_PLUS); Serial.print(str_flash); Serial.print(CHAR_SPACE); Serial.println(newFlashVal);
   } else {
     //Serial.print(CHAR_MINUS); Serial.println(str_flash);
   }
 
 } // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// returns brightness, range 0.0 to 1.0
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

float HatBoard::fetchBrightness() {
  word pwm = Flash::ifetch(FLASH_BRIGHTNESS);
  float retv = pwmToBrightness(pwm);
  //Serial.print("flashBr:"); Serial.print(pwm); Serial.print(", float:"); Serial.println(retv);
  return retv;
}


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// saves the value in flash
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void HatBoard::stashColor1(uint32_t value) {

  word r  = (value >> 16) & 0x0000FFFF;
  word gb = value & 0x0000FFFF;

  if( Flash::ifetch(FLASH_COLOR1R) != r) {
    Flash::istash(FLASH_COLOR1R, r);
  }
  if( Flash::ifetch(FLASH_COLOR1GB) != gb) {
    Flash::istash(FLASH_COLOR1GB, gb);
  }

} // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// returns the flashed value
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

uint32_t HatBoard::fetchColor1() {
  return ((uint32_t)Flash::ifetch(FLASH_COLOR1R) << 16) | Flash::ifetch(FLASH_COLOR1GB);
}




//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// saves the value in flash
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void HatBoard::stashColor2(uint32_t value) {

  word r  = (value >> 16) & 0x0000FFFF;
  word gb = value & 0x0000FFFF;

  if( Flash::ifetch(FLASH_COLOR2R) != r) {
    Flash::istash(FLASH_COLOR2R, r);
  }
  if( Flash::ifetch(FLASH_COLOR2GB) != gb) {
    Flash::istash(FLASH_COLOR2GB, gb);
  }

} // method


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// returns the flashed value
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

uint32_t HatBoard::fetchColor2() {
  return ((uint32_t)Flash::ifetch(FLASH_COLOR2R) << 16) | Flash::ifetch(FLASH_COLOR2GB);
}








//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// This notices and handles somebody pressing the mode switch button.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

void HatBoard::handleModeSwitch() {

  if( !modeSwitch.checkModeSwitch() ) {
    return;
  }

  //
  // Advance to next mode.
  // If in brightness changing mode then stash the current brightness in flash.
  //
  if( modeSwitch.getPrevMode() == eMODE_BRIGHTNESS ) {
    stashBrightness( pixels.getBrightness() );
  }
  
  //
  // This locks down the the "color1 & color2" values that lightController should have been cycling through.
  //
  if( (modeSwitch.getPrevMode() == eMODE_COLORSELECT) ) {
    
      stashColor1( lightController.getColor1() );
      stashColor2( lightController.getColor2() );
    
//      Serial.print("fc1:");   Serial.print( fetchColor1(), HEX );
//      Serial.print(", fc2:"); Serial.println( fetchColor2(), HEX );
  }

  setMode( modeSwitch.getCurMode() );
  
  // end advance to next mode
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

} // method





//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// This is where most of the work is done.
// It will halt the app if no mode is instantiated.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
 
void HatBoard::loop() {

  // These are useful during development.
  
  switch( (int)board() ) {
    case Uno: // the uno I use in dev
      lightController.setDisableFlicker( !digitalRead(DIO_IN_DISABLE_FLICKER)  );
      eegReceiver.setSimMode( !digitalRead(DIO_IN_ENABLE_SIM) );
    break;
  }
  

  handleModeSwitch();

  if( m_pMode == NULL ) {
    errstop(UserError_NullMode, Serial);
  }
  //
  // otherwise...
  //
  //\/\/\/\/\/\/\/\/
  m_pMode->loop();
  //\/\/\/\/\/\/\/\/

} // method


// the end.


