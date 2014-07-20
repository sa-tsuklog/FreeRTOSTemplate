/*
 * BridgeTim.h
 *
 *  Created on: 2014/07/02
 *      Author: sa
 */

#ifndef BRIDGETIM_H_
#define BRIDGETIM_H_

class BridgeTim{
public:
	virtual void setDuty(int ch,unsigned short duty)=0;
	virtual void timerStart()=0;
};

#endif /* BRIDGETIM_H_ */
