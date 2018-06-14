/*****************************************************************************/
//  ???t??   ￡osharefun.c
//  ??òa     ￡o ctrl°?1?ó?3ìDò
//  ×÷??     ￡o sunyifeng
//  °?±????? ￡? 3?°?  2017-05-10
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>


#include "sharefun.h"
int DEBUG = 0;

void VOS_FREE(void *pData)
{
    if (NULL != pData)
    {
        free(pData);
        pData = NULL;
    }
    return;
}

void VOS_PRINTF_LOG(char* msg)
{
    VOS_PRINTF(msg);
    //VOS_LOG(msg);
    return;
}

void VOS_PRINTF(char *fmt, ...)
{
    va_list ap;
    int d;
    double f;
    char c;
    char *s;
    char flag;
    int x;

    if (0 == DEBUG)
    {
        return;
    }


    va_start(ap, fmt);
    while (*fmt)
    {
        flag = *fmt++;
        if(flag != '%')
        {
            putchar(flag);
            continue;
        }
        flag = *fmt++;//??μ?oóò?ò???
        switch (flag)
        {
            case 's':
                s=va_arg(ap,char*);
                printf("%s",s);
                break;
            case 'd': /* int */
                d = va_arg(ap, int);
                printf("%d", d);
                break;
            case 'f': /* double*/
                f = va_arg(ap,double);
                printf("%f", f);
                break;
            case 'c': /* char*/
                c = (char)va_arg(ap,int);
                printf("%c", c);
                break;
            case 'x': /* 16*/
                x = va_arg(ap,int);
                printf("%x", x);
                break;
            default:
                putchar(flag);
            break;
        }
    }
    va_end(ap);
    return;
}

/*
int VOS_PRINTF(char *pMessage)
{
    if (NULL == pMessage)
    {
        return VOS_ERR;
    }

    printf(pMessage);

    return VOS_OK;
}
*/

int VOS_CALCULATE_GUZHANG_NUMBER(char* p,int a)
{
	int i = 0;
	int j = 0;
	int test = 1;
	int num  = 0;
	int size = a;
	for (i=0;i<size;i++)
	{
		for(j=0;j<8;j++)
		{
			if(test&(*(p+i)))
			{
				num++;
			}
			test = test<<1;
		}
		test = 1;
	}
	return num;
}
int VOS_PRINTF_ENABLE()
{
    DEBUG = 1;
    return 0;
}

int VOS_PRINTF_DISABLE()
{
    DEBUG = 0;
    return 0;
}


int VOS_PrintData(void *pMessage, int iMessageLen)
{
    int i = 0;

    if (NULL == pMessage)
    {
        VOS_PRINTF("SHARE: VOS_PrintData FAIL!  NO Message.\n");
        return VOS_ERR;
    }

    if (0 == iMessageLen)
    {
        VOS_PRINTF("SHARE: VOS_PrintData FAIL!  The Message is NULL.\n");
        return VOS_ERR;
    }

    for(i = 0; i < iMessageLen; i++)
    {
        if (i % 10 == 0)
            printf("    ");
        if (i % 30 == 0)
            printf("\n");
        printf("%02x ", ((unsigned char *)pMessage)[i]);
    }
    printf("\n\n");

    return VOS_OK;

}


