#include "oledfont.h"
#include "oled.h"

#define OLED_ADD 0x78
static void oled_cmd(uint8_t i2c_cmd)
{
	uint8_t *cmd;
	cmd = &i2c_cmd;
	HAL_I2C_Mem_Write(&hi2c2, OLED_ADD, 0x00,
					  I2C_MEMADD_SIZE_8BIT, cmd, 1,
					  100);
}
static void oled_data(uint8_t i2c_data)
{
	uint8_t *data;
	data = &i2c_data;
	HAL_I2C_Mem_Write(&hi2c2, OLED_ADD, 0x40,
					  I2C_MEMADD_SIZE_8BIT, data, 1,
					  100);
}
static void oled_origin(uint8_t x, uint8_t y)
{
	oled_cmd(0xb0 + y);
	oled_cmd(((x & 0xf0) >> 4) | 0x10);
	oled_cmd((x & 0x0f) | 0x01);
}

void oled_clear()
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		oled_cmd(0xb0 + i);
		oled_cmd(0x00);
		oled_cmd(0x10);
		for (n = 0; n < 128; n++)
			oled_data(0);
	}
}

void oled_full(uint8_t data) // 0xff
{
	uint8_t m, n;
	for (m = 0; m < 8; m++)
	{
		oled_cmd(0xb0 + m); // page0-page1
		oled_cmd(0x00);		// low column start address
		oled_cmd(0x10);		// high column start address
		for (n = 0; n < 128; n++)
		{
			oled_data(data);
		}
	}
}
void oled_init(void)
{
	HAL_Delay(100);

	oled_cmd(0xAE); // display off
	oled_cmd(0x20); // Set Memory Addressing Mode
	oled_cmd(0x10); // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	oled_cmd(0xb0); // Set Page Start Address for Page Addressing Mode,0-7
	oled_cmd(0xc8); // Set COM Output Scan Direction
	oled_cmd(0x00); //---set low column address
	oled_cmd(0x10); //---set high column address
	oled_cmd(0x40); //--set start line address
	oled_cmd(0x81); //--set contrast control register
	oled_cmd(0xff); //áá?èμ÷?ú 0x00~0xff
	oled_cmd(0xa1); //--set segment re-map 0 to 127
	oled_cmd(0xa6); //--set normal display
	oled_cmd(0xa8); //--set multiplex ratio(1 to 64)
	oled_cmd(0x3F); //
	oled_cmd(0xa4); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	oled_cmd(0xd3); //-set display offset
	oled_cmd(0x00); //-not offset
	oled_cmd(0xd5); //--set display clock divide ratio/oscillator frequency
	oled_cmd(0xf0); //--set divide ratio
	oled_cmd(0xd9); //--set pre-charge period
	oled_cmd(0x22); //
	oled_cmd(0xda); //--set com pins hardware configuration
	oled_cmd(0x12);
	oled_cmd(0xdb); //--set vcomh
	oled_cmd(0x20); // 0x20,0.77xVcc
	oled_cmd(0x8d); //--set DC-DC enable
	oled_cmd(0x14); //
	oled_cmd(0xaf); //--turn on oled panel

	oled_full(0x00);
}

//在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 16/12
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
	unsigned char c = 0, i = 0;
	c = chr - ' '; //得到偏移后的值
	if (x > 128 - 1)
	{
		x = 0;
		y = y + 2;
	}
	if (Char_Size == 16)
	{
		oled_origin(x, y);
		for (i = 0; i < 8; i++)
			oled_data(F8X16[c * 16 + i]);
		oled_origin(x, y + 1);
		for (i = 0; i < 8; i++)
			oled_data(F8X16[c * 16 + i + 8]);
	}
	else
	{
		oled_origin(x, y);
		for (i = 0; i < 6; i++)
			oled_data(F6x8[c][i]);
	}
}

void oled_show_string(uint8_t x, uint8_t y, char ch[], uint8_t TextSize)
{
	uint8_t c = 0, i = 0, j = 0;
	switch (TextSize)
	{
	case 1:
	{
		while (ch[j] != '\0')
		{
			c = ch[j] - 32;
			if (x > 126)
			{
				x = 0;
				y++;
			}
			oled_origin(x, y);
			for (i = 0; i < 6; i++)
				oled_data(F6x8[c][i]);
			x += 6;
			j++;
		}
	}
	break;
	case 2:
	{
		while (ch[j] != '\0')
		{
			c = ch[j] - 32;
			if (x > 120)
			{
				x = 0;
				y++;
			}
			oled_origin(x, y);
			for (i = 0; i < 8; i++)
				oled_data(F8X16[c * 16 + i]);
			oled_origin(x, y + 1);
			for (i = 0; i < 8; i++)
				oled_data(F8X16[c * 16 + i + 8]);
			x += 8;
			j++;
		}
	}
	break;
	}
}

//显示汉字
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
	uint8_t t, adder = 0;
	oled_origin(x, y);
	for (t = 0; t < 16; t++)
	{
		oled_data(word[2 * no][t]);
		adder += 1;
	}
	oled_origin(x, y + 1);
	for (t = 0; t < 16; t++)
	{
		oled_data(word[2 * no + 1][t]);
		adder += 1;
	}
}

//显示一个字符号串
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size)
{
	uint8_t j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2)
{         	
	uint32_t t,temp;
	uint32_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
}

