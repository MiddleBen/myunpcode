/*
 * socketun.c
 *
 *  Created on: 2015Äê8ÔÂ18ÈÕ
 *      Author: ben01.li
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<errno.h>
#define MAX_LINE 1024
void err_quite(char *msg) {
        fprintf(stderr, "error: %s, %s\n", msg, strerror(errno));
        exit(1);
}

extern void str_echo(int fd);
int main(int argc, char ** argv) {
        if (argc != 2) {
                err_quite("usage: ./server <path>");
        };
        int sockunfd, addrlen;
        struct sockaddr_un socketunaddr, clientaddr;
        addrlen = sizeof(struct sockaddr_un);
        socketunaddr.sun_family = AF_UNIX;
        strncpy(socketunaddr.sun_path, argv[1], sizeof(socketunaddr.sun_path));
        unlink(socketunaddr.sun_path);
        sockunfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (bind(sockunfd, (struct sockaddr *)&socketunaddr, addrlen) < 0){
                err_quite("bind error");
        }
        if (listen(sockunfd, 10) < 0) {
                err_quite("listen error");
        }
        for(;;) {
                int newfd = accept(sockunfd, (struct sockaddr *)&clientaddr, &addrlen);//²»×ö´íÎóÅÐ¶ÏÁË¡£
                printf("new connection!\n");
                str_echo(newfd);
                close(newfd);
        }
}

void str_echo(int fd) {
        char *buf = malloc(MAX_LINE + 1);
        int ret = 0;
        for(;;) {
                ret = read(fd, buf, MAX_LINE);
                if (ret == 0) {
                        printf("read eof, close connection!\n");
                        break;
                } else if (ret < 0) {
                        err_quite("read error!");
                } else {
                        buf[ret] = 0;
                        printf("server read: %s\n", buf);
                        int wrt = write(fd, buf, ret + 1);//¼òµ¥´¦Àí£¬²»´¦Àí×öÒ»´ÎÃ»Ð´ÍêµÄÇé¿ö¡£
                        if (wrt != ret + 1) {
                                err_quite("write error!");
                        }
                }
        }
        free(buf);
}
