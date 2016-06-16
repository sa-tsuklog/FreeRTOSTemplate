#ifndef __HAL_SERIAL_COMMAND_H__
#define __HAL_SERIAL_COMMAND_H__

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_MESSAGE "TTTTT"

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
	static void readFloatParam(const char* message,float* ptr);
public:
	static void prvSerialCommandTaskEntry(void* pvParameters);
	void serialCommandTask(void* pvParameters);
	void handleSerialCommand(char* line);
	char* getArgs();
	
	static void usctl();
	static void printHelp();
	static void startLogging(char* arg);
	static void stopLogging();
	static void echoOn();
	static void echoOff();
	static void initializeUserFlash();
	static void printTaskList();
	static void runTimeStats();
	static void startTrace();
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
