/*
 * myFileHandle.c
 *
 *  Created on: 2014/05/28
 *      Author: sa
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "MyFileHandle.h"
#include "stm32f4xx.h"
#include "ff.h"

#include "MyLib/Stdout/Stdout.h"
#include "MyLib/Gains/Driver/Gps/USART2.h"
#include "MyLib/CmdServo/Driver/USART1.h"
#include "MyLib/Logger/Logger.h"

#include "MyLib/Util/Util.h"

#ifdef __cplusplus
 extern "C" {
#endif

//file handle
//1-2 :stdin,stdout,stderr
//3-9 :mapped to usart.
//10- :mapped to sdcard.


int fileHandleInUse[MAX_FILEHANDLE_NUM];
FIL fileHandles[MAX_FILEHANDLE_NUM];

FATFS* fatfs=NULL;
void printfFresult(FRESULT res){
	Util::GetInstance()->myFprintf(0,stdout,"file:");
	switch(res){
		case FR_OK:Util::GetInstance()->myFprintf(0,stdout,"FR_OK");break;
		case FR_DISK_ERR:Util::GetInstance()->myFprintf(0,stdout,"FR_DISK_ERR");break;
		case FR_INT_ERR:Util::GetInstance()->myFprintf(0,stdout,"FR_INT_ERR");break;
		case FR_NOT_READY:Util::GetInstance()->myFprintf(0,stdout,"FR_NOT_READY");break;
		case FR_NO_FILE:Util::GetInstance()->myFprintf(0,stdout,"FR_NO_FILE");break;
		case FR_NO_PATH:Util::GetInstance()->myFprintf(0,stdout,"FR_NO_PATH");break;
		case FR_INVALID_NAME:Util::GetInstance()->myFprintf(0,stdout,"FR_INVALID_NAME");break;
		case FR_DENIED:Util::GetInstance()->myFprintf(0,stdout,"FR_DENIED");break;
		case FR_EXIST:Util::GetInstance()->myFprintf(0,stdout,"FR_EXIST");break;
		case FR_INVALID_OBJECT:Util::GetInstance()->myFprintf(0,stdout,"FR_INVALID_OBJECT");break;
		case FR_WRITE_PROTECTED:Util::GetInstance()->myFprintf(0,stdout,"FR_WRITE_PROTECTED");break;
		case FR_INVALID_DRIVE:Util::GetInstance()->myFprintf(0,stdout,"FR_INVALIDE_DRIVE");break;
		case FR_NOT_ENABLED:Util::GetInstance()->myFprintf(0,stdout,"FR_NOT_ENABLED");break;
		case FR_NO_FILESYSTEM:Util::GetInstance()->myFprintf(0,stdout,"FR_MKFS_ABORTED");break;
		case FR_TIMEOUT:Util::GetInstance()->myFprintf(0,stdout,"FR_TIMEOUT");break;
		case FR_LOCKED:Util::GetInstance()->myFprintf(0,stdout,"FR_LOCKED");break;
		case FR_NOT_ENOUGH_CORE:Util::GetInstance()->myFprintf(0,stdout,"FR_NOT_ENOUGH_CORE");break;
		case FR_TOO_MANY_OPEN_FILES:Util::GetInstance()->myFprintf(0,stdout,"FR_TOO_MANY_OPEN_FILES");break;
		case FR_INVALID_PARAMETER:Util::GetInstance()->myFprintf(0,stdout,"FR_INVALID_PARAMETER");break;
		default: Util::GetInstance()->myFprintf(0,stdout,"error code = %d",res);break;
	}
	Util::GetInstance()->myFprintf(0,stdout,"\r\n");
}
void printFatfsMode(BYTE mode){
	Util::GetInstance()->myFprintf(0,stdout,"mode:");
	if(mode & FA_READ){
		Util::GetInstance()->myFprintf(0,stdout,"FA_READ|");
	}
	if(mode & FA_WRITE){
		Util::GetInstance()->myFprintf(0,stdout,"FA_WRITE|");
	}
	if(mode & FA_OPEN_EXISTING){
		Util::GetInstance()->myFprintf(0,stdout,"FA_OPEN_EXISTING|");
	}
	if(mode & FA_OPEN_ALWAYS){
		Util::GetInstance()->myFprintf(0,stdout,"FA_OPEN_ALWAYS|");
	}
	if(mode & FA_CREATE_NEW){
		Util::GetInstance()->myFprintf(0,stdout,"FA_CREATE_NEW|");
	}
	if(mode & FA_CREATE_ALWAYS){
		Util::GetInstance()->myFprintf(0,stdout,"FA_CREATE_ALWAYS|");
	}
	Util::GetInstance()->myFprintf(0,stdout,"\r\n");
}


DWORD get_fattime(){
	return ((34)<<25)|((4)<<21)|((1)<<16)|((0)<<11)|((0)<<5)|((0)<<0);
}

BYTE newlibModeToFatFsMode(int mode){
	BYTE fatMode=0;
	
	if((mode+1) & FREAD){
		fatMode |= FA_READ;
	} 
	if((mode+1) & FWRITE){
		fatMode |= FA_WRITE;
	}
	
	if(!(mode & O_CREAT)){
		fatMode |= FA_OPEN_EXISTING;
	}else if((mode & O_CREAT) && (mode & O_APPEND)){
		fatMode |= FA_OPEN_ALWAYS;
	}else if((mode & O_CREAT) && (mode & O_TRUNC)){
		fatMode |= FA_CREATE_ALWAYS;
	}else if((mode & O_CREAT) && !(mode & O_APPEND) && !(mode & O_TRUNC)){
		fatMode |= FA_CREATE_NEW;
	} 
	
	return fatMode;
}

/***************************************************************************/

