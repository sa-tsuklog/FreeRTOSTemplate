/*
 * Ad7176-2Seeker.hpp
 *
 *  Created on: 2014/03/05
 *      Author: sa
 */

#ifndef AD7176_2SEEKER_HPP_
#define AD7176_2SEEKER_HPP_

void write8(char cmd,char data);
void write16(char cmd,unsigned short data);
void write24(char cmd,int data);
unsigned char read8(char cmd);
unsigned short read16(char cmd);
unsigned int read24(char cmd);
unsigned int read32(char cmd);
void resetIf();
void initAd7176();

#endif /* AD7176_2SEEKER_HPP_ */
