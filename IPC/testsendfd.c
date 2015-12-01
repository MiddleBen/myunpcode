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
#include <sys/un.h>
#include <string.h>
#define CMSGLEN CMSG_LEN(sizeof(int))
#define MODE S_IRWXU | S_IRWXG | S_IRWXO
#define ADDSIZE sizeof(struct sockaddr_un)
void err_quite(char *msg) {
	fprintf(stderr, "error: %s, %s\n", msg, strerror(errno));
	exit(1);
}
extern void sendfd(struct sockaddr *addr, int sentfd);

int main(int argc, char **argv) {
	if (argc != 3) {
		err_quite("usage:　./myopenfile <unpath ><filepath>");
	}
	int fd = open(argv[2], O_RDONLY, MODE);
	if (fd < 0) {
		err_quite("open file error!");
	}
	struct sockaddr_un unaddr;
	memset(&unaddr, 0, ADDSIZE);
	unaddr.sun_family = AF_UNIX;
	strncpy(unaddr.sun_path, argv[1], sizeof(unaddr.sun_path));
	sendfd((struct sockaddr *)&unaddr, fd);
	exit(0);
}

void sendfd(struct sockaddr *addr, int sentfd) {
	int pipefd = socket(AF_UNIX, SOCK_STREAM, 0);
	 if (connect(pipefd, addr, ADDSIZE) < 0) {
	                err_quite("connection error");
	}
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
	smsghdr.msg_name = addr;
	smsghdr.msg_namelen = ADDSIZE;
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
	printf("send msg done!\n");
}

