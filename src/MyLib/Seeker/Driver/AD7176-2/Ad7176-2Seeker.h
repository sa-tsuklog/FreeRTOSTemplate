/*
 * Ad7176-2Seeker.hpp
 *
 *  Created on: 2014/03/05
 *      Author: sa
 */

#ifndef AD7176_2SEEKER_HPP_
#define AD7176_2SEEKER_HPP_

class Ad7176_2Seeker {
	// Singleton pattern definition
private:
	Ad7176_2Seeker();
	Ad7176_2Seeker(const Ad7176_2Seeker& rhs);
	Ad7176_2Seeker& operator=(const Ad7176_2Seeker& rhs);
	virtual ~Ad7176_2Seeker() {}
public:
	static Ad7176_2Seeker* GetInstance() {
		static Ad7176_2Seeker instance;
		return &instance;
	}

	// Class definition
private:
	
public:
	void write8(char cmd,char data);
	void write16(char cmd,unsigned short data);
	void write24(char cmd,int data);
	unsigned char read8(char cmd);
	unsigned short read16(char cmd);
	unsigned int read24(char cmd);
	unsigned int read32(char cmd);
	unsigned int readAdData(unsigned char* outChannel);
	void resetIf();
	void initAd7176();
	void initAd7176ForSingleChannel();
};







#endif /* AD7176_2SEEKER_HPP_ */
