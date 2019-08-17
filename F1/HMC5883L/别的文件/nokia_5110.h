#include "system.h"

#define 		CLK_5110_0 				(GPIOA -> BRR = 1 << 3)
#define 		CLK_5110_1 				(GPIOA -> BSRR = 1 << 3)
#define 		DIN_5110_0 				(GPIOA -> BRR = 1 << 4)
#define 		DIN_5110_1 				(GPIOA -> BSRR = 1 << 4)
#define 		DC_5110_0 				(GPIOA -> BRR = 1 << 5)
#define 		DC_5110_1 				(GPIOA -> BSRR = 1 << 5)
#define 		CE_5110_0 				(GPIOA -> BRR = 1 << 6)
#define 		CE_5110_1 				(GPIOA -> BSRR = 1 << 6)
#define 		RST_5110_0 				(GPIOA -> BRR = 1 << 7)
#define 		RST_5110_1 				(GPIOA -> BSRR = 1 << 7)

void LCD_init(void);
void LCD_clear(void);
void LCD_move_chinese_string(unsigned char X, unsigned char Y, unsigned char T); 
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);
void chinese_string(unsigned char X, unsigned char Y, unsigned char T);                 
void LCD_write_char(unsigned char c);
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                  unsigned char Pix_x,unsigned char Pix_y);
void LCD_write_byte(unsigned char dat, unsigned char dc);
void LCD_set_XY(unsigned char X, unsigned char Y);
void delay_1us(void);                 