int myRead (struct _reent *r, int file, char * ptr, int len)
{
	UINT br;
	FRESULT res = f_read(&fileHandles[file-FILEHANDLE_OFFSET],ptr,len,&br);
	
	if(res != FR_OK){
		return -1;
	}
	
	return br;
}

/***************************************************************************/

int myLseek (struct _reent *r, int file, int ptr, int dir)
{
	int offset;
	if(dir == 0){
		offset = ptr;
	}else if(dir == 1){
		offset = f_tell(&fileHandles[file-FILEHANDLE_OFFSET]);
	}else if(dir == 2){
		offset = f_size(&fileHandles[file-FILEHANDLE_OFFSET]);
	}else{
		return -1;
	}
	
	if(f_lseek(&fileHandles[file-FILEHANDLE_OFFSET],offset) == FR_OK){
		return offset;
	}else{
		return -1;
	}
}

/***************************************************************************/

int myWrite (struct _reent *r, int file, char * ptr, int len)
{
	unsigned int ret=0;
	if(file == 1){//stdout
		ret = len;
		if(Stdout::GetInstance()->getTxQueue() != NULL){
			for(int i=0;i<len;i++){
				if(xQueueSendToBackFromISR(Stdout::GetInstance()->getTxQueue(),&ptr[i],(BaseType_t *)pdFALSE)!=pdPASS){
					ret--;
				}else{
					
				}
			}
		}
	}else if(file == 2){//stderr
	}else if(file == 3){//cmd servo out
		ret = len;
		if(USART1Class::GetInstance()->GetQueue() != NULL){
			for(int i=0;i<len;i++){
				if(xQueueSendToBackFromISR(USART1Class::GetInstance()->GetQueue(),&ptr[i],(BaseType_t *)pdFALSE)!=pdPASS){
					ret--;
				}
			}
		}
	}else if(file == 4){//gps out
		ret = len;
		if(USART2Class::GetInstance()->GetQueue() != NULL){
			for(int i=0;i<len;i++){
				if(xQueueSendToBackFromISR(USART2Class::GetInstance()->GetQueue(),&ptr[i],(BaseType_t *)pdFALSE)!=pdPASS){
					ret--;
				}
			}
		}
	}else if(file == 5 || file == 6 || file ==7 || file == 8){//LOGGER_CH0-3
		Logger::GetInstance()->appendLog(file-5,ptr,len);
	
	}else if(FILEHANDLE_OFFSET <= file && file < FILEHANDLE_OFFSET+MAX_FILEHANDLE_NUM){
		int id = file - FILEHANDLE_OFFSET;
		if(fileHandleInUse[id]==0){
			return -1;
		}
		UINT bw;
		FIL* fp = &fileHandles[id];
		FRESULT res = f_write(fp,ptr,len,&bw);
		
		//Util::GetInstance()->myFprintf(0,stdout,"write %d:",id);
		//printfFresult(res);
		
		if(res != FR_OK){
			return -1;
		}
		ret = bw;
	}
	
	return ret;
}

