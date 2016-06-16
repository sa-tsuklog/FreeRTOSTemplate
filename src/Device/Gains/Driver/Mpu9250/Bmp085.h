/*
 * Bmp085.h
 *
 *  Created on: 2015/06/17
 *      Author: sa
 */

#ifndef BMP085_H_
#define BMP085_H_

class Bmp085 {
	// Singleton pattern definition
private:
	Bmp085();
	Bmp085(const Bmp085& rhs);
	Bmp085& operator=(const Bmp085& rhs);
	virtual ~Bmp085() {}
public:
	static Bmp085* GetInstance() {
		static Bmp085 instance;
		return &instance;
	}

	// Class definition
private:
	int16_t ac1;
	int16_t ac2;
	int16_t ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t b1;
	int16_t b2;
	int16_t mb;
	int16_t mc;
	int16_t md;
	
	int32_t ut;
	int32_t up;
	
	int degX10Temp;
	int paPressure;
	int b5;
public:
	void init();
	void startMeasureingTemp();
	void startMeasureingPressure();
	void updateTemp();
	void updatePressure();
	
	float getPaPressure();
};

#endif /* BMP085_H_ */