int VOS_SendByUDP(char *psendIP, int isendPort, void *pMessage, int iMessageLen)
{
    int i = 0;
    int ret = -1;
    int err = 0;
    int sendsockfd = -1;
    char logMessage[128] = {0};

    struct sockaddr_in addrSendto;


    if (NULL == pMessage)
    {
        VOS_PRINTF_LOG("SHARE: send UDP FAIL!  NO Message.\n");
        return VOS_ERR;
    }

    if (0 == iMessageLen)
    {
        VOS_PRINTF_LOG("SHARE: send UDP FAIL!  The Message is NULL.\n");
        return VOS_ERR;
    }

    if (NULL == psendIP)
    {
        VOS_PRINTF_LOG("SHARE: send UDP FAIL!  NO IP.\n");
        return VOS_ERR;
    }

    if (0 == isendPort)
    {
        VOS_PRINTF_LOG("SHARE: send UDP FAIL!  NO PORT.\n");
        return VOS_ERR;
    }

    sendsockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sendsockfd < 0)
    {
        //VOS_PRINTF("SHARE: Send UDP socket FAIL.\n");
        sprintf(logMessage,"SHARE: create UDP socket fail(%d) %s.\n", errno, strerror(errno));
        VOS_PRINTF_LOG(logMessage);
        return VOS_ERR;
    }

    memset((void*)&addrSendto, 0, sizeof(struct sockaddr_in));
    addrSendto.sin_family = AF_INET;
    addrSendto.sin_port = htons(isendPort);
    inet_pton(AF_INET, psendIP, &addrSendto.sin_addr.s_addr);

    memset(logMessage, 0, 128);
    sprintf(logMessage,"SHARE: UDP send message(len = %d). ip %s, port %d.\n", iMessageLen, psendIP, isendPort);
    VOS_PRINTF_LOG(logMessage);

    (void)VOS_PrintData(pMessage, iMessageLen);

    /*
    for(i = 0; i < iMessageLen; i++)
    {
        if (i % 16 == 0)
            printf("\n");
        printf("%02x ", ((unsigned char *)pMessage)[i]);
    }
    printf("\n");
    */

    ret = sendto(sendsockfd, pMessage, iMessageLen, 0, (struct sockaddr*)&addrSendto, sizeof(struct sockaddr_in));
    if(-1 == ret)
    {
        //VOS_PRINTF("SHARE: send UDP fail.\n");
        sprintf(logMessage,"SHARE: send UDP fail(%d) %s.\n", errno, strerror(errno));
        VOS_PRINTF_LOG(logMessage);
        close(sendsockfd);
        return VOS_ERR;
    }
    else
    {
        sprintf(logMessage,"SHARE: UDP send message(len = %d) OK!\n", iMessageLen);
        VOS_PRINTF_LOG(logMessage);
    }

    close(sendsockfd);
    return VOS_OK;
}


//sharefunction
unsigned char VOS_func_check(unsigned char *buff, unsigned int size)
{
	unsigned char value = 0;
	int i;
	for(i=0;i<size;i++)
	{
		value += ~(*(buff+i));
	}
//	value = (~value)+1;

	return value;
}
/*
 * datacompare()函数说明
 * 功能：字符串比较函数
 * 描述：比较两个指定长度字符串
 */
unsigned char VOS_datacompare(unsigned char *array, unsigned char *tranf, unsigned char poit, unsigned int dimen)
{
	int i;
	for(i = 0; i < dimen ; i++)
	{
		if(*(array+poit+i) != *(tranf+poit+i))
		{
			//printf("VOS_datacompare:%d.%02x.\n",i,*(tranf+poit+i) );
			//*(tranf+poit+i) = *(array+poit+i);
			return 1;
		}
	}
	return 0;
}
/*
 * timeupdata()函数说明
 * 功能：时间统计函数
 * 描述：输出年月日时分秒
 */
