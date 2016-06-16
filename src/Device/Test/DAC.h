/*
 * DAC.h
 *
 *  Created on: 2014/04/28
 *      Author: sa
 */

#ifndef DAC_H_
#define DAC_H_

class DACClass {
	// Singleton pattern definition
private:
	DACClass();
	DACClass(const DACClass& rhs);
	DACClass& operator=(const DACClass& rhs);
	virtual ~DACClass() {}
public:
	static DACClass* GetInstance() {
    	static DACClass instance;
    	return &instance;
	}

	// Class definition
private:
public:
	void setValue(int ch,int value);	//0 to 4096
	void setValue(int ch,float value);
	void setValues(int value1,int value2);	//0.0 to 1.0
	void setValues(float value1,float value2);
};

#endif /* DAC_H_ */
