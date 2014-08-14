/*
 * Propo.h
 *
 *  Created on: 2014/06/08
 *      Author: sa
 */

#ifndef PROPO_H_
#define PROPO_H_

/**
 * @class Propo
 * @brief プロポ信号取得用クラス
 */

class Propo {
	// Singleton pattern definition
private:
	Propo();
	Propo(const Propo& rhs);
	Propo& operator=(const Propo& rhs);
	virtual ~Propo() {}
public:
	static Propo* GetInstance() {
		static Propo instance;
		return &instance;
	}

	// Class definition
private:
	static const int USE_PROPO = 1;
	
	static const int US_DUTY_CENTER = 1500; /**<@brief 中立位置でのパルス幅[us]*/
	static const int US_DUTY_GAIN = 500; /**<@brief プロポ信号のパルス幅が最大/最小位置の時の中立位置からのパルス幅の差*/
	
public:
	static void initPropo();
	void start();
	float getInput(int ch);
};

#endif /* PROPO_H_ */
