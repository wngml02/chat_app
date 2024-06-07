#include "common.h"

#define SERVER_MSG_QUEUE 1234
#define USER1_MSG_QUEUE 5678
#define LOG_FILE "user1_log.txt"

// 서버로 메시지를 전송하는 함수
void send_message_to_server(int server_msgid, const char *msg_text) {
    struct message msg;
    msg.msg_type = 1; // 메시지 타입 설정
    strncpy(msg.msg_text, msg_text, MAX_TEXT);

    // 서버로 메시지 송신
    if (msgsnd(server_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    // 로그 파일에 메시지 기록
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
    dprintf(log_fd, "user1: %s\n", msg_text);
    close(log_fd);
}

// 서버로부터 메시지를 수신하여 화면에 출력하는 함수
void receive_message_from_server(int user1_msgid) {
    struct message msg;

    while (1) {
        // 서버로부터 메시지 수신
        if (msgrcv(user1_msgid, (void *)&msg, sizeof(msg.msg_text), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        // 서버로부터 수신한 메시지 화면에 출력
        printf("Server: %s\n", msg.msg_text);

        // 메시지가 "exit"인 경우 대화 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0)
            break;
    }
}

int main() {
    int server_msgid, user1_msgid;

    // 메시지 큐 생성
    server_msgid = msgget((key_t)SERVER_MSG_QUEUE, 0666 | IPC_CREAT);
    user1_msgid = msgget((key_t)USER1_MSG_QUEUE, 0666 | IPC_CREAT);

    if (server_msgid == -1 || user1_msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // 서버로 메시지 송신
    send_message_to_server(server_msgid, "User1 connected.");

    // 서버로부터 메시지 수신 및 화면 출력
    receive_message_from_server(user1_msgid);

    // 메시지 큐 삭제
    msgctl(user1_msgid, IPC_RMID, NULL);

    return 0;
}
