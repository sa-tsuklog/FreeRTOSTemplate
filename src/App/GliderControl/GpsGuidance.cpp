/*
 * GpsGuidance.cpp
 *
 *  Created on: 2015/04/05
 *      Author: sa
 */
#include <pch.h>

#include "GpsGuidance.h"

#include "Device/Gains/Driver/Gps/Gps.h"

//TODO:waypointのappend,clearをタスク間通信を使うようにする。

GpsGuidance::GpsGuidance(){
	clearWaypoints();
}

void GpsGuidance::clearWaypoints(){
	waypointCount = 0;
	nextWaypoint =  0;
}

void GpsGuidance::resetCurrentWaypoint(){
	if(waypointCount > 1){
		nextWaypoint = 1;		
	}else{
		nextWaypoint = 0;
	}
	
}

void GpsGuidance::setCurrentWaypoint(int waypointId){
	if(waypointId < waypointCount && waypointId >= 0){
		nextWaypoint = waypointId;
	}
}

int stringToDegX1M(char* degAngleString){
	int i=0;
	int degX1M=0;
	while(degAngleString[i]!='.'){
		if(degAngleString[i]<'0' || degAngleString[i]>'9'){
			printf("invalid coordinate %c\r\n",degAngleString[i]);
			return 0;
		}
		degX1M = degX1M*10 + degAngleString[i]-'0';
		i++;
	}
	int dotIndex = i;
	degX1M*=1000000;
	int multiplyer = 100000;
	i++;
	for(;i<dotIndex+7;i++){
		if(degAngleString[i]<'0' || degAngleString[i]>'9'){
			return degX1M;
		}
		
		degX1M = degX1M + (degAngleString[i]-'0') * multiplyer;
		multiplyer/=10;
	}
	return degX1M;
}

void GpsGuidance::appendWaypoint(char* degWaypointString){
//	if(!Gps::GetInstance()->isRefValid()){
//		printf("position refierence is not valid\r\n");
//		return;
//	}
	
	int i=0;
	while(degWaypointString[i] != ','){
		if(degWaypointString[i] == 0){
			printf("invalid coordinate\r\n");
			return;
		}
		i++;
	}
	degWaypointString[i] = 0;
	int longitudeIndex = i+1;
	
	int degX1MLattitude = stringToDegX1M(degWaypointString);
	int degX1MLongitude = stringToDegX1M(degWaypointString+longitudeIndex);
	
	float relativePosX = Gps::GetInstance()->degX1MtoM_RelativePosX(degX1MLattitude);
	float relativePosY = Gps::GetInstance()->degX1MtoM_RelativePosY(degX1MLongitude);
	
	appendWaypoint(relativePosX,relativePosY);
	
}

void GpsGuidance::appendWaypoint(float relativePosX, float relativePosY){
	if(waypointCount >= MAX_WAYPOINT_COUNT){
		printf("too many waypoints\r\n");
		return;
	}
	waypointX[waypointCount] = relativePosX;
	waypointY[waypointCount] = relativePosY;
	waypointCount++;
	
	if(waypointCount > 1 && nextWaypoint == 0){
		nextWaypoint = 1;
	}
}

void GpsGuidance::confirmWaypoints(){
	printf("$WPAFM,%d",waypointCount);
	for(int i=0;i<waypointCount;i++){
		int degX1MLattitude = Gps::GetInstance()->mPosXToDegX1M_Latitude(waypointX[i]);
		int degX1MLongitude = Gps::GetInstance()->mPosYToDegX1M_Longitude(waypointY[i]);
		
		printf(",%d.%d,%d.%d",degX1MLattitude/1000000,degX1MLattitude%1000000,degX1MLongitude/1000000,degX1MLongitude%1000000);
		vTaskDelay(1);
	}
	printf("\r\n");
}

float GpsGuidance::degToRad(float degAngle){

    return (float)(degAngle*(M_PI/180.0));
}
float GpsGuidance::radToDeg(float radAngle){
    return (float)(radAngle*(180.0/M_PI));
}

float GpsGuidance::radVectorToWaypoint(float mPosX,float mPosY,float mWaypointPosX,float mWaypointPosY){
    return atan2f(mWaypointPosY-mPosY,mWaypointPosX-mPosX);
}

float GpsGuidance::mDistanceToWaypoint(float mPosX,float mPosY,float mWaypointPosX,float mWaypointPosY){
    return sqrtf((mWaypointPosX-mPosX)*(mWaypointPosX-mPosX) + (mWaypointPosY-mPosY)*(mWaypointPosY-mPosY));
}


