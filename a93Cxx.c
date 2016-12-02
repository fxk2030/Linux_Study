/***********************************************
Copyright (C) ,2016 深圳长广有限公司

File name:	    a93Cxx.c

Aunthor:	    冯晓康    Version: V0.1    Date:2016.11.04

Description: 
			    此C 主要包含了对93c46 56 57 66 76 86系列 的 读写操作！！！
				8脚芯片 的电源 直接用普通IO口 提供就可以。AVR 单片机的 io 驱动能力 是较强的。
Version:	
			    V0.1  初行版本

Function List:  
				
History: //历史修改记录
<author>  <time>  <version>  <desc>



************************************************/ 

#include <avr/io.h>
#include "bord.h"
#include "delay.h"
#include "config.h"

extern EE_IO_info_t ee_io_info;

void high46(void)
{
  AT93_DI_HIGH(ee_io_info.SDA_DI);
  _delay_us(3);
  AT93_CLK_HIGH(ee_io_info.SCL_SCK);
  _delay_us(3);
  AT93_CLK_LOW(ee_io_info.SCL_SCK);
  _delay_us(3);
}
void low46(void)
{
  AT93_DI_LOW(ee_io_info.SDA_DI);
  _delay_us(3);
  AT93_CLK_HIGH(ee_io_info.SCL_SCK);
  _delay_us(3);
  AT93_CLK_LOW(ee_io_info.SCL_SCK);
  _delay_us(3);
}

void wd46(unsigned char dd)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    if (dd >= 0x80)
      high46();
    else
      low46();
    dd = dd << 1;
  }
}
unsigned char rd46(void)
{
  unsigned char i, dd,j;
  for (i = 0; i < 8; i++)
  {
    dd <<= 1;
    AT93_CLK_HIGH(ee_io_info.SCL_SCK);
    _delay_us(3);
    AT93_CLK_LOW(ee_io_info.SCL_SCK);
    _delay_us(3);
	j = AT93_DO_GET(ee_io_info.A2_DO);
    if ((j==1)||((j&0x08)==0x08))
      dd |= 1;
  }
  return (dd);
}

void ewen46(uint8_t mode)
{
  _delay_us(1);
  AT93_CS_HIGH(ee_io_info.A0_CS);
  _delay_us(1);
  if (mode==1)
  {
	   high46();
	   wd46(0x30);
  }
  else
  {
	   high46();
	   low46();
	   wd46(0x60);
  }
 
  AT93_CS_LOW(ee_io_info.A0_CS);
}


uint16_t read93c46_word(uint8_t address,uint8_t *dat,uint8_t mode)
{
  uint8_t i = 0;

	AT93_VCC_HIGH(ee_io_info.VCC);
  AT93_GND_LOW(ee_io_info.GND);
  AT93_CS_LOW(ee_io_info.A0_CS);
  AT93_CLK_LOW(ee_io_info.SCL_SCK);
  //AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
  AT93_CS_HIGH(ee_io_info.A0_CS);
 
  if (mode==1)
  {
	  AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
	   _delay_us(3);
	   address = address >> 1;
	   address = address | 0x80;
	   high46();
	   wd46(address);
	  // dat[1] = rd46();
	  // dat[0] = rd46();
	   dat[0] = rd46(); //0801  
	   dat[1] = rd46();
  }
  else
  {
	   AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
	   _delay_us(3);
	  // address = address >> 1;
	  high46();
	  high46();
	  wd46(address);
	  dat[0] = rd46();
  }
 
  AT93_CS_LOW(ee_io_info.A0_CS);
}

