/*
 * ImuData.cpp
 *
 *  Created on: 2014/06/12
 *      Author: sa
 */

#include "ImuData.h"
#include "math.h"

/**
 * @brief �C�������瑊�΍��x�����߂�
 * 
 * ���x���傫���ꍇ�܂��͊�ƂȂ鍂�x���C�ʍ��x����傫������Ă���ꍇ���x���򉻂���
 * 
 * @param[in] paPressure ���݂̋C��[pa]
 * @param[in] paRefPressure ��ƂȂ�ʒu�ł̋C��[pa]
 * @return ���x��[m]
 */

float ImuData::paToRelativeHeight(float paPressure,float paRefPressure){
	return -44330.0*(1-powf(paPressure/paRefPressure,1/5.2255));
}


