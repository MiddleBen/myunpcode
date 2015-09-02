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
#include <stdio.h>
#include <sys/stat.h>
#define CREATE_MODE S_IRWXU | S_IRWXG
void error_quite(char *msg) {
	fprintf(stderr, "error: %s, %s\n", msg, strerror(errno));
	exit(1);
}
int main(int argc, char **argv) {
	if (argc != 2) {
		error_quite("usage: ./mmap <filepath>");
	}
	char *addr, *in;
	off_t filesize;
	struct stat filestat;
	int fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		error_quite("open error");
	}
	if (fstat(fd, &filestat) != 0) {
		error_quite("fstat error");
	}
	filesize = filestat.st_size;
	printf("filesize: %ld\n", filesize);
	addr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		error_quite("mmap error");
	}
	close(fd);
	printf("content: %s\n", addr);
	addr[20] = '{';
	printf("sleeping ...\n");
	sleep(10);
	printf("wake up!\n");
	/**if (msync(addr, filesize, MS_SYNC) != 0) {
		error_quite("msync error!");
	}
	if (munmap(addr, filesize) < 0) {
		error_quite("munmap error");
	}**/
	exit(EXIT_SUCCESS);
}

