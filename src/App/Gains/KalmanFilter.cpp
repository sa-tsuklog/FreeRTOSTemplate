/*
 * KalmanFilter.cpp
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "KalmanFilter.h"

//TODO:逆行列の計算を高速化するべきか。

//float<->double切り替えに置換が必要な関数:sin,cos,atan2

KalmanFilter::KalmanFilter(Quaternion* velocity, Quaternion* position, Quaternion* attitude) {
    //this->EARTH_RATE = new Quaternion(0,0,0,M_PI/24/60/60);
    //this->EARTH_RATE = new Quaternion(0,0,0,0);
    this->GRAVITY = new Quaternion(0,0,0,-9.8);
    this->CMPS_BASE = new Quaternion(0.0f,0.6624362551216327f,0.08216263629415599f,0.7445988914157886f);
    
    this->velocity = new Quaternion();
    this->position = new Quaternion();
    this->attitude = new Quaternion();
    this->velocity->clone(velocity);
    this->position->clone(position);
    this->attitude->clone(attitude);
    
    initializeP();
    initializeQ();
    initializeR();
    
    //allocate working area
    sysErrorPhi = new Matrix(9);
    sysErrorPhi->nums[3][0] = 1.0*SEC_TIMESTEP;
    sysErrorPhi->nums[4][1] = 1.0*SEC_TIMESTEP;
    sysErrorPhi->nums[5][2] = 1.0*SEC_TIMESTEP;
    ctrlErrorGamma = new Matrix(9,6,0.0);
    
    attitudeDelta = new Quaternion();
    positionDelta = new Quaternion();
    velocityDelta = new Quaternion();
    
    w_ne_n = new Quaternion();
    positionCon = new Quaternion();
    attitudeCon = new Quaternion();
    
    attitudeDcm = new Matrix(3,3,0);
    
    errorP = new Matrix(9,9,0);
    p_add_r = new Matrix(9,9,0);
    p_add_r_inverse = new Matrix(9,9,0);
    
    kalmanGainK = new Matrix(9,9,0);
    
    inverse_work = new Matrix(9,9,0);
    
    bCmps = new Quaternion(0,0,0,0);
    cmpsProducts = new Quaternion(0,0,0,0);
    diff = new Matrix(9,1,0);
    estimatedErrorDx = new Matrix(9,1,0);
    normalizeCmps = new Quaternion();
    tmpErrorP = new Matrix(9,9,0);
    phiP = new Matrix(9,9,0);
    
    errorP_bu1_00 = new Matrix(3,3,0);
    errorP_bu1_21 = new Matrix(3,3,0);
    gammaTrans_00 = new Matrix(3,3,0);
    gammaTrans_21 = new Matrix(3,3,0);
    errorP_bu_00 = new Matrix(3,3,0);
    errorP_bu_22 = new Matrix(3,3,0);
}

KalmanFilter::KalmanFilter(const KalmanFilter& orig) {
}

KalmanFilter::~KalmanFilter() {
}

void KalmanFilter::predict(Quaternion* mpspsAccel, Quaternion* rpsGyro){
    predictMisc();
    predictVelocityDelta(mpspsAccel);
    predictPositionDelta();
    predictAttitudeDelta(rpsGyro);
    
    velocity->add(velocityDelta);
    position->add(positionDelta);
    attitude->add(attitudeDelta);
    attitude->normalize();
    
    predictP(mpspsAccel);
}
void KalmanFilter::predictMisc(){
    w_ne_n->x = velocity->y/EARTH_RADIUS;
    w_ne_n->y = -velocity->x/EARTH_RADIUS;
    
    positionCon->con(position);
    attitudeCon->con(attitude);
    
    attitudeDcm->quatToDcm(attitude);
}

void KalmanFilter::predictVelocityDelta(Quaternion* mpspsAccel){
    velocityDelta->clone(attitude);
    ((velocityDelta->mul(mpspsAccel))->mul(attitudeCon)->sub(GRAVITY))->mul(SEC_TIMESTEP);
}
void KalmanFilter::predictPositionDelta(){
    (positionDelta->mul(velocity,SEC_TIMESTEP));
}
void KalmanFilter::predictAttitudeDelta(Quaternion* rpsGyro){
    (attitudeDelta->mul(attitude,rpsGyro))->mul(0.5*SEC_TIMESTEP);
}
void KalmanFilter::predictP(Quaternion* mpspsAccel){
    predictPhi(mpspsAccel);
    predictGamma();
    
    //axの計算 : Phi * P
    for (int i = 0; i<9; i++) {
        phiP->nums[0][i]=sysErrorPhi->nums[0][7]*errorP->nums[7][i]+sysErrorPhi->nums[0][8]*errorP->nums[8][i];
        phiP->nums[1][i]=sysErrorPhi->nums[1][6]*errorP->nums[6][i]+sysErrorPhi->nums[1][8]*errorP->nums[8][i];
        phiP->nums[2][i]=sysErrorPhi->nums[2][6]*errorP->nums[6][i]+sysErrorPhi->nums[2][7]*errorP->nums[7][i];
        phiP->nums[3][i]=errorP->nums[0][i]*SEC_TIMESTEP;
        phiP->nums[4][i]=errorP->nums[1][i]*SEC_TIMESTEP;
        phiP->nums[5][i]=errorP->nums[2][i]*SEC_TIMESTEP;
    }
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<9; j++) {
            errorP->nums[i][j] += phiP->nums[i][j];
        }
    }
    
    //axの計算 : (Phi*P) * Phi_T
    for (int i = 0; i<9; i++) {//phiP,eerorPを使い回し。
        phiP->nums[i][0]=errorP->nums[i][7]*sysErrorPhi->nums[0][7]+errorP->nums[i][8]*sysErrorPhi->nums[0][8];
        phiP->nums[i][1]=errorP->nums[i][6]*sysErrorPhi->nums[1][6]+errorP->nums[i][8]*sysErrorPhi->nums[1][8];
        phiP->nums[i][2]=errorP->nums[i][6]*sysErrorPhi->nums[2][6]+errorP->nums[i][7]*sysErrorPhi->nums[2][7];
        phiP->nums[i][3]=errorP->nums[i][0]*SEC_TIMESTEP;
        phiP->nums[i][4]=errorP->nums[i][1]*SEC_TIMESTEP;
        phiP->nums[i][5]=errorP->nums[i][2]*SEC_TIMESTEP;
    }
   for (int i = 0; i<9; i++) {
        for (int j = 0; j<6; j++) {
            errorP->nums[i][j] += phiP->nums[i][j];
        }
    }
    
    //bx
    for (int i = 0; i<3; i++) {
        for (int j = 0; j<3; j++) {
            errorP_bu1_00->nums[i][j] = ctrlErrorGamma->nums[i][j]*ctrlErrorCovQ->nums[j][j];
            errorP_bu1_21->nums[i][j] = ctrlErrorGamma->nums[i+6][j+3]*ctrlErrorCovQ->nums[j+3][j+3];
        }
    }
    for (int i = 0; i<3; i++) {
        for (int j = 0; j<3; j++) {
            gammaTrans_00->nums[i][j] = ctrlErrorGamma->nums[j][i];
            gammaTrans_21->nums[i][j] = ctrlErrorGamma->nums[j+6][i+3];
        }
    }
    
    errorP_bu_00->mul(errorP_bu1_00,gammaTrans_00);
    errorP_bu_22->mul(errorP_bu1_21,gammaTrans_21);

    for (int i = 0; i<3; i++) {
        for (int j = 0; j<3; j++) {
            errorP->nums[i][j] += errorP_bu_00->nums[i][j];
            errorP->nums[i+6][j+6] += errorP_bu_22->nums[i][j];
        }
    }
}

void KalmanFilter::predictPhi(Quaternion* mpspsAccel){
    sysErrorPhi->nums[0][7] = -2*SEC_TIMESTEP*(-attitudeDcm->nums[0][2]*mpspsAccel->x - attitudeDcm->nums[1][2]*mpspsAccel->y - attitudeDcm->nums[2][2]*mpspsAccel->z);
    sysErrorPhi->nums[0][8] = -2*SEC_TIMESTEP*( attitudeDcm->nums[0][1]*mpspsAccel->x + attitudeDcm->nums[1][1]*mpspsAccel->y + attitudeDcm->nums[2][1]*mpspsAccel->z);
    sysErrorPhi->nums[1][6] = -sysErrorPhi->nums[0][7];
    sysErrorPhi->nums[1][8] = -2*SEC_TIMESTEP*(-attitudeDcm->nums[0][0]*mpspsAccel->x - attitudeDcm->nums[1][0]*mpspsAccel->y - attitudeDcm->nums[2][0]*mpspsAccel->z);
    sysErrorPhi->nums[2][6] = -sysErrorPhi->nums[0][8];
    sysErrorPhi->nums[2][7] = -sysErrorPhi->nums[1][8];
}
void KalmanFilter::predictGamma(){
    //B_00 (attitude.con).getDCM
    ctrlErrorGamma->nums[0][0] = SEC_TIMESTEP*attitudeDcm->nums[0][0];
    ctrlErrorGamma->nums[0][1] = SEC_TIMESTEP*attitudeDcm->nums[1][0];
    ctrlErrorGamma->nums[0][2] = SEC_TIMESTEP*attitudeDcm->nums[2][0];
    
    ctrlErrorGamma->nums[1][0] = SEC_TIMESTEP*attitudeDcm->nums[0][1];
    ctrlErrorGamma->nums[1][1] = SEC_TIMESTEP*attitudeDcm->nums[1][1];
    ctrlErrorGamma->nums[1][2] = SEC_TIMESTEP*attitudeDcm->nums[2][1];
    
    ctrlErrorGamma->nums[2][0] = SEC_TIMESTEP*attitudeDcm->nums[0][2];
    ctrlErrorGamma->nums[2][1] = SEC_TIMESTEP*attitudeDcm->nums[1][2];
    ctrlErrorGamma->nums[2][2] = SEC_TIMESTEP*attitudeDcm->nums[2][2];
    
    //B_21
    ctrlErrorGamma->nums[6][3] = SEC_TIMESTEP*attitudeDcm->nums[0][0]*0.5;
    ctrlErrorGamma->nums[6][4] = SEC_TIMESTEP*attitudeDcm->nums[1][0]*0.5;
    ctrlErrorGamma->nums[6][5] = SEC_TIMESTEP*attitudeDcm->nums[2][0]*0.5;
    
    ctrlErrorGamma->nums[7][3] = SEC_TIMESTEP*attitudeDcm->nums[0][1]*0.5;
    ctrlErrorGamma->nums[7][4] = SEC_TIMESTEP*attitudeDcm->nums[1][1]*0.5;
    ctrlErrorGamma->nums[7][5] = SEC_TIMESTEP*attitudeDcm->nums[2][1]*0.5;
    
    ctrlErrorGamma->nums[8][3] = SEC_TIMESTEP*attitudeDcm->nums[0][2]*0.5;
    ctrlErrorGamma->nums[8][4] = SEC_TIMESTEP*attitudeDcm->nums[1][2]*0.5;
    ctrlErrorGamma->nums[8][5] = SEC_TIMESTEP*attitudeDcm->nums[2][2]*0.5;
}

void KalmanFilter::update(Quaternion* gpsVel, Quaternion* gpsPos, Quaternion* uTCmps){
    updateMisc();
    updateKalmanGain();
    updateEstimatedErrorDx(gpsVel,gpsPos,uTCmps);
    updateX();
    updateP();
}
void KalmanFilter::updateMisc(){
    attitudeCon->con(attitude);
}
void KalmanFilter::updateKalmanGain(){
    p_add_r->add(errorP,gpsErrorCovR);
    
    p_add_r_inverse->inverse(p_add_r,inverse_work);
    
    kalmanGainK->mul(errorP,p_add_r_inverse);
}
void KalmanFilter::updateEstimatedErrorDx(Quaternion* gpsVel, Quaternion* gpsPos, Quaternion* uTCmps){
    bCmps->mul(attitudeCon,CMPS_BASE)->mul(attitude);
    normalizeCmps->normalize(uTCmps);
    cmpsProducts->mul(bCmps,uTCmps);
    
    diff->nums[0][0] = gpsVel->x - velocity->x;
    diff->nums[1][0] = gpsVel->y - velocity->y;
    diff->nums[2][0] = gpsVel->z - velocity->z;
    
    diff->nums[3][0] = gpsPos->x - position->x;
    diff->nums[4][0] = gpsPos->y - position->y;
    diff->nums[5][0] = gpsPos->z - position->z;
    
    diff->nums[6][0] = -cmpsProducts->x;
    diff->nums[7][0] = -cmpsProducts->y;
    diff->nums[8][0] = -cmpsProducts->z;
    
    estimatedErrorDx->mul(kalmanGainK,diff);
}
void KalmanFilter::updateX(){
    velocity->x = velocity->x+estimatedErrorDx->nums[0][0];
    velocity->y = velocity->y+estimatedErrorDx->nums[1][0];
    velocity->z = velocity->z+estimatedErrorDx->nums[2][0];
    
    position->x = position->x+estimatedErrorDx->nums[3][0];
    position->y = position->y+estimatedErrorDx->nums[4][0];
    position->z = position->z+estimatedErrorDx->nums[5][0];
    
    Quaternion attError = Quaternion(1,estimatedErrorDx->nums[6][0],estimatedErrorDx->nums[7][0],estimatedErrorDx->nums[8][0]);
    attitude->mul(&attError,attitude)->normalize();
}
void KalmanFilter::updateP(){
    kalmanGainK->mul(kalmanGainK,-1.0f);
    for (int i = 0; i<9; i++) {
        kalmanGainK->nums[i][i] += 1;
    }
    tmpErrorP->copy(errorP);
    errorP->mul(kalmanGainK,tmpErrorP);
    
}
void KalmanFilter::initializeP(){
    this->errorP = new Matrix(9,9);
//    this->errorP->nums[0][0] = 0.1;//velocity x error
//    this->errorP->nums[1][1] = 0.1;//velocity y error
//    this->errorP->nums[2][2] = 0.1;//velocity z error
//    this->errorP->nums[3][3] = 0.0001;//position x error
//    this->errorP->nums[4][4] = 0.0001;//position y error
//    this->errorP->nums[5][5] = 0.0001;//position z error
//    this->errorP->nums[6][6] = 2*M_PI/32;//attitude x error
//    this->errorP->nums[7][7] = 2*M_PI/32;//attitude y error
//    this->errorP->nums[8][8] = 2*M_PI/32;//attitude z error
    this->errorP->nums[0][0] = 3;//velocity x error
    this->errorP->nums[1][1] = 3;//velocity y error
    this->errorP->nums[2][2] = 3;//velocity z error
    this->errorP->nums[3][3] = 10;//position x error
    this->errorP->nums[4][4] = 10;//position y error
    this->errorP->nums[5][5] = 10;//position z error
    this->errorP->nums[6][6] = 0.2;//attitude x error
    this->errorP->nums[7][7] = 0.2;//attitude y error
    this->errorP->nums[8][8] = 0.2;//attitude z error
}
void KalmanFilter::initializeQ(){
    this->ctrlErrorCovQ = new Matrix(6,6,0);
    this->ctrlErrorCovQ->nums[0][0] = powf(0.01,2);      //accel x error
    this->ctrlErrorCovQ->nums[1][1] = powf(0.01,2);      //accel y error
    this->ctrlErrorCovQ->nums[2][2] = powf(0.01,2);      //accel z error
    this->ctrlErrorCovQ->nums[3][3] = powf(0.001,2);    //gyro x error
    this->ctrlErrorCovQ->nums[4][4] = powf(0.001,2);    //gyro y error
    this->ctrlErrorCovQ->nums[5][5] = powf(0.001,2);    //gyro z error
}
void KalmanFilter::initializeR(){
    this->gpsErrorCovR = new Matrix(9,9,0);
    
    gpsErrorCovR->nums[0][0] = powf(0.2,2);       //velocity x error
    gpsErrorCovR->nums[1][1] = powf(0.2,2);       //velocity y error
    gpsErrorCovR->nums[2][2] = powf(10,2);      //velocity z error
    gpsErrorCovR->nums[3][3] = powf(10,2);      //position x error
    gpsErrorCovR->nums[4][4] = powf(10,2);      //position y error
    gpsErrorCovR->nums[5][5] = powf(10,2);      //position z error
    gpsErrorCovR->nums[6][6] = powf(0.1,2);     //compass x error(when normalized)
    gpsErrorCovR->nums[7][7] = powf(0.1,2);     //compass y error(when normalized)
    gpsErrorCovR->nums[8][8] = powf(0.1,2);     //compass z error(when normalized)
    
    
}


Quaternion KalmanFilter::intToAttitude(Quaternion* mpspsAccel, Quaternion* uTCompass){
    Quaternion attitudeFromAccel;
    
    float xyLength = sqrt(mpspsAccel->x*mpspsAccel->x + mpspsAccel->y * mpspsAccel->y);
    if(xyLength == 0){
        if(mpspsAccel->z > 0){
            attitudeFromAccel = Quaternion(cosf(M_PI/2),sinf(M_PI/2),0,0);
        }else{
            attitudeFromAccel = Quaternion(1,0,0,0);
        }
    }else{
        float axisX = -mpspsAccel->y;
        float axisY = mpspsAccel->x;
        
        axisX /= xyLength;
        axisY /= xyLength;
        
        float radAngle = atan2f(mpspsAccel->z,xyLength) + M_PI/2;
        
        attitudeFromAccel = Quaternion(cosf(radAngle/2),axisX*sinf(radAngle/2),axisY*sinf(radAngle/2),0);   
    }
    
    Quaternion rotatedCompass;
    Quaternion attitudeFromAccelCon;
    attitudeFromAccelCon.con(&attitudeFromAccel);
    rotatedCompass.mul(&attitudeFromAccel,uTCompass);
    rotatedCompass.mul(attitudeFromAccel.con());
    
    float radCompassAngle = -atan2f(rotatedCompass.y,rotatedCompass.x);
    Quaternion attitudeFromCompass = Quaternion(cosf(radCompassAngle/2),0,0,sinf(radCompassAngle/2));
    
    Quaternion attitude;
    attitude.mul(&attitudeFromCompass,&attitudeFromAccel);
    attitude.normalize();
    
    return attitude;
}

Quaternion KalmanFilter::angleToPosition(float lambda, float phi){
    float posW = cosf(lambda/2) * (cosf(-phi/2)+sinf(-phi/2))/sqrtf(2);
    float posX = sinf(lambda/2) * (cosf(-phi/2)-sinf(-phi/2))/sqrtf(2);
    float posY =-cosf(lambda/2) * (cosf(-phi/2)-sinf(-phi/2))/sqrtf(2);
    float posZ = sinf(lambda/2) * (cosf(-phi/2)+sinf(-phi/2))/sqrtf(2);
    
    return Quaternion(posW,posX,posY,posZ);
}

Quaternion KalmanFilter::getMPos(){
    return *position;
}
Quaternion KalmanFilter::getMpsSpeed(){
    return *velocity;
}
Quaternion KalmanFilter::getAttitude(){
    return *attitude;
}

