#ifndef __HAL_SERIAL_COMMAND_H__
#define __HAL_SERIAL_COMMAND_H__

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @class SerialCommand
 * 
 * @brief コンソール入力の処理を行う
 * 
 * このクラスの関数は基本的に外部からは呼ばれず、
 * handleSerialCommandからコマンド入力によって呼ばれる。
 */

class SerialCommand{
private:
	SerialCommand();
	SerialCommand(const SerialCommand& rhs);
	SerialCommand& operator=(const SerialCommand& rhs);
	virtual ~SerialCommand() {}
public:
	static SerialCommand* GetInstance() {
		static SerialCommand instance;
    	return &instance;
	}

// Class definition
private:
	char* args;
	static unsigned char hexToUchar(char* hexString);
public:
	static void SerialCommand::prvSerialCommandTaskEntry(void* pvParameters);
	void SerialCommand::serialCommandTask(void* pvParameters);
	void handleSerialCommand(char* line);
	char* getArgs();
	
	static void setServo();
	static void printHelp();
	static void printTaskList();
	static void startLogging(char* arg);
	static void stopLogging();
	static void resetImu();
	static void resetGpsRef();
	static void gpsInvalidate();
	static void gpsValidate();
	static void echoOn();
	static void echoOff();
	static void setServosTrim();
	static void setPidGain();
	static void calibrateMpuGyro();
	static void setMpuAclBias();
	static void setMpuCmpsBias();
	static void setAdisCmpsBias();
	static void showGainsConfig();
	static void setGainsConfig();
	static void setGainsPrintMode(char* arg);
	static void initializeUserFlash();
	static void testCmd0();
	static void testCmd1();
	static void testCmd2();
};

typedef struct{
	const char* command;
	void (*function)(char* arg);
}command_t;

#ifdef __cplusplus
}
#endif


#endif
