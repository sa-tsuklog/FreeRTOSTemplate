/*
 * CommandList.h
 *
 *  Created on: 2014/05/29
 *      Author: sa
 */

///////////////////////////////////////////////////////////////
//                                                           //
// DO NOT INCLUDE THIS FILE EXCEPT IN "SerialCommand.c"      //
//                                                           //
///////////////////////////////////////////////////////////////


#ifndef COMMANDLIST_H_
#define COMMANDLIST_H_

command_t commandList[32] = {
		{"setservo",&SerialCommand::setServo},
		{"help",&SerialCommand::printHelp},
		{"printTaskList",&SerialCommand::printTaskList},
		{"startLogging",&SerialCommand::startLogging},
		{"stopLogging",&SerialCommand::stopLogging},
		{"resetImu",&SerialCommand::resetImu},
		{"resetGpsRef",&SerialCommand::resetGpsRef},
		{"echoOn",&SerialCommand::echoOn},
		{"echoOff",&SerialCommand::echoOff},
		{"setServosTrim",&SerialCommand::setServosTrim},
		{"setPidGain",&SerialCommand::setPidGain},
		{"calibrateMpuGyro",&SerialCommand::calibrateMpuGyro},
		{"setMpuAclBias",&SerialCommand::setMpuAclBias},
		{"setMpuCmpsBias",&SerialCommand::setMpuCmpsBias},
		{"setAdisCmpsBias",&SerialCommand::setAdisCmpsBias},
		{"showGainsConfig",&SerialCommand::showGainsConfig},
		{"setGainsConfig",&SerialCommand::setGainsConfig},
		{"setGainsPrintMode",&SerialCommand::setGainsPrintMode},
		{"initializeUserFlash",&SerialCommand::initializeUserFlash},
		{"testCmd0",&SerialCommand::testCmd0},
		{"testCmd1",&SerialCommand::testCmd1},
		{"testCmd2",&SerialCommand::testCmd2},
};


#endif /* COMMANDLIST_H_ */
