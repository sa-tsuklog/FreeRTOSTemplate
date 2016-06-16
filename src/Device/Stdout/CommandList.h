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
		{"help",&SerialCommand::printHelp},
		{"startLogging",&SerialCommand::startLogging},
		{"stopLogging",&SerialCommand::stopLogging},
		{"echoOn",&SerialCommand::echoOn},
		{"echoOff",&SerialCommand::echoOff},
		{"initializeUserFlash",&SerialCommand::initializeUserFlash},
		{"printTaskList",&SerialCommand::printTaskList},
		{"runTimeStats",&SerialCommand::runTimeStats},
		{"startTrace",&SerialCommand::startTrace},
		{"testCmd0",&SerialCommand::testCmd0},
		{"testCmd1",&SerialCommand::testCmd1},
		{"testCmd2",&SerialCommand::testCmd2},
};


#endif /* COMMANDLIST_H_ */
