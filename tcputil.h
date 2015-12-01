/*
 * tcputil.h
 *
 *  Created on: 2015年7月19日
 *      Author: lhm
 */
#define	MAXLINE	4096
extern int Tcp_listen(const char* host, const char *serv);
extern int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
extern void error_quite(char *msg);
extern int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
#ifndef eq
#define eq
	void err_quite(const char *msg) {
		printf("line %d error: %s, errno: %s\n", __LINE__, msg, strerror(errno));
		exit(1);
	}
#endif
