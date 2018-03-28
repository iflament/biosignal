//
// IlluminoDuino GZLL DEVICE, Port DB00BXRS
// RFduino       GZLL   HOST, Port DN00091L6
// 

//
// Master list of #includes. keep Definitions.h synchronized with it.
//
#include <RFduinoBLE.h>
#include <RFduinoGZLL.h>
#include <Board.h>
#include <Flash.h>s
#include <Averager.h>
#include <Timer.h>
#include <LowFreqOsc.h>
#include <Color.h>
#include <Leds.h>
#include <WS2812_RFDuinoDriver.h>
#include <LightCycles.h>
#include <Wire.h>
#include <LSM303.h>
#include <TinyAccel.h>
#include <ModeSwitch.h>
#include <RFduinoGZLLStream.h>
#include "Definitions.h"


/*
Product names:
    IlluminoDuino - core cpu board.
    IlluminoDuino w/Muscle Shield
    IlluminoDuino w/Heart Shield
    IlluminoDuino w/EEG Shield

*/

/*
  RFduino processor pins
  -----------------------------------------
  pin1  gnd
  pin2  gnd
  pin3  gpio2  sda-accelerometer & rtc
  pin4  gpio3  scl-accelerometer & rtc
  pin5  gpio4  neopixel top ring, 6pin hdr4
  pin6  gpio5  mode switch,
  pin7  gpio6  microphone input,  6pin hdr6
  pin8  gnd    
  pin9  gnd
  pin10 gnd
  pin11 external antenna
  pin12 gnd
  pin13 +3.3   
  pin14 reset,  connected to ftdi chip
  pin15 factory?
  pin16 gpio0  TXData (hardware uart)
  pin17 gpio1  RXData (hardware uart)
  pin18 gnd
  pin19 gnd

  6pin header pins:
  -----------------------------------------
  1 - battery voltage, raw, no regulation
  2 - regulated 3.3v
  3 - gnd
  4 - gpio4, neopixel top ring
  5 - no-jumper=top ring last neopixel data out, jumper=gpio4
  6 - microphone, gpio6

  Arduino pins (same as gpio pin numbers);
  -----------------------------------------
  pin 0  output - TXData (hardware uart)
  pin 1  input  - RXData (hardware uart)
  pin 2  i/o    - SDA, accelerometer & rtc
  pin 3  i/o    - SCL, accelerometer & rtc
  pin 4  output - Neopixel, top ring (6pin hdr4)
  pin 5  input  - Mode switch
  pin 6  input  - Microphone
*/


const uint8_t txPin = 0;
const uint8_t rxPin = 1;
const uint8_t sdaPin = 2;
const uint8_t sclPin = 3;
const uint8_t neoPin = 4;
const uint8_t modeswPin = 5;
const uint8_t analogPin = 6;

#define stream Serial
//#define stream gazelle
#define IS_DEV          false
#define IS_SN1_DEVICE   false
#define IS_SN1_HOST     true

#if IS_DEV
#define USE_GAZELLE         true
#define GAZELL_ROLE         DEVICE0
#define stream              gazelle
#define BRIGHTNESS          24
#define FORCE_SHOW          false
#define SEND_LOOPFREQ       true
#define ACCEL_ENABLED       false
#define ACCEL_SWITCHG       1.40
#define ACCEL_SEND_G        false
#define ACCEL_SEND_MINMAX   false
#define ACCEL_SEND_HDG      false
#define SEND_TELEMETRY      false
#define SEND_HEART          false
#define HEART_BPM_MS        30000
#endif

#if IS_SN1_DEVICE
#define USE_GAZELLE         true
#define GAZELL_ROLE         DEVICE0
#define stream              gazelle
#define BRIGHTNESS          24
#define FORCE_SHOW          false
#define SEND_LOOPFREQ       true
#define ACCEL_ENABLED       true
#define ACCEL_SWITCHG       1.40
#define ACCEL_SEND_G        false
#define ACCEL_SEND_MINMAX   false
#define ACCEL_SEND_HDG      false
#define SEND_TELEMETRY      false
#define SEND_HEART          false
#define HEART_BPM_MS        30000
#endif

