/*
 * dgramserver1.c
 *
 *  Created on: 2015年8月16日
 *      Author: lhm
 */
#include <arpa/inet.h>
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#define MAX_LINE 1024
#ifndef eq
#define eq
void err_quite(const char *msg) {
	printf("line %d error: %s, errno: %s\n", __LINE__, msg, strerror(errno));
	exit(1);
}
#endif
int main(int argc, char ** argv) {
	if (argc != 3) {
		err_quite("usage ./server <port>");
	}

	char buf[MAX_LINE + 1];
	int dgsockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in clientaddr, serveraddr;
	bzero(&clientaddr, sizeof(clientaddr));
	bzero(&serveraddr, sizeof(serveraddr));
	inet_pton(dgsockfd, argv[1], &serveraddr.sin_addr.s_addr);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	if (bind(dgsockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
		err_quite("bind error!");
	}
	for (;;) {
		int ret;
		int addrlen = sizeof(serveraddr);
		printf("waiting for recv!\n");
		if ((ret = recvfrom(dgsockfd, buf, MAX_LINE, 0,
				(struct sockaddr *) &serveraddr, &addrlen)) < 0) {
			err_quite("recvmsg error");
		}
		buf[ret] = 0;
		printf("recv buf=%s", buf);
	}

}
