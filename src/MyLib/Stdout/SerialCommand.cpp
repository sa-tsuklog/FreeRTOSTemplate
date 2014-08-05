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

#include "ControlParams.h"

#include "Test.h"


#include "MyLib/Util/Util.h"

#ifdef __cplusplus
extern "C" {
#endif

char* vTaskListBuf;

SerialCommand::SerialCommand(){
	vTaskListBuf = (char*)malloc(sizeof(char)*768);
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
 * @brief �R�}���h���p�[�X���ACommandList.h�Œ�`���ꂽ�A�Ή�����֐����Ăяo���B
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
 * @brief �R�}���h�̈�����Ԃ�
 * 
 * @return �R�}���h�̈���
 */
char* SerialCommand::getArgs(){
	return args;
}

/**
 * @brief �W���C�p�b�h���͂𐧌�^�X�N�ɓn��
 * 
 * ������HH,HH,HH,HH,HH,HH\n�ł��邱�ƁBH��ascii�R�[�h��16�i���̈ꕶ���������B
 */
void SerialCommand::setServo(){
	SerialCommand* cmd = SerialCommand::GetInstance();
	char* args = cmd->getArgs();
	unsigned char pitch    = cmd->hexToUchar(args);
	unsigned char roll     = cmd->hexToUchar(args+3);
	unsigned char yaw      = cmd->hexToUchar(args+6);
	unsigned char throttle = cmd->hexToUchar(args+9);
	unsigned char flaps    = cmd->hexToUchar(args+12);
	
	printf("pitch,role=%x,%x\r\n",pitch,roll);
	
	ControlParams controlParams = ControlParams(pitch,roll,yaw,throttle,flaps);
	
	setControlParms(&controlParams);
}

/**
 * @brief ascii�R�[�h��16�i��2���̕������unsigned char�ɕϊ�����B
 * 
 * ��FhexString = "2A"�̏ꍇ�A16*2 + 10 = 42��Ԃ� 
 * 
 * @return �����񂪕\���l
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
 * @brief CommandList.h�Œ�`���ꂽ�R�}���h�̈ꗗ��W���o�͂ɏo�͂���B
 */
void SerialCommand::printHelp(){
	int i=0;
	while(commandList[i].command != NULL){
		printf("%s\r\n",commandList[i].command);
		i++;
	}
}

/**
 * @brief ���݋N�����̃^�X�N�Ǝc��X�^�b�N�ʂ̈ꗗ��Ԃ��B
 * 
 * ������vTaskList���ĂсA���̌��ʂ�W���o�͂ɏo�͂��Ă���B
 */
void SerialCommand::printTaskList(){
	vTaskList(vTaskListBuf);
	printf("task name\tstat\tprirty\tstack\ttasknum\r\n");
	printf(vTaskListBuf);
}

/**
 * @brief ���O��SD�J�[�h�̏������݂��J�n����B
 * 
 * @param[in] arg �������݂��J�n����t�@�C����
 */
void SerialCommand::startLogging(char* arg){
	if(arg[0] == 0){
		printf("usage: startLogging filename\r\n");
		return;
	}
	
	Logger::GetInstance()->startLogging(arg+1);
	
}

/**
 * @brief ���O��SD�J�[�h�ւ̏������݂��~����
 */
void SerialCommand::stopLogging(){
	Logger::GetInstance()->stopLogging();
}

/**
 * @brief EKF�̈ʒu�A���x�̐���l��0�ɖ߂�
 */
void SerialCommand::resetImu(){
	Gains::GetInstance()->resetImu();
}
/**
 * @brief GPS�̊���W�����݂̍��W�ɂ���
 */
void SerialCommand::resetGpsRef(){
	Gps::GetInstance()->resetRefPosition();
}
/**
 * @brief �R���\�[�����͎��A���͂���������W���o�͂ɕ\������
 */
void SerialCommand::echoOn(){
	USART3Class::GetInstance()->setEcho(1);
}
/**
 * @brief �R���\�[�����͎��A���͂���������W���o�͂ɕ\�����Ȃ�
 */
void SerialCommand::echoOff(){
	USART3Class::GetInstance()->setEcho(0);
}
/**
 * @brief �T�[�{�̃g�����������s��
 * 
 * �g����������u�Ad����͂��邱�Ƃōs���B
 * 
 * u�Ńp���X���������Ȃ�����ɁAd�Ńp���X�����Z���Ȃ�����ɕω�����B
 * 
 * shift�L�[�������Ȃ���u,d���������ƂŔ��������s����B
 * 
 * �g�����ݒ肪����������enter���������ƂŁA���̃T�[�{�̃g���������Ɉڂ�
 * 
 * �g�����̒l�̓t���b�V���ɋL�^����A�d����؂�������L���ł���B
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
 * @brief PID����̂��߂̃p�����[�^��ݒ肷��
 * 
 * ���������_������͂��Aenter���������ƂŃp�����[�^���㏑�������B
 * 
 * �s���ȕ����񂪓��͂��ꂽ�ꍇ�A�p�����[�^��0�ƂȂ�B
 * 
 * �������͂�����enter���������ꍇ�A�ȑO�̐ݒ�l�����̂܂ܕێ������B
 * 
 * �p�����[�^��P,I,D���ꂼ���3�Â���A0:role 1:pitch 2:yaw�����̃Q�C���ł���B
 * 
 * �p�����[�^�̓t���b�V���ɋL�^����A�d����؂�������L���ł���B
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
 * @brief MPU-9250�̃W���C���̍Z�����s���B
 * 
 * 5�b�ԃW���C���̃f�[�^���v�����A���ϒl��0�ƂȂ�悤�W���C���̕␳�l��ݒ肷��B
 * 
 * �v�����s���Ă���5�b�Ԃ̊Ԃ͊�Ɋp���x��^����ׂ��ł͂Ȃ��B
 * 
 * �␳�l�̓t���b�V���ɋL�^����A�d����؂�������L���ł���B
 */
void SerialCommand::calibrateMpuGyro(){
	Mpu9250::getInstance()->startGyroCalibration();
}

/**
 * @brief MPU-9250�̉����x�̍Z�����s��
 * 
 * �␳�l�𕂓������_���œ��͂���B
 * 
 * �␳�l�̐���͎蓮�ōs���K�v������B
 * 
 * �␳�l�̓t���b�V���ɋL�^����A�d����؂�������L���ł���B
 */
void SerialCommand::setMpuAclBias(){
	for(int i=0;i<3;i++){
		float f;
		printf("%d th axis:",i);
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
 * @brief MPU-9250�̒n���C�Z���T�̍Z�����s��
 * 
 * �␳�l�𕂓������_���œ��͂���B
 * 
 * �␳�l�̐���͎蓮�ōs���K�v������B
 * 
 * �␳�l�̓t���b�V���ɋL�^����A�d����؂�������L���ł���B
 */
void SerialCommand::setMpuCmpsBias(){
	for(int i=0;i<3;i++){
		float f;
		printf("%d th axis:",i);
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
 * @brief ADIS16488�̒n���C�Z���T�̍Z�����s��
 * 
 * �␳�l�𕂓������_���œ��͂���B
 * 
 * �␳�l�̐���͎蓮�ōs���K�v������B
 * 
 * �␳�l�̓t���b�V���ɋL�^����A�d����؂�������L���ł���B
 */
void SerialCommand::setAdisCmpsBias(){
	for(int i=0;i<3;i++){
		float f;
		printf("%d th axis:",i);
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
 * @brief Gains�Ŏg�p����Z���T��\������
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
 * @brief Gains�Ŏg�p����Z���T��ݒ肷��B
 * 
 * �{�ݒ�͎���N��������L���ł���B
 * 
 * �ݒ�̓t���b�V���ɕۑ�����A�d����؂�������L���ł���B
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
	}else if(strncmp(arg,"debug",5)==0){
		Gains::GetInstance()->setPrintType(GainsPrintMode::DEBUG);
	}
}

void SerialCommand::initializeUserFlash(){
	Util::GetInstance()->flashData.imuType = ImuType::MPU9250;
	Util::GetInstance()->flashData.gpsType = GpsType::DUMMY_GPS;
	for(int i=0;i<3;i++){
		Util::GetInstance()->flashData.mpuAclBias[i]=0.0;
		Util::GetInstance()->flashData.mpuGyroBias[i]=0.0;
		Util::GetInstance()->flashData.mpuCmpsBias[i]=0.0;
	}
	Util::GetInstance()->userflashFlush();
}

/**
 * @brief �f�o�b�O�p�R�}���h0
 * 
 * �֐��̒��g�����������A�f�o�b�O�p�Ɏg�p�\
 */
void SerialCommand::testCmd0(){
	while(1){
		char c = USART3Class::GetInstance()->getChar();
		printf("getchar = %c\r\n",c);
		vTaskDelay(1);
	}
}
/**
 * @brief �f�o�b�O�p�R�}���h1
 * 
 * �֐��̒��g�����������A�f�o�b�O�p�Ɏg�p�\
 */
void SerialCommand::testCmd1(){
	
}
/**
 * @brief �f�o�b�O�p�R�}���h2
 * 
 * �֐��̒��g�����������A�f�o�b�O�p�Ɏg�p�\
 */
void SerialCommand::testCmd2(){
	
}
#ifdef __cplusplus
}
#endif
