/*
 * Tasks.h
 *
 *  Created on: 2014/03/14
 *      Author: Lark_mp
 */

#ifndef TASKS_H_
#define TASKS_H_

extern "C"{

void ADCTask(void *pvParameters);

void prvTxTask(void *pvParameters);
void prvRxTask(void *pvParameters);

void prvAdis16488Task(void *pvParameters);

void prvI2C2SendTask(void *pvParameters);

}



#endif /* TASKS_H_ */
