/*
 * GliderControlTestBench.h
 *
 *  Created on: 2015/05/25
 *      Author: sa
 */

#ifndef GLIDERCONTROLTESTBENCH_H_
#define GLIDERCONTROLTESTBENCH_H_

#include "GpsGuidance.h"

class GliderControlTestBench{
public:
	int testNumber;
	
	GliderControlTestBench();
	void testEntry(GpsGuidance* gpsGuidance);
	void overrunAndDistanceFromCourseTest(GpsGuidance* gpsGuidance);
	void waypointDirectionAndUpdateTest(GpsGuidance* gpsGuidance);
	void setTestNumber(int testNumber);
};

#endif /* GLIDERCONTROLTESTBENCH_H_ */
