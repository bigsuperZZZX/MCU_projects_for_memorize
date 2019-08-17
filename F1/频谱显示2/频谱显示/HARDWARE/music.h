#ifndef __MUSIC_H
#define __MUSIC_H	

//灯引脚定义，从最左至右
//	B14 A2 A3 | A6 A8 A9 | A13 A11 A12 | B8 B9 B10 | B11 B12 B13 | B5 B6 B7 | C0 C1 C2| C6 C7 C8

#define L11_ON() GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define L12_ON() GPIO_SetBits(GPIOA,GPIO_Pin_2)
#define L13_ON() GPIO_SetBits(GPIOA,GPIO_Pin_3)
#define L21_ON() GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define L22_ON() GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define L23_ON() GPIO_SetBits(GPIOA,GPIO_Pin_9)
#define L31_ON() GPIO_SetBits(GPIOA,GPIO_Pin_13)
#define L32_ON() GPIO_SetBits(GPIOA,GPIO_Pin_11)
#define L33_ON() GPIO_SetBits(GPIOA,GPIO_Pin_12)
#define L41_ON() GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define L42_ON() GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define L43_ON() GPIO_SetBits(GPIOB,GPIO_Pin_10)
#define L51_ON() GPIO_SetBits(GPIOB,GPIO_Pin_11)
#define L52_ON() GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define L53_ON() GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define L61_ON() GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define L62_ON() GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define L63_ON() GPIO_SetBits(GPIOB,GPIO_Pin_7)

#define L71_ON() GPIO_SetBits(GPIOC,GPIO_Pin_0)
#define L72_ON() GPIO_SetBits(GPIOC,GPIO_Pin_1)
#define L73_ON() GPIO_SetBits(GPIOC,GPIO_Pin_2)
#define L81_ON() GPIO_SetBits(GPIOC,GPIO_Pin_6)
#define L82_ON() GPIO_SetBits(GPIOC,GPIO_Pin_7)
#define L83_ON() GPIO_SetBits(GPIOC,GPIO_Pin_8)

#define L11_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define L12_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_2)
#define L13_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_3)
#define L21_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define L22_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define L23_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_9)
#define L31_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_13)
#define L32_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define L33_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define L41_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define L42_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define L43_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_10)
#define L51_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_11)
#define L52_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define L53_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define L61_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define L62_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define L63_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define L71_OFF() GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define L72_OFF() GPIO_ResetBits(GPIOC,GPIO_Pin_1)
#define L73_OFF() GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define L81_OFF() GPIO_ResetBits(GPIOC,GPIO_Pin_6)
#define L82_OFF() GPIO_ResetBits(GPIOC,GPIO_Pin_7)
#define L83_OFF() GPIO_ResetBits(GPIOC,GPIO_Pin_8)

void adc_init(void); //ADC 初始化
void time_init(void);
void FFT(void);
void light_init(void);
void light_show(char led,char act);






#endif


