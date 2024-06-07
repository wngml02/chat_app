#include "common.h"

#define USER1_MSG_QUEUE 1234
#define USER2_MSG_QUEUE 5678
#define LOG_FILE "chat_log.txt"

int main() {
    int user1_msgid, user2_msgid;
    struct message msg;

    // 메시지 큐 생성
    user1_msgid = msgget((key_t)USER1_MSG_QUEUE, 0666 | IPC_CREAT);
    user2_msgid = msgget((key_t)USER2_MSG_QUEUE, 0666 | IPC_CREAT);

    if (user1_msgid == -1 || user2_msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // 사용자가 입력한 메시지를 상대방에게 전송
    while (1) {
        printf("User1: ");
        fgets(msg.msg_text, MAX_TEXT, stdin);

        // 채팅 기록을 로그 파일에 저장
        FILE *log_file = fopen(LOG_FILE, "a");
        if (log_file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        fprintf(log_file, "User1: %s", msg.msg_text);
        fclose(log_file);

        // user2에게 메시지 전송
        msg.msg_type = 1;
        if (msgsnd(user2_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        // user2로부터 메시지 수신
        if (msgrcv(user1_msgid, (void *)&msg, sizeof(msg.msg_text), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        printf("User2: %s", msg.msg_text);
    }

    return 0;
}
