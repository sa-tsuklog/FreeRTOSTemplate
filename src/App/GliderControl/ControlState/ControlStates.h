/*
 * ControlStates.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef CONTROLSTATES_H_
#define CONTROLSTATES_H_

class ControlState{
public:
	enum State{
		LAUNCH_STANDBY,
		BOOST_PHASE0,
		BOOST_PHASE,
		GLIDE_PHASE,
		MANUAL_CONTROL,
		EMERGENCY,
		PITCH_HEADING_HOLD,
	};
};

#endif /* CONTROLSTATES_H_ */
