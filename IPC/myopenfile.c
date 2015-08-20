/*
 * myopenfile.c
 *
 *  Created on: 2015年8月20日
 *      Author: ben01.li
 */
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#define CMSGLEN CMSG_LEN(sizeof(int))
void err_quite(char *msg) {
	fprintf(stderr, "error: %s, %s\n", msg, strerror(errno));
	exit(1);
}
extern void sendfd(int pipefd, int sentfd);
int main(int argc, char **argv) {
	if (argc != 4) {
		err_quite("usage:　./myopenfile <pipefd> <filepath> <mode>");
	}
	int i = 0;
	for(; i < argc; i++) {
		printf("argv[%d]=%s", i, argv[i]);
	}
	printf("\n");
	int mode = atoi(argv[3]);
	int pipefd = atoi(argv[1]);
	int fd = open(argv[2], mode, 0);
	if (fd < 0) {
		err_quite("open file error!");
	}
	sendfd(pipefd, fd);
	exit(0);
}

void sendfd(int pipefd, int sentfd) {
	char iobuf[2];
	struct iovec iov[1];
	iobuf[0] = 't';
	iobuf[1] = 0;
	iov[0].iov_base = iobuf;
	iov[0].iov_len = 2;
	struct msghdr smsghdr;
	struct cmsghdr cmsghdr;
	bzero(&smsghdr, sizeof(smsghdr));
	bzero(&cmsghdr, sizeof(cmsghdr));
	//初始化消息头
	smsghdr.msg_iov = iov;
	smsghdr.msg_iovlen = 1;
	smsghdr.msg_name = NULL;
	smsghdr.msg_namelen = 0;
	smsghdr.msg_control = &cmsghdr;
	smsghdr.msg_controllen = CMSG_SPACE(sizeof(int));
	//初始化控制消息头
	cmsghdr.cmsg_level = SOL_SOCKET;
	cmsghdr.cmsg_type = SCM_RIGHTS;
	cmsghdr.cmsg_len = CMSGLEN;
	*(int *) CMSG_DATA(&cmsghdr) = sentfd;
	if (sendmsg(pipefd, &smsghdr, 0) < 0) {
		err_quite("sendmsg error");
	}
}

