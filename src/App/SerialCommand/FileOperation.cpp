/*
 * FileOperation.cpp
 *
 *  Created on: 2016/08/22
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"

#include "FileOperation.h"
#include "ff.h"
#include "stdio.h"
#include "stdint.h"

FileOperation::FileOperation(){
	
}

void FileOperation::ls(){
	FILE* out = fopen("/blockingStdout","w");
	FILE* dummy = fopen("/dummyFileOpen","r");
	if(dummy != NULL){
		fclose(dummy);
	}
	
	
	FRESULT res;
	DIR dir;
	static FILINFO fno;
	fno.lfname = lfn;
	fno.lfsize = MAX_LFN_LEN;
	
	res = f_opendir(&dir,"");
	if(res == FR_OK){
		for(;;){
			res = f_readdir(&dir,&fno);
			if(res!=FR_OK||fno.fname[0] == 0){
				break;
			}
			if(fno.fattrib & AM_DIR){
				
			}else{
				if(fno.lfname[0] != 0){
					fprintf(out,"%s\r\n",fno.lfname);
				}else{
					fprintf(out,"%s\r\n",fno.fname);
				}
			}
		}
	}
	f_closedir(&dir);
	fclose(out);
}

void FileOperation::cat(char* filename){
	FILE* fp = fopen(filename,"r");
	FILE* out = fopen("/blockingStdout","w");
	if(fp == NULL){
		printf("cat: %s: No such file or directory\r\n",filename);
		return;
	}
	
	int c;
	while((c = fgetc(fp)) != EOF){
		fputc(c,out);
	}
	fclose(fp);
	fclose(out);
}

