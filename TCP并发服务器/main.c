#include<stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "sharefun.h"

#define PORT 5000
#define MAX_SIZE FD_SETSIZE //最大可用已连接套接字的个数
#define LISTEN_MAX 5
#define BUF_SIZE 10240

fd_set fds;

static void PrintMesg(int i , char buf[])
{
	printf("fd : %d, msg: %s\n", i , buf);
}

void *TCP_Analyzer(void *pPara)
{
	char Buf[BUF_SIZE] = {0};
	ssize_t Size  = 0;
	int *pConnfd = 0;
	unsigned char *pReceiveData = NULL;
	int ReceiveLen = 0;

    if (NULL == pPara)
    {
        VOS_PRINTF_LOG("TCP_Analyzer: TCP_Analyzer Failed!\n");
        return NULL;
    }

	pConnfd = (int *)pPara;
	if(*pConnfd < 0)
	{
		VOS_PRINTF_LOG("TCP_Analyzer:Get pConnfd Failed \n" );
		return NULL;
	}

	while(1)
	{
		memset(Buf, '\0', sizeof(Buf));
		Size = read(*pConnfd, Buf,sizeof(Buf) );
		if (Size <= 0) //没有接收到数据，关闭描述符，释放在TCPServer申请的空间
		{
			VOS_PRINTF_LOG("TCP_Analyzer:remote client close\n" );
			close(*pConnfd);
			VOS_FREE(pConnfd);
			return NULL;
		}
		else
		{
			printf("TCP_Analyzer:%s,%d\n",Buf,(int)Size);
		}
	}
	close(*pConnfd);
	VOS_FREE(pConnfd);

	return NULL;
}

void *TCPServer()
{
	pthread_t threadID = 0;
	struct sockaddr_in Server;
	struct sockaddr_in Client;
	int Listenfd = 0;
	int *pConnectfd = NULL;
	int i = 0;
	int j = 0;
	int yes = 0;
	int index = 0;
	int Connfd = 0;
	int ret = 0;
	socklen_t len = 0;
	char logMessage[128] = {0};
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	Listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (Listenfd < 0)
	{
		perror("socket");
		exit(1);
	}
	VOS_PRINTF_LOG("TCPServer:create socket success\n");

	setsockopt(Listenfd, SOL_SOCKET, SO_REUSEADDR, &yes ,sizeof(int));// 允许IP地址复用

	bzero(&Server, sizeof(Server));
	Server.sin_family = AF_INET;
	Server.sin_port = htons(PORT );
	Server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(Listenfd, (struct sockaddr*)&Server, sizeof(Server)) < 0)
	{
		perror("bind");
		exit(2);
	}
	VOS_PRINTF_LOG("TCPServer:bind socket success\n");

	if (listen(Listenfd, LISTEN_MAX) < 0)
	{
		perror("listen error");
		exit(3);
	}
	VOS_PRINTF_LOG("TCPServer:listen socket success\n");

	while (1)
	{
		FD_ZERO(&fds);//描述符集合初始化
		FD_SET(Listenfd,&fds);

		struct timeval timeout = {30, 0};

		switch(select(Listenfd + 1, &fds,NULL, NULL, &timeout ))
		{
			case -1:
				memset(logMessage, 0, 128);
	            sprintf(logMessage,"main: create TCP server select fail(%d) %s.\n", errno, strerror(errno));
	            VOS_PRINTF_LOG(logMessage);
				return NULL;

			case 0:
				VOS_PRINTF_LOG("TCPServer:TCP receiving nothing......\n");
				break;

			default:
			{
				if (FD_ISSET(Listenfd, &fds))
				{
					len = sizeof(Client);
					bzero(&Client, len);

					Connfd = accept(Listenfd, (struct sockaddr*)&Client,&len );
					//若有新的连接
					if (Connfd != -1)
					{
						pConnectfd = (int *)malloc(sizeof(int));
		                if (NULL == pConnectfd)
                        {
                            VOS_PRINTF_LOG("TCPServer: pConnectfd malloc Failed.\n");
                            break;
                        }

						memset(pConnectfd,0,sizeof(int));
						*pConnectfd = Connfd;

						VOS_PRINTF_LOG("TCPServer:get a new request!\n");
						printf("TCPServer:the connect fd is %d\n",Connfd);

						ret = pthread_create(&threadID, &attr, TCP_Analyzer, (void *)pConnectfd);
						if(0 != ret)
						{
							VOS_PRINTF_LOG("TCPServer: TCP_Analyzer build Fail!\n");
							FD_CLR(Listenfd, &fds);// 清除 fds中相应的文件描述符
							VOS_FREE(pConnectfd);
							return NULL;
						}

					}
				}
			}
		}
	}
	close(Listenfd);
	FD_CLR(Listenfd, &fds);// 清除 fds中相应的文件描述符
	pthread_attr_destroy(&attr);//线程属性销毁
	VOS_FREE(pConnectfd);
	return NULL;
}

int main()
{
	int ret = 0;
	pthread_t threadID = 0;

    VOS_PRINTF_ENABLE();

	VOS_PRINTF_LOG("Main Init OK!\n");

	ret = pthread_create(&threadID, NULL, TCPServer, NULL);
	if(0 != ret)
	{
		(void)VOS_PRINTF_LOG("TCPServer: TCPServer build Fail!\n");
		return VOS_ERR;
	}

    while(1)
    {
        sleep(10);
    }

	return 0;
}

