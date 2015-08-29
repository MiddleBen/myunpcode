/*
 * mmap.c
 *
 *  Created on: 2015年8月27日
 *      Author: ben01.li
 */
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define CREATE_MODE S_IRWXU | S_IRWXG
void err_quite(char *msg) {
	fprintf(stderr, "error: %s, %s\n", msg, strerror(errno));
	exit(1);
}
int main(int argc, char **argv) {
	if (argc != 2) {
			err_quite("usage: ./mmap <filepath>");
	}
	int filefd = open(argv[1], O_RDWR, CREATE_MODE);
	if (filefd < 0) {
		err_quite("open file error");
	}
	mmap() {

	}
}



