/*
 * _93Cxx.h
 *
 * Created: 2015/12/29 10:46:36
 *  Author: Administrator
 */ 


#ifndef A93CXX_H_
#define A93CXX_H_
#include "transplant.h"


uint16_t read93c46_word(uint8_t address,uint8_t *dat,uint8_t mode);
uint8_t write93c46_word(uint8_t address, uint8_t *dat,uint8_t mode);
void a93c46_init();

uint16_t read93c56_word(uint16_t address,uint8_t *dat,uint8_t mode);
uint8_t write93c56_word(uint16_t address,uint8_t *dat,uint8_t mode);

uint16_t read93c57_word(uint8_t address,uint8_t *dat,uint8_t mode);
uint8_t write93c57_word(uint8_t address,uint8_t *dat,uint8_t mode);

uint16_t read93c76_word(uint16_t address,uint8_t *dat,uint8_t mode);
uint8_t write93c76_word(uint16_t address,uint8_t *dat,uint8_t mode);


#endif /* 93CXX_H_ */