void VOS_timeupdata(unsigned char *array, unsigned int poit)
{
	time_t timep;
	struct tm *tpointer;
	time(&timep);
	tpointer=localtime(&timep);
	array[poit] = tpointer->tm_year-100;
	array[poit+1] = tpointer->tm_mon+1;
	array[poit+2] = tpointer->tm_mday;
	array[poit+3] = tpointer->tm_hour;
	array[poit+4] = tpointer->tm_min;
	array[poit+5] = tpointer->tm_sec;

    return;
}
//读取位值
unsigned char VOS_get_bit(void* ptr,  unsigned short int byteoffset, unsigned char bitoffset)
{
    unsigned char value;
    value = 0;
    unsigned char* ptr_temp;
    ptr_temp = (unsigned char*)ptr;
    value =  *(ptr_temp + byteoffset);
    value = (value>>bitoffset) & 1;
    return value;
}
//设置位值
void VOS_set_bit(void* ptr,  unsigned short int byteoffset, unsigned char bitoffset, unsigned char value)
{
    unsigned char* ptr_temp;
    ptr_temp = (unsigned char*)ptr;
    *(ptr_temp + byteoffset) =  (255-(unsigned char)(1<<bitoffset)) & (*(ptr_temp + byteoffset));
    if (value != 0)
    {
        *(ptr_temp + byteoffset) =  ((unsigned char)(1<<bitoffset)) | (*(ptr_temp + byteoffset));
    }

}
//读取字
unsigned short VOS_get_word(void* ptr, unsigned short int byteoffset)
{
    unsigned short value;
    unsigned char * ptr_temp;
    unsigned char * ptr_temp2;
    ptr_temp = (unsigned char*)(&value);
    ptr_temp2 = (unsigned char*)(ptr);

    *(ptr_temp + 0) = *(ptr_temp2 + byteoffset + 0);
    *(ptr_temp + 1) = *(ptr_temp2 + byteoffset + 1);

    return value;
}
//读取4字节
unsigned long VOS_get_dword(void* ptr, unsigned short int byteoffset)
{
    unsigned long value;
    unsigned char * ptr_temp;
    unsigned char * ptr_temp2;
    ptr_temp = (unsigned char*)(&value);
    ptr_temp2 = (unsigned char*)(ptr);

    /* 16字节=1,17字节=2,18字节=3,19字节=4，
     转换的结果为1+2*256+3*65536+4*262144=1048576+196608+512+1=1245697 */
    *(ptr_temp + 0) = *(ptr_temp2 + byteoffset + 0);
    *(ptr_temp + 1) = *(ptr_temp2 + byteoffset + 1);
    *(ptr_temp + 2) = *(ptr_temp2 + byteoffset + 2);
    *(ptr_temp + 3) = *(ptr_temp2 + byteoffset + 3);

    return value;
}
//取字节7、6位
unsigned char VOS_get_bit_7and6(unsigned char byte)
{
	return (byte>>6)&0x03;
}
//设置字节7、6位
unsigned char VOS_set_bit_7and6(unsigned char *buffer, unsigned short int byte, unsigned char value)
{
	*(buffer+byte) = (*(buffer+byte))|(value<<6);
	return *(buffer+byte);
}
//取字节高四位或低四位
unsigned char VOS_get_4bit(unsigned short int bit, unsigned char byte)
{
	return (byte>>bit)&0x0F;
}
//设置字节高四位或低四位
unsigned char VOS_set_4bit(unsigned char *buffer, unsigned short int byte, unsigned char value_high_4bit, unsigned char value_low_4bit)
{
	*(buffer+byte) = (*(buffer+byte))|(value_high_4bit<<4);
	*(buffer+byte) = (*(buffer+byte))|value_low_4bit;
	return *(buffer+byte);
}
int VOS_IsWlanON()
{
	return VOS_YES;
}

int VOS_calcByte(unsigned int crc , unsigned char b)
{
    int i ;
    crc = crc ^ (int)b << 8 ;

    for ( i = 0; i < 8; i++)
    {
        if ((crc & 0x8000) == 0x8000)
            crc = crc << 1 ^ 0x1021;
        else
            crc = crc << 1;
    }
    return crc & 0xffff ;
}

int VOS_CRC16(unsigned char *pBuffer, unsigned int length)
{
    int wCRC16=0;
    int i;
    if (( pBuffer==0 )||( length==0 ))
    {
        return 0;
    }
    for ( i = 0; i < length; i++)
    {
        wCRC16 = VOS_calcByte( wCRC16 , pBuffer[i] );
    }
    return wCRC16;
}

void VOS_value_sep(unsigned char *value, unsigned int data_length)
{
    unsigned int crc_value = VOS_CRC16( value , data_length - 2 ) ;
    DATA_DOUBLE_SEP a ;
    a.val_dec = crc_value ;

    value [ data_length - 2 ] = a.val_chr[ 0 ]  ;
    value [ data_length - 1 ] = a.val_chr[ 1 ]  ;
}

unsigned char VOS_str_add (unsigned char *to, unsigned char *from, unsigned int num, unsigned count)
{
    int  i ;
    for ( i = 0  ; i < count ; i++ )
    {
        *(to+num+i) = *(from+i) ;
    }
    return 0;
}
int VOS_check_addsum(unsigned char *pData, int len, unsigned char *pCheckNUM)
{
    int i = 0;
    unsigned char check = 0;
    for (i = 0; i < len; i++)
    {
        check = check + (unsigned char)(pData[i]);
    }
    *pCheckNUM = check;
    return 0;
}

