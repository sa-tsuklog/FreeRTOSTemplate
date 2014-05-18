/*
 * FLASH.h
 *
 *  Created on: 2014/05/14
 *      Author: sa
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

class FLASHClass {
	// Singleton pattern definition
private:
	FLASHClass();
	FLASHClass(const FLASHClass& rhs);
	FLASHClass& operator=(const FLASHClass& rhs);
	virtual ~FLASHClass() {}
public:
	static FLASHClass* GetInstance() {
    	static FLASHClass instance;
    	return &instance;
	}	
	// Class definition
private:
	static const int WORD_BLOCK_SIZE = 0x100/4;	//512Byte = 128 WORD 
	
	/* Adjust these parameters to the specification of the flash */
	static const int BYTE_SECTOR_SIZE = 0x4000;	//Sector size = 128kB.
	static const int USER_FLASH_START_ADDRESS = 0x080E0000; 
	static const int BLOCK_NUM = BYTE_SECTOR_SIZE/WORD_BLOCK_SIZE/sizeof(uint32_t);
	static const uint16_t sector = FLASH_Sector_11;
	static const uint16_t voltage_range = VoltageRange_1;
	
	uint32_t cache[WORD_BLOCK_SIZE];
	int dirty;
	
	int getBlockOffset();
	void erase();
public:
	uint32_t read(uint32_t wordOffset);
	void write(uint32_t wordOffset, uint32_t data);
	void flush();
	int getWordBlockSize(){return WORD_BLOCK_SIZE;}
	
};

#endif /* FLASH_H_ */
