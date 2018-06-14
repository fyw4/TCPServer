/*****************************************************************************/
//  ??????   ??sharefun.h
//  ????     ?? ctrl??????????¡§¡ã
//  ?¨¢????     ?? sunyifeng
//  ????¨¤????? ?? ?????  2017-05-10
/*****************************************************************************/


#ifndef __SHAREFUN_H__
#define __SHAREFUN_H__

#define VOS_OK 0
#define VOS_ERR 1

#define VOS_YES   1
#define VOS_NO    0
#define VOS_OVER  -1
#define VOS_MESSAGE_NULL 2

typedef struct UDP_Server_Thread_Para
{
    int len;
    void *pData;
}TCP_SERVER_THREAD_PARA;


typedef union
{
    unsigned short int val_dec;
    unsigned char val_chr[2];
}DATA_DOUBLE_SEP;

typedef union
{
    unsigned int val_dec;
    unsigned char val_chr[4];
}DATA_FOUR_SEP;

typedef enum
{
	datapackage_version = 10 //Êý¾Ý¿é°æ±¾ºÅ
}VERSION;


unsigned char VOS_func_check(unsigned char *buff, unsigned int size);//¦Ì£¤¡Á??¨²o¨ªD¡ê?¨¦

unsigned char VOS_datacompare(unsigned char *array, unsigned char *tranf, unsigned char poit, unsigned int dimen);//¨ºy?Y¡ã¨¹????¨º??t??¡À¨¨

void VOS_timeupdata(unsigned char *array, unsigned int poit);//¨º¡À??¡À¨º??

unsigned char VOS_get_bit(void* ptr,  unsigned short int byteoffset, unsigned char bitoffset); //?¨¢¨¨???

void VOS_set_bit(void* ptr,  unsigned short int byteoffset, unsigned char bitoffset, unsigned char value); //¨¦¨¨????

unsigned short VOS_get_word(void* ptr, unsigned short int byteoffset); //?¨¢¨¨?¡Á?

unsigned long VOS_get_dword(void* ptr, unsigned short int byteoffset); //?¨¢¨¨?4¡Á??¨²

unsigned char VOS_get_bit_7and6(unsigned char byte); //¨¨?¡Á??¨²7?¡é6??

unsigned char VOS_set_bit_7and6(unsigned char *buffer, unsigned short int byte, unsigned char value); //¨¦¨¨??¡Á??¨²7?¡é6??

unsigned char VOS_get_4bit(unsigned short int bit, unsigned char byte); //¨¨?¡Á??¨²???????¨°¦Ì¨ª????

unsigned char VOS_set_4bit(unsigned char *buffer, unsigned short int byte, unsigned char value_high_4bit, unsigned char value_low_4bit); //¨¨?¡Á??¨²???????¨°¦Ì¨ª????

int VOS_calcByte(unsigned int i, unsigned char c);

int VOS_CRC16(unsigned char *p, unsigned int i);

void VOS_value_sep (unsigned char *p, unsigned int i);

unsigned char VOS_str_add (unsigned char *to, unsigned char *from, unsigned int num, unsigned count);

int VOS_IsWlanON();

int VOS_PRINTF_ENABLE();

int VOS_PRINTF_DISABLE();

void VOS_PRINTF_LOG(char* msg);

void VOS_PRINTF(char *fmt, ...);

//int VOS_PRINTF(char *pMessage);

int VOS_SendByUDP(char *psendIP, int isendPort, void *pMessage, int iMessageLen);

void VOS_FREE(void *pData);

int VOS_PrintData(void *pMessage, int iMessageLen);

int VOS_check_addsum(unsigned char *pData, int len, unsigned char *pCheckNUM);

int VOS_CALCULATE_GUZHANG_NUMBER(char* p,int a);


#endif //__SHAREFUN_H__


