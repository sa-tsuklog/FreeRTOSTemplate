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

#define MAX_COMMAND_NUM 64

command_t commandList[MAX_COMMAND_NUM] = {
		{"$USCTL",&SerialCommand::usctl},
		{"help",&SerialCommand::printHelp},
		{"startLogging",&SerialCommand::startLogging},
		{"stopLogging",&SerialCommand::stopLogging},
		{"resetImu",&SerialCommand::resetImu},
		{"resetGpsRef",&SerialCommand::resetGpsRef},
		{"setWaypoint",&SerialCommand::setWaypoint},
		{"clearWaypoints",&SerialCommand::clearWaypoints},
		{"confirmWaypoints",&SerialCommand::confirmWaypoints},
		{"echoOn",&SerialCommand::echoOn},
		{"echoOff",&SerialCommand::echoOff},
		{"setServosTrim",&SerialCommand::setServosTrim},
		{"setPidGain",&SerialCommand::setPidGain},
		{"setFlightParameters",&SerialCommand::setFlightParameters},
		{"showFlightParameters",&SerialCommand::showFlightParameters},
		{"calibrateMpuGyro",&SerialCommand::calibrateMpuGyro},
		{"setMpuAclBias",&SerialCommand::setMpuAclBias},
		{"setMpuCmpsBias",&SerialCommand::setMpuCmpsBias},
		{"calibrateMpuCmpsBias",&SerialCommand::calibrateMpuCmpsBias},
		{"setMpuTempCoeff",&SerialCommand::setMpuTempCoeff},
		{"setAdisCmpsBias",&SerialCommand::setAdisCmpsBias},
		{"showGainsConfig",&SerialCommand::showGainsConfig},
		{"setGainsConfig",&SerialCommand::setGainsConfig},
		{"setPrintMode",&SerialCommand::setPrintMode},
		{"initializeUserFlash",&SerialCommand::initializeUserFlash},
		{"showVoltage",&SerialCommand::showInputVoltage},
		{"setCmdServoCh",&SerialCommand::setCmdServoCh},
		{"printTaskList",&SerialCommand::printTaskList},
		{"runTimeStats",&SerialCommand::runTimeStats},
		{"startTrace",&SerialCommand::startTrace},
		{"testCmd0",&SerialCommand::testCmd0},
		{"testCmd1",&SerialCommand::testCmd1},
		{"testCmd2",&SerialCommand::testCmd2},
};


#endif /* COMMANDLIST_H_ */
