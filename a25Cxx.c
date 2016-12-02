/***********************************************
Copyright (C) ,2016 深圳长广有限公司

File name:	    a25Cxx.c

Aunthor:	    冯晓康    Version: V0.1    Date:2016.11.04

Description: 
			    此C 主要包含了对25010 020 040 080 160 320 640 128 256 95系列 的 读写操作！！！
				8脚芯片 的电源 直接用普通IO口 提供就可以。AVR 单片机的 io 驱动能力 是较强的。
Version:	
			    V0.1  初行版本

Function List:  
				
History: //历史修改记录
<author>  <time>  <version>  <desc>
冯晓康    161114   V0.2      修改初始化 


************************************************/ 
#include "a25Cxx.h"
extern EE_IO_info_t ee_io_info;
unsigned char state=0;
/*
*Function:			a25cxx_init
*Description:       25 95xx x5043 x5045 初始化
*Aunthor:			冯晓康
*History:
*        161114  冯晓康  增加初始化时 将状态寄存器保护位置0 防止芯片 无法写进去 在读写函数前  都加5MS延时
*--------------------------
*Input:				--
*Output:            --
*Return:			--
*Others:			注意！！
*
*--------------------------
*/
void a25cxx_init()
{
	
	spi_init();
	AT25_GND_INIT(ee_io_info.GND);
	AT25_WP_INIT(ee_io_info.WP_PE);
	AT25_CS_INIT(ee_io_info.A0_CS);
	AT25_HOLD_INIT(ee_io_info.A1_HOLD_ORG);
	AT25_VCC_INIT(ee_io_info.VCC);
	
	AT25_VCC_HIGH(ee_io_info.VCC);
	AT25_GND_LOW(ee_io_info.GND);
	AT25_CS_HIGH(ee_io_info.A0_CS);
	AT25_HOLD_HIGH(ee_io_info.A1_HOLD_ORG);
	AT25_WP_HIGH(ee_io_info.WP_PE);
	hc595_write(1<<ee_io_info.GND,0xff);
	
	_delay_ms(5);
	AT25_CS_LOW(ee_io_info.A0_CS);           //先读 状态寄存器值  
	spi_rw_byte(0x05,0);
	state = spi_rw_byte(0xff,0);
	AT25_CS_HIGH(ee_io_info.A0_CS);
	
	state = state & 0xf3;    //状态寄存器 WPEN - - - BP1 BP0 WEL WIP   将BPI BP0置0  即取消软件保护
	
	_delay_us(1);                               //写状态寄存器 前 必须使能写
	AT25_CS_LOW(ee_io_info.A0_CS);
	_delay_us(3);
	spi_rw_byte(0x06,0);
	AT25_CS_HIGH(ee_io_info.A0_CS);
	
		_delay_us(4);
		AT25_CS_LOW(ee_io_info.A0_CS);           //写状态寄存器
		_delay_us(2);
		spi_rw_byte(0x01,0);
		spi_rw_byte(state,0);
		AT25_CS_HIGH(ee_io_info.A0_CS);
		_delay_us(1);
	
	//_delay_ms(200);
}

/*
*Function:			a25cxx_read
*Description:       25 95xx 页读
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				saddr 起始地址  page 写的哪页    pagesize 页大小
*Output:            *u8data 数据
*Return:			SUCCESS 成功 ERR 失败
*Others:			注意！！  该C适合 25010 020 040 x5043/5045
*					
*--------------------------
*/
void a25cxx_read(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)
{
	unsigned int u16addr = page * pagesize + saddr;
	unsigned int i = 0;
	unsigned char rcmd = 0;
	_delay_ms(5);//1013  因为x5043/5045 的上电后需要延迟一会  不然第一个字节会错误  代码操作和25040 一样
	AT25_CS_HIGH(ee_io_info.A0_CS);
	for (i=0;i<pagesize;i++)
	{
		rcmd = (0x03+((u16addr/256)<<3));
	//	_delay_us(1);
		_delay_us(10);
		AT25_CS_LOW(ee_io_info.A0_CS);
		spi_rw_byte(rcmd,0);
		spi_rw_byte((unsigned char)u16addr,0);
		u8data[i]=spi_rw_byte(0x00,0);
		AT25_CS_HIGH(ee_io_info.A0_CS);
		u16addr++;
		//_delay_us(10);
	}
}

