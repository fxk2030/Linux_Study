/***********************************************
Copyright (C) ,2016 ���ڳ������޹�˾

File name:	    a24Cxx.c

Aunthor:	    ������    Version: V0.1    Date:2016.11.04

Description: 
			    ��C ��Ҫ�����˶�24c01 - 24c64 �� ��д����.�ô��벢δ��֤24c128���ϵ�оƬ������
				8��оƬ �ĵ�Դ ֱ������ͨIO�� �ṩ�Ϳ��ԡ�AVR ��Ƭ���� io �������� �ǽ�ǿ�ġ�
Version:	
			    V0.1  ���а汾

Function List:  
				AT24Cx_Init						��ʼ��
				AT24Cx_PageWrite				ҳд    24c01 02 04 08 16  
				AT24Cx_PageRead					ҳ��	24c01 02 04 08 16 
				AT24C32_PageWrite				ҳд	24c32 64
History: //��ʷ�޸ļ�¼
<author>  <time>  <version>  <desc>



************************************************/ 
#include "a24Cxx.h"
extern EE_IO_info_t ee_io_info;   //�洢IO ���Ŷ�λ�� �ṹ��
/*
*Function:			AT24Cx_Init
*Description:       24cxx ��ʼ��
*Aunthor:			������
*History:
*
*--------------------------
*Input:				--
*Output:            --
*Return:			--
*Others:			ע�⣡��
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
	hc595_write(1<<ee_io_info.GND,0xff); //���ؽ� ���ӵ���
}
/*
*Function:			AT24Cx_PageWrite
*Description:       24cxx ҳд
*Aunthor:			������
*History:
*
*--------------------------
*Input:				saddr ��ʼ��ַ  page д����ҳ  *u8data ����  pagesize ҳ��С
*Output:            --
*Return:			SUCCESS �ɹ� ERR ʧ��
*Others:			ע�⣡��  ��C�ʺ� 24c01 02 04 08 16  
*					����Ϊ �ȶ�һ���ֽ�  Ȼ��ȽϺ�Ҫд�������Ƿ���ͬ ����������д  �����д���ֽ� Ȼ��������Ƿ�д��ȥ  ��û�������ظ� ��5����ûд��ȥ �򷵻ش���
*--------------------------
*/
int AT24Cx_PageWrite(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)  //01,02,04,08,16  //0422 ���̸���  �ȶ���Ҫд������һ����������д���ٶ���ȷ��
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
		i2c_SendByte(addr1);        //ע��˴���Ϊ��������ַ��
		r_data = i2c_ReceiveByte();
		i2c_SendNack();
		i2c_end();
		if (r_data != u8data[i]) //�ж�д�������ǲ��Ǻ���ǰ������ͬ
		{
		y1:	i2c_start();
			i2c_SendByte(addr);
			i2c_SendByte((unsigned char)u16addr);
			i2c_SendByte(u8data[i]);
			i2c_end();
			
			while(out_time--)     //���������Լ40������
			{	
				i2c_start();
				if (i2c_SendByte(addr)==0)   //�жϵ�ǰ�ֽ��Ƿ�д��
				break;
				_delay_us(10);
				i2c_end();
			}							
			i2c_start();
			i2c_SendByte(addr);
			i2c_SendByte((unsigned char)u16addr);
			i2c_start();
			i2c_SendByte(addr1);        //ע��˴���Ϊ��������ַ��
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
*Description:       24cxx ҳ��
*Aunthor:			������
*History:
*
*--------------------------
*Input:				saddr ��ʼ��ַ  page ������ҳ  *u8data ����  pagesize ҳ��С
*Output:            --
*Return:			--
*Others:			ע�⣡��  ��C�ʺ� 24c01 02 04 08 16
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
	i2c_SendByte(addr1);        //ע��˴���Ϊ��������ַ��
	for (i=0;i<pagesize;i++)
	{
		u8data[i] = i2c_ReceiveByte();
		i2c_SendAck();
	}
	i2c_end();
	

}

/*
*Function:			AT24C32_PageWrite
*Description:       24cxx ҳд
*Aunthor:			������
*History:
*
*--------------------------
*Input:				saddr ��ʼ��ַ  page д����ҳ  *u8data ����  pagesize ҳ��С
*Output:            --
*Return:			SUCCESS �ɹ� ERR ʧ��
*Others:			ע�⣡��  ��C�ʺ� 24c32 64
*					����Ϊ �ȶ�һ���ֽ�  Ȼ��ȽϺ�Ҫд�������Ƿ���ͬ ����������д  �����д���ֽ� Ȼ��������Ƿ�д��ȥ  ��û�������ظ� ��5����ûд��ȥ �򷵻ش���
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
		i2c_SendByte(0xa1);        //ע��˴���Ϊ��������ַ��
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
			
			while(out_time--)    //���������30������
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
			i2c_SendByte(0xa1);        //ע��˴���Ϊ��������ַ��
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
*Description:       24cxx ҳ��
*Aunthor:			������
*History:
*
*--------------------------
*Input:				saddr ��ʼ��ַ  page ������ҳ  *u8data ����  pagesize ҳ��С
*Output:            --
*Return:			--
*Others:			ע�⣡��  ��C�ʺ� 24c32 64
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
	i2c_SendByte(0xa1);        //ע��˴���Ϊ��������ַ��
	for (i=0;i<pagesize;i++)
	{
		u8data[i] = i2c_ReceiveByte();
		i2c_SendAck();
	}
	i2c_end();
}