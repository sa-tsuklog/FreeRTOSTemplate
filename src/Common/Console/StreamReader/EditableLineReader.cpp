/*
 * EditableLineReader.cpp
 *
 *  Created on: 2016/06/16
 *      Author: sa
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "../Ascii/Ascii.h"
#include "Interfaces/StreamReader.h"
#include "EditableLineReader.h"
#include "Lib/CommandLog.h"

EditableLineReader::EditableLineReader(StreamReader* reader,uint32_t lineLength,uint32_t logDepth,FILE* echoFp){
	this->reader = reader;
	this->lineLength = lineLength;
	commandLog = CommandLog(lineLength,logDepth);
	lineBuf = (uint8_t*)(malloc(sizeof(uint8_t) * lineLength));
	if(lineBuf == NULL){
		while(1);
	}
	this->echoFp = echoFp;
	echo=0;
}
EditableLineReader::~EditableLineReader(){
	
}

void EditableLineReader::printArrowLeft(){
	if(echoFp != NULL){
		fputc('\b',echoFp);
	}
}

void EditableLineReader::printArrowRight(){
	if(echoFp != NULL){
		fputc(Ascii::ESC,echoFp);
		fputc('9',echoFp);
	}
}
void EditableLineReader::printLineClear(){
	if(echoFp != NULL){
		fputc(Ascii::ESC,echoFp);
		fputc((uint8_t)((Ascii::LINE_CLEAR>>16)  & 0xFF),echoFp);
		fputc((uint8_t)((Ascii::LINE_CLEAR>>8)  & 0xFF),echoFp);
		fputc((uint8_t)((Ascii::LINE_CLEAR>>0)  & 0xFF),echoFp);
	}
}

void EditableLineReader::printMoveCursorTo(uint32_t cursorPos){
	if(echoFp != NULL){;
		fputc(Ascii::ESC,echoFp);
		fprintf(echoFp,"[%dG",cursorPos);
	}
}

uint32_t EditableLineReader::getChar(){
	return reader->getChar();
}
uint8_t* EditableLineReader::readLine(){
	int lineBufEnd = 0;
	int lineBufCursor = 0;
	lineBuf[0] = 0;

	uint32_t c;
	
	
	while(1){		
		c = getChar();
		if(' '<=c && c<='~'){
			if(lineBufEnd < lineLength){
				for(int i=lineBufEnd;i >= lineBufCursor;i--){
					lineBuf[i+1] = lineBuf[i];
				}
				
				lineBuf[lineBufCursor] = (uint8_t)c;
				lineBufEnd++;
				lineBufCursor++;
				
				
				if(echo && echoFp!=NULL){
					for(int i=lineBufCursor-1;i<lineBufEnd;i++){
						fputc(lineBuf[i],echoFp);
					}
					for(int i=lineBufCursor;i<lineBufEnd;i++){
						fputc('\b',echoFp);
					}
					fflush(stdout);
				}
			}
		}else if(c == '\r'){
			//do nothing				
		}else if(c == '\n'){
			if(lineBufEnd != 0){
				if(echo && echoFp!=NULL){
					for(int i=lineBufCursor;i<lineBufEnd;i++){
						printArrowRight();
					}
					
					fputc('\r',echoFp);
					fputc('\n',echoFp);
					fflush(stdout);
				}
				
				lineBuf[lineBufEnd] = 0;
				
				commandLog.appendCommandList(lineBuf);
				
				return lineBuf;
			}else{
				fputc('\r',echoFp);
				fputc('\n',echoFp);
				fflush(stdout);
			}
		}else if(c == '\b'){
			if(lineBufCursor>0){
				for(int i= lineBufCursor; i<=lineBufEnd;i++){
					if(i > 0){
						lineBuf[i-1] = lineBuf[i];
					}
				}
				lineBufEnd--;
				lineBufCursor--;
				
				if(echo && echoFp!=NULL){
					fputc('\b',echoFp);
					for(int i=lineBufCursor;i<lineBufEnd;i++){
						fputc(lineBuf[i],echoFp);
					}
					fputc(' ',echoFp);
					fputc('\b',echoFp);
					for(int i=lineBufCursor;i<lineBufEnd;i++){
						fputc('\b',echoFp);
					}
					fflush(stdout);
				}
			}
		}else if(c == Ascii::DEL){
			if(lineBufCursor<lineBufEnd){
				for(int i= lineBufCursor; i<lineBufEnd;i++){
					lineBuf[i] = lineBuf[i+1];
				}
				lineBufEnd--;
				
				if(echo && echoFp!=NULL){
					for(int i=lineBufCursor;i<lineBufEnd;i++){
						fputc(lineBuf[i],echoFp);
					}
					fputc(' ',echoFp);
					fputc('\b',echoFp);
					for(int i=lineBufCursor;i<lineBufEnd;i++){
						fputc('\b',echoFp);
					}
					fflush(stdout);
				}
			}
		}else if(c == Ascii::ARROW_LEFT){
			if(lineBufCursor > 0){
				lineBufCursor--;
				
				if(echo && echoFp!=NULL){
					printArrowLeft();
					fflush(stdout);
				}
			}
			
		}else if(c == Ascii::ARROW_RIGHT){
			if(lineBufCursor < lineBufEnd){
				lineBufCursor++;
				
				if(echo && echoFp!=NULL){
					printArrowRight();
					fflush(stdout);
				}
			}
			
		}else if(c == Ascii::ARROW_UP){
			uint8_t* loggedCommand = commandLog.getPreviousCommand(lineBuf);
			
			int cmdLength = strlen((const char*)loggedCommand); 
			for(int i=0;i<cmdLength;i++){
				lineBuf[i] = loggedCommand[i];
				
			}
			lineBuf[cmdLength] = 0;
			
			lineBufCursor = cmdLength;
			lineBufEnd = cmdLength;
			
			if(echo && echoFp!=NULL){
				printLineClear();
				printMoveCursorTo(0);
				for(int i=0;i<cmdLength;i++){
					fputc(lineBuf[i],echoFp);
				}
				fflush(stdout);
			}
		}else if(c == Ascii::ARROW_DOWN){
			uint8_t* loggedCommand = commandLog.getNextCommand(lineBuf);
			
			int cmdLength = strlen((const char*)loggedCommand); 
			for(int i=0;i<cmdLength;i++){
				lineBuf[i] = loggedCommand[i];
				
			}
			lineBuf[cmdLength] = 0;
			
			lineBufCursor = cmdLength;
			lineBufEnd = cmdLength;
			
			if(echo && echoFp!=NULL){
				printLineClear();
				printMoveCursorTo(0);
				for(int i=0;i<cmdLength;i++){
					fputc(lineBuf[i],echoFp);
				}
				fflush(stdout);
			}
		}else if(c == Ascii::HOME){
			lineBufCursor=0;
			
			if(echo && echoFp!=NULL){
				printMoveCursorTo(0);
				fflush(stdout);
			}		
		}else if(c == Ascii::END){
			lineBufCursor=lineBufEnd;
			
			if(echo && echoFp!=NULL){
				printMoveCursorTo(lineBufEnd+1);
				fflush(stdout);
			}			
		}else{
			//do nothing.
		}
	}
}