#if IS_SN1_HOST
#define USE_GAZELLE         false
#define GAZELL_ROLE         HOST
#define stream              Serial
#define BRIGHTNESS          255
#define FORCE_SHOW          false
#define SEND_LOOPFREQ       true
#define ACCEL_ENABLED       true
#define ACCEL_SWITCHG       1.40
#define ACCEL_SEND_G        false
#define ACCEL_SEND_MINMAX   false
#define ACCEL_SEND_HDG      false
#define SEND_TELEMETRY      true
#define SEND_HEART          false
#define HEART_BPM_MS        30000
#endif


RFduinoGZLLStream
        gazelle;
uint32_t
        loopCount = 0;
uint16_t
        systemMode = 0;
boolean
        bSentNameAndVersion = false;
device_t
        role = GAZELL_ROLE;

// RFduinoGZLLStream
// gazelle;

Board
        board;

TinyAccel
        accel(stream);

char report[80];

WS2812_RFDuinoDriver
        pixels(/*19*/ 39, neoPin, 3000);

Leds *
        leds = &pixels;

//RTC_DS1307
//    rtc;


Timer
        oneSec(1000);

Timer
        gazelleTimer(100);

Timer
        modeSwitchTimer;

Timer
        accelTimer(20);

Timer
        telemetryTimer(50);

// pin 6 output rate
Timer
        heartTimer(1000);

uint16_t
        heartSignal;

uint16_t
        pin6Threshold = 640;

boolean
        pin6Raw = false;

LowFreqOsc
        lfoHeart;


ColorChaser
        chaser(*leds, 100);

StrobeLight
        strobe(*leds, 500, Color::WHITE);

RainbowCycle
        rainbowCycle(*leds, 10);

BrightnessChanger
        brightnessChanger(*leds);

static boolean
        bChaseCompleted = false;

const uint8_t
        MAXBRIGHTNESS = 255;

uint8_t XColor = Color::BLACK;
uint8_t YColor = Color::BLACK;
uint8_t ZColor = Color::BLACK;

uint32_t solidColor = Color::GREEN;

float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


// Reads nChars (a decimal number) from the serial port, then parses and returns the result.
int parseSerialDecimal(int nChars) {

    char valBuf[nChars + 1];

    for (int i = 0; i < nChars; i++) {
        valBuf[i] = stream.read();
        if (USE_GAZELLE && (role == HOST)) {
            gazelle.write(valBuf[i]);
        }

    }

    valBuf[nChars] = 0;
    return atoi(valBuf);

} // method


// Reads nChars (a hex number) from the serial port, then parses and returns the result.
int parseSerialHex(int nChars) {

    char valBuf[nChars + 1];

    for (int i = 0; i < nChars; i++) {
        valBuf[i] = stream.read();
        if (USE_GAZELLE && (role == HOST)) {
            gazelle.write(valBuf[i]);
        }
    }

    valBuf[nChars] = 0;

    int value = 0;
    sscanf(valBuf, "%x", &value);

    return value;

} // method


// literal char's consume ram, these don't
#define CHAR_SPACE     (char)32
#define CHAR_LPAREN    (char)40
#define CHAR_RPAREN    (char)41
#define CHAR_STAR      (char)42
#define CHAR_PLUS      (char)43
#define CHAR_COMMA     (char)44
#define CHAR_MINUS     (char)45
#define CHAR_FWDSLASH  (char)47
#define CHAR_0         (char)48
#define CHAR_1         (char)49
#define CHAR_2         (char)50
#define CHAR_3         (char)51
#define CHAR_4         (char)52
#define CHAR_5         (char)53
#define CHAR_6         (char)54
#define CHAR_7         (char)55
#define CHAR_8         (char)56
#define CHAR_9         (char)57
#define CHAR_QUESTION  (char)63
#define CHAR_A         (char)65
#define CHAR_C         (char)67
#define CHAR_I         (char)73
#define CHAR_L         (char)76
#define CHAR_P         (char)80
#define CHAR_R         (char)82
#define CHAR_S         (char)83
#define CHAR_a         (char)97
#define CHAR_b         (char)98
#define CHAR_c         (char)99
#define CHAR_d        (char)100
#define CHAR_e        (char)101
#define CHAR_f        (char)102
#define CHAR_g        (char)103
#define CHAR_h        (char)104
#define CHAR_k        (char)107
#define CHAR_l        (char)108
#define CHAR_m        (char)109
#define CHAR_r        (char)114
#define CHAR_s        (char)115
#define CHAR_v        (char)118
#define CHAR_w        (char)119
#define CHAR_x        (char)120