uint8_t write93c46_word(uint8_t address, uint8_t *dat,uint8_t mode)
{
  uint8_t e, temp = address,j;
  uint32_t out_time = 10000;
	AT93_VCC_HIGH(ee_io_info.VCC);
    AT93_GND_LOW(ee_io_info.GND);

    AT93_CS_LOW(ee_io_info.A0_CS);
    AT93_CLK_LOW(ee_io_info.SCL_SCK);

   // AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
    AT93_CS_HIGH(ee_io_info.A0_CS);
	
	if (mode==1)
	{
		AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
		ewen46(1);
		_delay_us(1);
		AT93_CS_HIGH(ee_io_info.A0_CS);
		_delay_us(1);
		high46();
		address |= 0x80;  //?
		address >>= 1;
		wd46(address);
		//wd46(dat[1]);
		//wd46(dat[0]);
		wd46(dat[0]); //0801
		wd46(dat[1]);
		AT93_CS_LOW(ee_io_info.A0_CS);
	} 
	else
	{
		AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
		ewen46(2);
		_delay_us(1);
		AT93_CS_HIGH(ee_io_info.A0_CS);
		_delay_us(1);
		high46();
		low46();
		address |= 0x80;
		//address >>= 1;
		wd46(address);
		//wd46(dat[1]);
		wd46(dat[0]);
		AT93_CS_LOW(ee_io_info.A0_CS);
	}

    
    _delay_us(1);
    AT93_CS_HIGH(ee_io_info.A0_CS);
    while (out_time--)
    {
		
		j = AT93_DO_GET(ee_io_info.A2_DO);
      if ((j==1)||((j&0x08)==0x08))
	  {
        break;
	  }		
    }
    AT93_CS_LOW(ee_io_info.A0_CS);
  return (1);
}


void a93c46_init()
{
	AT93_CS_INIT(ee_io_info.A0_CS);
	AT93_CLK_INIT(ee_io_info.SCL_SCK);
	AT93_DI_INIT(ee_io_info.SDA_DI);
	AT93_GND_INIT(ee_io_info.GND);
	AT93_ORG_INIT(ee_io_info.A1_HOLD_ORG);
	AT93_VCC_INIT(ee_io_info.VCC);
	AT93_PE_INIT(ee_io_info.WP_PE);
	AT93_DO_INIT(ee_io_info.A2_DO);
	AT93_PE_HIGH(ee_io_info.WP_PE);
	AT93_VCC_HIGH(ee_io_info.VCC);
	//hc595_write(0x10,0xff);
	hc595_write(1<<ee_io_info.GND,0xff);
}
//=============================================================
//93c57
void ewen57(uint8_t mode)
{
  _delay_us(1);
  AT93_CS_HIGH(ee_io_info.A0_CS);
  if (mode == 1)
  {
	   high46();
	   low46();
	   wd46(0x60); 
  } 
  else
  {
	    high46();
		low46();
	    low46();
	    wd46(0xc0);
  }

  AT93_CS_LOW(ee_io_info.A0_CS);
}

uint16_t read93c57_word(uint8_t address,uint8_t *dat,uint8_t mode)
{
  AT93_GND_LOW(ee_io_info.GND);
  AT93_VCC_HIGH(ee_io_info.VCC);
  AT93_CS_LOW(ee_io_info.A0_CS);
  AT93_CLK_LOW(ee_io_info.SCL_SCK);
  //AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
  AT93_CS_HIGH(ee_io_info.A0_CS);
  
  if (mode==1)
  {
	  AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
	  address = address >> 1;
	  high46();
	  high46();
	  wd46(address);
	 // dat[1] = rd46();
	 // dat[0] = rd46();
	  dat[0] = rd46();//0801
	  dat[1] = rd46();
  } 
  else
  {
	  AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
	 // address = address >> 1;
	  high46();
	  high46();
	  low46();
	  wd46(address);
	  dat[0] = rd46();
  }
  
  
  AT93_CS_LOW(ee_io_info.A0_CS);
}

