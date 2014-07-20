/*
 * Gps.cpp
 *
 *  Created on: 2014/05/11
 *      Author: sa
 */

#include "Gps.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "../../GpsData.h"
#include "../../Gains.h"
#include "MyLib/Logger/Logger.h"

//TODO: 速度、相対位置のチェックまだ。
//TODO: getterをmutex使って安全にする。

Gps::Gps(){
	isRefValid = 0;
	degX1MLattitudeRef = 0;
	degX1MLongitudeRef = 0;
	valid = 0;
	hour=0;
	min=0;
	sec=0.0;
	degX1MLatitude=0;
	degX1MLongitude=0;
	mpsSpeed=0;
	degCourse=0;
	valid=0;
	
	fp = fopen("/log1","w");
}


/////////////////////////////////////
// public methods
/////////////////////////////////////
void Gps::decodeNMEA(char* line){
	if(strncmp(line,"$GPGGA,",7)==0){
		decodeGPGGA(line);
	}else if(strncmp(line,"$GPRMC,",7)==0){
		decodeGPRMC(line);
		if(!isRefValid && valid){
			setRefPosition();
		}
		
		if(valid){
			GpsData gpsData = GpsData(getM_RelativePosX(),getM_RelativePosY(),getRelativeHeight(),
					getMpsSpeedX(),getMpsSpeedY(),0);
			Gains::GetInstance()->appendGpsData(&gpsData);
		}
	}else{
		//do nothing.
	}

	//Util::GetInstance()->myFprintf(0,fp,"%s\r\n",line);
	//Util::GetInstance()->myFprintf(0,stdout,"%s\r\n",line);

}

int Gps::isValid(){
	return valid;
}
int Gps::getHour(){
	return hour;
}
int Gps::getMin(){
	return min;
}
float Gps::getSec(){
	return sec;
}
int Gps::getDate(){
	return date;
}
int Gps::getDegX1MLatitude(){
	return degX1MLatitude;
}
int Gps::getDegX1MLongitude(){
	return degX1MLongitude;
}
float Gps::getMpsSpeed(){
	return mpsSpeed;
}
float Gps::getDegCourse(){
	return degCourse;
}
float Gps::getMpsSpeedX(){
	return getMpsSpeed()*cosf(getDegCourse()*M_PI/180);
}
float Gps::getMpsSpeedY(){
	return getMpsSpeed()*(sinf(getDegCourse()*M_PI/180));
}
void Gps::setRefPosition(){
	degX1MLattitudeRef = degX1MLatitude;
	degX1MLongitudeRef = degX1MLongitude;
	heightRef = height;
	isRefValid = 1;
}

void Gps::resetRefPosition(){
	isRefValid = 0;
}

float Gps::getM_RelativePosX(){ //north/south
	int degX1MRelative = degX1MLatitude-degX1MLattitudeRef;

	float degRelative = degX1MRelative*0.000001;

	return degRelative * M_EARTH_RADIUS * M_PI /180.0;
}

float Gps::getM_RelativePosY(){ //west/east
	int degX1MRelative = degX1MLongitude-degX1MLongitudeRef;

	float degRelative = degX1MRelative*0.000001;

	return degRelative * M_EARTH_RADIUS * cosf(degX1MLattitudeRef*0.000001/180*M_PI) * M_PI /180.0;
}
float Gps::getRelativeHeight(){
	return height-heightRef;
}