// The readSerialPort method looks for and executes one command per-call.
void readSerialPort() {

    if (!stream.available()) {
        return;
    }

//  Serial.print("+serial ");
//  Serial.println(stream.available());

    // msg switch
    if (stream.peek() == CHAR_m && (stream.available() >= 3)) {

        doSerialModeCommand();

    } else if (stream.peek() == CHAR_f && (stream.available() >= 3)) {

        //doSerialFormatCommand();

    } else if (stream.peek() == CHAR_v && (stream.available() >= 10)) {

        doSerialVariableCommand();

    } else if (stream.peek() == CHAR_h && (stream.available() >= 11)) {

        doSerialVariableHexCommand();

    } else {

        // unrecognized message, eat all the characters
        if (stream.available() > 10) {
            while (stream.available()) {
                stream.read();
            }
        } // too many characters?
    }


    // msg switch

    // need this delay or misses messages...
    //delay(15);


} // method


void doSerialModeCommand() {

    char c = stream.read(); // I already know it's an 'm' and it's not needed.
    if (USE_GAZELLE && (role == HOST)) {
        gazelle.write(c);
    }

    // ModeQuery?

//  if( stream.read() == CHAR_QUESTION ) {
//    verbosePrintln("mquery");
//    serialTransmitSystemMode();
//  } 
//  else {

    // ModeSet
    word mode = parseSerialDecimal(2);
    //stream.print("MODE:"); stream.println(mode);
    systemMode = mode;

//  }

} // method


void doSerialVariableCommand() {

    char c = stream.read();
    if (USE_GAZELLE && (role == HOST)) {
        gazelle.write(c);
    }


    if (stream.peek() == CHAR_QUESTION) {

        // VariableQuery
        c = stream.read(); // throw away the '?'
        if (USE_GAZELLE && (role == HOST)) {
            gazelle.write(c);
        }

        int index = parseSerialDecimal(3);

//    word value = getFlashWord(index);
//    verbosePrintln("vquery");
//    serialTransmitVariableSet(index, value);

    } else {

        // VariableSet
        int index = parseSerialDecimal(3);

        c = stream.read(); // throw away the '='
        if (USE_GAZELLE && (role == HOST)) {
            gazelle.write(c);
        }

        word value = parseSerialDecimal(5);
//    if( isDebugMode(DEBUG_VERBOSE) ) {
//      stream.print("vset ");
//      stream.print(index);
//      stream.print(CHAR_FWDSLASH);
//      stream.println(value);
//    }

        if (index > 255) {
//      if( isDebugMode(DEBUG_VERBOSE) ) {
//        errMsg(ERR_VARIABLESET_OUTOFRANGE);
//      }
        } else {

            boolean bSet = false;

            switch (index) {

                case 0: // brightness
                    leds->setBrightness(value);
                    bSet = false;
                    break;

                case 1: // persistence enable
                    ((WS2812_RFDuinoDriver *) leds)->setPersistenceEnable((boolean) value);
                    bSet = false;
                    break;

                case 2: // persistence interval
                    ((WS2812_RFDuinoDriver *) leds)->setPersistenceInterval((uint16_t) value);
                    bSet = false;
                    break;

                case 3: // persistence delta
                    ((WS2812_RFDuinoDriver *) leds)->setPersistenceDelta((uint8_t) value);
                    bSet = false;
                    break;

                case 4: // ColorChaser dontErase
                    chaser.dontErase = (boolean) value;
                    bSet = false;
                    break;

                case 5: // ColorChaser reversable
                    chaser.reversable = (boolean) value;
                    bSet = false;
                    break;

                case 6: // ColorChaser variableRate
                    chaser.variableRate = (boolean) value;
                    bSet = false;
                    break;

                case 7: // ColorChaser variableBrightness
                    chaser.variableBrightness = (boolean) value;
                    bSet = false;
                    break;

                case 8: // ColorChaser rate - max
                    chaser.rateMax = (uint16_t) value;
                    bSet = false;
                    break;

                case 9: // ColorChaser rate - min
                    chaser.rateMin = (uint16_t) value;
                    bSet = false;
                    break;

                case 10: // ColorChaser rate - color
                    chaser.lfoColor.setPeriod((uint16_t) value);
                    bSet = false;
                    break;

                case 11: // ColorChaser variable rate lfo
                    chaser.lfoRate.setPeriod((uint16_t) value);
                    bSet = false;
                    break;

                case 12: // strobe rate
                    strobe.timer1.setInterval((uint16_t) value);
                    bSet = false;
                    break;


                case 15: // pin6 threshold
                    pin6Threshold = (uint16_t) value;
                    bSet = true;
                    break;

                case 16: // pin6 raw
                    pin6Raw = (uint16_t) value;
                    bSet = true;
                    break;


                default:
                    break;

            } // switch

            if (bSet) {
                stream.print("set ");
                stream.print(index);
                stream.print(" = ");
                stream.println(value);
            }
        } // index in range?
    } // query or set?
} // method


