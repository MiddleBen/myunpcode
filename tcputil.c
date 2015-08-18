/*
 * tcputil.cpp
 *
 *  Created on: 2015年7月19日
 *      Author: lhm
 */
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<errno.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
void
Close(int fd)
{
	if (close(fd) == -1)
		err_quite("close error");
}
int Tcp_listen(const char* host, const char *serv, char *backlog) {
	const int on = 1;
	int listenfd = -1, n;
	struct addrinfo hint, *saveres, *res;
	bzero(&hint, sizeof(struct addrinfo));
	hint.ai_family = AF_INET;
	hint.ai_flags = 1;
	hint.ai_socktype = SOCK_STREAM;
	if ((n = getaddrinfo(host, serv, &hint, &res)) != 0) {
		err_quite(gai_strerror(n));
	}
	saveres = res;
	do {
		listenfd = socket(saveres->ai_family, saveres->ai_socktype, saveres->ai_protocol);
		if (listenfd < 0) {
			continue;
		}
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, saveres->ai_addr, saveres->ai_addrlen) == 0) {
			char buf[1000];
			struct sockaddr_in *inaddr = (struct sockaddr_in *)saveres->ai_addr;
			printf("bind addr= %s, port = %d\n", inet_ntop(AF_INET, &(inaddr->sin_addr), buf, 1000), ntohs(inaddr->sin_port));
			break; //success
		}
		saveres = saveres->ai_next;
		Close(listenfd); //bind error, close and try next one;
	} while(saveres != NULL);
	if (saveres == NULL) {
		err_quite("could not bind\n");
	}
	if (backlog != NULL) {
		listen(listenfd, atoi(backlog));
	} else {
		listen(listenfd, 10);
	}
	freeaddrinfo(res);
	return listenfd;
}

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int		n;

again:
	if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			err_quite("accept error");
	}
	return(n);
}

static char readlineBuf[1024];
static char *ptr;
static int readCount = 0;
int my_read(int fd) {
	again:
	if (readCount == 0) {
		bzero(readlineBuf, 1024);
		if ((readCount = read(fd, readlineBuf, 1024)) == 0) {
			if (errno == EINTR) {
				goto again;
			}
			printf("system call read return 0 \n");
			return 0;
		} else if (readCount < 0) {
			err_quite("read line error !\n");
		}
		ptr = readlineBuf;
		return 1;
	} else {
		readCount --;
		ptr ++;
		return 1;
	}
}

//线程不安全
/**
 * 读取一行，最后以0结束的字符串.
 * 如果读了len-2个字节还没有换行符，返回后面自己加换行符，剩余的后面再读取。
 */
int ReadLine(int fd, char *buf, size_t len) {
	int i = 0, n = 0;
	char * temp = buf;
	for (n = 0; n < len - 2; n++) {
		if (readCount == 0) {
			int ret = my_read(fd);
			if (ret == 0) { //eof
				printf("ret == 0\n");
				*temp = 0;
				return n;
			}
		}
		readCount --;
		*temp = *ptr ++;
		if (*temp == '\n') {
			*(++ temp) = 0;
			return n + 1;
		} else {
			temp ++;
		}
	}
	*temp++ = '\n';
	*temp = 0;
	return n + 1;
}

-----------------------------4057237995973262705082722
Content-Disposition: form-data; name="mode"

formupload