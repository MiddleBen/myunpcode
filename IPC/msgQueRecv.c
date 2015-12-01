#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
/**
 * 使用消息队列接收数据
 */
#define MSG_SIZE 20
struct MYMSG {
	long msgid;
	char msgtext[MSG_SIZE];
};

int main() {
	int msqid;
	key_t key = 0x33;
	struct MYMSG msg;
	struct msqid_ds msginfo;
	size_t msgsz = sizeof(struct MYMSG);
	if ((msqid = msgget(key, IPC_CREAT | 0600)) < 0) {
			perror("msgget error");
	}
	if (msgctl(msqid, IPC_STAT, &msginfo) < 0) {
		perror("msgctl error!");
	}
	printf("recv queue has %ld msgs with %ld bytes\n", msginfo.msg_qnum,
			msginfo.__msg_cbytes);
	bzero(&msg, msgsz);
	if (msgrcv(msqid, &msg, msgsz, 007, IPC_NOWAIT) < 0) {
		perror("msg recv error!");
	}
	printf("msg.msgid = %ld\n", msg.msgid);
	printf("msg.msgtext = %s\n", msg.msgtext);

	bzero(&msg, msgsz);
	if (msgrcv(msqid, &msg, msgsz, 0, IPC_NOWAIT) < 0) {
		perror("msg recv error!");
	}
	printf("msg.msgid = %ld\n", msg.msgid);
	printf("msg.msgtext = %s\n", msg.msgtext);

	bzero(&msg, msgsz);
	if (msgrcv(msqid, &msg, msgsz, 006, IPC_NOWAIT) < 0) {
		perror("msg recv error!");
	}
	printf("msg.msgid = %ld\n", msg.msgid);
	printf("msg.msgtext = %s\n", msg.msgtext);
	exit(0);
}
