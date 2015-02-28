#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SerialCommand.h"
#include "CommandList.h"
#include "GeneralConfig.h"

#include "MyLib/Logger/Logger.h"
#include "MyLib/Stdout/Driver/USART3.h"
#include "MyLib/Gains/Gains.h"
#include "MyLib/Gains/Driver/Gps/Gps.h"
#include "MyLib/Gains/Driver/Mpu9250/MPU9250.h"
#include "MyLib/Servo/Servo.h"
#include "MyLib/CmdServo/CmdServo.h"

#include "ControlParams.h"

#include "App/TankControl/TankControl.h"

#include "Test.h"


#include "MyLib/Util/Util.h"

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
		line = USART3Class::GetInstance()->readLine();
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
 * @brief ジョイパッド入力を制御タスクに渡す
 * 
 * 引数はHH,HH,HH,HH,HH,HH\nであること。Hはasciiコードの16進数の一文字を現す。
 */
void SerialCommand::usctl(){
	SerialCommand* cmd = SerialCommand::GetInstance();
	char* args = cmd->getArgs();
	unsigned char mode     = cmd->hexToUchar(args);
	unsigned char pitch    = cmd->hexToUchar(args+3);
	unsigned char roll     = cmd->hexToUchar(args+6);
	unsigned char yaw      = cmd->hexToUchar(args+9);
	unsigned char rz       = cmd->hexToUchar(args+12);
	unsigned char throttle = cmd->hexToUchar(args+15);
	unsigned char cameraH_H= cmd->hexToUchar(args+18);
	unsigned char cameraH_L= cmd->hexToUchar(args+21);
	unsigned char cameraV_H= cmd->hexToUchar(args+24);
	unsigned char cameraV_L= cmd->hexToUchar(args+27);
	unsigned char reserved = cmd->hexToUchar(args+30);
	unsigned char camMode  = cmd->hexToUchar(args+33);
	
	ControlParams controlParams = ControlParams(mode,
			pitch,
			roll,
			yaw,
			rz,
			throttle,
			cameraH_H<<8 | cameraH_L,
			cameraV_H<<8 | cameraV_L,
			reserved,
			camMode);
	
	TankControl::GetInstance()->setControlParms(&controlParams);
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
		vTaskDelay(1);
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
 * @brief EKFの位置、速度の推定値を0に戻す
 */
void SerialCommand::resetImu(){
	Gains::GetInstance()->resetImu();
}
/**
 * @brief GPSの基準座標を現在の座標にする
 */
void SerialCommand::resetGpsRef(){
	Gps::GetInstance()->resetRefPosition();
}
/**
 * @brief コンソール入力時、入力した文字を標準出力に表示する
 */
void SerialCommand::echoOn(){
	USART3Class::GetInstance()->setEcho(1);
}
/**
 * @brief コンソール入力時、入力した文字を標準出力に表示しない
 */
void SerialCommand::echoOff(){
	USART3Class::GetInstance()->setEcho(0);
}
/**
 * @brief サーボのトリム調整を行う
 * 
 * トリム調整はu、dを入力することで行う。
 * 
 * uでパルス幅が長くなる方向に、dでパルス幅が短くなる方向に変化する。
 * 
 * shiftキーを押しながらu,dを押すことで微調整が行える。
 * 
 * トリム設定が完了したらenterを押すことで、次のサーボのトリム調整に移る
 * 
 * トリムの値はフラッシュに記録され、電源を切った後も有効である。
 */
void SerialCommand::setServosTrim(){
	for(int i=0;i<Servo::CH_NUM;i++){
		printf("ch %d,%d\r\n",i,Util::GetInstance()->flashData.servoTrim[i]);
		while(1){
			char c = USART3Class::GetInstance()->getChar();
			
			if(c == 'u'){
				Util::GetInstance()->flashData.servoTrim[i] += 20;
			}else if(c == 'U'){
				Util::GetInstance()->flashData.servoTrim[i] += 1;
			}else if(c == 'd'){
				Util::GetInstance()->flashData.servoTrim[i] -= 20;
			}else if(c == 'D'){
				Util::GetInstance()->flashData.servoTrim[i] -= 1;
			}else if(c == '0'){
				Util::GetInstance()->flashData.servoTrim[i] = 0;
			}else if(c == '\n'){
				break;
			}
			printf("ch %d,%d\r\n",i,Util::GetInstance()->flashData.servoTrim[i]);
		}
	}
	Util::GetInstance()->userflashFlush();
}

/**
 * @brief PID制御のためのパラメータを設定する
 * 
 * 浮動小数点数を入力し、enterを押すことでパラメータが上書きされる。
 * 
 * 不正な文字列が入力された場合、パラメータは0となる。
 * 
 * 何も入力せずにenterを押した場合、以前の設定値がそのまま保持される。
 * 
 * パラメータはP,I,Dそれぞれに3つづつあり、0:role 1:pitch 2:yaw方向のゲインである。
 * 
 * パラメータはフラッシュに記録され、電源を切った後も有効である。
 */
void SerialCommand::setPidGain(){
	for(int i=0;i<3;i++){
		float f;
		printf("p gain %d:",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString) != 0){
			f = atof(biasString);
			Util::GetInstance()->flashData.pGain[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.pGain[i]);
	}
	for(int i=0;i<3;i++){
		float f;
		printf("i gain %d:",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString) != 0){
			f = atof(biasString);
			Util::GetInstance()->flashData.iGain[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.iGain[i]);
	}
	for(int i=0;i<3;i++){
		float f;
		printf("d gain %d:",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString) != 0){
			f = atof(biasString);
			Util::GetInstance()->flashData.dGain[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.dGain[i]);
	}
	Util::GetInstance()->userflashFlush();
}

/**
 * @brief MPU-9250のジャイロの校正を行う。
 * 
 * 5秒間ジャイロのデータを計測し、平均値が0となるようジャイロの補正値を設定する。
 * 
 * 計測を行っている5秒間の間は基板に角速度を与えるべきではない。
 * 
 * 補正値はフラッシュに記録され、電源を切った後も有効である。
 */
void SerialCommand::calibrateMpuGyro(){
	Mpu9250::getInstance()->startGyroCalibration();
}

/**
 * @brief MPU-9250の加速度の校正を行う
 * 
 * 補正値を浮動小数点数で入力する。
 * 
 * 補正値の推定は手動で行う必要がある。
 * 
 * 補正値はフラッシュに記録され、電源を切った後も有効である。
 */
void SerialCommand::setMpuAclBias(){
	for(int i=0;i<3;i++){
		float f;
		printf("%d th axis:\r\n",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString)!=0){
			f = atof(biasString);
			Util::GetInstance()->flashData.mpuAclBias[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.mpuAclBias[i]);
	}
	Util::GetInstance()->userflashFlush();	
}

/**
 * @brief MPU-9250の地磁気センサの校正を行う
 * 
 * 補正値を浮動小数点数で入力する。
 * 
 * 補正値の推定は手動で行う必要がある。
 * 
 * 補正値はフラッシュに記録され、電源を切った後も有効である。
 */
void SerialCommand::setMpuCmpsBias(){
	for(int i=0;i<3;i++){
		float f;
		printf("%d th axis:\r\n",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString)){
			f = atof(biasString);
			Util::GetInstance()->flashData.mpuCmpsBias[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.mpuCmpsBias[i]);
	}
	Util::GetInstance()->userflashFlush();
}

/**
 * @brief MPU-9250の温度係数の設定を行う
 * 
 * 補正値を浮動小数点数で入力する。
 * 
 * 補正値の推定は手動で行う必要がある。
 * 
 * 補正値はフラッシュに記録され、電源を切った後も有効である。
 */
void SerialCommand::setMpuTempCoeff(){
	for(int i=0;i<3;i++){
		float f;
		printf("acl %d th axis:\r\n",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString)){
			f = atof(biasString);
			Util::GetInstance()->flashData.mpuAclTempCoefficient[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.mpuAclTempCoefficient[i]);
	}
	
	for(int i=0;i<3;i++){
		float f;
		printf("gyro %d th axis:\r\n",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString)){
			f = atof(biasString);
			Util::GetInstance()->flashData.mpuGyroTempCoefficient[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.mpuGyroTempCoefficient[i]);
	}
	
	for(int i=0;i<3;i++){
		float f;
		printf("cmps %d th axis:\r\n",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString)){
			f = atof(biasString);
			Util::GetInstance()->flashData.mpuCmpsTempCoefficient[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.mpuCmpsTempCoefficient[i]);
	}
	
	Util::GetInstance()->userflashFlush();
}

/**
 * @brief ADIS16488の地磁気センサの校正を行う
 * 
 * 補正値を浮動小数点数で入力する。
 * 
 * 補正値の推定は手動で行う必要がある。
 * 
 * 補正値はフラッシュに記録され、電源を切った後も有効である。
 */
void SerialCommand::setAdisCmpsBias(){
	for(int i=0;i<3;i++){
		float f;
		printf("%d th axis:\r\n",i);
		fflush(stdout);
		char* biasString = USART3Class::GetInstance()->readLine();
		if(strlen(biasString)){
			f = atof(biasString);
			Util::GetInstance()->flashData.adisCmpsBias[i] = f;
		}
		printf("%f\r\n",Util::GetInstance()->flashData.adisCmpsBias[i]);
	}
	Util::GetInstance()->userflashFlush();
}



/**
 * @brief Gainsで使用するセンサを表示する
 */
void SerialCommand::showGainsConfig(){
	printf("imu type:\r\n");
	if(Util::GetInstance()->flashData.imuType == ImuType::ADIS16488){
		printf("    ADIS16488\r\n");
	}else if(Util::GetInstance()->flashData.imuType == ImuType::MPU9250){
		printf("    MPU-9250\r\n");
	}else if(Util::GetInstance()->flashData.imuType == ImuType::MPU9250){
		printf("    MPU-9250 + BMP085\r\n");
	}else{
		printf("    invalid value\r\n");
	}
	
	printf("gps type:\r\n");
	if(Util::GetInstance()->flashData.gpsType == GpsType::USART_GPS){
		printf("    USART GPS\r\n");
	}else if(Util::GetInstance()->flashData.gpsType == GpsType::DUMMY_GPS){
		printf("    DUMMY GPS\r\n");
	}else{
		printf("    invalid value\r\n");
	}
}
/**
 * @brief Gainsで使用するセンサを設定する。
 * 
 * 本設定は次回起動時から有効である。
 * 
 * 設定はフラッシュに保存され、電源を切った後も有効である。
 */
void SerialCommand::setGainsConfig(){
	char c;
	char* line;
	printf("select imu type\r\n");
	printf("    %d:ADIS16488\r\n",ImuType::ADIS16488);
	printf("    %d:MPU-9250\r\n",ImuType::MPU9250);
	printf("    %d:MPU-9250 + BMP085\r\n",ImuType::MPU9250_BMP850);
	printf(">>");
	fflush(stdout);
	line = USART3Class::GetInstance()->readLine();
	
	if(line[0]-'0' == ImuType::ADIS16488){
		//printf("ADIS16488 selected\r\n");
		Util::GetInstance()->flashData.imuType = ImuType::ADIS16488;
	}else if(line[0]-'0' == ImuType::MPU9250){
		//printf("MPU-9250 selected\r\n");
		Util::GetInstance()->flashData.imuType = ImuType::MPU9250;
	}else if(line[0]-'0' == ImuType::MPU9250){
		//printf("MPU-9250 + BMP085 selected\r\n");
		Util::GetInstance()->flashData.imuType = ImuType::MPU9250_BMP850;
	}else if(line[0] == 0){
		//do nothing
	}else{
		printf("invalid value\r\n");
	}
	
	printf("select gps type\r\n");
	printf("    %d:USART GPS\r\n",GpsType::USART_GPS);
	printf("    %d:DUMMY GPS\r\n",GpsType::DUMMY_GPS);
	printf(">>");
	fflush(stdout);
	line = USART3Class::GetInstance()->readLine();
	
	if(line[0]-'0' == GpsType::USART_GPS){
		//printf("USART GPS selected\r\n");
		Util::GetInstance()->flashData.gpsType = GpsType::USART_GPS;
	}else if(line[0]-'0' == GpsType::DUMMY_GPS){
		//printf("DUMMY GPS selected\r\n");
		Util::GetInstance()->flashData.gpsType = GpsType::DUMMY_GPS;
	}else if(line[0]==0){
		
	}else{
		printf("invalid value\r\n");
	}
	Util::GetInstance()->userflashFlush();
	printf("\r\n");
	showGainsConfig();
	
}

void SerialCommand::setGainsPrintMode(char* arg){
	if(arg[0] == 0){
		return;
	}
	arg++;//skip first white space
	
	if(strncmp(arg,"none",4)==0){
		Gains::GetInstance()->setPrintType(GainsPrintMode::NONE);
	}else if(strncmp(arg,"maritime",8)==0){
		Gains::GetInstance()->setPrintType(GainsPrintMode::MARITIME);
	}else if(strncmp(arg,"eframe",6)==0){
		Gains::GetInstance()->setPrintType(GainsPrintMode::E_FRAME);
	}else if(strncmp(arg,"ins",3)==0){
		Gains::GetInstance()->setPrintType(GainsPrintMode::INS);
	}else if(strncmp(arg,"quat",4)==0){
		Gains::GetInstance()->setPrintType(GainsPrintMode::QUATERNION);
	}else if(strncmp(arg,"gpaio",5)==0){
		Gains::GetInstance()->setPrintType(GainsPrintMode::GPAIO);
	}else if(strncmp(arg,"debug",5)==0){
		Gains::GetInstance()->setPrintType(GainsPrintMode::DEBUG);
	}else{
		printf("usage: setGainsPrintMode none|maritime|eframe|ins|quat|gpaio|debug\r\n");
	}
}

void SerialCommand::initializeUserFlash(){
	Util::GetInstance()->flashData.imuType = ImuType::MPU9250;
	Util::GetInstance()->flashData.gpsType = GpsType::DUMMY_GPS;
	for(int i=0;i<3;i++){
		Util::GetInstance()->flashData.mpuAclBias[i]=0.0;
		Util::GetInstance()->flashData.mpuGyroBias[i]=0.0;
		Util::GetInstance()->flashData.mpuCmpsBias[i]=0.0;
		Util::GetInstance()->flashData.mpuAclTempCoefficient[i]=0.0;
		Util::GetInstance()->flashData.mpuGyroTempCoefficient[i]=0.0;
		Util::GetInstance()->flashData.mpuCmpsTempCoefficient[i]=0.0;
	}
	Util::GetInstance()->userflashFlush();
}

void SerialCommand::setCmdServoCh(){
	int oldCh,newCh;
	
	printf("old channel:");
	fflush(stdout);
	char* line = USART3Class::GetInstance()->readLine();
	oldCh = atoi(line);
	printf("new channel:");
	fflush(stdout);
	line = USART3Class::GetInstance()->readLine();
	newCh = atoi(line);
	
	printf("change ch %d to ch %d\r\n",oldCh,newCh);
	CmdServo::GetInstance()->setServoId(oldCh,newCh);
	CmdServo::GetInstance()->flashFinalize(newCh);
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
	while(1){
		char c = USART3Class::GetInstance()->getChar();
		printf("getchar = %c\r\n",c);
		vTaskDelay(1);
	}
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
