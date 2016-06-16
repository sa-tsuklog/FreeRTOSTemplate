/*
 * StdoutConfig.h
 *
 *  Created on: 2016/04/05
 *      Author: sa
 */

#ifndef STDOUTCONFIG_H_
#define STDOUTCONFIG_H_

class StdoutConfig{
public:
	static const int32_t TX_BUFFERSIZE = 768;
	static const int32_t RX_BUFFERSIZE = 768;
	static const int32_t LINE_BUF_SIZE = 768;
	static const int32_t COMMAND_LOG_DEPTH = 8;
};

#endif /* STDOUTCONFIG_H_ */
