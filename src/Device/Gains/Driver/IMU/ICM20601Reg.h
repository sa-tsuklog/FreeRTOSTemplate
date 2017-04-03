/*
 * ICM20601Reg.h
 *
 *  Created on: 2016/09/29
 *      Author: sa
 */

#ifndef ICM20601REG_H_
#define ICM20601REG_H_

#include "stdint.h"

class Icm20601Reg{
public:
	static const uint8_t SELF_TEST_X_GYRO = 0x00;
	static const uint8_t SELF_TEST_Y_GYRO = 0x01;
	static const uint8_t SELF_TEST_Z_GYRO = 0x02;
	static const uint8_t SELF_TEST_X_ACCEL = 0x0D;
	static const uint8_t SELF_TEST_Y_ACCEL = 0x0E;
	static const uint8_t SELF_TEST_Z_ACCEL = 0x0F;
	static const uint8_t XG_OFFS_USRH = 0x13;
	static const uint8_t XG_OFFS_USRL = 0x14;
	static const uint8_t YG_OFFS_USRH = 0x15;
	static const uint8_t YG_OFFS_USRL = 0x16;
	static const uint8_t ZG_OFFS_USRH = 0x17;
	static const uint8_t ZG_OFFS_USRL = 0x18;
	static const uint8_t SMPLRT_DIV = 0x19;
	static const uint8_t CONFIG = 0x1A;
	static const uint8_t GYRO_CONFIG = 0x1B;
	static const uint8_t ACCEL_CONFIG = 0x1C;
	static const uint8_t ACCEL_CONFIG_2 = 0x1D;
	static const uint8_t LP_MODE_CFG = 0x1E;
	static const uint8_t ACCEL_WOM_THR = 0x1F;
	static const uint8_t FIFO_EN = 0x23;
	static const uint8_t FSYNC_INT = 0x36;
	static const uint8_t INT_PIN_CFG = 0x37;
	static const uint8_t INT_ENABLE = 0x38;
	static const uint8_t INT_STATUS = 0x3A;
	static const uint8_t ACCEL_XOUT_H = 0x3B;
	static const uint8_t ACCEL_XOUT_L = 0x3C;
	static const uint8_t ACCEL_YOUT_H = 0x3D;
	static const uint8_t ACCEL_YOUT_L = 0x3E;
	static const uint8_t ACCEL_ZOUT_H = 0x3F;
	static const uint8_t ACCEL_ZOUT_L = 0x40;
	static const uint8_t TEMP_OUT_H = 0x41;
	static const uint8_t TEMP_OUT_L = 0x42;
	static const uint8_t GYRO_XOUT_H = 0x43;
	static const uint8_t GYRO_XOUT_L = 0x44;
	static const uint8_t GYRO_YOUT_H = 0x45;
	static const uint8_t GYRO_YOUT_L = 0x46;
	static const uint8_t GYRO_ZOUT_H = 0x47;
	static const uint8_t GYRO_ZOUT_L = 0x48;
	static const uint8_t SIGNAL_PATH_RESET = 0x68;
	static const uint8_t ACCEL_INTEL_CTRL = 0x69;
	static const uint8_t USER_CTRL = 0x6A;
	static const uint8_t PWR_MGMT_1 = 0x6B;
	static const uint8_t PWR_MGMT_2 = 0x6C;
	static const uint8_t FIFO_COUNTH = 0x72;
	static const uint8_t FIFO_COUNTL = 0x73;
	static const uint8_t FIFO_R_W = 0x74;
	static const uint8_t WHO_AM_I = 0x75;
	static const uint8_t XA_OFFSET_H = 0x77;
	static const uint8_t XA_OFFSET_L = 0x78;
	static const uint8_t YA_OFFSET_H = 0x7A;
	static const uint8_t YA_OFFSET_L = 0x7B;
	static const uint8_t ZA_OFFSET_H = 0x7D;
	static const uint8_t ZA_OFFSET_L = 0x7E;
};

#endif /* ICM20601REG_H_ */
