#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

}

#pragma import(__use_no_semihosting_swi)  
#pragma import(_main_redirection)  
  
  
const char __stdin_name[150];  
const char __stdout_name[150];  
const char __stderr_name[150];  
typedef int FILEHANDLE;  
  
//��д��׼�⺯������ʱprintf��fopen��fclose���ļ�������������ʱ�ͻ���������д��������Щ��д����ֻ�Ǽ����򵥵����ӣ���û����д���е��ļ���������  
  
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
