/*
 * ImuData.cpp
 *
 *  Created on: 2014/06/12
 *      Author: sa
 */

#include "ImuData.h"
#include "math.h"

/**
 * @brief 気圧差から相対高度を求める
 * 
 * 高度差大きい場合または基準となる高度が海面高度から大きく離れている場合精度が劣化する
 * 
 * @param[in] paPressure 現在の気圧[pa]
 * @param[in] paRefPressure 基準となる位置での気圧[pa]
 * @return 高度差[m]
 */

float ImuData::paToRelativeHeight(float paPressure,float paRefPressure){
	return -44330.0*(1-powf(paPressure/paRefPressure,1/5.2255));
}


