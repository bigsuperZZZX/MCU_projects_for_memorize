#ifndef _CUMT_IMAGE_H
#define _CUMT_IMAGE_H


void dot_track(uint8_t get_color_cmd , uint8_t color_black_select);
void line_track(void);
uint8_t decode_frame(void);
uint8_t decode_frame_get_color_cmd(void);
unsigned short ReadPoint(unsigned short x,unsigned short y);





#endif