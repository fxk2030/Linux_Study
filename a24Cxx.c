/***********************************************
Copyright (C) ,2016 深圳长广有限公司

File name:	    a24Cxx.c

Aunthor:	    冯晓康    Version: V0.1    Date:2016.11.04

Description: 
			    此C 主要包含了对24c01 - 24c64 的 读写操作.该代码并未验证24c128以上的芯片！！！
				8脚芯片 的电源 直接用普通IO口 提供就可以。AVR 单片机的 io 驱动能力 是较强的。
Version:	
			    V0.1  初行版本

Function List:  
				AT24Cx_Init						初始化
				AT24Cx_PageWrite				页写    24c01 02 04 08 16  
				AT24Cx_PageRead					页读	24c01 02 04 08 16 
				AT24C32_PageWrite				页写	24c32 64
History: //历史修改记录
<author>  <time>  <version>  <desc>



************************************************/ 
#include "a24Cxx.h"
extern EE_IO_info_t ee_io_info;   //存储IO 引脚定位的 结构体
/*
*Function:			AT24Cx_Init
*Description:       24cxx 初始化
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				--
*Output:            --
*Return:			--
*Others:			注意！！
*
*--------------------------
*/
void AT24Cx_Init()
{
	AT24_A0_INIT(ee_io_info.A0_CS);
	AT24_A1_INIT(ee_io_info.A1_HOLD_ORG);
	AT24_A2_INIT(ee_io_info.A2_DO);
	AT24_GND_INIT(ee_io_info.GND);
	I2C_SDA_OUTINIT(ee_io_info.SDA_DI);
	I2C_SCL_INIT(ee_io_info.SCL_SCK);
	AT24_WP_INIT(ee_io_info.WP_PE);
	AT24_VCC_INIT(ee_io_info.VCC);
	
	AT24_A0_LOW(ee_io_info.A0_CS);
	AT24_A1_LOW(ee_io_info.A1_HOLD_ORG);
	AT24_A2_LOW(ee_io_info.A2_DO);
	AT24_GND_LOW(ee_io_info.GND);
	I2C_SDA_LOW(ee_io_info.SDA_DI);
	I2C_SCL_LOW(ee_io_info.SCL_SCK);
	AT24_WP_LOW(ee_io_info.WP_PE);
	AT24_VCC_HIGH(ee_io_info.VCC);
	hc595_write(1<<ee_io_info.GND,0xff); //将地脚 连接到地
}
/*
*Function:			AT24Cx_PageWrite
*Description:       24cxx 页写
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				saddr 起始地址  page 写的哪页  *u8data 数据  pagesize 页大小
*Output:            --
*Return:			SUCCESS 成功 ERR 失败
*Others:			注意！！  该C适合 24c01 02 04 08 16  
*					流程为 先读一个字节  然后比较和要写的数据是否相同 是则跳过不写  否则就写该字节 然后读出来是否写进去  若没有则再重复 在5次内没写进去 则返回错误
*--------------------------
*/
int AT24Cx_PageWrite(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)  //01,02,04,08,16  //0422 流程改了  先读和要写的数据一样则跳过，写完再读，确认
{
	unsigned int u16addr = page * pagesize+ saddr;
	unsigned int i = 0,out_time,j=0;
	unsigned char addr;
	unsigned char addr1;
	unsigned char r_data;
	for (i = 0; i < pagesize; i++)
	{
		out_time = 100;
		addr = (0xa0+((u16addr/256)<<1));
		 addr1 = (0xa1+((u16addr/256)<<1));
		 
		i2c_start();
		i2c_SendByte(addr);
		i2c_SendByte((unsigned char)u16addr);
		i2c_start();
		i2c_SendByte(addr1);        //注意此处该为从器件地址！
		r_data = i2c_ReceiveByte();
		i2c_SendNack();
		i2c_end();
		if (r_data != u8data[i]) //判断写的数据是不是和先前数据相同
		{
		y1:	i2c_start();
			i2c_SendByte(addr);
			i2c_SendByte((unsigned char)u16addr);
			i2c_SendByte(u8data[i]);
			i2c_end();
			
			while(out_time--)     //正常情况下约40次左右
			{	
				i2c_start();
				if (i2c_SendByte(addr)==0)   //判断当前字节是否写完
				break;
				_delay_us(10);
				i2c_end();
			}							
			i2c_start();
			i2c_SendByte(addr);
			i2c_SendByte((unsigned char)u16addr);
			i2c_start();
			i2c_SendByte(addr1);        //注意此处该为从器件地址！
			r_data = i2c_ReceiveByte();
			i2c_SendNack();
			i2c_end();
			
			if (r_data != u8data[i])
			{
				j++;
				goto y1;
			}
			if (j>5)
			{
				return ERR;
			}
		}	
		u16addr++;	
		j=0;
	}
	return SUCCESS;
}

