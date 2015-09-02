#include <sys/types.h>
#include <sys/ipc.h>
#include　<sys/msg.h>
#include <stdio.h>
#include <errno.h>
#define MSG_SIZE 20
/**
 * 使用消息队列发送数据
 */
struct MYMSG {
	long msgid;
	char msgtext[MSG_SIZE];
};

int main () {
	int msqid;
	key_t key = 0x33;
	struct MYMSG msg;
	struct msqid_ds msginfo;
	size_t msgsz = sizeof(struct MYMSG);
	if ((msqid = msgget(key, O_CREAT | O_EXCL)) < 0) {
		perror("msgget error");
	}
	bzero(&msg, msgsz);
	msg.msgid = 007;
	msg.msgtext = "hello, i am 007";
	if (msgsnd(msqid, &msg, msgsz, IPC_NOWAIT) < 0) {
		perror("send msg to queue error!");
	}
	bzero(&msg, msgsz);
	msg.msgid = 006;
	msg.msgtext = "hello, i am 006";
	if (msgsnd(msqid, &msg, msgsz, IPC_NOWAIT) < 0) {
			perror("send msg to queue error!");
	}
	bzero(&msg, msgsz);
	msg.msgid = 006;
	msg.msgtext = "hello, i am 006 tow";
	if (msgsnd(msqid, &msg, msgsz, IPC_NOWAIT) < 0) {
			perror("send msg to queue error!");
	}
	if (msgctl(msqid, IPC_INFO, &msginfo) < 0) {
		perror("msgctl error!");
	}
	printf("send queue has %d msgs with %d bytes\n", msginfo.msg_qnum, msginfo.__msg_cbytes);
	exit(0);
}