int mySync(struct _reent *r,int file){
	f_sync(&fileHandles[file-FILEHANDLE_OFFSET]);
	return 0;
}

int myOpen(struct _reent *r,const char *path,int mode){
	FRESULT res;
	
	if(strncmp(path,CMD_SERVO_OUT,strlen(CMD_SERVO_OUT))==0){
		return 3;
	}else if(strncmp(path,GPS_OUT,strlen(GPS_OUT))==0){
		return 4;
	}else if(strncmp(path,LOGGER_CH0,strlen(LOGGER_CH0))==0){
		return 5;
	}else if(strncmp(path,LOGGER_CH1,strlen(LOGGER_CH1))==0){
		return 6;
	}else if(strncmp(path,LOGGER_CH2,strlen(LOGGER_CH2))==0){
		return 7;
	}else if(strncmp(path,LOGGER_CH3,strlen(LOGGER_CH3))==0){
		return 8;
	}
	
	
	
	//lazy initialization. mount drive.
	if(fatfs == NULL){
		fatfs = malloc(sizeof(FATFS));
		
		res = f_mount(fatfs,"sd",1);
//		Util::GetInstance()->myFprintf(0,stdout,"mount:");
//		printfFresult(res);
		
		if(res != FR_OK){
			free(fatfs);
			Util::GetInstance()->myFprintf(0,stdout,"not mounted\r\n");
			return -1;
		}
	}
	
	//
	int handleId;
	
	//search file handle not in use.
	for(handleId = 0;handleId < MAX_FILEHANDLE_NUM;handleId++){
		if(fileHandleInUse[handleId] == 0){
			break;
		}
	}
	if(handleId == MAX_FILEHANDLE_NUM){
		Util::GetInstance()->myFprintf(0,stdout,"cannot open\r\n");
		return -1;
	}
	
	//open file.
	BYTE fatMode = newlibModeToFatFsMode(mode);
	res = f_open(&fileHandles[handleId],path,fatMode);
	
	//Util::GetInstance()->myFprintf(0,stdout,"open:%s",path);
	//printfFresult(res);
	//printFatfsMode(fatMode);
	//Util::GetInstance()->myFprintf(0,stdout,"id = %d\r\n",handleId);
	
	if(res == FR_OK){
		if(mode & O_APPEND){
			res = f_lseek(&fileHandles[handleId],f_size(&fileHandles[handleId]));
			if(res != FR_OK){
				return -1;
			}
		}
		
		fileHandleInUse[handleId] = 1;
		return handleId+FILEHANDLE_OFFSET;
	}else{
		fileHandleInUse[handleId] = 0;
		return -1;
	}
}

/***************************************************************************/

int myClose (struct _reent *r, int file)
{
	if(file < FILEHANDLE_OFFSET || FILEHANDLE_OFFSET+MAX_FILEHANDLE_NUM <= file){
		//Util::GetInstance()->myFprintf(0,stdout,"close: invalid file %d\r\n",file);
		return -1;
	}
	
	int id = file - FILEHANDLE_OFFSET;
	if(fileHandleInUse[id] != 1){
		//Util::GetInstance()->myFprintf(0,stdout,"id %d is not open\r\n",id);
		return -1;
	}
	
	FIL* fp = &fileHandles[id];
	FRESULT res = f_close(fp);
	
	fileHandleInUse[id] = 0;
	
	if(res == FR_OK){
		//Util::GetInstance()->myFprintf(0,stdout,"id %d closed\r\n",id);
		return 0;
	}else{
		return -1;
	}
}

int myFsync(FILE* fp){
	return mySync(NULL,fp->_file);
}

#ifdef __cplusplus
}
#endif