float GpsGuidance::mDistanceFromCourse(float mPosX,float mPosY,float mWaypoint2PosX,float mWaypoint2PosY, float mWaypoint1PosX, float mWaypoint1PosY){
    float mRelPosX = mPosX-mWaypoint1PosX;
    float mRelPosY = mPosY-mWaypoint1PosY;
    float mRelWaypoint2PosX = mWaypoint2PosX-mWaypoint1PosX;
    float mRelWaypoint2PosY = mWaypoint2PosY-mWaypoint1PosY;

    float mAbsDistance = (fabs(mRelPosX*mRelWaypoint2PosY - mRelPosY*mRelWaypoint2PosX) / sqrtf(mRelWaypoint2PosX*mRelWaypoint2PosX + mRelWaypoint2PosY*mRelWaypoint2PosY));

    float radMissAngle = radVectorToWaypoint(mPosX, mPosY, mWaypoint1PosX, mWaypoint1PosY) - radVectorToWaypoint(mWaypoint2PosX, mWaypoint2PosY, mWaypoint1PosX, mWaypoint1PosY);


    if(radMissAngle < -M_PI){
        radMissAngle += M_PI*2;
    }else if(radMissAngle > M_PI){
    	radMissAngle -= M_PI*2;
    }

    if(radMissAngle>0){
        return mAbsDistance;
    }else{
        return -mAbsDistance;
    }
}

float GpsGuidance::radVectorToNextWaypoint(float mPosX,float mPosY){
	if(waypointCount == 0){
		return 0;
	}else{
		return radVectorToWaypoint(mPosX,mPosY,waypointX[nextWaypoint],waypointY[nextWaypoint]);
	}
}

float GpsGuidance::radVectorToNextWaypointFromPreviousWaypoint(float mPosX,float mPosY){
	if(waypointCount == 0){
		return 0;
	}else if(nextWaypoint == 0){
		return radVectorToNextWaypoint(mPosX,mPosY);
	}else{
		return radVectorToWaypoint(waypointX[nextWaypoint-1],waypointY[nextWaypoint-1],waypointX[nextWaypoint],waypointY[nextWaypoint]);
	}
}

float GpsGuidance::mDistanceToNextWaypoint(float mPosX,float mPosY){
	if(waypointCount == 0){
		return 0;
	}else{
		return mDistanceToWaypoint(mPosX,mPosY,waypointX[nextWaypoint],waypointY[nextWaypoint]);
	}
}

float GpsGuidance::mDistanceFromCurrentCourse(float mPosX,float mPosY){
	if(waypointCount == 0){
		return 0;
	}else if(nextWaypoint == 0){
		return 0;
	}else{
		return mDistanceFromCourse(mPosX,mPosY,waypointX[nextWaypoint],waypointY[nextWaypoint],waypointX[nextWaypoint-1],waypointY[nextWaypoint-1]);
	}
}
void GpsGuidance::updateNextWaypoint(float mPosX,float mPosY){
	if(waypointCount == 0){
		return;
	}
	
	if(mDistanceToWaypoint(mPosX,mPosY,waypointX[nextWaypoint],waypointY[nextWaypoint]) < M_WAYPOINT_RADIUS){
		if(nextWaypoint+1 < waypointCount){
			nextWaypoint++;
		}
	}
}

int GpsGuidance::isOVerrun(float mPosX,float mPosY){
	static const int OVERRUN = 1;
	static const int NOT_OVERRUN = 0;
	
	if(waypointCount == 0){
		return NOT_OVERRUN;
	}
	
	if(nextWaypoint == 0){
		return NOT_OVERRUN;
	}
	
	float mDistanceFromPreviousWaypoint = mDistanceToWaypoint(mPosX,mPosY,waypointX[nextWaypoint-1],waypointY[nextWaypoint-1]);
	float mDistanceFromNextWaypoint = mDistanceToWaypoint(mPosX,mPosY,waypointX[nextWaypoint],waypointY[nextWaypoint]);
	float mDistanceBetweenWaypoints = mDistanceToWaypoint(waypointX[nextWaypoint],waypointY[nextWaypoint],waypointX[nextWaypoint-1],waypointY[nextWaypoint-1]);
	
	if(mDistanceFromPreviousWaypoint < mDistanceBetweenWaypoints + M_WAYPOINT_RADIUS && mDistanceFromNextWaypoint < mDistanceBetweenWaypoints + M_WAYPOINT_RADIUS){
		return NOT_OVERRUN;
	}else{
		return OVERRUN;
	}
}

int GpsGuidance::getNextWaypointId(){
	return nextWaypoint;
}

Quaternion GpsGuidance::getNextWaypoint(){
	if(waypointCount == 0){
		return Quaternion(0,0,0,0);
	}
	return Quaternion(0,waypointX[nextWaypoint],waypointY[nextWaypoint],0);
}
