//
//  Devices.cpp
//  AwesomeEEG
//
//  Created by Kurt Olsen on 6/30/13.
//
//

#include <TinyAccel.h>
#include <Wire.h>



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

TinyAccel::TinyAccel()
: m_shakeAverager(1)
{
	m_rollAxis	= XAXIS;
	m_pitchAxis = YAXIS;
	m_yawAxis	= ZAXIS;
	
	m_shakeThresholdG = 1.2;
//	m_shakeAxis = XAXIS;
	m_isShaking = false;
	
	m_shakeG = 1;
	
	resetMinMax();
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void TinyAccel::resetMinMax() {

	m_minX =  512;
	m_maxX = -512;

	m_minY =  512;
	m_maxY = -512;

	m_minZ =  512;
	m_maxZ = -512;
}


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void TinyAccel::update() {
	
	uint8_t ReadBuff[8];
	
	// Read the 7 data bytes from the BMA250
	
	Wire.beginTransmission(m_i2cAddress);
	Wire.write(0x02);
	Wire.endTransmission();
	
	Wire.requestFrom(m_i2cAddress, 7);
	
	for(int i = 0; i < 7; i++ ) {
		ReadBuff[i] = Wire.read();
	}


	m_X  =  ReadBuff[0];
	m_X |= (ReadBuff[1] << 8);
	m_X >>= 6;
	
	m_Y  =  ReadBuff[2];
	m_Y |= (ReadBuff[3] << 8);
	m_Y >>= 6;
 	
	m_Z  =  ReadBuff[4];
	m_Z |= (ReadBuff[5] << 8);
	m_Z >>= 6;
	
	m_T = (ReadBuff[6] * 0.5) + 24.0;


	m_minX = min(m_minX, m_X);
	m_minY = min(m_minY, m_Y);
	m_minZ = min(m_minZ, m_Z);

	m_maxX = max(m_maxX, m_X);
	m_maxY = max(m_maxY, m_Y);
	m_maxZ = max(m_maxZ, m_Z);
	
	m_updated = true;
	
} // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void TinyAccel::printGs() {

	char COMMA = 44;

	Serial.print("xyzg:");
	Serial.print(getXG());	Serial.print(COMMA);
	Serial.print(getYG());	Serial.print(COMMA);
	Serial.print(getZG());	Serial.println();

}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void TinyAccel::printMinMax() {

	char COMMA = 44;

	Serial.print("ming:");
	Serial.print(getMinXG()); Serial.print(COMMA);
	Serial.print(getMinYG()); Serial.print(COMMA);
	Serial.print(getMinZG()); Serial.println();

	Serial.print("maxg:");
	Serial.print(getMaxXG()); Serial.print(COMMA);
	Serial.print(getMaxYG()); Serial.print(COMMA);
	Serial.print(getMaxZG()); Serial.println();
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void TinyAccel::setAxis(AXIS rollAxis, AXIS pitchAxis, AXIS yawAxis) {
	m_rollAxis	= rollAxis;
	m_pitchAxis	= pitchAxis;
	m_yawAxis	= yawAxis;
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
float TinyAccel::getRollG() {
	switch( m_rollAxis ) {
		case XAXIS:	return getXG();	break;
		case YAXIS:	return getYG();	break;
		case ZAXIS:	return getZG();	break;
	}
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
float TinyAccel::getPitchG() {
	switch( m_pitchAxis ) {
		case XAXIS:	return getXG();	break;
		case YAXIS:	return getYG();	break;
		case ZAXIS:	return getZG();	break;
	}
}



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
float TinyAccel::getYawG() {
	switch( m_yawAxis ) {
		case XAXIS:	return getXG();	break;
		case YAXIS:	return getYG();	break;
		case ZAXIS:	return getZG();	break;
	}
}


bool TinyAccel::isAnyAxisShaking() {
	bool retv = false;
	
	retv |= (getXG() >= m_shakeThresholdG);
	retv |= (getYG() >= m_shakeThresholdG);
	retv |= (getZG() >= m_shakeThresholdG);

	return retv;

}

float TinyAccel::getAxisMaxG(AXIS axis) {
	switch( (int)axis ) {
		case XAXIS: return getMaxXG(); break;
		case YAXIS: return getMaxYG(); break;
		case ZAXIS: return getMaxZG(); break;
	}
}

int TinyAccel::getAxisMax(AXIS axis) {
	switch( (int)axis ) {
		case XAXIS: return getMaxX(); break;
		case YAXIS: return getMaxY(); break;
		case ZAXIS: return getMaxZ(); break;
	}
}

TinyAccel::AXIS TinyAccel::getHighGAxis() {
	
	/*
	AXIS  axis = XAXIS;
	float maxg = getMaxXG();

	if( getMaxYG() >= maxg ) {
		axis = YAXIS;
		maxg = getMaxYG();
	}
	
	if( getMaxZG() >= maxg ) {
		axis = ZAXIS;
		maxg = getMaxZG();
	}
	
	return axis;
	*/
	
	AXIS  axis = XAXIS;

	if( getMaxYG() >= getAxisMaxG(axis) ) {
		axis = YAXIS;
	}
	if( getMaxZG() >= getAxisMaxG(axis) ) {
		axis = ZAXIS;
	}
	
	return axis;

} // method


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void TinyAccel::setup(int i2cAddress, int range, int bandwidth, bool accelTelemetry, bool minMaxTelemetry) {
	
	m_i2cAddress = i2cAddress;
	
	// Setup the range measurement setting
	Wire.beginTransmission(m_i2cAddress);
	Wire.write(0x0F);
	Wire.write(range);
	Wire.endTransmission();
	
	// Setup the bandwidth
	Wire.beginTransmission(m_i2cAddress);
	Wire.write(0x10);
	Wire.write(bandwidth);
	Wire.endTransmission();
	
	int delayTime = 0;
	
	switch( bandwidth ) {
		case BMA250_BW_64ms:	delayTime = 65;	break;
		case BMA250_BW_32ms:	delayTime = 33;	break;
		case BMA250_BW_16ms:	delayTime = 17;	break;
		case BMA250_BW_8ms:		delayTime = 9;	break;
		case BMA250_BW_4ms:		delayTime = 5;	break;
		case BMA250_BW_2ms:		delayTime = 3;	break;
		case BMA250_BW_1ms:		delayTime = 2;	break;
		case BMA250_BW_500us:	delayTime = 1;	break;
	}
	
	switch( range ) {
		case BMA250_RANGE_2G:	m_lsbG = 0.00391;	break;
		case BMA250_RANGE_4G:	m_lsbG = 0.00781;	break;
		case BMA250_RANGE_8G:	m_lsbG = 0.01562;	break;
		case BMA250_RANGE_16G:	m_lsbG = 0.03125;	break;
	}
	
	
	m_updateTimer.start(delayTime);

	if( accelTelemetry ) {
		m_xyzGTimer.start(delayTime+1);
	}

	if( minMaxTelemetry ) {
		m_minMaxGTimer.start(1000);
	}
	
	
} // method



//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void TinyAccel::loop() {
	
	// g's telemetry?
	if( m_xyzGTimer.isExpired(true) ) {
		printGs();
	}
	
	// min/max telemetery?
	if( m_minMaxGTimer.isExpired(true) ) {
		printMinMax();
	}
	
	
	if( !m_updateTimer.isExpired(true)  ) {
		return;
	}

	
	update();

	if( isAnyAxisShaking() ) {

		AXIS
			axis = getHighGAxis();
			
		m_shakeG = getAxisMaxG(axis);
		
		//
		// If we just started shaking let's just reset
		// and not record any data on the first pass.
		//
		m_shakeAverager.add(getAxisMax(axis));

		m_isShaking = true;
		
//		Serial.print("SHAKE: ");
//		Serial.print(axis);
//		Serial.print(" = " );
//		Serial.println(getAxisMaxG(axis));
		
	
		
	} else {

		m_shakeG = 1.0;

		// were we shaking before? (and have now stopped)
		if( m_isShaking ) {

			float
				avgShake = m_shakeAverager.average() * m_lsbG;
			
//			if( m_shakeAverager.isReady() ) {
//				Serial.print("AVGSHAKE: ");
//				Serial.println(avgShake);
//			} else {
//				Serial.println("NOTREADY");
//			}
			
			m_isShaking = false;
			m_shakeAverager.reset();
			resetMinMax();
			
		} // was shaking?

	} // if shaking
	
	

} // method



// the end






