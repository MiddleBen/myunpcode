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
#include"../tcputil.h"
#include<poll.h>
#define MAX_FD 2
#define MAX_LINE 5
void error_quit(char *msg) {
	printf("line %d error: %s, errno: %s\n", __LINE__, msg, strerror(errno));
	exit(1);
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
	struct pollfd pfds[MAX_FD];
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
	pfds[0].fd = sockfd;
	pfds[0].events = POLLRDNORM;
	int temp = 1;
	for (; temp < MAX_FD; temp++) {
		pfds[temp].fd = -1;
	}
	int nfds = 0;
	for (;;) {
		int ready = 0;
		if((ready = poll(pfds, nfds + 1, -1)) < 0) {
			error_quit("poll error!");
		} else {
			printf("poll wake up!\n");
		}
		if (pfds[0].revents && POLLRDNORM) {
			int newfd;
			int len = sizeof(clientAddr);
			if ((newfd = accept(sockfd, (struct sockaddr *) &clientAddr, &len))
					< 0) {
				printf("new connection but accept error!");
			}
			printf("new connection! fd is %d\n", newfd);
			int i;
			for (i = 1; i < MAX_FD; i++) {
				if (pfds[i].fd == -1) {
					pfds[i].fd = newfd;
					pfds[i].events = POLLRDNORM;
					break;
				}
			}
			if (i >= MAX_FD) { // 如果大于最大连接数了
				error_quit("max connections reach!");
			}
			if (i > nfds) {
				nfds = i;
			}
			if ((--ready) == 0) {
				printf("poll go to wait1!\n");
				continue;
			}
		};
		int i;
		for (i = 1; i < MAX_FD; i++) {
			int curfd = pfds[i].fd;
			if (curfd != -1) {
				if (pfds[i].revents && POLLRDNORM) {
					printf("curfd: %d reading..\n", curfd);
					bool eofflag = false;
					int rall =ReadLine(curfd, buf, MAX_LINE);
					printf("read: %d bytes\n", rall);
					if (rall == 0) { //eof or err;
						close(curfd);
						pfds[i].fd = -1;
						continue;
					}
					if (write(curfd, buf, strlen(buf)) != strlen(buf)) {
						error_quit("simple write error!");
					} else {
						printf("write %s, %d bytes\n", buf, (int) strlen(buf));
					}
					if ((--ready) == 0) {
						printf("poll go to wait2!\n");
						break;
					}
				}
			};
		};
	}
}
