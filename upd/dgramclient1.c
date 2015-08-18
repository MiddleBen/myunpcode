/*
 * dgramserver1.c
 *
 *  Created on: 2015年8月16日
 *      Author: lhm
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include<errno.h>
#define MAX_LINE 1024
#ifndef eq
#define eq
void err_quite(const char *msg) {
	printf("line %d error: %s, errno: %s\n", __LINE__, msg, strerror(errno));
	exit(1);
}
#endif
int main(int argc, char ** argv) {
	if (argc != 2) {
		err_quite("usage ./server <ip> <port>");
	}

	char buf[] = "this is my first udp\n";
	char *rtbuf = malloc(MAX_LINE);
	int dgsockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[1]));
	int addr_len = sizeof(struct sockaddr_in);
	int ret, i;
	for (i = 0; i < 10000; i++) {
		if ((ret = sendto(dgsockfd, buf, sizeof(buf), 0,
				(struct sockaddr *) &serveraddr, sizeof(serveraddr))) < 0) {
			err_quite("sendto error");
		}
	}
}

