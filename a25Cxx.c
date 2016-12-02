/***********************************************
Copyright (C) ,2016 ���ڳ������޹�˾

File name:	    a25Cxx.c

Aunthor:	    ������    Version: V0.1    Date:2016.11.04

Description: 
			    ��C ��Ҫ�����˶�25010 020 040 080 160 320 640 128 256 95ϵ�� �� ��д����������
				8��оƬ �ĵ�Դ ֱ������ͨIO�� �ṩ�Ϳ��ԡ�AVR ��Ƭ���� io �������� �ǽ�ǿ�ġ�
Version:	
			    V0.1  ���а汾

Function List:  
				
History: //��ʷ�޸ļ�¼
<author>  <time>  <version>  <desc>
������    161114   V0.2      �޸ĳ�ʼ�� 


************************************************/ 
#include "a25Cxx.h"
extern EE_IO_info_t ee_io_info;
unsigned char state=0;
/*
*Function:			a25cxx_init
*Description:       25 95xx x5043 x5045 ��ʼ��
*Aunthor:			������
*History:
*        161114  ������  ���ӳ�ʼ��ʱ ��״̬�Ĵ�������λ��0 ��ֹоƬ �޷�д��ȥ �ڶ�д����ǰ  ����5MS��ʱ
*--------------------------
*Input:				--
*Output:            --
*Return:			--
*Others:			ע�⣡��
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
	AT25_CS_LOW(ee_io_info.A0_CS);           //�ȶ� ״̬�Ĵ���ֵ  
	spi_rw_byte(0x05,0);
	state = spi_rw_byte(0xff,0);
	AT25_CS_HIGH(ee_io_info.A0_CS);
	
	state = state & 0xf3;    //״̬�Ĵ��� WPEN - - - BP1 BP0 WEL WIP   ��BPI BP0��0  ��ȡ���������
	
	_delay_us(1);                               //д״̬�Ĵ��� ǰ ����ʹ��д
	AT25_CS_LOW(ee_io_info.A0_CS);
	_delay_us(3);
	spi_rw_byte(0x06,0);
	AT25_CS_HIGH(ee_io_info.A0_CS);
	
		_delay_us(4);
		AT25_CS_LOW(ee_io_info.A0_CS);           //д״̬�Ĵ���
		_delay_us(2);
		spi_rw_byte(0x01,0);
		spi_rw_byte(state,0);
		AT25_CS_HIGH(ee_io_info.A0_CS);
		_delay_us(1);
	
	//_delay_ms(200);
}

/*
*Function:			a25cxx_read
*Description:       25 95xx ҳ��
*Aunthor:			������
*History:
*
*--------------------------
*Input:				saddr ��ʼ��ַ  page д����ҳ    pagesize ҳ��С
*Output:            *u8data ����
*Return:			SUCCESS �ɹ� ERR ʧ��
*Others:			ע�⣡��  ��C�ʺ� 25010 020 040 x5043/5045
*					
*--------------------------
*/
void a25cxx_read(unsigned int saddr,unsigned int page, unsigned char *u8data, unsigned int pagesize)
{
	unsigned int u16addr = page * pagesize + saddr;
	unsigned int i = 0;
	unsigned char rcmd = 0;
	_delay_ms(5);//1013  ��Ϊx5043/5045 ���ϵ����Ҫ�ӳ�һ��  ��Ȼ��һ���ֽڻ����  ���������25040 һ��
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
*Description:       25 95xx ҳ
*Aunthor:			������
*History:
*
*--------------------------
*Input:				saddr ��ʼ��ַ  page д����ҳ    pagesize ҳ��С
*Output:            *u8data ����
*Return:			SUCCESS �ɹ� ERR ʧ��
*Others:			ע�⣡��  ��C�ʺ� 25010 020 040 x5043/5045
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
	_delay_ms(5);//1013  ��Ϊx5043/5045 ���ϵ����Ҫ�ӳ�һ��  ��Ȼ��һ���ֽڻ���� ���������25040 һ��
	AT25_CS_HIGH(ee_io_info.A0_CS);
	for (i=0;i<pagesize;i++)
	{
		rcmd = (0x03+((u16addr/256)<<3));          //ʵ��֤������Ӷ�����
		wcmd = (0x02+((u16addr/256)<<3));
		AT25_CS_LOW(ee_io_info.A0_CS);
		spi_rw_byte(rcmd,0);
		spi_rw_byte((unsigned char)u16addr,0);
		data=spi_rw_byte(0x00,0);
		AT25_CS_HIGH(ee_io_info.A0_CS);
		
		if (data != u8data[i])       //�ж�д��ֵ�Ƿ����Ѵ�ֵ��ͬ
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
			for (j=0;j<100;j++)          //�������������10������
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
*Description:       25 95xx ҳ��
*Aunthor:			������
*History:
*
*--------------------------
*Input:				saddr ��ʼ��ַ  page д����ҳ    pagesize ҳ��С
*Output:            *u8data ����
*Return:			SUCCESS �ɹ� ERR ʧ��
*Others:			ע�⣡��  ��C�ʺ� 25080 160 320 640 128 256
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
*Description:       25 95xx ҳ
*Aunthor:			������
*History:
*
*--------------------------
*Input:				saddr ��ʼ��ַ  page д����ҳ    pagesize ҳ��С
*Output:            *u8data ����
*Return:			SUCCESS �ɹ� ERR ʧ��
*Others:			ע�⣡��  ��C�ʺ� 25080 160 320 640 128 256
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
			for (j=0;j<100;j++)    //���������Լ20������
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
			AT25_CS_LOW(ee_io_info.A0_CS);				 //ʵ��֤������Ӷ�����
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