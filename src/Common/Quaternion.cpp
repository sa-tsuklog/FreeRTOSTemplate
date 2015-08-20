/**
 * @file Quaternion.cpp
 * @brief 単精度浮動小数点によるクォータニオン
 *
 * @author sa
 * @date 2014-04-13
 * 
 */
#include <pch.h>
#include "MyLib/Util/Util.h"

#include "Quaternion.h"

/**
 * @brief 全ての要素が0であるクォータニオンを作成する
 */
Quaternion::Quaternion() {
    this->w = this->x = this->y = this->z = 0;
}

/**
 * @brief 要素がそれぞれw,x,y,zであるクォータニオンを作成する。
 */
Quaternion::Quaternion(float w, float x, float y, float z) {
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

/**
 * @brief クォータニオンをコピーする。
 */
Quaternion::Quaternion(const Quaternion& orig){
    this->w = orig.w;
    this->x = orig.x;
    this->y = orig.y;
    this->z = orig.z;
}


Quaternion::~Quaternion() {
}

/**
 * @brief クォータニオンをコピーする
 * 
 * @param[in] q1 コピー元のクォータニオンのポインタ 
 * @return 自身のポインタ
 */
Quaternion* Quaternion::clone(const Quaternion* q1){
    this->w = q1->w;
    this->x = q1->x;
    this->y = q1->y;
    this->z = q1->z;
    
    return this;
}
/**
 * @brief クォータニオンの和
 * 
 * 自身+q1の和を計算し、結果を自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::add(const Quaternion* q1) {
    return add(this,q1);
}
/**
 * @brief クォータニオンの和
 * 
 * q1+q2を計算し、結果を自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::add(const Quaternion* q1,const Quaternion* q2) {
    this->w = q1->w+q2->w;
    this->x = q1->x+q2->x;
    this->y = q1->y+q2->y;
    this->z = q1->z+q2->z;
    return this;
}

/**
 * @brief クォータニオンの差
 * 
 * 自身-q1を計算し、結果を自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::sub(const Quaternion* q1) {
    return sub(this,q1);
}
/**
 * @brief クォータニオンの差
 * 
 * q1-q2を計算し、結果を自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::sub(const Quaternion* q1,const Quaternion* q2) {
    this->w = q1->w-q2->w;
    this->x = q1->x-q2->x;
    this->y = q1->y-q2->y;
    this->z = q1->z-q2->z;
    return this;
}

/**
 * @brief クォータニオンの積
 * 
 * q1*q2を計算し、結果を自身に格納する
 * @return 自身へのポインタを返す
 */
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

/**
 * @brief クォータニオンの積
 * 
 * 自身*q1を計算し、結果を自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::mul(const Quaternion* q1){
    return mul(this,q1);
}

/**
 * @brief クォータニオンと実数の積
 * 
 * q1*fを計算し、結果を自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::mul(const Quaternion* q1,float f) {
    this->w =q1->w * f;
    this->x =q1->x * f;
    this->y =q1->y * f;
    this->z =q1->z * f;
    
    return this;
}

/**
 * @brief クォータニオンと実数の積
 * 
 * 自身*fを計算し、結果を自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::mul(float f){
    return mul(this,f);
}

/**
 * @brief 共役クォータニオン
 * 
 * q1の共役クォータニオンを自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::con(const Quaternion* q1) {
    this->w = q1->w;
    this->x = -q1->x;
    this->y = -q1->y;
    this->z = -q1->z;
    
    return this;
}

/**
 * @brief 共役クォータニオン
 * 
 * 自身の共役クォータニオンを自身に格納する
 * @return 自身のポインタを返す
 */
Quaternion* Quaternion::con() {
    this->x *= -1;
    this->y *= -1;
    this->z *= -1;
    
    return this;
}

float Quaternion::abs(){
	return sqrt(this->w*this->w + this->x*this->x + this->y*this->y + this->z*this->z);
}

/**
 * @brief クォータニオンのノルム
 * 
 * @return 自身のノルムを返す
 */
float Quaternion::norm() const{
    return sqrtf(this->w * this->w + this->x * this->x + this->y * this->y + this->z * this->z);
}

/**
 * @brief クォータニオンの正規化
 * 
 * 自身を正規化し、結果を自身に格納する
 * @return 正規化前のノルムを返す
 */
float Quaternion::normalize() {
    float norm = sqrtf(this->w * this->w + this->x * this->x + this->y * this->y + this->z * this->z);
    this->w /= norm;
    this->x /= norm;
    this->y /= norm;
    this->z /= norm;

    return norm;
}

/**
 * @brief クォータニオンの正規化
 * 
 * qを正規化し、結果を自身に格納する
 * @return 正規化前のノルムを返す
 */
float Quaternion::normalize(const Quaternion* q) {
    float norm = q->norm();
    this->w = q->w/norm;
    this->x = q->x/norm;
    this->y = q->y/norm;
    this->z = q->z/norm;

    return norm;
}

/**
 * @brief ベクトルの回転
 * 
 * ベクトルを回転し、結果を自身に格納する
 * 
 * @param[in] vect 回転させるベクトル
 * @param[in] roter 回転を表すクォータニオン
 * @return 自身へのポインタを返す。
 */
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

/**
 * @brief ベクトルの回転
 * 
 * ベクトルを回転し、結果を自身に格納する
 * 自身はベクトル(w=0)である必要がある。
 * 
 * @param[in] roter 回転を表すクォータニオン
 * @return 自身へのポインタを返す。
 */
Quaternion* Quaternion::rotate(const Quaternion* roter) {
    return rotate(this,roter);
}

/**
 * @brief 回転を表すクオータニオンからオイラー角のheadingを計算する
 * 
 * @return headingの値[rad]
 */
float Quaternion::getRadHeading() {
    Quaternion tmpQ1 = Quaternion(*this);
    Quaternion tmpQ2 = Quaternion(0.0, 1.0, 0.0, 0.0);
    Quaternion tmpQ3 = Quaternion(*this);
    tmpQ1.con();
    tmpQ2.mul(&tmpQ1);
    tmpQ3.mul(&tmpQ2);

    return atan2f(tmpQ3.y, tmpQ3.x);
}

/**
 * @brief 回転を表すクオータニオンからオイラー角のheadingを計算する
 * 
 * @param[in] クォータニオンのheading[rad]
 * @return pitchの値[rad]
 */
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

/**
 * @brief 回転を表すクオータニオンからオイラー角のrollを計算する
 * 
 * @param[in] クォータニオンのheading[rad]
 * @param[in] クォータニオンのpitch[rad]
 * @return rollの値[rad]
 */
float Quaternion::getRadRoll(float radHeading, float radPitch) {
    float roll;
    Quaternion tmpQ2 = Quaternion(cosf(-radHeading / 2), 0.0f, 0.0f, sinf(-radHeading / 2));
    Quaternion tmpQ3 = Quaternion(cosf(-radPitch / 2), 0.0f, sinf(-radPitch / 2), 0.0f);
    tmpQ2.mul(this);
    tmpQ3.mul(&tmpQ2);
    roll = atan2f(tmpQ3.x, tmpQ3.w)*2;
    if (roll<-M_PI) {
        roll += M_PI * 2;
    } else if (M_PI < roll) {
        roll -= M_PI * 2;
    }

    return roll;
}

/**
 * @brief 回転を表すクオータニオンからオイラー角のpitch,roll,headingを計算する
 * 
 * @param[out] クォータニオンのpitch[rad]
 * @param[out] クォータニオンのroll[rad]
 * @param[out] クォータニオンのheading[rad]
 */
void Quaternion::getRadPitchRollHeading(float* pitch, float* roll, float* heading) {
    *heading = getRadHeading();
    *pitch = getRadPitch(*heading);
    *roll = getRadRoll(*heading, *pitch);
}


/**
 * @brief 回転を表すクォータニオンからスラント角を計算する
 * @return スラント角
 */
float Quaternion::getRadSlant() {
    float radHeading = this->getRadHeading();

    Quaternion tmpQ1 = Quaternion(cosf(-radHeading / 2), 0.0f, 0.0f, sinf(-radHeading / 2));

    tmpQ1.mul(this);

    return 2 * acosf(tmpQ1.w);

}

/**
 * @brief ベクトルのドット積を計算する
 * 
 * 自身とvectのドット積を計算する。自身、vectはベクトル(w=0)である必要がある
 * @return 自身・vectの値
 */
float Quaternion::vectDot(const Quaternion* vect) {
    return this->x * vect->x + this->y * vect->y + this->z * vect->z;
}

/**
 * @brief ベクトルのクロス積を計算する
 * 
 * 自身とvectのクロス積を計算する。自身、vectはベクトル(w=0)である必要がある。結果は自身に格納される
 * @return 自身のポインタを返す
 */
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

/**
 * @brief クォータニオンを標準出力に出力する
 * 
 * 形式はタブ区切りでw,x,y,zの順
 */
void Quaternion::print() {
	printf("%f\t%fi\t%fj\t%fk\r\n", w, x, y, z);
}

/**
 * @brief 回転を表すクォータニオンのオイラー角を標準出力に出力する
 */
void Quaternion::printPitchRollHeading() {
    float pitch, roll, heading;
    heading = getRadHeading();
    pitch = getRadPitch(heading);
    roll = getRadRoll(heading, pitch);

    Util::GetInstance()->myFprintf(0,stdout,"pitch:%f,\troll%f,\theading%f\r\n", pitch * 180 / M_PI, roll * 180 / M_PI, heading * 180 / M_PI);
}