/////////////////////////////////////
// private methods
/////////////////////////////////////
int Gps::decodeTime(char* message){
	hour = (message[0]-'0') * 10 + (message[1]-'0');
	if(24<hour){
		hour -=24;
	}
	min = (message[2]-'0')*10 + (message[3]-'0');
	sec = float((message[4]-'0')*10 + (message[5]-'0'));

	int index = 7;
	float digit = 0.1;
	while(message[index]!=','){
		sec += (message[index]-'0')*digit;
		digit *= 0.1;
		index++;
	}
	return index+1;
}
int Gps::decodeIsValid(char* message){
	if(message[0] == 'A'){
		valid = 1;
	}else{
		valid = 0;
	}
	return 2;
}
int Gps::decodeLattitude_Longitude(char* message,int* degX1MPos){
	int buf=0;
	int index=0;
	int tmpDeg;
	int tmpMin_x10_6;
	int digit = 1000000;
	
	while(message[index] != '.'){
		buf*=10;
		buf += (message[index] - '0');
		index++;
	}
	index++;			//skip '.'
	tmpDeg = buf/100;
	tmpMin_x10_6 = buf%100;
	tmpMin_x10_6 *= digit;

	while(message[index] != ','){
		digit /= 10;
		tmpMin_x10_6 += (message[index] - '0') * digit;
		index++;
	}

	*degX1MPos = tmpDeg*1000000 + tmpMin_x10_6/60;
	
	return index+1;
}
float Gps::knotToMps(float knot){
	return knot * 0.514444444;
}
int Gps::decodeSpeed(char* message){
	int index = 0;
	float knotSpeed = 0;
	float digit = 0.1;

	while(message[index] != '.'){
		knotSpeed *=10;
		knotSpeed += message[index]-'0';
		index++;
	}
	index++; //'.'

	while(message[index] != ','){
		knotSpeed += (message[index]-'0') * digit;
		digit*=0.1;
		index++;
	}
	index++;	//','

	mpsSpeed = knotToMps(knotSpeed);

	return index;
}
int Gps::decodeCourse(char* message){
	int index = 0;
	float tpmDegCourse = 0;
	float digit = 0.1;
	int sign = 1;

	if(message[0]=='-'){
		sign = -1;
		index++;
	}

	while(message[index] != '.'){
		tpmDegCourse *=10;
		tpmDegCourse += message[index]-'0';
		index++;
	}
	index++; 	//'.'

	while(message[index] != ','){
		tpmDegCourse += (message[index]-'0') * digit;
		digit*=0.1;
		index++;
	}
	index++;	//','

	degCourse = sign*tpmDegCourse;

	return index;
}
int Gps::decodeDate(char* message){
	int tmpDate=0;
	
	for(int i=0;i<6;i++){
		tmpDate = tmpDate*10 + (message[i]-'0');
	}
	date = tmpDate;
	return 6;
}

void Gps::decodeGPRMC(char* line){
	int index=7;
	index += decodeTime(line+index);
	index += decodeIsValid(line+index);
	if(!valid){
		return;
	}
	index += decodeLattitude_Longitude(line+index,&degX1MLatitude);
	index += 2;	//"N,". northern hemisphere only.
	index += decodeLattitude_Longitude(line+index,&degX1MLongitude);
	index += 2; //"E,". east longitude only.
	index += decodeSpeed(line+index);
	index += decodeCourse(line+index);
	index += decodeDate(line+index);
}

void Gps::decodeGPGGA(char* line){
	int index = 8;
	for(int i=0;i<5;i++){	//skip UTC,latitude,longitude
		while(line[index] != ','){
			if(line[index] == 0){
				return;
			}
			index++;
		}
		index++;
	}
	
	if(line[index] == '0'){
		return;
	}
	
	for(int i=0;i<3;i++){
		while(line[index] != ','){
			if(line[index] == 0){
				return;
			}
			index++;
		}
		index++;
	}
	
	float num=0;
	while(line[index] != '.'){
		num = num*10 + line[index]-'0';
		index++;
	}
	index++;
	float digit = 0.1;
	while(line[index] != ','){
		num += digit*(line[index]-'0');
		index++;
		digit *= 0.1;
	}
	this->height = num;
	
}

int Gps::mPosXToDegX1M_Latitude(float mPosX){
	float degX1M_Relative = mPosX/M_EARTH_RADIUS*180/M_PI*1000000.0f;
	return (int)degX1M_Relative+degX1MLattitudeRef;
}

int Gps::mPosYToDegX1M_Longitude(float mPosY){
	float degX1M_Relative = mPosY/M_EARTH_RADIUS*180/M_PI*1000000.0f/cosf(degX1MLattitudeRef*0.000001/180*M_PI);
	return (int)degX1M_Relative+degX1MLongitudeRef;
}

int Gps::degX1MToDeg(int degX1M){
	return degX1M/1000000;
}
float Gps::degX1MToMin(int degX1M){
	return (degX1M%1000000)*0.000060;
}
float Gps::mpsToKnot(float mpsSpeed){
	return mpsSpeed / 0.514444444;
}
float Gps::speedToDegDirection(float speedX,float speedY){
	float degDirection = atan2(speedY,speedX)*180/M_PI;
	if(degDirection<0){
		degDirection += 360;
	}
	
	return degDirection;
}
