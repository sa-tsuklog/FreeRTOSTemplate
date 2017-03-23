/*
 * EditableLineReader.cpp
 *
 *  Created on: 2016/06/16
 *      Author: sa
 */

#include "FreeRTOS.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "../Ascii/Ascii.h"
#include "Interfaces/StreamReader.h"
#include "EditableLineReader.h"
#include "Lib/CommandLog.h"
#include "App/SerialCommand/SerialCommand.h"

EditableLineReader::EditableLineReader(StreamReader* reader,uint32_t lineLength,uint32_t logDepth,FILE* echoFp){
	this->reader = reader;
	this->lineLength = lineLength;
	commandLog = CommandLog(lineLength,logDepth);
	lineBuf = (uint8_t*)(malloc(sizeof(uint8_t) * lineLength));
	if(lineBuf == NULL){
		while(1);
	}
	this->echoFp = echoFp;
	echo=1;
}
EditableLineReader::~EditableLineReader(){
	free(lineBuf);
}

int EditableLineReader::appendCharToLine(char c,int* ioLineBufCursor,int* ioLineBufEnd){
	if(*ioLineBufEnd >= lineLength){
		return 0;
	}
	
	for(int i=*ioLineBufEnd;i >= *ioLineBufCursor;i--){
		lineBuf[i+1] = lineBuf[i];
	}
	
	lineBuf[*ioLineBufCursor] = (uint8_t)c;
	(*ioLineBufEnd)++;
	(*ioLineBufCursor)++;
	
	
	if(echo && echoFp!=NULL){
		for(int i=*ioLineBufCursor-1;i<*ioLineBufEnd;i++){
			fputc(lineBuf[i],echoFp);
		}
		for(int i=*ioLineBufCursor;i<*ioLineBufEnd;i++){
			fputc('\b',echoFp);
		}
		fflush(echoFp);
	}
	
	return 1;
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

int EditableLineReader::handleTab(int* ioLineBufCursor,int* ioLineBufEnd){
	if(*ioLineBufCursor==0){
		return 0;
	}
	
	int i=0;
	int matchCount = 0;
	int lastMatchIndex = 0;
	
	command_t* commandList = SerialCommand::GetInstance()->getCommandList();
	while(commandList[i].command != NULL){
		if(strncmp((const char*)lineBuf,commandList[i].command,*ioLineBufCursor) == 0){
			matchCount++;
			lastMatchIndex = i;
		}
		i++;
	}
	
	if(matchCount == 0){
		return 0;
	}else if(matchCount == 1){
		
		int appendedCharCount = 0; 

		for(i = *ioLineBufCursor;i<strlen(commandList[lastMatchIndex].command);i++){
			appendedCharCount += appendCharToLine(commandList[lastMatchIndex].command[i],ioLineBufCursor,ioLineBufEnd);
		}
		
		return appendedCharCount;
		
		
	}else{
		int matchCount2;
		int tmpCursor=*ioLineBufCursor;
		
		while(true){
			i=0;
			matchCount2 = 0;
			while(commandList[i].command != NULL){
				if(strncmp(commandList[lastMatchIndex].command,commandList[i].command,tmpCursor) == 0){
					matchCount2++;
				}
				i++;
			}
			if(matchCount != matchCount2){
				break;
			}
			tmpCursor++;
		}
		
		int appendedCharCount = 0;
		
		for(i = *ioLineBufCursor;i<tmpCursor-1;i++){
			appendedCharCount += appendCharToLine(commandList[lastMatchIndex].command[i],ioLineBufCursor,ioLineBufEnd);
		}
		
		return appendedCharCount;
	}
}

void EditableLineReader::handleDoubleTab(int lineBufCursor,int lineBufEnd){
	if(echo && echoFp != NULL){
		command_t* commandList = SerialCommand::GetInstance()->getCommandList();
		
		int i=0;
		int matchCount = 0;
		while(commandList[i].command != NULL){
			if(strncmp((const char*)lineBuf,commandList[i].command,lineBufCursor) == 0){
				fprintf(echoFp,"\r\n%s",commandList[i].command);
				matchCount++;
			}
			i++;
		}
		
		if(matchCount==0){
			return;
		}
		
		fprintf(echoFp,"\r\n");
		fprintf(echoFp,"\r\n");
		fprintf(echoFp,">>");
		for(i=0;i<lineBufEnd;i++){
			fputc(lineBuf[i],echoFp);
		}
		printMoveCursorTo(lineBufCursor+3);
		
		fflush(echoFp);
	}
	
	
}

uint32_t EditableLineReader::getChar(uint32_t msBlockTime){
	return reader->getChar(msBlockTime);
}

uint32_t EditableLineReader::getChar(){
	return getChar(portMAX_DELAY);
}

void appdendCharToLine(char c,int* ioLineBufCursor,int* ioLineBufEnd){
	
}

uint8_t* EditableLineReader::readLine(){
	int lineBufEnd = 0;
	int lineBufCursor = 0;
	lineBuf[0] = 0;

	uint32_t c;
	
	int tabCount = 0;
	
	if(echo && echoFp!=NULL){
		fprintf(echoFp,">>");
		fflush(echoFp);
	}
	
	while(1){		
		c = getChar();
		
		if(c != '\t'){
			tabCount = 0;
		}
		
		
		if(' '<=c && c<='~'){
			appendCharToLine(c,&lineBufCursor,&lineBufEnd);
		}else if(c == '\r'){
			//do nothing				
		}else if(c == '\n'){
			lineBuf[lineBufEnd] = 0;
			if(lineBufEnd != 0){
				if(echo && echoFp!=NULL){
					for(int i=lineBufCursor;i<lineBufEnd;i++){
						printArrowRight();
					}
					
					fputc('\r',echoFp);
					fputc('\n',echoFp);
					fflush(echoFp);
				}
				
				commandLog.appendCommandList(lineBuf);
			}else{
				fputc('\r',echoFp);
				fputc('\n',echoFp);
				fflush(echoFp);
			}
			
			return lineBuf;
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
					fflush(echoFp);
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
					fflush(echoFp);
				}
			}
		}else if(c == Ascii::ARROW_LEFT){
			if(lineBufCursor > 0){
				lineBufCursor--;
				
				if(echo && echoFp!=NULL){
					printArrowLeft();
					fflush(echoFp);
				}
			}
			
		}else if(c == Ascii::ARROW_RIGHT){
			if(lineBufCursor < lineBufEnd){
				lineBufCursor++;
				
				if(echo && echoFp!=NULL){
					printArrowRight();
					fflush(echoFp);
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
				fprintf(echoFp,">>");
				for(int i=0;i<cmdLength;i++){
					fputc(lineBuf[i],echoFp);
				}
				fflush(echoFp);
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
				fprintf(echoFp,">>");
				for(int i=0;i<cmdLength;i++){
					fputc(lineBuf[i],echoFp);
				}
				fflush(echoFp);
			}
		}else if(c == Ascii::HOME){
//			lineBufCursor=0;
//			
//			if(echo && echoFp!=NULL){
//				printMoveCursorTo(0);
//				fflush(echoFp);
//			}
			if(echo && echoFp!=NULL){
				while(lineBufCursor != 0){
					lineBufCursor--;
					printArrowLeft();
					fflush(echoFp);
				}
			}
		}else if(c == Ascii::END){
//			lineBufCursor=lineBufEnd;
//			
//			if(echo && echoFp!=NULL){
//				printMoveCursorTo(lineBufEnd+1);
//				fflush(echoFp);
//			}
			if(echo && echoFp != NULL){
				while(lineBufCursor != lineBufEnd){
					lineBufCursor++;
					printArrowRight();
					fflush(echoFp);
				}
			}
		}else if(c == '\t'){
			int charAdded = handleTab(&lineBufCursor,&lineBufEnd);
			
			if(charAdded != 0){
				tabCount = 0;
			}else{
				tabCount++;
			}
			
			if(tabCount >= 2){
				handleDoubleTab(lineBufCursor,lineBufEnd);
			}
			
		}else{
			//do nothing.
		}
	}
}