/*
*Function:			AT24Cx_PageRead
*Description:       24cxx 页读
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				saddr 起始地址  page 读的哪页  *u8data 数据  pagesize 页大小
*Output:            --
*Return:			--
*Others:			注意！！  该C适合 24c01 02 04 08 16
*					
*--------------------------
*/
void AT24Cx_PageRead(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)
{
	unsigned int u16addr = page * pagesize+ saddr;
	unsigned int i = 0;
	unsigned char addr = (0xa0+((u16addr/256)<<1));
	unsigned char addr1 = (0xa1+((u16addr/256)<<1));

	i2c_start();
	i2c_SendByte(addr);
	i2c_SendByte((unsigned char)u16addr);
	
	i2c_start();
	i2c_SendByte(addr1);        //注意此处该为从器件地址！
	for (i=0;i<pagesize;i++)
	{
		u8data[i] = i2c_ReceiveByte();
		i2c_SendAck();
	}
	i2c_end();
	

}

/*
*Function:			AT24C32_PageWrite
*Description:       24cxx 页写
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				saddr 起始地址  page 写的哪页  *u8data 数据  pagesize 页大小
*Output:            --
*Return:			SUCCESS 成功 ERR 失败
*Others:			注意！！  该C适合 24c32 64
*					流程为 先读一个字节  然后比较和要写的数据是否相同 是则跳过不写  否则就写该字节 然后读出来是否写进去  若没有则再重复 在5次内没写进去 则返回错误
*--------------------------
*/
int AT24C32_PageWrite(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)  //32 64
{
	unsigned int u16addr = page * pagesize+ saddr;
	unsigned int i = 0,out_time,j=0;
	unsigned char r_data;
	for (i = 0; i < pagesize; i++)
	{
		out_time = 100;
		
		i2c_start();
		i2c_SendByte(0xa0);
		i2c_SendByte(u16addr>>8);
		i2c_SendByte((unsigned char)u16addr);
		i2c_start();
		i2c_SendByte(0xa1);        //注意此处该为从器件地址！
		r_data = i2c_ReceiveByte();
		i2c_SendNack();
		i2c_end();
		
		if (r_data != u8data[i])
		{
		y2:	i2c_start();
			i2c_SendByte(0xa0);
			i2c_SendByte(u16addr>>8);
			i2c_SendByte((unsigned char)u16addr);
			i2c_SendByte(u8data[i]);
			i2c_end();
			
			while(out_time--)    //正常情况下30次左右
			{
				
				i2c_start();
				if (i2c_SendByte(0xa0)==0)
				break;
				_delay_us(10);
				i2c_end();
			}
			
			i2c_start();
			i2c_SendByte(0xa0);
			i2c_SendByte(u16addr>>8);
			i2c_SendByte((unsigned char)u16addr);
			i2c_start();
			i2c_SendByte(0xa1);        //注意此处该为从器件地址！
			r_data = i2c_ReceiveByte();
			i2c_SendNack();
			i2c_end();
			
			if (r_data != u8data[i])
			{
				j++;
				goto y2;
			}
			if (j>5)
			{
				return ERR;
			}
		}		
		u16addr++;
		j=0;
		
				
	}
	return SUCCESS;
}

/*
*Function:			AT24Cx_PageRead
*Description:       24cxx 页读
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				saddr 起始地址  page 读的哪页  *u8data 数据  pagesize 页大小
*Output:            --
*Return:			--
*Others:			注意！！  该C适合 24c32 64
*
*--------------------------
*/
void AT24C32_PageRead(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)
{
	unsigned int u16addr = page * pagesize+ saddr;
	unsigned int i = 0;

	i2c_start();
	i2c_SendByte(0xa0);
	i2c_SendByte(u16addr>>8);
	i2c_SendByte((unsigned char)u16addr);
	
	i2c_start();
	i2c_SendByte(0xa1);        //注意此处该为从器件地址！
	for (i=0;i<pagesize;i++)
	{
		u8data[i] = i2c_ReceiveByte();
		i2c_SendAck();
	}
	i2c_end();
}