#include <sys/types.h>
#include <sys/ipc.h>
#include　<sys/msg.h>
#include <stdio.h>
#include <errno.h>
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
	if ((msqid = msgget(key, O_EXCL)) < 0) {
			perror("msgget error");
	}
	if (msgctl(msqid, IPC_INFO, &msginfo) < 0) {
		perror("msgctl error!");
	}
	printf("recv queue has %d msgs with %d bytes\n", msginfo.msg_qnum,
			msginfo.__msg_cbytes);
	bzero(&msg, msgsz);
	if (msgrcv(msgid, &msg, msgsz, 007, IPC_NOWAIT) < 0) {
		perror("msg recv error!");
	}
	printf("msg.msgid = %ld\n", msg.msgid);
	printf("msg.msgtext = %s\n", msg.msgtext);

	bzero(&msg, msgsz);
	if (msgrcv(msgid, &msg, msgsz, 0, IPC_NOWAIT) < 0) {
		perror("msg recv error!");
	}
	printf("msg.msgid = %ld\n", msg.msgid);
	printf("msg.msgtext = %s\n", msg.msgtext);

	bzero(&msg, msgsz);
	if (msgrcv(msgid, &msg, msgsz, 006, IPC_NOWAIT) < 0) {
		perror("msg recv error!");
	}
	printf("msg.msgid = %ld\n", msg.msgid);
	printf("msg.msgtext = %s\n", msg.msgtext);
	exit(0);
}
