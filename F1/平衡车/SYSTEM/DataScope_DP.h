#ifndef __DATA_PRTOCOL_H
#define __DATA_PRTOCOL_H


void DataScope_Get_Channel_Data(unsigned char *buf,float Data,unsigned char Channel);    // 写通道数据至 待发送帧数据缓存区

unsigned char DataScope_Data_Generate(unsigned char *buf,unsigned char Channel_Number);  // 发送帧数据生成函数 
 
 
#endif 



