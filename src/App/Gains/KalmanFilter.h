/*
 * KalmanFilter.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef KALMANFILTER_H_
#define KALMANFILTER_H_

#include "../Util/Quaternion.h"
#include "../Util/Matrix.h"

class KalmanFilter {
public:
    KalmanFilter(Quaternion* velocity, Quaternion* position, Quaternion* attitude);
    KalmanFilter(const KalmanFilter& orig);
    virtual ~KalmanFilter();

    void predict(Quaternion* mpspsAccel, Quaternion* rpsGyro);
    void update(Quaternion* gpsVel, Quaternion* gpsPos, Quaternion* uTCmps);

    Quaternion getMPos();
    Quaternion getMpsSpeed();
    Quaternion getAttitude();

    static Quaternion intToAttitude(Quaternion* mpspsAccel, Quaternion* uTCompass);
    static Quaternion angleToPosition(float lambda, float phi);


private:
    static const float SEC_TIMESTEP = 1.0f/102.5f;
    static const float EARTH_RADIUS = 6378137.0f;
    const Quaternion* EARTH_RATE;
    const Quaternion* GRAVITY;
    const Quaternion* CMPS_BASE;

    Quaternion* attitude;
    Quaternion* position;
    Quaternion* velocity;
    Matrix* errorP;



    /*working area*/
    Matrix* sysErrorPhi;
    Matrix* ctrlErrorGamma;
    Matrix* ctrlErrorCovQ;
    Matrix* kalmanGainK;
    Matrix* gpsErrorCovR;

    Quaternion* positionCon;
    Quaternion* attitudeCon;

    Quaternion* attitudeDelta;
    Quaternion* positionDelta;
    Quaternion* velocityDelta;
    Quaternion* w_ne_n;
    
    Matrix* attitudeDcm;
    //update
    Matrix* p_add_r;
    Matrix* p_add_r_inverse;
    Matrix* inverse_work;
    Quaternion* bCmps;
    Quaternion* cmpsProducts;
    Matrix* diff;
    Matrix* estimatedErrorDx;
    Quaternion* normalizeCmps;
    Matrix* tmpErrorP;
    Matrix* phiP;
    Matrix* errorP_bu1_00;
    Matrix* errorP_bu1_21;
    Matrix* gammaTrans_00;
    Matrix* gammaTrans_21;
    Matrix* errorP_bu_00;
    Matrix* errorP_bu_22;

    void initializeP();
    void initializeQ();
    void initializeR();

    void predictMisc();
    void predictVelocityDelta(Quaternion* mpspsAccel);
    void predictPositionDelta();
    void predictAttitudeDelta(Quaternion* rpsGyro);
    void predictP(Quaternion* mpspsAccel);
    void predictPhi(Quaternion* mpspsAccel);
    void predictGamma();

    void updateMisc();
    void updateKalmanGain();
    void updateEstimatedErrorDx(Quaternion* gpsVel, Quaternion* gpsPos, Quaternion* uTCmps);
    void updateX();
    void updateP();
};


#endif /* KALMANFILTER_H_ */
