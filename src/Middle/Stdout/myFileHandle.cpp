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

#include "Drivers/PeriphLib/USART1.h"
#include "Drivers/PeriphLib/USART2.h"
#include "Drivers/PeriphLib/USART3.h"
#include "myFileHandle.h"
#include "stm32f4xx.h"
#include "ff.h"

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
	printf("file:");
	switch(res){
		case FR_OK:printf("FR_OK");break;
		case FR_DISK_ERR:printf("FR_DISK_ERR");break;
		case FR_INT_ERR:printf("FR_INT_ERR");break;
		case FR_NOT_READY:printf("FR_NOT_READY");break;
		case FR_NO_FILE:printf("FR_NO_FILE");break;
		case FR_NO_PATH:printf("FR_NO_PATH");break;
		case FR_INVALID_NAME:printf("FR_INVALID_NAME");break;
		case FR_DENIED:printf("FR_DENIED");break;
		case FR_EXIST:printf("FR_EXIST");break;
		case FR_INVALID_OBJECT:printf("FR_INVALID_OBJECT");break;
		case FR_WRITE_PROTECTED:printf("FR_WRITE_PROTECTED");break;
		case FR_INVALID_DRIVE:printf("FR_INVALIDE_DRIVE");break;
		case FR_NOT_ENABLED:printf("FR_NOT_ENABLED");break;
		case FR_NO_FILESYSTEM:printf("FR_MKFS_ABORTED");break;
		case FR_TIMEOUT:printf("FR_TIMEOUT");break;
		case FR_LOCKED:printf("FR_LOCKED");break;
		case FR_NOT_ENOUGH_CORE:printf("FR_NOT_ENOUGH_CORE");break;
		case FR_TOO_MANY_OPEN_FILES:printf("FR_TOO_MANY_OPEN_FILES");break;
		case FR_INVALID_PARAMETER:printf("FR_INVALID_PARAMETER");break;
		default: printf("error code = %d",res);break;
	}
	printf("\n\r");
}
void printFatfsMode(BYTE mode){
	printf("mode:");
	if(mode & FA_READ){
		printf("FA_READ|");
	}
	if(mode & FA_WRITE){
		printf("FA_WRITE|");
	}
	if(mode & FA_OPEN_EXISTING){
		printf("FA_OPEN_EXISTING|");
	}
	if(mode & FA_OPEN_ALWAYS){
		printf("FA_OPEN_ALWAYS|");
	}
	if(mode & FA_CREATE_NEW){
		printf("FA_CREATE_NEW|");
	}
	if(mode & FA_CREATE_ALWAYS){
		printf("FA_CREATE_ALWAYS|");
	}
	printf("\n\r");
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
//	r = r;
//	file = file;
//	ptr = ptr;
//	dir = dir;

	return 0;
}

/***************************************************************************/

int myWrite (struct _reent *r, int file, char * ptr, int len)
{
	unsigned int ret=0;
	if(file == 1){//stdout
		ret = len;
		if(USART3Class::GetInstance()->GetQueue() != NULL){
			for(int i=0;i<len;i++){
				if(xQueueSendToBackFromISR(USART3Class::GetInstance()->GetQueue(),&ptr[i],(BaseType_t *)pdFALSE)!=pdPASS){
					ret--;
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
	}else if(FILEHANDLE_OFFSET <= file && file < FILEHANDLE_OFFSET+MAX_FILEHANDLE_NUM){
		int id = file - FILEHANDLE_OFFSET;
		if(fileHandleInUse[id]==0){
			return -1;
		}
		UINT bw;
		FIL* fp = &fileHandles[id];
		FRESULT res = f_write(fp,ptr,len,&bw);
		
		//printf("write %d:",id);
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
	}
	
	//lazy initialization. mount drive.
	if(fatfs == NULL){
		fatfs = malloc(sizeof(FATFS));
		
		res = f_mount(fatfs,"sd",1);
//		printf("mount:");
//		printfFresult(res);
		
		if(res != FR_OK){
			free(fatfs);
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
		printf("cannot open\n\r");
		return -1;
	}
	
	//open file.
	BYTE fatMode = newlibModeToFatFsMode(mode);
	res = f_open(&fileHandles[handleId],path,fatMode);
	
	//printf("open:%s",path);
	//printfFresult(res);
	//printFatfsMode(fatMode);
	//printf("id = %d\n\r",handleId);
	
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
		//printf("close: invalid file %d\n\r",file);
		return -1;
	}
	
	int id = file - FILEHANDLE_OFFSET;
	if(fileHandleInUse[id] != 1){
		//printf("id %d is not open\n\r",id);
		return -1;
	}
	
	FIL* fp = &fileHandles[id];
	FRESULT res = f_close(fp);
	
	fileHandleInUse[id] = 0;
	
	if(res == FR_OK){
		//printf("id %d closed\n\r",id);
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
