/*
 * HalI2c2.hpp
 *
 *  Created on: 2014/02/23
 *      Author: sa
 */

#ifndef MPU_9250_H_
#define MPU_9250_H_

class Mpu9250 {
	// Singleton pattern definition
private:
	Mpu9250();
	Mpu9250(const Mpu9250& rhs);
	Mpu9250& operator=(const Mpu9250& rhs);
	virtual ~Mpu9250() {}
public:
	static Mpu9250* getInstance() {
    	static Mpu9250 instance;
    	return &instance;
	}

	// Class definition
private:
	float cmpsGain[3];
	
	
	
public:
	void init();
	void readMpu9250();
	
	void prvMpu9250Task(void *pvParameters);
};



void prvMpu9250TaskEntry(void *pvParameters);

#endif /* HALI2C2_HPP_ */
