/**
 *文件描述符进程间传递
 **/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include<errno.h>
#include <fcntl.h>
#define MAX_LINE 1024
#define CMSGLEN CMSG_LEN(sizeof(int))
#define MODE S_IRWXU | S_IRWXG | S_IRWXO
void err_quite(char *msg) {
	fprintf(stderr, "error: %s, %s\n", msg, strerror(errno));
	exit(1);
}
extern void readfd(int pipefd, int *fd);
int main(int argc, char **argv) {
	char *buf = malloc(MAX_LINE + 1);
	if (argc != 2) {
		err_quite("usage: ./fdf <filepath>");
	}
	struct msghdr rmsghdr;
	int childstatus;
	int pipe[2];
	bzero(&rmsghdr, sizeof(struct msghdr));
	int pid = fork();
	socketpair(AF_UNIX, SOCK_STREAM, 0, pipe);
	if (pid == 0) {
		close(pipe[0]);
		char * piplestr = malloc(100);
		char * modestr = malloc(2);
		snprintf(piplestr, 100, "%d", pipe[1]);
		snprintf(modestr, 2, "%d", MODE);
		execl("./myopenfile", "myopenfile", pipe[1], argv[1], modestr, NULL);
		err_quite("child return ,so got error");
	} else if (pid < 0) {
		err_quite("fork error!");
	} else {
		close(pipe[1]);
		printf("waiting for child open file!\n");
		waitpid(pid, &childstatus, 0);
		printf("wake up!\n");
		if (WIFEXITED(childstatus) != true) {
			err_quite("child process return error！");
		}
		int openfd;
		readfd(pipe[0], &openfd);
		int rt = 0;
		for (;;) {
			rt = read(openfd, buf, MAX_LINE);
			if (rt < 0) {
				err_quite("read error");
			} else if (rt == 0) {
				printf("read eof");
			} else {
				buf[MAX_LINE] = 0;
				printf("read content: %s", buf);
			}
		}
	}
}

void readfd(int pipefd, int *fd) {
	char *iobuf[2];
	struct iovec iov[1];
	iov[0].iov_base = iobuf;
	iov[0].iov_len = 2;
	struct msghdr rmsghdr;
	struct cmsghdr *cmsghdr;
	bzero(&rmsghdr, sizeof(rmsghdr));
	bzero(cmsghdr, sizeof(struct cmsghdr));
	//初始化消息头
	rmsghdr.msg_iov = iov;
	rmsghdr.msg_iovlen = 1;
	rmsghdr.msg_name = NULL;
	rmsghdr.msg_namelen = 0;
	rmsghdr.msg_control = cmsghdr;
	rmsghdr.msg_controllen = CMSG_SPACE(sizeof(int));
	//初始化控制消息头
	cmsghdr->cmsg_level = SOL_SOCKET;
	cmsghdr->cmsg_type = SCM_RIGHTS;
	cmsghdr->cmsg_len = CMSGLEN;
	if (recvmsg(pipefd, &rmsghdr, 0) < 0) {
		err_quite("sendmsg error");
	}
	cmsghdr = CMSG_FIRSTHDR(&rmsghdr);
	fd = (int *) CMSG_DATA(cmsghdr);
	printf("get fd: %d\n", *fd);
}
