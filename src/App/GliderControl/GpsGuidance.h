/*
 * Guidance.h
 *
 *  Created on: 2015/04/05
 *      Author: sa
 */

#ifndef GPS_GUIDANCE_H_
#define GPS_GUIDANCE_H_

#include "Common/Quaternion.h"

class GpsGuidance{
private:
	static const int MAX_WAYPOINT_COUNT = 16;
	float waypointX[MAX_WAYPOINT_COUNT];
	float waypointY[MAX_WAYPOINT_COUNT];
	int waypointCount;
	
	static const float M_WAYPOINT_RADIUS = 5;
	int nextWaypoint;
	
	static float degToRad(float degAngle);
	static float radToDeg(float radAngle);
	static float radVectorToWaypoint(float mPosX,float mPosY,float mWaypointPosX,float mWaypointPosY);
	static float mDistanceToWaypoint(float mPosX,float mPosY,float mWaypointPosX,float mWaypointPosY);
	static float mDistanceFromCourse(float mPosX,float mPosY,float mWaypoint2PosX,float mWaypoint2PosY, float mWaypoint1PosX, float mWaypoint1PosY);
public:
	GpsGuidance();
	void appendWaypoint(char* degWaypointString);
	void appendWaypoint(float relativePosX,float relativePosY);
	void confirmWaypoints();
	void clearWaypoints();
	void resetCurrentWaypoint();
	void setCurrentWaypoint(int waypointId);
	
	float radVectorToNextWaypoint(float mPosX,float mPosY);
	float radVectorToNextWaypointFromPreviousWaypoint(float mPosX,float mPosY);
	float mDistanceToNextWaypoint(float mPosX,float mPosY);
	float mDistanceFromCurrentCourse(float mPosX,float mPosY);
	void updateNextWaypoint(float mPosX,float mPosY);
	int isOVerrun(float mPosX,float mPosY);
	int getNextWaypointId();
	Quaternion getNextWaypoint();
};


#endif /* GPS_GUIDANCE_H_ */
