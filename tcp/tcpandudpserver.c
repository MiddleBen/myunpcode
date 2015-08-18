/*
 ============================================================================
 Name        : test.c
 Author      : ben.li
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include<sys/types.h>

#define MAX_LINE 1024

int addr_in_len = sizeof(struct sockaddr_in);

extern void clearandsetserveraddr(struct sockaddr_in *serveraddr, char ** argv);

void err_quite(char *msg) {
	fprintf(stderr, "error: %s\n", msg);
	exit(1);
}

int max(int fd1, int fd2) {
	if (fd1 == fd2) {
		return fd2;
	} else if (fd2 > fd1) {
		return fd2;
	} else {
		return fd1;
	}
}

int main(int argc, char ** argv) {
	if (argc != 3) {
		err_quite("usage: ./server <ip> <port>");
	}
	struct fd_set rfds;
	int tcpsockfd, udpsockfd, maxfd;
	struct sockaddr_in serveraddr, clientaddr;
	tcpsockfd = socket(AF_INET, SOCK_STREAM, 0);
	clearandsetserveraddr(&serveraddr, argv);
	udpsockfd = socket(AF_INET, SOCK_DGRAM, 0);//试试不适用socket reuse会不会有问题。
	if (bind(tcpsockfd, (struct sockaddr *)&serveraddr, addr_in_len) < 0) {
		err_quite("tcp bind eror");
	}
	if (listen(tcpsockfd, 10) < 0) {
		err_quite("listen error");
	}
	clearandsetserveraddr(&serveraddr, argv);
	if (bind(udpsockfd, (struct sockaddr *)&serveraddr, addr_in_len) < 0) {
			err_quite("udp bind eror");
	}
	maxfd = max(tcpsockfd, udpsockfd);
	for(;;) {
		FD_SET(tcpsockfd, &rfds);
		FD_SET(udpsockfd, &rfds);
		int ready = select(maxfd + 1, &rfds, NULL, NULL, NULL);//等一下试试timeout
		printf("select wake up!\n");
		if (FD_ISSET(tcpsockfd, &rfds)) {
			struct sockaddr_in clientaddr;
			int newfd;
			if ((newfd = accept(tcpsockfd, &clientaddr, addr_in_len)) < 0) {
				err_quite("accept error");
			};
			printf("tcp recv new connection!\n");
			int pid = fork();
			if (pid > 0) {
				close(newfd);
			} else if(pid == 0) {
				close(tcpsockfd);
				str_echo(tcpsockfd);
				close(newfd);
			}
		}
		if (FD_ISSET(udpsockfd, &rfds)) {
			char * buf = malloc(MAX_LINE + 1);
			printf("udp recv new package\n");
			int rb = recvfrom(udpsockfd, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, &addr_in_len); //我猜这样肯定接受不到包，因为地址应该填服务器的才对！
			buf[rb] = 0;
			sendto(udpsockfd, buf, rb + 1, 0, (struct sockaddr *)&clientaddr, &addr_in_len);
			free(buf);
		}
	}
}

void str_echo(int fd) {
	char *buf = malloc(MAX_LINE + 1);
	int ret = 0;
	while((ret = read(fd, buf, MAX_LINE)) > 0) {
		if (ret == 0) {
			printf("read eof, close connection!\n");
			break;
		} else if (ret < 0) {
			err_quite("read error!");
		} else {
			buf[ret] = 0;
			write(ret, buf, ret + 1);//简单处理，不处理做一次没写完的情况。
		}
	}
	free(buf);
}

void clearandsetserveraddr(struct sockaddr_in *serveraddr, char ** argv) {
	bzero(serveraddr, addr_in_len);
	inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_family = AF_INET;
}
