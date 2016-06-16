/*
 * CanBusMonitor.h
 *
 *  Created on: 2015/03/14
 *      Author: sa
 */

#ifndef CANBUSMONITOR_H_
#define CANBUSMONITOR_H_

class CanBusMonitor {
	// Singleton pattern definition
private:
	CanBusMonitor();
	CanBusMonitor(const CanBusMonitor& rhs);
	CanBusMonitor& operator=(const CanBusMonitor& rhs);
	virtual ~CanBusMonitor() {}
public:
	static CanBusMonitor* GetInstance() {
		static CanBusMonitor instance;
		return &instance;
	}

	// Class definition
private:
	void monitorTask();
public:
	static void prvMonitorTaskEntry(void* pvParameters);
	static void initCanBusMonitor();
};

#endif /* CANBUSMONITOR_H_ */
