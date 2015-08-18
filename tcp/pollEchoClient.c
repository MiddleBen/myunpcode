/*
 * simpleEchoClient.c
 *
 *  Created on: 2015年8月2日
 *      Author: lhm
 */
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/time.h>
#include<poll.h>
/**
 * 1.先发送fin的一方会处以fin_wait（等待终结嘛）,被动关闭方处以close_wait（等待关闭）。
 * 2.shutdown(sockfd, SHUT_WR)，会直接发送fin给对方，但shutdown(sockfd, SHUT_RD)，不会，只会收到对方数据返回reset。
 * （注，无论是ack信息还是数据，还是收到fin，都会返回reset给对方，所以，也不会有time_wait状态。
 */
#define MAX_BUF 1024
#define MAX_FD 64
#ifndef eq
#define eq
void err_quite(const char *msg) {
	fprintf(stderr, "err: %s, errno: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}
#endif
int max(int fd1, int fd2) {
	if (fd1 == fd2) {
		return fd1;
	} else if(fd1 < fd2) {
		return fd2;
	} else {
		return fd1;
	}
}
void select_cli(int sockfd, FILE *file) {
	int readCount2 = 0;//一次读了多少数据给服务端。
	int writeCount = 0;//一次读了多少数据给服务端。
	struct pollfd pfds[MAX_FD];
	char *buf = malloc(MAX_BUF);
	int inputfd = fileno(file);
	pfds[0].fd = inputfd;
	pfds[0].events = POLLRDNORM;
	pfds[1].fd = sockfd;
	pfds[1].events = POLLRDNORM;
	for (;;) {
		poll(pfds, 2, -1);
		printf("poll wakeup\n");
		if (pfds[0].revents && POLLRDNORM) { //即使可度，也只读一行。。
			if (fgets(buf, MAX_BUF, file) == NULL) {
				shutdown(sockfd, SHUT_WR);
				printf("read end of inputfile \n");
			} else {
				if ((write(sockfd, buf, strlen(buf)) != strlen(buf))) {
					err_quite("write error!\n");
				} else {
					printf("write %d bytes\n", (int)strlen(buf));
					writeCount += (int)strlen(buf);
				}
			}
		}
		if (pfds[1].revents && POLLRDNORM) {
			int n = 0;
			if ((n = ReadLine(sockfd, buf, MAX_BUF)) == 0) {
				printf("read end of socket! \n");
				break;
			} else {
				printf("read %d bytes\n", n);
				readCount2 += n;
			}
			fputs(buf, stdout);
		}
	}
	printf("clint send %d bytes\n", writeCount);
	printf("clint rec %d bytes\n", readCount2);
	if (ferror(stdout)) {
		err_quite("fgets error!\n");
	}
}

int main(int argc, char ** argv) {
	if (argc != 3) {
		err_quite("usage: tcpcli <ipAdddr>");
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) != 1 ) {//atten 用serverAddr.sin_addr.s_addr可以吗？
		err_quite("int_pton error!");
	}
	printf("serverAddr.sin_addr.s_addr = %d\n", serverAddr.sin_addr.s_addr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[2]));//atten
	if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in)) < 0) {
		err_quite("connect error");
	}
	select_cli(sockfd, stdin);
}
