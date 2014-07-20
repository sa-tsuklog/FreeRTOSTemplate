/*
 * Servo.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>

/**
 * @class Servo
 * @brief �T�[�{����p�N���X
 */


class Servo {
	// Singleton pattern definition
private:
	Servo();
	Servo(const Servo& rhs);
	Servo& operator=(const Servo& rhs);
	virtual ~Servo() {}
public:
	/**
	 * @brief ���̃N���X�̃C���X�^���X��Ԃ�
	 */
	static Servo* GetInstance() {
		static Servo instance;
		return &instance;
	}

	// Class definition
private:
	static const int USE_CH0_3 = 1;/**<1:ch0�`3���g�p���� 0:ch0�`3���g�p���Ȃ�*/
	static const int USE_CH4_7 = 1;/**<1:ch4�`7���g�p���� 0:ch4�`7���g�p���Ȃ�*/
	static const int USE_CH8_11 = 0;/**<1:ch8�`11���g�p���� 0:ch8�`11���g�p���Ȃ��Bch8�`11��"Propo"�ƃV���N����Ă���*/
	static const int USE_CH12 = 0;/**<1:ch0�`3���g�p���� 0:ch0�`3���g�p���Ȃ��Bch12��"height"�ƃV���N����Ă���*/
	
	static const uint32_t US_DUTY_CENTER = 1500;/**<�����ʒu(pos = 0)�ł̃p���X��*/
	static const uint32_t US_DUTY_GAIN = 500;/**<�p���X���̕ω���*/
	
public:
	static const int CH_NUM = 13; /**<�ő�`�����l����*/
	
	void start();
	void setPos(int ch,float pos);
};

#endif /* SERVO_H_ */
