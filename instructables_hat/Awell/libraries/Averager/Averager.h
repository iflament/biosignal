
#ifndef Averager_H
#define Averager_H

#include <Arduino.h>


class Averager {
	
public:
	
	Averager(byte nAverages);
	~Averager();
	
	void    reset();
	int     add(int value);
	boolean isReady();
	
	int average();
	
private:
	
	byte    m_size;
	byte    m_next;
	boolean m_ready;
	int*    p_values;
	
}; // class


#endif
// the end.