void doSerialVariableHexCommand() {

    char c = stream.read(); // I already know it's a 'h' and it's not needed.
    if (USE_GAZELLE && (role == HOST)) {
        gazelle.write(c);
    }

    if (stream.peek() == CHAR_QUESTION) {

        // VariableQuery
        c = stream.read(); // throw away the '?'
        if (USE_GAZELLE && (role == HOST)) {
            gazelle.write(c);
        }

        int index = parseSerialDecimal(3);

//    word value = getFlashWord(index);
//    verbosePrintln("vquery");
//    serialTransmitVariableSet(index, value);

    } else {

        // VariableSet
        int index = parseSerialDecimal(3);

        char c = stream.read(); // throw away the '='
        if (USE_GAZELLE && (role == HOST)) {
            gazelle.write(c);
        }

        uint32_t value = parseSerialHex(6);

//    stream.print("HEX: "); stream.println(value, HEX);

        if (index > 255) {
//      if( isDebugMode(DEBUG_VERBOSE) ) {
//        errMsg(ERR_VARIABLESET_OUTOFRANGE);
//      }
        } else {

            boolean bSet = false;


            switch (index) {

                case 13: // strobe Color
                    strobe.color = value;
                    bSet = false;
                    break;

                case 14: // solid Color
                    solidColor = value;
                    bSet = false;
                    break;

                default:
                    break;

            } // switch

            if (bSet) {
                stream.print("set ");
                stream.print(index);
                stream.print(" = ");
                stream.println(value);
            }

        } // index in range?

    } // query or set?

} // method


void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
    gazelle.onReceive(device, rssi, data, len);
}


void sendTelemetry() {

    stream.print("xyzg:");
    stream.print(accel.getXG());
    stream.print(CHAR_COMMA);
    stream.print(accel.getYG());
    stream.print(CHAR_COMMA);
    stream.print(accel.getZG());
    stream.print(CHAR_COMMA);
    stream.print(analogRead(analogPin));
    stream.println();

}


