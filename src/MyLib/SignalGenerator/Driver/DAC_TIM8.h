/*
 * DAC_TIM8.h
 *
 *  Created on: 2015/02/25
 *      Author: sa
 */

#ifndef DAC_TIM8_H_
#define DAC_TIM8_H_

class DAC_TIM6Class {
	// Singleton pattern definition
private:
	DAC_TIM6Class();
	DAC_TIM6Class(const DAC_TIM6Class& rhs);
	DAC_TIM6Class& operator=(const DAC_TIM6Class& rhs);
	virtual ~DAC_TIM6Class() {}
	
	static const int WAVE_MEMORY_LENGTH = 1000;
	uint16_t* waveBuffer;
public:
	static DAC_TIM6Class* GetInstance() {
		static DAC_TIM6Class instance;
		return &instance;
	}

	// Class definition
private:
	
public:
	void setData(int i);
	
};

#endif /* DMA_TIM8_H_ */
