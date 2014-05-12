/*
 * Gps.h
 *
 *  Created on: 2014/05/11
 *      Author: sa
 */

#ifndef GPS_H_
#define GPS_H_

class Gps {
	// Singleton pattern definition
private:
	Gps();
	Gps(const Gps& rhs);
	Gps& operator=(const Gps& rhs);
	virtual ~Gps() {}
public:
	static Gps* GetInstance() {
		static Gps instance;
    	return &instance;
	}

	// Class definition
private:
	static const float M_EARTH_RADIUS = 6371000;
	
	int isRefValid;
	int degX1MLattitudeRef;
	int degX1MLongitudeRef;
	
	int hour,min;
	float sec;
	int degX1MLatitude;
	int degX1MLongitude;
	float mpsSpeed;
	float degCourse;
	int valid;
	float height;
	
	int decodeTime(char* message);
	int decodeIsValid(char* message);
	int decodeLattitude_Longitude(char* message,int* degX1MPos);
	float knotToMps(float knot);
	int decodeSpeed(char* message);
	int decodeCourse(char* message);
	void decodeGPRMC(char* line);
	
	void decodeGPGGA(char* line);
public:
	void decodeNMEA(char* line);
	int isValid();
	int getHour();
	int getMin();
	float getSec();
	int getDegX1MLatitude();
	int getDegLattitudeFraction();
	int getDegX1MLongitude();
	int getDegLongitudeFraction();
	float getMpsSpeed();
	float getDegCourse();
	float getMpsSpeedX();
	float getMpsSpeedY();
	void setRefPosition();
	void resetRefPosition();
	float getM_RelativePosX();
	float getM_RelativePosY();
};


#endif /* GPS_H_ */
