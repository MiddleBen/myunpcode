/*
 * tcputil.h
 *
 *  Created on: 2015年7月19日
 *      Author: lhm
 */
#define	MAXLINE	4096
extern int Tcp_listen(const char* host, const char *serv);
extern int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
#ifndef eq
#define eq
	void err_quite(const char *msg) {
		printf("line %d error: %s, errno: %s\n", __LINE__, msg, strerror(errno));
		exit(1);
	}
#endif

-----------------------------909014532537626944486111073
Content-Disposition: form-data; name="mode"

formupload