#ifndef __OLED_H_
#define __OLED_H_

#include "main.h"

extern I2C_HandleTypeDef hi2c2;

void oled_full(uint8_t data);
void oled_init(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);
void oled_show_string(uint8_t x, uint8_t y, char ch[], uint8_t TextSize);
void oled_clear(void);
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2);
#endif