uint8_t write93c57_word(uint8_t address,uint8_t *dat,uint8_t mode)
{
 
  uint8_t j;
  uint16_t temp = address;
  uint32_t out_time = 10000;
    AT93_GND_LOW(ee_io_info.GND);
	AT93_VCC_HIGH(ee_io_info.VCC);
    AT93_CS_LOW(ee_io_info.A0_CS);
    AT93_CLK_LOW(ee_io_info.SCL_SCK);
    //AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
    AT93_CS_HIGH(ee_io_info.A0_CS);
	
	if (mode==1)
	{
		AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
		 ewen57(1);
		AT93_CS_HIGH(ee_io_info.A0_CS);
		_delay_us(1);
		high46();
		low46();
		address >>= 1;
		address |= 0x80;
		wd46(address);
		//wd46(dat[1]);
		//wd46(dat[0]);
		wd46(dat[0]);//0801
		wd46(dat[1]);
	} 
	else
	{
		AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
		ewen57(2);
		AT93_CS_HIGH(ee_io_info.A0_CS);
		_delay_us(1);
		high46();
		low46();
		high46();
		wd46(address);
		wd46(dat[0]);
	}
   
    AT93_CS_LOW(ee_io_info.A0_CS);
    _delay_us(3);
    AT93_CS_HIGH(ee_io_info.A0_CS);
 
    while (out_time--)
    {
		
		j = AT93_DO_GET(ee_io_info.A2_DO);
      if ((j==1)||((j&0x08)==0x08))
      {
	      break;
      }
    }

    AT93_CS_LOW(ee_io_info.A0_CS);
  return (1);
}

//=================================================

void ewen56(uint8_t mode)
{
	_delay_us(1);
	AT93_CS_HIGH(ee_io_info.A0_CS);
	if (mode==1)
	{
		high46();
		low46();
		low46();
		wd46(0xc0);
	} 
	else
	{
		high46();
		low46();
		low46();
		high46();
		wd46(0x80);
	}
	
	AT93_CS_LOW(ee_io_info.A0_CS);
}

uint16_t read93c56_word(uint16_t address,uint8_t *dat,uint8_t mode)
{
	AT93_GND_LOW(ee_io_info.GND);
	AT93_VCC_HIGH(ee_io_info.VCC);
	AT93_CS_LOW(ee_io_info.A0_CS);
	AT93_CLK_LOW(ee_io_info.SCL_SCK);
	//AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
	AT93_CS_HIGH(ee_io_info.A0_CS);
	if (mode==1)
	{
		AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
		address = address >> 1;
		high46();
		high46();
		low46();
		wd46(address);
		//dat[1] = rd46();
		//dat[0] = rd46();
		dat[0] = rd46();//0801
		dat[1] = rd46();
	} 
	else
	{
		AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
		high46();
		high46();
		low46();
		if ((address & 0x100) == 0x100)
		{
			high46();
		} 
		else
		{
			low46();
		}
		wd46((uint8_t)address);
		dat[0] = rd46();
	}
	AT93_CS_LOW(ee_io_info.A0_CS);
}

uint8_t write93c56_word(uint16_t address,uint8_t *dat,uint8_t mode)
{
	uint8_t j;
	uint16_t temp = address;
	uint32_t out_time = 10000;
		AT93_GND_LOW(ee_io_info.GND);
		AT93_VCC_HIGH(ee_io_info.VCC);
		AT93_CS_LOW(ee_io_info.A0_CS);
		AT93_CLK_LOW(ee_io_info.SCL_SCK);
	//	AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
		AT93_CS_HIGH(ee_io_info.A0_CS);
		
		if (mode==1)
		{
			AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
				ewen56(1);
				_delay_us(3);
				AT93_CS_HIGH(ee_io_info.A0_CS);
				_delay_us(1);
				high46();
				low46();
				high46();
				if ((address & 0x100) == 0x100)
				{
					address >>= 1;
					address |= 0x80;
				}
				else
				{
					address >>= 1;
				}
				wd46(address);
				//wd46(dat[1]);
				//wd46(dat[0]);
				wd46(dat[0]);//0801
				wd46(dat[1]);
		} 
		else
		{	
			AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
				ewen56(2);
				_delay_us(3);
				AT93_CS_HIGH(ee_io_info.A0_CS);
				_delay_us(1);
				high46();
				low46();
				high46();
				if ((address & 0x100) == 0x100)
				{
					high46();
				}
				else
				{
					low46();
				}
				wd46((uint8_t)address);
				wd46(dat[0]);
		}
	
		AT93_CS_LOW(ee_io_info.A0_CS);
		_delay_us(1);
		AT93_CS_HIGH(ee_io_info.A0_CS);
		while (out_time--)
		{
			
			j = AT93_DO_GET(ee_io_info.A2_DO);
			if ((j==1)||((j&0x08)==0x08))
			{
				break;
			}
		}
		AT93_CS_LOW(ee_io_info.A0_CS);
	return (1);
}



