/*
 * GainsConfig.h
 *
 *  Created on: 2014/07/03
 *      Author: sa
 */

#ifndef GAINSCONFIG_H_
#define GAINSCONFIG_H_

class ImuType{
public:
	enum Type{
		ADIS16488,
		MPU9250,
		MPU9250_BMP850,
		SELECTABLE,
	};
};

class GpsType{
public:
	enum Type{
		USART_GPS,
		DUMMY_GPS,
		SELECTABLE,
	};
};

class GainsPrintMode{
public:
	enum Mode{
		NONE,
		MARITIME,
		E_FRAME,
		DEBUG,
		INS,
		QUATERNION,
		GPAIO,
	};
};

static ImuType::Type IMU_TYPE = ImuType::SELECTABLE;
static GpsType::Type GPS_TYPE = GpsType::SELECTABLE;

#endif /* GAINSCONFIG_H_ */
