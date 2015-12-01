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
#include<fcntl.h>
#include"../tcputil.h"
#define MAX_FD 2
#define MAX_LINE 5
#define MAX_BUF_SIZE 10
extern void setnoblock(int fd);
extern void cli_echo(int inputfd, int sockfd);
int max(int fd1, int fd2) {
	if (fd1 == fd2) {
		return fd1;
	} else if (fd1 < fd2) {
		return fd2;
	} else {
		return fd1;
	}
}

int curMaxFd = 0;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		error_quite("usage: ./server <ip> <port>");
	}
	int sockfd;
	fd_set rset, allset;
	struct sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	inet_pton(AF_INET, argv[1], &(clientAddr.sin_addr));
	clientAddr.sin_port = htons(atoi(argv[2]));
	clientAddr.sin_family = AF_INET;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	Connect(sockfd, (struct sockaddr*) &clientAddr, sizeof(struct sockaddr_in));
	cli_echo(STDIN_FILENO, sockfd);
}

void cli_echo(int inputfd, int sockfd) {
	char gobuf[MAX_BUF_SIZE];
	char backbuf[MAX_BUF_SIZE];
	char *go_send_point = gobuf, *go_input_point = gobuf;
	char *back_rec_point = backbuf, *back_put_point = backbuf;
	fd_set readfds, writefds;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	setnoblock(inputfd);
	setnoblock(sockfd);
	setnoblock(STDOUT_FILENO);
	for (;;) {
		FD_ZERO(&readfds);//一定要记得执行zero清理
		FD_ZERO(&writefds);
		FD_SET(inputfd, &readfds);
		FD_SET(sockfd, &readfds);
		if (back_rec_point != back_put_point) {
			FD_SET(STDOUT_FILENO, &writefds);
		}
		if (go_send_point != go_input_point) {
			FD_SET(sockfd, &writefds);
		}
		curMaxFd = max(max(inputfd, sockfd), STDOUT_FILENO) + 1;
		printf("select waiting ..\n");
		select(curMaxFd, &readfds, &writefds, NULL, NULL);
		if (FD_ISSET(inputfd, &readfds)) {
			printf("inputfd wakeup ..\n");
			int canread = 1;
			if (&gobuf[MAX_BUF_SIZE] == go_input_point) { //unp中如果输入到达了buf最大值如何处理(&gobuf[MAX_BUF_SIZE] == go_input_point)？
				if (go_input_point != go_send_point) {
					canread = 0;
				} else {
					go_send_point = gobuf;
					go_input_point = gobuf;
				}
			}
			if (canread) {
				int rn = 0;
				rn = read(inputfd, go_input_point,
						(&gobuf[MAX_BUF_SIZE] - go_input_point));
				if (rn < 0) {
					if (errno == EWOULDBLOCK) {
						printf("input EWOULDBLOCK happen! error:%s\n", strerror(errno));
					} else {
						error_quite("read error");
					}
				} else if (rn == 0) {
					printf("read eof\n");
					shutdown(sockfd, SHUT_WR);
				} else {
					go_input_point += rn;
				}
			}
		}
		if (FD_ISSET(sockfd, &readfds)) { //sock可读
			printf("sockfd read wakeup ..\n");
			int canread = 1;
			if (&backbuf[MAX_BUF_SIZE ] == back_rec_point) {
				if (back_rec_point != back_put_point) {
					canread = 0;
				} else {
					back_rec_point = backbuf;
					back_put_point = backbuf;
				}
			}
			if (canread) {
				int rn = 0;
				rn = read(sockfd, back_rec_point,
						(&backbuf[MAX_BUF_SIZE] - back_rec_point));
				if (rn < 0) {
					if (errno == EWOULDBLOCK) {
						printf("sock read EWOULDBLOCK happen! %s\n", strerror(errno));
					} else {
						error_quite("read error");
					}
				} else if (rn == 0) {
					printf("sock read eof\n");
					shutdown(sockfd, SHUT_RD);
					break;
				} else {
					back_rec_point += rn;
				}
			}
		}
		if (FD_ISSET(sockfd, &writefds)) { // sock可写
				printf("sockfd write wakeup ..\n");
				int wn = 0;
				wn = write(sockfd, go_send_point,
						(go_input_point - go_send_point));
				if (wn < 0) {
					if (errno == EWOULDBLOCK) {
						printf("write sock EWOULDBLOCK happen! error : %s\n", strerror(errno));
					}
				} else {
					go_send_point += wn;
					if (go_input_point == &gobuf[MAX_BUF_SIZE] && go_send_point == go_input_point) {
						go_send_point = go_input_point = gobuf;
					}
				}
		}
		if (FD_ISSET(STDOUT_FILENO, &writefds)) { // 标准输出可写。
				printf("std write wakeup ..\n");
				int wn = write(STDOUT_FILENO, back_put_point, (back_rec_point - back_put_point));
				if (wn < 0) {
					if (errno == EWOULDBLOCK) {
						printf("stdout write EWOULDBLOCK happen! error: %s\n", strerror(errno));
					}
				} else {
					back_put_point += wn;
				}
				if (back_rec_point == back_put_point && back_rec_point == &backbuf[MAX_BUF_SIZE]) {
					back_rec_point = back_put_point = backbuf;
				}
		}
	}

}

void setnoblock(int fd) {
	int oldctl = fcntl(fd, F_GETFL);
	oldctl |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, oldctl) < 0) {
		error_quite("set nonblock error!");
	}
}
