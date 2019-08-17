#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//系统中断分组设置化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/10
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}

#pragma import(__use_no_semihosting_swi)  
#pragma import(_main_redirection)  
  
  
const char __stdin_name[150];  
const char __stdout_name[150];  
const char __stderr_name[150];  
typedef int FILEHANDLE;  
  
//重写标准库函数，这时printf、fopen、fclose等文件操作函数运行时就会调用你的重写函数，这些重写函数只是几个简单的例子，并没有重写所有的文件操作函数  
  
FILEHANDLE _sys_open(const char *name, int openmode)  
{  
    return 0;  
}  
  
int _sys_close(FILEHANDLE fh)  
{  
    return 0;  
}  
  
int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)  
{  
    return 0;  
}  
  
int _sys_read(FILEHANDLE fh, unsigned char*buf, unsigned len, int mode)  
{  
    return 0;  
}  
  
int _sys_istty(FILEHANDLE fh)  
{  
    return 0;  
}  
  
int _sys_seek(FILEHANDLE fh, long pos)  
{  
    return 0;  
}  
  
int _sys_ensure(FILEHANDLE fh)  
{  
    return 0;  
}  
  
long _sys_flen(FILEHANDLE fh)  
{  
    return 0;  
}  
  
int _sys_tmpnam(char *name, int fileno, unsigned maxlength)  
{  
    return 0;  
}  
  

int remove(const char *filename)  
{  
    return 0;  
}  
  
char *_sys_command_string(char *cmd, int len)  
{  
    return 0;  
}  
  
int clock(void)  
{  
    return 0;  
}  
