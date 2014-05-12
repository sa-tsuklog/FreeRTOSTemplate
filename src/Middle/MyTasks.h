/*
 * Tasks.h
 *
 *  Created on: 2014/03/14
 *      Author: Lark_mp
 */

#ifndef TASKS_H_
#define TASKS_H_

extern "C"{

void prvADCTask(void *pvParameters);

void prvAd7176Task(void *pvParameters);
void prvSeekerTask(void *pvParameters);

}



#endif /* TASKS_H_ */
