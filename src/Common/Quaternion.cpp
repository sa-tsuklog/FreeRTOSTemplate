/**
 * @file Quaternion.cpp
 * @brief �P���x���������_�ɂ��N�H�[�^�j�I��
 *
 * @author sa
 * @date 2014-04-13
 * 
 */

#include "stdio.h"
#include "math.h"
#include "MyLib/Util/Util.h"

#include "Quaternion.h"

/**
 * @brief �S�Ă̗v�f��0�ł���N�H�[�^�j�I�����쐬����
 */
Quaternion::Quaternion() {
    this->w = this->x = this->y = this->z = 0;
}

/**
 * @brief �v�f�����ꂼ��w,x,y,z�ł���N�H�[�^�j�I�����쐬����B
 */
Quaternion::Quaternion(float w, float x, float y, float z) {
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

/**
 * @brief �N�H�[�^�j�I�����R�s�[����B
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
 * @brief �N�H�[�^�j�I�����R�s�[����
 * 
 * @param[in] q1 �R�s�[���̃N�H�[�^�j�I���̃|�C���^ 
 * @return ���g�̃|�C���^
 */
Quaternion* Quaternion::clone(const Quaternion* q1){
    this->w = q1->w;
    this->x = q1->x;
    this->y = q1->y;
    this->z = q1->z;
    
    return this;
}
/**
 * @brief �N�H�[�^�j�I���̘a
 * 
 * ���g+q1�̘a���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::add(const Quaternion* q1) {
    return add(this,q1);
}
/**
 * @brief �N�H�[�^�j�I���̘a
 * 
 * q1+q2���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::add(const Quaternion* q1,const Quaternion* q2) {
    this->w = q1->w+q2->w;
    this->x = q1->x+q2->x;
    this->y = q1->y+q2->y;
    this->z = q1->z+q2->z;
    return this;
}

/**
 * @brief �N�H�[�^�j�I���̍�
 * 
 * ���g-q1���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::sub(const Quaternion* q1) {
    return sub(this,q1);
}
/**
 * @brief �N�H�[�^�j�I���̍�
 * 
 * q1-q2���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::sub(const Quaternion* q1,const Quaternion* q2) {
    this->w = q1->w-q2->w;
    this->x = q1->x-q2->x;
    this->y = q1->y-q2->y;
    this->z = q1->z-q2->z;
    return this;
}

/**
 * @brief �N�H�[�^�j�I���̐�
 * 
 * q1*q2���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�ւ̃|�C���^��Ԃ�
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
 * @brief �N�H�[�^�j�I���̐�
 * 
 * ���g*q1���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::mul(const Quaternion* q1){
    return mul(this,q1);
}

/**
 * @brief �N�H�[�^�j�I���Ǝ����̐�
 * 
 * q1*f���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::mul(const Quaternion* q1,float f) {
    this->w =q1->w * f;
    this->x =q1->x * f;
    this->y =q1->y * f;
    this->z =q1->z * f;
    
    return this;
}

/**
 * @brief �N�H�[�^�j�I���Ǝ����̐�
 * 
 * ���g*f���v�Z���A���ʂ����g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::mul(float f){
    return mul(this,f);
}

/**
 * @brief �����N�H�[�^�j�I��
 * 
 * q1�̋����N�H�[�^�j�I�������g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::con(const Quaternion* q1) {
    this->w = q1->w;
    this->x = -q1->x;
    this->y = -q1->y;
    this->z = -q1->z;
    
    return this;
}

/**
 * @brief �����N�H�[�^�j�I��
 * 
 * ���g�̋����N�H�[�^�j�I�������g�Ɋi�[����
 * @return ���g�̃|�C���^��Ԃ�
 */
Quaternion* Quaternion::con() {
    this->x *= -1;
    this->y *= -1;
    this->z *= -1;
    
    return this;
}

/**
 * @brief �N�H�[�^�j�I���̃m����
 * 
 * @return ���g�̃m������Ԃ�
 */
float Quaternion::norm() const{
    return sqrtf(this->w * this->w + this->x * this->x + this->y * this->y + this->z * this->z);
}

/**
 * @brief �N�H�[�^�j�I���̐��K��
 * 
 * ���g�𐳋K�����A���ʂ����g�Ɋi�[����
 * @return ���K���O�̃m������Ԃ�
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
 * @brief �N�H�[�^�j�I���̐��K��
 * 
 * q�𐳋K�����A���ʂ����g�Ɋi�[����
 * @return ���K���O�̃m������Ԃ�
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
 * @brief �x�N�g���̉�]
 * 
 * �x�N�g������]���A���ʂ����g�Ɋi�[����
 * 
 * @param[in] vect ��]������x�N�g��
 * @param[in] roter ��]��\���N�H�[�^�j�I��
 * @return ���g�ւ̃|�C���^��Ԃ��B
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
 * @brief �x�N�g���̉�]
 * 
 * �x�N�g������]���A���ʂ����g�Ɋi�[����
 * ���g�̓x�N�g��(w=0)�ł���K�v������B
 * 
 * @param[in] roter ��]��\���N�H�[�^�j�I��
 * @return ���g�ւ̃|�C���^��Ԃ��B
 */
Quaternion* Quaternion::rotate(const Quaternion* roter) {
    return rotate(this,roter);
}

/**
 * @brief ��]��\���N�I�[�^�j�I������I�C���[�p��heading���v�Z����
 * 
 * @return heading�̒l[rad]
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
 * @brief ��]��\���N�I�[�^�j�I������I�C���[�p��heading���v�Z����
 * 
 * @param[in] �N�H�[�^�j�I����heading[rad]
 * @return pitch�̒l[rad]
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
 * @brief ��]��\���N�I�[�^�j�I������I�C���[�p��role���v�Z����
 * 
 * @param[in] �N�H�[�^�j�I����heading[rad]
 * @param[in] �N�H�[�^�j�I����pitch[rad]
 * @return role�̒l[rad]
 */
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

/**
 * @brief ��]��\���N�I�[�^�j�I������I�C���[�p��pitch,role,heading���v�Z����
 * 
 * @param[out] �N�H�[�^�j�I����pitch[rad]
 * @param[out] �N�H�[�^�j�I����role[rad]
 * @param[out] �N�H�[�^�j�I����heading[rad]
 */
void Quaternion::getRadPitchRoleHeading(float* pitch, float* role, float* heading) {
    *heading = getRadHeading();
    *pitch = getRadPitch(*heading);
    *role = getRadRole(*heading, *pitch);
}


/**
 * @brief ��]��\���N�H�[�^�j�I������X�����g�p���v�Z����
 * @return �X�����g�p
 */
float Quaternion::getRadSlant() {
    float radHeading = this->getRadHeading();

    Quaternion tmpQ1 = Quaternion(cosf(-radHeading / 2), 0.0f, 0.0f, sinf(-radHeading / 2));

    tmpQ1.mul(this);

    return 2 * acosf(tmpQ1.w);

}

/**
 * @brief �x�N�g���̃h�b�g�ς��v�Z����
 * 
 * ���g��vect�̃h�b�g�ς��v�Z����B���g�Avect�̓x�N�g��(w=0)�ł���K�v������
 * @return ���g�Evect�̒l
 */
float Quaternion::vectDot(const Quaternion* vect) {
    return this->x * vect->x + this->y * vect->y + this->z * vect->z;
}

/**
 * @brief �x�N�g���̃N���X�ς��v�Z����
 * 
 * ���g��vect�̃N���X�ς��v�Z����B���g�Avect�̓x�N�g��(w=0)�ł���K�v������B���ʂ͎��g�Ɋi�[�����
 * @return ���g�̃|�C���^��Ԃ�
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
 * @brief �N�H�[�^�j�I����W���o�͂ɏo�͂���
 * 
 * �`���̓^�u��؂��w,x,y,z�̏�
 */
void Quaternion::print() {
	printf("%f\t%fi\t%fj\t%fk\r\n", w, x, y, z);
}

/**
 * @brief ��]��\���N�H�[�^�j�I���̃I�C���[�p��W���o�͂ɏo�͂���
 */
void Quaternion::printPitchRoleHeading() {
    float pitch, role, heading;
    heading = getRadHeading();
    pitch = getRadPitch(heading);
    role = getRadRole(heading, pitch);

    Util::GetInstance()->myFprintf(0,stdout,"pitch:%f,\trole%f,\theading%f\r\n", pitch * 180 / M_PI, role * 180 / M_PI, heading * 180 / M_PI);
}
