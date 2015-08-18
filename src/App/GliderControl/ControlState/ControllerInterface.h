/*
 * ControllerInterface.h
 *
 *  Created on: 2015/04/27
 *      Author: sa
 */

#ifndef CONTROLLERINTERFACE_H_
#define CONTROLLERINTERFACE_H_

class ControllerInterface{
	virtual void reset()=0;
	virtual void control()=0;
};

#endif /* CONTROLLERINTERFACE_H_ */
