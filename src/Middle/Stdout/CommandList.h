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
		{"help",&SerialCommand::printHelp},
		{"printTaskList",&SerialCommand::printTaskList},
		{"startLogging",&SerialCommand::startLogging},
		{"stopLogging",&SerialCommand::stopLogging},
		{"resetImu",&SerialCommand::resetImu},
		{"resetGpsRef",&SerialCommand::resetGpsRef},
		{"echoOn",&SerialCommand::echoOn},
		{"echoOff",&SerialCommand::echoOff}
};


#endif /* COMMANDLIST_H_ */