void setup() {
    Flash::formatImmediately();

    Serial.begin(9600);

    if (Serial) {
        Serial.println("GlowOrb Firmware 2.0");
        Serial.print("Flash/Ram used: ");
        Serial.print(flashUsed());
        Serial.print("/");
        Serial.println(ramUsed());

        Serial.print("Flash status: ");
        if (Flash::isFormatted()) {
            Serial.println("ok");
        } else {
            Serial.println("error");
        }
        Serial.flush();
    } // Serial?

    // The accelerometer and rtc use the wire library
    Wire.beginOnPins(sclPin, sdaPin);

    delay(500);

    // Accelerometer
    if (ACCEL_ENABLED) {
        accel.setup(RANGE_2G, BW_64ms, ACCEL_SEND_G, ACCEL_SEND_MINMAX, ACCEL_SEND_HDG);
        //accel.m_accel.setTimeout(1);
        stream.println("Accel Initialized");
    }


    // Neopixels
    leds->begin();
    leds->setBrightness(BRIGHTNESS);
//    leds->setColor(Color::WHITE);
//    leds->show(true);

    // Microphone
    pinMode(analogPin, INPUT);

    // Heart BPM Simulator
    lfoHeart.setPeriod(HEART_BPM_MS);

#if USE_GAZELLE
    gazelle.begin(role);
#endif

//  chaser.variableRate = true;
//  chaser.rateMax = 59;
//  chaser.rateMin = 1;
//  chaser.lfoRate.setPeriod(6286);
//  chaser.lfoColor.setPeriod(620);
//  chaser.dontErase = true;
//  chaser.reversable = false;
//  
//  strobe.color = Color::WHITE;
//  strobe.timer1.setInterval(250);

} // setup


