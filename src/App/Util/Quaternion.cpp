/*
 * Quaternion.cpp
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */




/* 
 * File:   Quaternion.cpp
 * Author: sa
 * 
 * Created on 2014/03/15, 17:40
 */

#include "stdio.h"
#include "math.h"

#include "Quaternion.h"

Quaternion::Quaternion() {
    this->w = this->x = this->y = this->z = 0;
}

Quaternion::Quaternion(float w, float x, float y, float z) {
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}
Quaternion::Quaternion(const Quaternion& orig){
    this->w = orig.w;
    this->x = orig.x;
    this->y = orig.y;
    this->z = orig.z;
}


Quaternion::~Quaternion() {
}

Quaternion* Quaternion::clone(const Quaternion* q1){
    this->w = q1->w;
    this->x = q1->x;
    this->y = q1->y;
    this->z = q1->z;
    
    return this;
}

Quaternion* Quaternion::add(const Quaternion* q1,const Quaternion* q2) {
    this->w = q1->w+q2->w;
    this->x = q1->x+q2->x;
    this->y = q1->y+q2->y;
    this->z = q1->z+q2->z;
    return this;
}
Quaternion* Quaternion::sub(const Quaternion* q1) {
    return sub(this,q1);
}

Quaternion* Quaternion::sub(const Quaternion* q1,const Quaternion* q2) {
    this->w = q1->w-q2->w;
    this->x = q1->x-q2->x;
    this->y = q1->y-q2->y;
    this->z = q1->z-q2->z;
    return this;
}
Quaternion* Quaternion::add(const Quaternion* q1) {
    return add(this,q1);
}

Quaternion* Quaternion::mul(const Quaternion* q1,const Quaternion* q2) {
    float w,x,y,z;
    
    w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
    x = q1->w * q2->x + q2->w * q1->x + q2->z * q1->y - q2->y * q1->z;
    y = q1->w * q2->y + q2->w * q1->y + q1->z * q2->x - q1->x * q2->z;
    z = q1->w * q2->z + q2->w * q1->z + q1->x * q2->y - q1->y * q2->x;
    
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
    
    return this;
}

Quaternion* Quaternion::mul(const Quaternion* q1){
    return mul(this,q1);
}

Quaternion* Quaternion::mul(const Quaternion* q,float f) {
    this->w =q->w * f;
    this->x =q->x * f;
    this->y =q->y * f;
    this->z =q->z * f;
    
    return this;
}
Quaternion* Quaternion::mul(float f){
    return mul(this,f);
}

Quaternion* Quaternion::con(const Quaternion* q) {
    this->w = q->w;
    this->x = -q->x;
    this->y = -q->y;
    this->z = -q->z;
    
    return this;
}

Quaternion* Quaternion::con() {
    this->x *= -1;
    this->y *= -1;
    this->z *= -1;
}

float Quaternion::norm() const{
    return sqrtf(this->w * this->w + this->x * this->x + this->y * this->y + this->z * this->z);
}

float Quaternion::normalize() {
    float norm = sqrtf(this->w * this->w + this->x * this->x + this->y * this->y + this->z * this->z);
    this->w /= norm;
    this->x /= norm;
    this->y /= norm;
    this->z /= norm;

    return norm;
}


float Quaternion::normalize(const Quaternion* q) {
    float norm = q->norm();
    this->w = q->w/norm;
    this->x = q->x/norm;
    this->y = q->y/norm;
    this->z = q->z/norm;

    return norm;
}

Quaternion* Quaternion::rotate(const Quaternion* vect,const Quaternion* roter){
    float norm;
    Quaternion tmp1 = *roter;
    tmp1.normalize();
    Quaternion tmp2 = tmp1;
    tmp2.con();
    norm = vect->norm();
    
    this->mul(&tmp1,vect);
    this->mul(&tmp2);
    this->mul(norm);
    
    return this;
}

Quaternion* Quaternion::rotate(const Quaternion* roter) {
    return rotate(this,roter);
}

float Quaternion::getRadHeading() {
    Quaternion tmpQ1 = Quaternion(*this);
    Quaternion tmpQ2 = Quaternion(0.0, 1.0, 0.0, 0.0);
    Quaternion tmpQ3 = Quaternion(*this);
    tmpQ1.con();
    tmpQ2.mul(&tmpQ1);
    tmpQ3.mul(&tmpQ2);

    return atan2f(tmpQ3.y, tmpQ3.x);
}

float Quaternion::getRadPitch(float radHeading) {
    Quaternion tmpQ1 = Quaternion(cosf(-radHeading / 2), 0.0f, 0.0f, sinf(-radHeading / 2));
    Quaternion tmpQ2 = Quaternion(0.0f, 1.0f, 0.0f, 0.0f);
    Quaternion tmpQ3 = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

    tmpQ1.mul(this);
    tmpQ3.con(&tmpQ1);
    tmpQ2.mul(&tmpQ3);
    tmpQ1.mul(&tmpQ2);

    return atan2f(-tmpQ1.z, tmpQ1.x);
}

float Quaternion::getRadRole(float radHeading, float radPitch) {
    float role;
    Quaternion tmpQ2 = Quaternion(cosf(-radHeading / 2), 0.0f, 0.0f, sinf(-radHeading / 2));
    Quaternion tmpQ3 = Quaternion(cosf(-radPitch / 2), 0.0f, sinf(-radPitch / 2), 0.0f);
    tmpQ2.mul(this);
    tmpQ3.mul(&tmpQ2);
    role = atan2f(tmpQ3.x, tmpQ3.w)*2;
    if (role<-M_PI) {
        role += M_PI * 2;
    } else if (M_PI < role) {
        role -= M_PI * 2;
    }

    return role;
}

void Quaternion::getRadPitchRoleHeading(float* pitch, float* role, float* heading) {
    *heading = getRadHeading();
    *pitch = getRadPitch(*heading);
    *role = getRadRole(*heading, *pitch);
}

float Quaternion::getRadSlant() {
    float radHeading = this->getRadHeading();

    Quaternion tmpQ1 = Quaternion(cosf(-radHeading / 2), 0.0f, 0.0f, sinf(-radHeading / 2));

    tmpQ1.mul(this);

    return 2 * acosf(tmpQ1.w);

}

float Quaternion::vectDot(const Quaternion* vect) {
    return this->x * vect->x + this->y * vect->y + this->z * vect->z;
}

Quaternion* Quaternion::vectCross(const Quaternion* vect) {
    float x, y, z;
    x = this->y * vect->z - this->z * vect->y;
    y = this->z * vect->x - this->x * vect->z;
    z = this->x * vect->y - this->y * vect->x;

    this->w = 0.0f;
    this->x = x;
    this->y = y;
    this->z = z;
    
    return this;
}

void Quaternion::print() {
    printf("%f\t%fi\t%fj\t%fk\r\n", w, x, y, z);
}

void Quaternion::printPitchRoleHeading() {
    float pitch, role, heading;
    heading = getRadHeading();
    pitch = getRadPitch(heading);
    role = getRadRole(heading, pitch);

    printf("pitch:%f,\trole%f,\theading%f\r\n", pitch * 180 / M_PI, role * 180 / M_PI, heading * 180 / M_PI);
}
