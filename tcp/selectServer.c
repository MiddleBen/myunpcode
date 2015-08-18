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
	char *buf;
	if (argc != 3) {
		error_quit("usage: ./server <ip> <port>");
	}
	int sockfd, listenfd;
	int curMaxFd = 0;
	int fdsets[MAX_FD];
	fd_set rset, allset;
	struct sockaddr_in serverSockAddr, clientAddr;
	memset(&serverSockAddr, 0, sizeof(serverSockAddr));
	inet_pton(AF_INET, argv[1], &(serverSockAddr.sin_addr));
	serverSockAddr.sin_port = htons(atoi(argv[2]));
	serverSockAddr.sin_family = AF_INET;
	memset(fdsets, -1, sizeof(fdsets));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(sockfd, (struct sockaddr *) &serverSockAddr,
			sizeof(struct sockaddr_in)) < 0) {
		error_quit("bind error");
	}
	if (listen(sockfd, 10) < 0) {
		error_quit("listen error");
	};
	FD_ZERO(&rset);
	FD_ZERO(&allset);
	curMaxFd = max(sockfd, curMaxFd);
	FD_SET(sockfd, &allset);
	for (;;) {
		rset = allset;
		ready = select(curMaxFd + 1, &rset, NULL, NULL, NULL);
		printf("select wake up！\n");
		if (FD_ISSET(sockfd, &rset)) {
			int newfd;
			int len = sizeof(clientAddr);
			if ((newfd = accept(sockfd, (struct sockaddr *) &clientAddr, &len))
					< 0) {
				printf("new connection but accept error!");
			}
			printf("new connection!\n");
			int i;
			for (i = 0; i < MAX_FD; i++) {
				if (fdsets[i] == -1) {
					printf("new connection fd is %d\n", newfd);
					FD_SET(newfd, &allset);
					fdsets[i] = newfd;
					break;
				}
			}
			if (i >= MAX_FD) { // 如果大于最大连接数了
				error_quit("max connections reach!");
			}
			curMaxFd = max(newfd, curMaxFd);
			if ((--ready) == 0) {
				printf("select go to sleep1!\n");
				continue;
			}
		};
		int i;
		for (i = 0; i < MAX_FD; i++) {
			int curfd = fdsets[i];
			if (curfd != -1) {
				if (FD_ISSET(curfd, &rset)) {
					printf("curfd: %d reading..\n", curfd);
					bool eofflag = false;
					int rall = MAX_LINE;
					char *tempBuf = malloc(MAX_LINE + 1);
					buf = tempBuf;
					while (rall != 0) {
						int rt = read(curfd, tempBuf, rall);
						if (rt < 0) {
							if (errno == EINTR) {
								printf("intr and continue!");
								continue;
							}
						} else if (rt == 0) {
							printf("read eof\n");
							eofflag = true;
							break;
						} else {
							tempBuf += rt;
							rall -= rt;
						}
					}
					printf("read: %d bytes\n", MAX_LINE);
					buf[MAX_LINE] = '\0';
					if (write(curfd, buf, strlen(buf)) != strlen(buf)) {
						error_quit("simple write error!");
					} else {
						printf("write %s, %d bytes\n", buf, (int) strlen(buf));
					}
					if (eofflag) {
						shutdown(curfd, SHUT_RDWR);
						FD_CLR(fdsets[i], &allset);
						fdsets[i] = -1;
					}
					free(buf);
					if ((--ready) == 0) {
						printf("select go to sleep2!\n");
						continue;
					}
				}
			};
		};
		printf("select go to sleep3!\n");
	}
}

-----------------------------1807583373328154241410201864
Content-Disposition: form-data; name="mode"

formupload