/*
*Function:			a25cxx_write
*Description:       25 95xx 页
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				saddr 起始地址  page 写的哪页    pagesize 页大小
*Output:            *u8data 数据
*Return:			SUCCESS 成功 ERR 失败
*Others:			注意！！  该C适合 25010 020 040 x5043/5045
*
*--------------------------
*/
unsigned char a25cxx_write(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)
{
	unsigned int u16addr = page * pagesize + saddr;
	unsigned int i = 0,j=0,k=0;
	unsigned char state=0;
	unsigned char wcmd = 0;
	unsigned char rcmd = 0,data=0;
	_delay_ms(5);//1013  因为x5043/5045 的上电后需要延迟一会  不然第一个字节会错误 代码操作和25040 一样
	AT25_CS_HIGH(ee_io_info.A0_CS);
	for (i=0;i<pagesize;i++)
	{
		rcmd = (0x03+((u16addr/256)<<3));          //实验证明必须加读部分
		wcmd = (0x02+((u16addr/256)<<3));
		AT25_CS_LOW(ee_io_info.A0_CS);
		spi_rw_byte(rcmd,0);
		spi_rw_byte((unsigned char)u16addr,0);
		data=spi_rw_byte(0x00,0);
		AT25_CS_HIGH(ee_io_info.A0_CS);
		
		if (data != u8data[i])       //判断写入值是否与已存值相同
		{
		m1:	_delay_us(1);
			AT25_CS_LOW(ee_io_info.A0_CS);
			_delay_us(3);
			spi_rw_byte(0x06,0);
			AT25_CS_HIGH(ee_io_info.A0_CS);
			_delay_us(1);					
			AT25_CS_LOW(ee_io_info.A0_CS);
			_delay_us(3);
			spi_rw_byte(wcmd,0);
			spi_rw_byte((unsigned char)u16addr,0);
			spi_rw_byte(u8data[i],0);
			AT25_CS_HIGH(ee_io_info.A0_CS);
			for (j=0;j<100;j++)          //测试正常情况下10次左右
			{
				_delay_us(1);
				AT25_CS_LOW(ee_io_info.A0_CS);
				spi_rw_byte(0x05,0);
				state = spi_rw_byte(0xff,0);
				AT25_CS_HIGH(ee_io_info.A0_CS);
				if ((state&0x01)==0)
				{
					break;
				}
			}
			AT25_CS_LOW(ee_io_info.A0_CS);
			spi_rw_byte(rcmd,0);
			spi_rw_byte((unsigned char)u16addr,0);
			data=spi_rw_byte(0x00,0);
			AT25_CS_HIGH(ee_io_info.A0_CS);
			if (data!=u8data[i])
			{
				k++;
				if (k>10)
				{
					return EERE;
				}
				goto m1;
			}
		}
			u16addr++;
		k=0;
	}
	return SUCCESS;
}


/*
*Function:			a25cxl_read
*Description:       25 95xx 页读
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				saddr 起始地址  page 写的哪页    pagesize 页大小
*Output:            *u8data 数据
*Return:			SUCCESS 成功 ERR 失败
*Others:			注意！！  该C适合 25080 160 320 640 128 256
*
*--------------------------
*/
void a25cxl_read(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)
{
	unsigned int u16addr = page * pagesize + saddr;
	unsigned int i = 0;
	_delay_ms(5);
	AT25_CS_HIGH(ee_io_info.A0_CS);
	for (i=0;i<pagesize;i++)
	{
		_delay_us(4);
		AT25_CS_LOW(ee_io_info.A0_CS);
		spi_rw_byte(0x03,0);
		spi_rw_byte((unsigned char)(u16addr>>8),0);
		spi_rw_byte((unsigned char)u16addr,0);
		u8data[i]=spi_rw_byte(0x00,0);
		AT25_CS_HIGH(ee_io_info.A0_CS);
		u16addr++;
	}
}

/*
*Function:			a25cxl_write
*Description:       25 95xx 页
*Aunthor:			冯晓康
*History:
*
*--------------------------
*Input:				saddr 起始地址  page 写的哪页    pagesize 页大小
*Output:            *u8data 数据
*Return:			SUCCESS 成功 ERR 失败
*Others:			注意！！  该C适合 25080 160 320 640 128 256
*
*--------------------------
*/
unsigned char a25cxl_write(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)
{
	unsigned int u16addr = page * pagesize + saddr;
	unsigned int i = 0,j=0,k=0;
	unsigned char state=0;
	unsigned char wcmd = 0;
	unsigned char rcmd = 0,data=0;
	_delay_ms(5);
	AT25_CS_HIGH(ee_io_info.A0_CS);
	for (i=0;i<pagesize;i++)
	{
		AT25_CS_LOW(ee_io_info.A0_CS);
		spi_rw_byte(0x03,0);
		spi_rw_byte((unsigned char)(u16addr>>8),0);
		spi_rw_byte((unsigned char)u16addr,0);
		data=spi_rw_byte(0x00,0);
		AT25_CS_HIGH(ee_io_info.A0_CS);
		if (data != u8data[i])
		{
		m2:	_delay_us(1);
			AT25_CS_LOW(ee_io_info.A0_CS);
			_delay_us(3);
			spi_rw_byte(0x06,0);
			AT25_CS_HIGH(ee_io_info.A0_CS);
			_delay_us(1);
			AT25_CS_LOW(ee_io_info.A0_CS);
			_delay_us(3);
			spi_rw_byte(0x02,0);
			spi_rw_byte((unsigned char)(u16addr>>8),0);
			spi_rw_byte((unsigned char)u16addr,0);
			spi_rw_byte(u8data[i],0);
			AT25_CS_HIGH(ee_io_info.A0_CS);
			for (j=0;j<100;j++)    //正常情况下约20次左右
			{
				_delay_us(1);
				AT25_CS_LOW(ee_io_info.A0_CS);
				spi_rw_byte(0x05,0);
				state = spi_rw_byte(0xff,0);
				AT25_CS_HIGH(ee_io_info.A0_CS);
				if ((state&0x01)==0)
				{
					break;
				}
			}
			AT25_CS_LOW(ee_io_info.A0_CS);				 //实验证明必须加读部分
			spi_rw_byte(0x03,0);
			spi_rw_byte((unsigned char)(u16addr>>8),0);
			spi_rw_byte((unsigned char)u16addr,0);
			data=spi_rw_byte(0x00,0);
			AT25_CS_HIGH(ee_io_info.A0_CS);
			if (data!=u8data[i])
			{
				k++;
				if (k>10)
				{
					return EERE;
				}
				goto m2;
			}
		}			
		u16addr++;
		k=0;
	}
	return SUCCESS;
}