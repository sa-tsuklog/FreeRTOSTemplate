/*
 * EscapeSequenceParser.cpp
 *
 *  Created on: 2016/06/15
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"

#include "../../Ascii/Ascii.h"
#include "EscapeSequenceReader.h"

EscapeSequenceReader::EscapeSequenceReader(StreamReader* parentReader){
	this->parentReader=parentReader;
}

uint32_t EscapeSequenceReader::getChar(){
	getChar(portMAX_DELAY);	
}

uint32_t EscapeSequenceReader::getChar(uint32_t msBlockTime){
	uint32_t c1 = parentReader->getChar(msBlockTime);
	uint8_t c2,c3,c4,c5;
	
	
	if(c1 != Ascii::ESC){
		return c1;
	}else{
		uint32_t c2 = parentReader->getChar(msBlockTime);
		if(c2 == EOF){
			return EOF;
		}
		uint32_t c3 = parentReader->getChar(msBlockTime);
		if(c3 == EOF){
			return EOF;
		}
		
		if((c2<<8 | c3) == Ascii::ARROW_UP){
			return Ascii::ARROW_UP;
		}else if((c2<<8 | c3) == Ascii::ARROW_DOWN){
			return Ascii::ARROW_DOWN;
		}else if((c2<<8 | c3) == Ascii::ARROW_LEFT){
			return Ascii::ARROW_LEFT;
		}else if((c2<<8 | c3) == Ascii::ARROW_RIGHT){
			return Ascii::ARROW_RIGHT;
		}else{
			uint32_t c4 = parentReader->getChar(msBlockTime);
			if(c4 == EOF){
				return EOF;
			}
			
			if((c2<<16 | c3 <<8 | c4) == Ascii::INS){
				return Ascii::INS;
			}else if((c2<<16 | c3 <<8 | c4) == Ascii::HOME){
				return Ascii::HOME;
			}else if((c2<<16 | c3 <<8 | c4) == Ascii::END){
				return Ascii::END;
			}else if((c2<<16 | c3 <<8 | c4) == Ascii::PAGEUP){
				return Ascii::PAGEUP;
			}else if((c2<<16 | c3 <<8 | c4) == Ascii::PAGEDN){
				return Ascii::PAGEDN;
			}else{
				uint32_t c5 = parentReader->getChar();
				if(c5 == EOF){
					return EOF;
				}
				
				if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F1){
					return Ascii::F1;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F2){
					return Ascii::F2;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F3){
					return Ascii::F3;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F4){
					return Ascii::F4;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F5){
					return Ascii::F5;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F6){
					return Ascii::F6;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F7){
					return Ascii::F7;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F8){
					return Ascii::F8;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F9){
					return Ascii::F9;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F10){
					return Ascii::F10;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F11){
					return Ascii::F11;
				}else if((c2<< 24 | c3<<16 | c4 <<8 | c5) == Ascii::F12){
					return Ascii::F12;
				}else{
					return '$';
				}
			}
		}
	}
}

