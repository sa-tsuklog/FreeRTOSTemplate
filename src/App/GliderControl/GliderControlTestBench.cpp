/*
 * GliderControlTestBench.cpp
 *
 *  Created on: 2015/05/25
 *      Author: sa
 */
#include <pch.h>

#include "MyLib/Gains/Gains.h"
#include "GliderControlTestBench.h"
#include "GpsGuidance.h"

GliderControlTestBench::GliderControlTestBench(){
	testNumber = 0;
}

void GliderControlTestBench::testEntry(GpsGuidance* gpsGuidance){
	if(testNumber == 0){
		waypointDirectionAndUpdateTest(gpsGuidance);
	}else if(testNumber == 1){
		overrunAndDistanceFromCourseTest(gpsGuidance);
	}
}

void GliderControlTestBench::overrunAndDistanceFromCourseTest(GpsGuidance* gpsGuidance){
	Quaternion mPosition = Gains::GetInstance()->getMRelativePos();
	
	gpsGuidance->setCurrentWaypoint(1);
	printf("overrun:%d, distance:%f\r\n",gpsGuidance->isOVerrun(mPosition.x,mPosition.y),gpsGuidance->mDistanceFromCurrentCourse(mPosition.x,mPosition.y));
	
}
void GliderControlTestBench::waypointDirectionAndUpdateTest(GpsGuidance* gpsGuidance){
	Quaternion mPosition = Gains::GetInstance()->getMRelativePos();
	
	
	float radVectorToNext = gpsGuidance->radVectorToNextWaypoint(mPosition.x,mPosition.y);
	Quaternion mNextWaypoint = gpsGuidance->getNextWaypoint();
	int nextWaypointId = gpsGuidance->getNextWaypointId();
	float mDistanceToNext = gpsGuidance->mDistanceToNextWaypoint(mPosition.x,mPosition.y);
	
	
	gpsGuidance->updateNextWaypoint(mPosition.x,mPosition.y);
	printf("waypointId:%d,waypointPosition:(%.1f,%.1f),myPos:(%.1f,%.1f),dif:(%.1f,%.1f),direction: %.2f[rad],distance: %.1f[m]\r\n",nextWaypointId,mNextWaypoint.x,mNextWaypoint.y,mPosition.x,mPosition.y,mNextWaypoint.x-mPosition.x,mNextWaypoint.y-mPosition.y,radVectorToNext*180/M_PI,mDistanceToNext);
}

void GliderControlTestBench::setTestNumber(int testNumber){
	this->testNumber = testNumber;
}
