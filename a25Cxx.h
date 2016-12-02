/*
 * a25Cxx.h
 *
 * Created: 2016/3/11 14:52:06
 *  Author: fxk
 */ 


#ifndef A25CXX_H_
#define A25CXX_H_

#include "config.h"

void a25cxx_init();
void a25cxx_read(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize);
unsigned char a25cxx_write(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize);

void a25cxl_read(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize);
unsigned char a25cxl_write(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize);

#endif /* A25CXX_H_ */