void loop() {
    if (oneSec.isExpired(true) && Serial) {

        if (!bSentNameAndVersion) {
            stream.println("GlowOrb Firmware 2.0");
            bSentNameAndVersion = true;
        }

        if (SEND_LOOPFREQ) {
            stream.print("loop:");
            stream.print(loopCount);
            stream.println();
        }
//        stream.print(", mode: ");  stream.print(systemMode);
//        stream.print(", hdg: ");   stream.print(accel.m_accel.heading());
//        stream.println();


//        snprintf(report, sizeof(report), "A: %6d %6d %6d    M: %6d %6d %6d",
//        accel.m_accel.a.x, accel.m_accel.a.y, accel.m_accel.a.z,
//        accel.m_accel.m.x, accel.m_accel.m.y, accel.m_accel.m.z);
//        stream.println(report);

        loopCount = 0;
    } // oneSec?


    // Handle any incoming serial commands
    readSerialPort();

    /*
    #if USE_GAZELLE
    //
    // incoming?
    //
    int nAvailable = gazelle.available();

    //
    // I'm expecting the device to send an int, as a string, terminated with crlf.
    //
    if( nAvailable ) {

        Serial.print("rx timestamp: "); Serial.print( millis() );
        Serial.print(", gzAvail:");     Serial.print(gazelle.available());
        //Serial.print(", msg: ");      Serial.print(gazelle.parseInt());

        for(int i = 0; i < gazelle.available(); i++ ) {
            Serial.print(", data["); Serial.print(i); Serial.print("] = "); Serial.print(gazelle.read());
        }

        Serial.println();

    } // nAvailable?
    #endif
    */


    //
    // Accelerometer
    //
    if (ACCEL_ENABLED && accelTimer.isExpired(true)) {

        accel.loop(0 /*chn4 unused*/);

    } // accelTimer?


    //
    // Scope Telemetry
    //
    if (ACCEL_ENABLED && SEND_TELEMETRY && telemetryTimer.isExpired(true)) {

        sendTelemetry();

    } // telemetryTimer?


//    if( heartTimer.isExpired(true) ) {
//
//        // Heart
//        heartSignal = analogRead(micPin);
//
//        int fakeHeartSignal = (heartSignal > pin6Threshold) ? 512 : 0;
//
//        if( pin6Raw ) {
//            accel.printGs(heartSignal);
//        } else {
//            accel.printGs(fakeHeartSignal);
//        }
//
//        //Serial.println(heartSignal);
//
//    }

    if (SEND_HEART && heartTimer.isExpired(true)) {

        int bpm = lfoHeart.apply(70, 40);

        stream.println(bpm);
        Serial.println(bpm);

    }


    //
    // let shake change modes if current mode < 5, meaning 5 and above stop this behavior.
    //
    if (ACCEL_ENABLED) {
        static const int nModes = 9;
        if (systemMode < nModes) {
            if (!(modeSwitchTimer.isStarted() && !modeSwitchTimer.isExpired())) {

                modeSwitchTimer.stop();

                if (accel.isUpdated()) {
                    if (accel.getAxisMaxG(accel.getHighGAxis()) > ACCEL_SWITCHG) {
                        ++systemMode %= nModes;
                        modeSwitchTimer.start(300);
                    }
                }

            } // started?
        }
    } // accel enabled?


//    pixels.setPersistenceEnable(false);
    switch (systemMode) {

        case 0: { // off
            leds->setColor(Color::BLACK);
            bChaseCompleted = false;
            break;
        }


        case 1: { // chase

//            pixels.setPersistenceEnable(true);
//            pixels.setPersistenceInterval(1);
//            pixels.setPersistenceDelta(1);

            chaser.loop();

//            if( !bChaseCompleted ) {
//                bChaseCompleted = chaser.loop();
//            } else {
//                leds->setColor(Color::BLACK);
//            }

        }
            break;


        case 2: { // rainbow
            rainbowCycle.loop();
        }
            break;


        case 3: { // strobe
            strobe.loop();
        }
            break;


        case 4: { // accel
            if (ACCEL_ENABLED) {
                if (accel.m_accel.timeoutOccurred()) {
                    stream.println("Accel timeout");
                    leds->setColor(Color::YELLOW);

                } else {
                    float rollG = accel.getRollG();
                    float pitchG = accel.getPitchG();
                    float yawG = accel.getYawG();
                    float maxG = accel.getRangeMaxG();

                    byte r = (byte) fmap(fabs(rollG), 0, maxG, 0, 255);
                    byte g = (byte) fmap(fabs(pitchG), 0, maxG, 0, 255);
                    byte b = (byte) fmap(fabs(yawG), 0, maxG, 0, 255);

                    uint32_t
                            color = Color::create(r, g, b);

                    static int nAvgs = 10;
                    static Averager avgR(nAvgs);
                    static Averager avgG(nAvgs);
                    static Averager avgB(nAvgs);

                    avgR.add(r);
                    avgG.add(g);
                    avgB.add(b);

                    color = Color::create(avgR.average(), avgG.average(), avgB.average());

                    color = Color::scaleColor(color, 1.25);

                    leds->setColor(color);
                } // timeout
            } // ACCEL_ENABLED
        }
            break;


        case 5: { // white
            leds->setColor(Color::WHITE);
        }
            break;


        case 6: { // solid color
            leds->setColor(solidColor);
        }
            break;


        case 7: { // red night
            leds->normalPixelMapping();
            leds->setColor(Color::BLACK);
            for (int i = 0; i < 19; i++) {
                leds->setColor(Color::scaleColor(Color::RED, 0.1), i);
            }
        }
            break;

        case 8: { // brightness
            brightnessChanger.loop();
        }
            break;
    };


//Serial.print("Mic: "); Serial.println(analogRead(micPin));


//#if USE_GAZELLE
//    
//    static uint8_t txByte = 0;
//    
//    if( gazelleTimer.isExpired(true) ) {
//
//        gazelle.println(txByte++);
//        
//        //gazelle.print("123456789 123456789 123456789 1234567890");
//        //gazelle.print("123456789 123456789 123456789 123");
//        
//    } // gazelle timer?
//    
//#endif


#if USE_GAZELLE

    gazelle.loop(/*true*/);
    
    if( gazelle.bSendAttempted && gazelle.bSendFailed ) {
        Serial.println("Resetting gazelle");
        gazelle.restart(1000);
    }

    // Maybe update the neopixels when using gazelle.
    
    // we transmitted, did we get the reply yet? if so then show, else don't show yet.
    if( !gazelle.bExpectingReply || gazelle.bHostReplied ) {
        leds->show(FORCE_SHOW);
    }

#else // not using gazelle
    // Update the neopixels when not using gazelle.
    leds->show();
#endif
    loopCount++;
} // loop


// the end.