//========================================================
//93c76 93c86

//=======================================================
void ewen76(uint8_t mode)
{
  _delay_us(1);
  AT93_CS_HIGH(ee_io_info.A0_CS);
  if (mode==1)
  {
	   high46();
	   low46();
	   low46();
	   high46();
	   high46();
	   wd46(0x00);
  } 
  else
  {
	  high46();
	  low46();
	  low46();
	  high46();
	  high46();
	  low46();
	  wd46(0x00);
  }
 
  AT93_CS_LOW(ee_io_info.A0_CS);
}

uint16_t read93c76_word(uint16_t address,uint8_t *dat,uint8_t mode)
{
  AT93_GND_LOW(ee_io_info.GND);
  AT93_VCC_HIGH(ee_io_info.VCC);
  AT93_CS_LOW(ee_io_info.A0_CS);
  AT93_CLK_LOW(ee_io_info.SCL_SCK);
  //AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
  AT93_CS_HIGH(ee_io_info.A0_CS);
  if (mode==1)
  {
	  AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
	  address >>= 1;
	  high46();
	  high46();
	  low46();
	  if ((address & 0x200) == 0x200)
	  high46();
	  else
	  low46();
	  if ((address & 0x100) == 0x100)
	  high46();
	  else
	  low46();
	  wd46(address);
	 // dat[1] = rd46();
	 // dat[0] = rd46(); 
	  dat[0] = rd46();//0801
	  dat[1] = rd46();
  } 
  else
  {
	  AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
	  high46();
	  high46();
	  low46();
	  if ((address & 0x400) == 0x400)
	  high46();
	  else
	  low46();
	  if ((address & 0x200) == 0x200)
	  high46();
	  else
	  low46();
	  if ((address & 0x100) == 0x100)
	  high46();
	  else
	  low46();
	  wd46(address);
	  dat[0] = rd46();
  }
 
  AT93_CS_LOW(ee_io_info.A0_CS);
}

uint8_t write93c76_word(uint16_t address, uint8_t *dat,uint8_t mode)
{

  uint8_t j;
  uint16_t temp = address;
  uint32_t out_time = 10000;
  
  
    AT93_GND_LOW(ee_io_info.GND);
	AT93_VCC_HIGH(ee_io_info.VCC);
    AT93_CS_LOW(ee_io_info.A0_CS);
    AT93_CLK_LOW(ee_io_info.SCL_SCK);
   // AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
    AT93_CS_HIGH(ee_io_info.A0_CS);
	
	if (mode==1)
	{
		AT93_ORG_HIGH(ee_io_info.A1_HOLD_ORG);
		ewen76(1);
		address >>= 1;
		_delay_us(1);
		AT93_CS_HIGH(ee_io_info.A0_CS);
		high46();
		low46();
		high46();
		if ((address & 0x200) == 0x200)
		high46();
		else
		low46();
		if ((address & 0x100) == 0x100)
		high46();
		else
		low46();
		wd46(address);
		//wd46(dat[1]);
		//wd46(dat[0]);
		wd46(dat[0]);//0801
		wd46(dat[1]);
	} 
	else
	{
		AT93_ORG_LOW(ee_io_info.A1_HOLD_ORG);
		ewen76(2);
		_delay_us(1);
		AT93_CS_HIGH(ee_io_info.A0_CS);
		high46();
		low46();
		high46();
		 if ((address & 0x400) == 0x400)
		 high46();
		 else
		 low46();
		if ((address & 0x200) == 0x200)
		high46();
		else
		low46();
		if ((address & 0x100) == 0x100)
		high46();
		else
		low46();
		wd46(address);
		//wd46(dat[1]);
		wd46(dat[0]);
	}
    
    AT93_CS_LOW(ee_io_info.A0_CS);
    _delay_us(1);
    AT93_CS_HIGH(ee_io_info.A0_CS);
   while (out_time--)        //约4000次
   {
	   
		j = AT93_DO_GET(ee_io_info.A2_DO);
		if ((j==1)||((j&0x08)==0x08))
		{
			break;
		}
   }
    AT93_CS_LOW(ee_io_info.A0_CS);
  return (1);
}

/************************字节读写******************/

