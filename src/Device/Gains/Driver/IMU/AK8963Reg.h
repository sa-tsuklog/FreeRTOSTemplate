/*
 * AK8963Reg.h
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#ifndef AK8963REG_H_
#define AK8963REG_H_

#include "stdint.h"

class AK8963Reg{
public:
	static const uint8_t WIA = 0x00;
	static const uint8_t INFO = 0x01;
	static const uint8_t ST1 = 0x02;
	static const uint8_t HXL = 0x03;
	static const uint8_t HXH = 0x04;
	static const uint8_t HYL = 0x05;
	static const uint8_t HYH = 0x06;
	static const uint8_t HZL = 0x07;
	static const uint8_t HZH = 0x08;
	static const uint8_t ST2 = 0x09;
	static const uint8_t CNTL1 = 0x0A;
	static const uint8_t CNTL2 = 0x0B;
	static const uint8_t ASTC = 0x0C;
	static const uint8_t TS1 = 0x0D;
	static const uint8_t TS2 = 0x0E;
	static const uint8_t I2CDIS = 0x0F;
	static const uint8_t ASAX = 0x10;
	static const uint8_t ASAY = 0x11;
	static const uint8_t ASAZ = 0x12;
};

#endif /* AK8963REG_H_ */
