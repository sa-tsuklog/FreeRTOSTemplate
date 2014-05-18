/*
 * FLASH.cpp
 *
 *  Created on: 2014/05/14
 *      Author: sa
 */

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "USER_FLASH.h"
#include "stdio.h"

FLASHClass::FLASHClass(){
	int blockOffset = getBlockOffset();
	
	printf("block offset %d\n\r",blockOffset);
	printf("block read%x\n\r",((USER_FLASH_START_ADDRESS+(blockOffset+1)*WORD_BLOCK_SIZE*sizeof(uint32_t))));
	
	for(int i=0;i<WORD_BLOCK_SIZE;i++){
		cache[i] = (uint32_t)(*(uint32_t*)(USER_FLASH_START_ADDRESS+(sizeof(uint32_t)*i)+(blockOffset+1)*WORD_BLOCK_SIZE*sizeof(uint32_t)));
	}
	dirty = 0;
}

uint32_t FLASHClass::read(uint32_t wordOffset){
	if(wordOffset<0 || wordOffset > WORD_BLOCK_SIZE){
		return 0;
	}
	return cache[wordOffset];
}

void FLASHClass::write(uint32_t wordOffset, uint32_t data){
	if(wordOffset<0 || wordOffset > WORD_BLOCK_SIZE){
		return;
	}
	cache[wordOffset] = data;
	dirty = 1;
}

void FLASHClass::flush(){
	if(!dirty){
		return;
	}
	int blockOffset = getBlockOffset();
	
	if(blockOffset+2 >= BLOCK_NUM){
		erase();
		blockOffset = -1;
	}
	
	
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGAERR|FLASH_FLAG_WRPERR);
	FLASH_Unlock();
	for(int i=0;i<WORD_BLOCK_SIZE;i++){
		FLASH_ProgramWord(USER_FLASH_START_ADDRESS+(sizeof(uint32_t)*i)+(blockOffset+2)*WORD_BLOCK_SIZE*sizeof(uint32_t),cache[i]);
	}
	FLASH_ProgramWord(USER_FLASH_START_ADDRESS+sizeof(uint32_t)*(blockOffset),0);
	
	int index = USER_FLASH_START_ADDRESS+sizeof(uint32_t)*(blockOffset);
	printf("index = %x\n\r",index);
	printf("wirte address= %x\n\r",USER_FLASH_START_ADDRESS+(blockOffset+2)*WORD_BLOCK_SIZE*sizeof(uint32_t));
	FLASH_Lock();
	dirty = 0;
}

int FLASHClass::getBlockOffset(){
	int blockOffset = 0;
	
	for(blockOffset = 0; blockOffset < BLOCK_NUM-1; blockOffset++){
		uint32_t blockUsed = (uint32_t)(*(uint32_t*)(USER_FLASH_START_ADDRESS+blockOffset*sizeof(uint32_t)));
		if(blockUsed == 0xFFFFFFFF){
			return blockOffset;
		}
	}
	return blockOffset;
}

void FLASHClass::erase(){
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGAERR|FLASH_FLAG_WRPERR);
	FLASH_Unlock();
	FLASH_EraseSector(sector,voltage_range);
	FLASH_Lock();
}
