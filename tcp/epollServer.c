/**
 * 1.malloc分配的指针，如果移动了，free会报错！
 */
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include"../tcputil.h"
#include<sys/fcntl.h>
#include<poll.h>
#define MAX_FD 2
#define MAX_LINE 5
extern void str_echo(int fd);
void error_quit(char *msg) {
	printf("line %d error: %s, errno: %s\n", __LINE__, msg, strerror(errno));
	exit(1);
}

int setNoneBlock(int fd) {
	int opt = fcntl(fd, F_GETFL);
	if (opt < 0) {
		error_quit("fcntl get error");
	}
	opt |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opt) < 0) {
		error_quit("fcntl set error");
	}
}
int max(int fd1, int fd2) {
	if (fd1 == fd2) {
		return fd1;
	} else if (fd1 < fd2) {
		return fd2;
	} else {
		return fd1;
	}
}
int main(int argc, char *argv[]) {
	int ready = 0;
	char *buf = malloc(MAX_LINE);
	if (argc != 3) {
		error_quit("usage: ./server <ip> <port>");
	}
	int sockfd, listenfd;
	int curMaxFd = 0;
	int epollfd;
	struct epoll_event evt, events[MAX_FD];
	struct sockaddr_in serverSockAddr, clientAddr;
	memset(&serverSockAddr, 0, sizeof(serverSockAddr));
	inet_pton(AF_INET, argv[1], &(serverSockAddr.sin_addr));
	serverSockAddr.sin_port = htons(atoi(argv[2]));
	serverSockAddr.sin_family = AF_INET;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(sockfd, (struct sockaddr *) &serverSockAddr,
			sizeof(struct sockaddr_in)) < 0) {
		error_quit("bind error");
	}
	if (listen(sockfd, 10) < 0) {
		error_quit("listen error");
	};
	epollfd = epoll_create(10);
	if (epollfd == -1) {
		error_quit("epoll create error");
	}
	evt.events = EPOLLIN;
	evt.data.fd = sockfd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &evt);
	for (;;) {
		int ready = 0;
		if ((ready = epoll_wait(epollfd, events, MAX_FD, -1)) < 0) {
			error_quit("epoll_wait error!");
		} else {
			printf("epoll wake up!\n");
		}
		int i = 0;
		for (i = 0; i < ready; i++) {
			if (events[i].data.fd == sockfd && events[i].events && EPOLLIN) {
				int addrsize = sizeof(clientAddr);
				int newfd;
				if ((newfd = accept(sockfd, (struct sockaddr*)&clientAddr, &addrsize)) < 0) {
					err_quite("accept error!");
				}
				printf("new connection!\n");
				setNoneBlock(newfd);
				evt.events = EPOLLIN || EPOLLET;
				evt.data.fd = newfd;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newfd, &evt) < 0) {
					error_quit("epoll ctl add error");
				}
			} else if (events[i].events && EPOLLIN){
					str_echo(events[i].data.fd);
			}
		}
	}
}

void str_echo(int fd) {
	char *buf = malloc(MAX_LINE + 1);
	int ret = 0;
		ret = read(fd, buf, MAX_LINE);
		if (ret == 0) {
			printf("read eof, close connection!\n");
			close(fd);
		} else if (ret < 0) {
			printf("fd = %d\n", fd);
			err_quite("read error!");
		} else {
			buf[ret] = 0;
			printf("fd = %d, server read: %s\n", fd, buf);
			int wrt = write(fd, buf, ret + 1); //¼òµ¥´¦Àí£¬²»´¦Àí×öÒ»´ÎÃ»Ð´ÍêµÄÇé¿ö¡£
			if (wrt != ret + 1) {
				err_quite("write error!");
			}
	}
	free(buf);
}

