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
	socketpair(AF_UNIX, SOCK_STREAM, 0, pipe);
	int pid = fork();
	if (pid == 0) {
		close(pipe[0]);
		char * piplestr = malloc(100);
		char * modestr = malloc(2);
		char * pipestr = malloc(3);
		snprintf(piplestr, 100, "%d", pipe[1]);
		snprintf(modestr, 2, "%d", MODE);
		snprintf(pipestr, 2, "%d", pipe[1]);
		execl("./myopenfile", "myopenfile", pipestr, argv[1], modestr, (char *)NULL);
		err_quite("child return ,so got error");
	} else if (pid < 0) {
		err_quite("fork error!");
	} else {
		close(pipe[1]);
		printf("waiting for child open file!\n");
		waitpid(pid, &childstatus, 0);
		printf("wake up and WIFEXITED(childstatus)=%d\n", WIFEXITED(childstatus));
		if (WIFEXITED(childstatus) == 0) {
			err_quite("child did not terminal！");
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
				break;
			} else {
				buf[MAX_LINE] = 0;
				printf("read content: %s", buf);
			}
		}
		exit(0);
	}
}

void readfd(int pipefd, int *fd) {
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	char *iobuf[2];
	struct iovec iov[1];
	iov[0].iov_base = iobuf;
	iov[0].iov_len = 2;
	struct msghdr rmsghdr;
	struct cmsghdr *cmsghdr;
	bzero(&rmsghdr, sizeof(rmsghdr));
	bzero(&cmsghdr, sizeof(struct cmsghdr));
	//初始化消息头
	rmsghdr.msg_iov = iov;
	rmsghdr.msg_iovlen = 1;
	rmsghdr.msg_name = NULL;
	rmsghdr.msg_namelen = 0;
	rmsghdr.msg_control = control_un.control; //注意为什么这里如果直接：malloc(CMSG_SPACE(sizeof(int))),就无法获取控制消息呢！
	rmsghdr.msg_controllen = CMSG_SPACE(sizeof(int));
	if (recvmsg(pipefd, &rmsghdr, 0) < 0) {
		err_quite("sendmsg error");
	}
	cmsghdr = CMSG_FIRSTHDR(&rmsghdr);
	*fd = *(int *) CMSG_DATA(cmsghdr);
	printf("get fd: %d\n", *fd);
}
