#include <pch.h>

#include "SerialCommand.h"
#include "CommandList.h"

#include "Device/Logger/Logger.h"
#include "Device/Stdout/Stdout.h"
#include "Test.h"


#include "Device/Util/Util.h"

#ifdef __cplusplus
extern "C" {
#endif

char* vTaskCommandBuf;

SerialCommand::SerialCommand(){
	vTaskCommandBuf = (char*)malloc(sizeof(char)*768);
}

void SerialCommand::prvSerialCommandTaskEntry(void* pvParameters){
	SerialCommand::GetInstance()->serialCommandTask(pvParameters);
}

void SerialCommand::serialCommandTask(void* pvParameters){
	vTaskDelay(MS_INITIAL_DELAY);
	char* line;
	while(1){
		line = Stdout::GetInstance()->readLine();
		handleSerialCommand(line);
	}
}

/**
 * @brief コマンドをパースし、CommandList.hで定義された、対応する関数を呼び出す。
 */
void SerialCommand::handleSerialCommand(char* line){
	int i=0;
	
	if(strlen(line)==0){
		return;
	}
	
	
	while(commandList[i].command != NULL){
		if(strncmp(line,commandList[i].command,strlen(commandList[i].command))==0){
			args = line+strlen(commandList[i].command)+1;
			(commandList[i].function)( line+strlen(commandList[i].command) );
			return;
		}
		i++;
	}
	printf("invalid command:%s\r\n",line);
}

/**
 * @brief コマンドの引数を返す
 * 
 * @return コマンドの引数
 */
char* SerialCommand::getArgs(){
	return args;
}

/**
 * @brief asciiコードの16進数2桁の文字列をunsigned charに変換する。
 * 
 * 例：hexString = "2A"の場合、16*2 + 10 = 42を返す 
 * 
 * @return 文字列が表す値
 */
unsigned char SerialCommand::hexToUchar(char* hexString){
	unsigned int upper;
	unsigned int lower;
	if(hexString[0]<='9'){
		upper = hexString[0]-'0';
	}else if(hexString[0]<='F'){
		upper = hexString[0]-'A'+10;
	}else{
		upper = hexString[0]-'a'+10;
	}
	if(hexString[1]<='9'){
		lower = hexString[1]-'0';
	}else if(hexString[1]<='F'){
		lower = hexString[1]-'A'+10;
	}else{
		lower = hexString[1]-'a'+10;
	}
	
	return (unsigned char)(upper<<4|lower); 
}

/**
 * @brief CommandList.hで定義されたコマンドの一覧を標準出力に出力する。
 */
void SerialCommand::printHelp(){
	int i=0;
	while(commandList[i].command != NULL){
		printf("%s\r\n",commandList[i].command);
		i++;
	}
}

/**
 * @brief ログのSDカードの書き込みを開始する。
 * 
 * @param[in] arg 書き込みを開始するファイル名
 */
void SerialCommand::startLogging(char* arg){
	if(arg[0] == 0){
		printf("usage: startLogging filename\r\n");
		return;
	}
	
	Logger::GetInstance()->startLogging(arg+1);
	
}

/**
 * @brief ログのSDカードへの書き込みを停止する
 */
void SerialCommand::stopLogging(){
	Logger::GetInstance()->stopLogging();
}


/**
 * @brief コンソール入力時、入力した文字を標準出力に表示する
 */
void SerialCommand::echoOn(){
	Stdout::GetInstance()->setEcho(1);
}
/**
 * @brief コンソール入力時、入力した文字を標準出力に表示しない
 */
void SerialCommand::echoOff(){
	Stdout::GetInstance()->setEcho(0);
}

void SerialCommand::initializeUserFlash(){
	Util::GetInstance()->userflashFlush();
}

/**
 * @brief 現在起動中のタスクと残りスタック量の一覧を返す。
 * 
 * 内部でvTaskListを呼び、その結果を標準出力に出力している。
 */
void SerialCommand::printTaskList(){
	vTaskList(vTaskCommandBuf);
	printf("task name\tstat\tprirty\tstack\ttasknum\r\n");
	printf(vTaskCommandBuf);
}

/**
 * @brief タスクのCPU使用率を表示する
 * 
 * 内部でvTaskGetRunTimeStatsを呼び、その結果を標準出力に出力している。
 */
void SerialCommand::runTimeStats(){
	vTaskGetRunTimeStats(vTaskCommandBuf);
	printf(vTaskCommandBuf);
}

void SerialCommand::startTrace(){
	Util::GetInstance()->traceStart();
}

/**
 * @brief デバッグ用コマンド0
 * 
 * 関数の中身を書き換え、デバッグ用に使用可能
 */
void SerialCommand::testCmd0(){
	
}
/**
 * @brief デバッグ用コマンド1
 * 
 * 関数の中身を書き換え、デバッグ用に使用可能
 */
void SerialCommand::testCmd1(){
	
}
/**
 * @brief デバッグ用コマンド2
 * 
 * 関数の中身を書き換え、デバッグ用に使用可能
 */
void SerialCommand::testCmd2(){
}
#ifdef __cplusplus
}
#endif
