/*
 * myFileHandle.h
 *
 *  Created on: 2014/05/28
 *      Author: sa
 */

#ifndef MYFILEHANDLE_H_
#define MYFILEHANDLE_H_

#include "stdio.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define MAX_FILEHANDLE_NUM 4
#define FILEHANDLE_OFFSET 10
#define CMD_SERVO_OUT "/cmdServoOut"
#define GPS_OUT "/gpsOut"
#define LOGGER_CH0 "/log0"
#define LOGGER_CH1 "/log1"
#define LOGGER_CH2 "/log2"
#define LOGGER_CH3 "/log3"

int myRead (struct _reent *r, int file, char * ptr, int len);
int myLseek (struct _reent *r, int file, int ptr, int dir);
int myWrite (struct _reent *r, int file, char * ptr, int len);
int mySync(struct _reent *r,int file);
int myOpen(struct _reent *r,const char *path,int mode);
int myClose (struct _reent *r, int file);

int myFsync(FILE* fp);

#ifdef __cplusplus
}
#endif

#endif /* MYFILEHANDLE_H_ */
