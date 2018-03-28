//
//  TinyAccel.h
//  AwesomeEEG
//
//  Created by Kurt Olsen on 5/27/14.
//
//

#ifndef TinyAccel_h
#define TinyAccel_h

#include <Arduino.h>
#include <Timer.h>
#include <Averager.h>


#define BMA250_I2CADDR		0x18

#define BMA250_RANGE_2G		0x03
#define BMA250_RANGE_4G		0x05
#define BMA250_RANGE_8G		0x08
#define BMA250_RANGE_16G	0x0C

#define BMA250_BW_64ms		0x08
#define BMA250_BW_32ms		0x09
#define BMA250_BW_16ms		0x0A
#define BMA250_BW_8ms		0x0B
#define BMA250_BW_4ms		0x0C
#define BMA250_BW_2ms		0x0D
#define BMA250_BW_1ms		0x0E
#define BMA250_BW_500us		0x0F

//
// update time	vs	bandwidth (1/update time) / 2
// ------------		------------------------------
// 0x08 - 64ms,        7.81hz
// 0x09 - 32ms		  15.63hz
// 0x0A - 16ms		  31.25hz
// 0x0B -  8ms		  62.5hz
// 0x0C -  4ms		 125hz
// 0x0D =  2ms		 250hz
// 0x0E =  1ms,		 500hz
// 0x0F =  500us,	1000hz
//


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

class TinyAccel {
public:
  
	enum AXIS {
		XAXIS,
		YAXIS,
		ZAXIS,
	};
  
  
	TinyAccel();

	void setup(int i2cAddress, int range, int bandwidth, bool accelTelemetry, bool minMaxTelemetry);
	void loop();
	void resetMinMax();


	// get raw values
	int		getX()		{ return m_X; }
	int		getY()		{ return m_Y; }
	int		getZ()		{ return m_Z; }
	int		getT()		{ return m_T; }


	// get values in g's
	float	getXG()		{ return m_lsbG * m_X; }
	float	getYG()		{ return m_lsbG * m_Y; }
	float	getZG()		{ return m_lsbG * m_Z; }

	
	// get raw min/max values
	int		getMinX()	{ return m_minX; }
	int		getMaxX()	{ return m_maxX; }
	
	int		getMinY()	{ return m_minY; }
	int		getMaxY()	{ return m_maxY; }
	
	int		getMinZ()	{ return m_minZ; }
	int		getMaxZ()	{ return m_maxZ; }
	
	
	// get min/max values in g's
	float	getMinXG()	{ return m_lsbG * m_minX; }
	float	getMaxXG()	{ return m_lsbG * m_maxX; }
	
	float	getMinYG()	{ return m_lsbG * m_minY; }
	float	getMaxYG()	{ return m_lsbG * m_maxY; }
	
	float	getMinZG()	{ return m_lsbG * m_minZ; }
	float	getMaxZG()	{ return m_lsbG * m_maxZ; }


	//
	// I need these because I don't know installation orientation of the accelerometer.
	// These  decouple an actual axis from a logical axis.
	// The m_roll/pitch/yawAxis variables are set to one of the AXIS enums
	// and the actual axis value returned is selected via a switch statement.
	//
	// By default roll=x, pitch=y and yaw=z
	//
	// I strongly recommend using these in an application
	// instead of the 'actual' axis methods.
	//
	void	setAxis(AXIS rollAxis, AXIS pitchAxis, AXIS yawAxis);
	
	float	getRollG();
	float	getPitchG();
	float	getYawG();
	
	float	getRangeMaxG() { return m_lsbG * 512; }

	
	void	setShakeThresholdG(float thresholdG) { m_shakeThresholdG = thresholdG; }
	bool	isAnyAxisShaking();
	float   getAxisMaxG(AXIS axis);
	int		getAxisMax(AXIS axis);
	AXIS	getHighGAxis();
	float	getShakeG()			{ return m_shakeG; }
	
	bool	isUpdated()	{ bool retv = m_updated; m_updated = false; return retv; }
	
private: // methods

	void update();
	void printGs();
	void printMinMax();
	
private: // properties

	int
		m_i2cAddress;
		
	float
		m_lsbG;

	Timer
		m_updateTimer; // causes the accelerometer to be read.

	Timer
		m_xyzGTimer; // used to transmit xyzG telemetry.
	
	Timer
		m_minMaxGTimer; // used to transmit minmaxG telemetry.
	
	Timer
		m_brightnessTimer; // helps measure g's in brightness mode.
		
	int
		m_X, m_Y, m_Z, m_T;


	int m_minX, m_maxX;
	int m_minY, m_maxY;
	int m_minZ, m_maxZ;

	// used to assign logical axis to physical axis
	AXIS m_rollAxis;
	AXIS m_pitchAxis;
	AXIS m_yawAxis;

	
	float		m_shakeThresholdG;
//	AXIS		m_shakeAxis;
	bool		m_isShaking;
	Averager	m_shakeAverager;
	float		m_shakeG;
	
	bool		m_updated;

};


#endif /* defined(TinyAccel_h) */




