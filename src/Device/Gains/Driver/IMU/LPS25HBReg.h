/*
 * LPS25HBReg.h
 *
 *  Created on: 2016/09/28
 *      Author: sa
 */

#ifndef LPS25HBREG_H_
#define LPS25HBREG_H_

#include "stdint.h"

class Lps25HbReg{
public:
	static const uint8_t REF_P_XL = 0x08;
	static const uint8_t REF_P_L = 0x09;
	static const uint8_t REF_P_H = 0xA;
	static const uint8_t WHO_AM_I = 0xF;
	static const uint8_t RES_CONF = 0x10;
	static const uint8_t CTRL_REG1 = 0x20;
	static const uint8_t CTRL_REG2 = 0x21;
	static const uint8_t CTRL_REG3 = 0x22;
	static const uint8_t CTRL_REG4 = 0x23;
	static const uint8_t INTERRUPT_CFG = 0x24;
	static const uint8_t STATUS_REG = 0x27;
	static const uint8_t PRESS_OUT_XL = 0x28;
	static const uint8_t PRESS_OUT_L = 0x29;
	static const uint8_t PRESS_OUT_H = 0x2A;
	static const uint8_t TMP_OUT_L = 0x2B;
	static const uint8_t TMP_OUT_H = 0x2C;
	static const uint8_t FIFO_CTRL = 0x2E;
	static const uint8_t FIFO_STATUS = 0x2F;
	static const uint8_t THS_P_L = 0x30;
	static const uint8_t THS_P_H = 0x31;
	static const uint8_t RPDS_L = 0x39;
	static const uint8_t RPDS_H = 0x3A;
};

#endif /* LPS25HBREG_H_ */
