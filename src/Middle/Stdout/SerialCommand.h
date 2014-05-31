#ifndef __HAL_SERIAL_COMMAND_H__
#define __HAL_SERIAL_COMMAND_H__

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

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
	
public:
void handleSerialCommand(char* line);
char* getArg();


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
};

typedef struct{
	const char* command;
	void (*function)(char* arg);
}command_t;

#ifdef __cplusplus
}
#endif


#endif
