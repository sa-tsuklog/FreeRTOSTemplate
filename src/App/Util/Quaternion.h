/*
 * Quaternion.h
 *
 *  Created on: 2014/04/13
 *      Author: sa
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

class Quaternion{
public:
  float w,x,y,z;
    Quaternion();
    Quaternion(float w, float x, float y, float z);
    Quaternion(const Quaternion& orig);
    ~Quaternion();
    Quaternion* clone(const Quaternion* q1);
    Quaternion* add(const Quaternion* q1,const Quaternion* q2);
    Quaternion* add(const Quaternion* q1);
    Quaternion* sub(const Quaternion* q1);
    Quaternion* sub(const Quaternion* q1,const Quaternion* q2);
    Quaternion* mul(const Quaternion* q1,const Quaternion* q2);
    Quaternion* mul(const Quaternion* q1);
    Quaternion* mul(const Quaternion* q,float f);
    Quaternion* mul(float f);
    Quaternion* con(const Quaternion* q);
    Quaternion* con();
    float normalize();
    float norm() const;
    Quaternion* rotate(const Quaternion* vect,const Quaternion* roter);
    Quaternion* rotate(const Quaternion* roter);
    float getRadHeading();
    float getRadPitch(float radHeading);
    float getRadRole(float radHeading, float radPitch);
    void getRadPitchRoleHeading(float* pitch, float* role, float* heading);
    float getRadSlant();
    float vectDot(const Quaternion* vect);
    Quaternion* vectCross(const Quaternion* vect);
    void print();
    void printPitchRoleHeading();  
};


#endif /* QUATERNION_H_ */
