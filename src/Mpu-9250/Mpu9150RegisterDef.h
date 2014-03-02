/*
 * Mpu9150RegisterDef.h
 *
 *  Created on: 2014/02/27
 *      Author: sa
 */

#ifndef MPU9150REGISTERDEF_H_
#define MPU9150REGISTERDEF_H_

#define MPU9150ADDR (0x68<<1)

#define SELF_TEST_X  0x0D
#define SELF_TEST_Y  0x0E
#define SELF_TEST_Z  0x0F
#define SELF_TEST_A  0x10
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define ACCEL_CONFIG 0x1C
#define FF_THR       0x1D
#define FF_DUR       0x1E
#define MOT_THR      0x1F
#define MOT_DUR      0x20
#define ZRMOT_THR    0x21
#define ZRMOT_DUR    0x22
#define FIFO_EN      0x23
#define I2C_MST_CTRL 0x24
#define I2C_SLV0_ADDR 0x25
#define I2C_SLV0_REG 0x26
#define I2C_MST_STATUS 0x36
#define INT_PIN_CFG  0x37
#define INT_ENABLE   0x38
#define INT_STATUS   0x3A
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define MOT_DETECT_STATUS 0x61
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET 0x68
#define MOT_DETECT_CTRL 0x69
#define USER_CTRL    0x6A
#define PWR_MGMT_1   0x6B
#define PWR_MGMT_2   0x6C
#define FIFO_COUNTH  0x72
#define FIFO_COUNTL  0x73
#define FIFO_R_W     0x74
#define WHO_AM_I     0x75

#define AK8975_ADDR (0x0C<<1) //or 0D, 0E, 0F;
#define WIA   0x00
#define INFO  0x01
#define ST1   0x02
#define HXL   0x03
#define HXH   0x04
#define HYL   0x05
#define HYH   0x06
#define HZL   0x07
#define HZH   0x08
#define ST2   0x09
#define CNTL  0x0A
#define ASTC  0x0C
#define TS1   0x0D
#define TS2   0x0E
#define I2CDIS 0x0F
#define ASAX  0x10
#define ASAY  0x11
#define ASAZ  0x12

#endif /* MPU9150REGISTERDEF_H_ */
