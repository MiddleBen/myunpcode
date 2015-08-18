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

int main(int argc, char ** argv) {
        if (argc != 2) {
                err_quite("usage: ./server <path>");
        };
        char *buf = malloc(MAX_LINE + 1);
        int sockunfd, addrlen;
        struct sockaddr_un socketunaddr;
        addrlen = sizeof(struct sockaddr_un);
        bzero(&socketunaddr, addrlen);
        socketunaddr.sun_family = AF_UNIX;
        strncpy(socketunaddr.sun_path, argv[1], sizeof(socketunaddr.sun_path));
        sockunfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (connect(sockunfd, (struct sockaddr *)&socketunaddr, addrlen) < 0) {
                err_quite("connection error");
        }
        for (;;) {
                int ret = read(STDIN_FILENO, buf, MAX_LINE);
                if (ret < 0) {
                        if (errno == EINTR)
                                continue;
                        err_quite("read error!");
                } else if (ret == 0) {
                        printf("read eof\n");
                        break;
                } else {
                        int wret = write(sockunfd, buf, ret);
                        if (wret != ret) { //Èç¹ûÓÐ´íÎó£¬¼òµ¥´¦ÀíÁË£¬²»×öÑ­»·Ð´Íê¡£
                                err_quite("write error");
                        }
                        ret = read(sockunfd, buf, MAX_LINE);
                        buf[ret] = 0;
                        printf("ret msg: %s\n", buf);
                }
        }
        free(buf);
        exit(0);
}
