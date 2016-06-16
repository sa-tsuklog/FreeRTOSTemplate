/*
 * Ascii.h
 *
 *  Created on: 2016/04/04
 *      Author: sa
 */

#ifndef ASCII_H_
#define ASCII_H_

#include "stdint.h"

class Ascii{
public:
	static const uint8_t NUL = 0x0;
	static const uint8_t SOH = 0x1;
	static const uint8_t STX = 0x2;
	static const uint8_t ETX = 0x3;
	static const uint8_t EOT = 0x4;
	static const uint8_t ENQ = 0x5;
	static const uint8_t ACK = 0x6;
	static const uint8_t BEL = 0x7;
	static const uint8_t BS = 0x8;
	static const uint8_t HT = 0x9;
	static const uint8_t LF = 0x0a;
	static const uint8_t VT = 0x0b;
	static const uint8_t FF = 0x0c;
	static const uint8_t CR = 0x0d;
	static const uint8_t SO = 0x0e;
	static const uint8_t SI = 0x0f;
	static const uint8_t DLE = 0x10;
	static const uint8_t DC1 = 0x11;
	static const uint8_t DC2 = 0x12;
	static const uint8_t DC3 = 0x13;
	static const uint8_t DC4 = 0x14;
	static const uint8_t NAK = 0x15;
	static const uint8_t SYN = 0x16;
	static const uint8_t ETB = 0x17;
	static const uint8_t CAN_ = 0x18;
	static const uint8_t EM = 0x19;
	static const uint8_t SUB = 0x1a;
	static const uint8_t ESC = 0x1b;
	static const uint8_t FS = 0x1c;
	static const uint8_t GS = 0x1d;
	static const uint8_t RS = 0x1e;
	static const uint8_t US = 0x1f;
	static const uint8_t DEL = 0x7f;

	static const uint32_t ARROW_UP    = 0x5b41;
	static const uint32_t ARROW_DOWN  = 0x5b42;
	static const uint32_t ARROW_RIGHT = 0x5b43;
	static const uint32_t ARROW_LEFT  = 0x5b44;
	
	static const uint32_t INS   = 0x5b327e;
	static const uint32_t HOME  = 0x5b317e;
	static const uint32_t END   = 0x5b347e;
	static const uint32_t PAGEUP= 0x5b357e;
	static const uint32_t PAGEDN= 0x5b367e;
	
	static const uint32_t F1 = 0x5b31317e;
	static const uint32_t F2 = 0x5b31327e;
	static const uint32_t F3 = 0x5b31337e;
	static const uint32_t F4 = 0x5b31347e;
	static const uint32_t F5 = 0x5b31357e;
	static const uint32_t F6 = 0x5b31377e;
	static const uint32_t F7 = 0x5b31387e;
	static const uint32_t F8 = 0x5b31397e;
	static const uint32_t F9 = 0x5b32307e;
	static const uint32_t F10 = 0x5b32317e;
	static const uint32_t F11 = 0x5b32337e;
	static const uint32_t F12 = 0x5b32347e;
	
	static const uint32_t LINE_CLEAR = 0x5b324b;
};

#endif /* ASCII_H_ */
