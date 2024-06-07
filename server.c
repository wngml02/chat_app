#include "common.h"

#define SERVER_MSG_QUEUE 1234
#define USER1_MSG_QUEUE 5678
#define USER2_MSG_QUEUE 91011
#define LOG_FILE "server_log.txt"

// user1으로 메시지를 전달하는 함수
void send_message_to_user1(int user1_msgid, const char *msg_text) {
    struct message msg;
    msg.msg_type = 1; // 메시지 타입 설정
    strncpy(msg.msg_text, msg_text, MAX_TEXT);

    // user1으로 메시지 송신
    if (msgsnd(user1_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
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

// user2로 메시지를 전달하는 함수
void send_message_to_user2(int user2_msgid, const char *msg_text) {
    struct message msg;
    msg.msg_type = 1; // 메시지 타입 설정
    strncpy(msg.msg_text, msg_text, MAX_TEXT);

    // user2로 메시지 송신
    if (msgsnd(user2_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    // 로그 파일에 메시지 기록
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
    dprintf(log_fd, "user2: %s\n", msg_text);
    close(log_fd);
}

int main() {
    int server_msgid, user1_msgid, user2_msgid;
    struct message msg;

    // 메시지 큐 생성
    server_msgid = msgget((key_t)SERVER_MSG_QUEUE, 0666 | IPC_CREAT);
    user1_msgid = msgget((key_t)USER1_MSG_QUEUE, 0666 | IPC_CREAT);
    user2_msgid = msgget((key_t)USER2_MSG_QUEUE, 0666 | IPC_CREAT);

    if (server_msgid == -1 || user1_msgid == -1 || user2_msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 메시지 수신 대기
        if (msgrcv(server_msgid, (void *)&msg, sizeof(msg.msg_text), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        // user1으로 메시지 전송
        send_message_to_user1(user1_msgid, msg.msg_text);

        // user2으로 메시지 전송
        send_message_to_user2(user2_msgid, msg.msg_text);

        // 특정 명령어 처리 (여기서는 "edit" 명령어를 입력하면 nano 실행)
        if (strncmp(msg.msg_text, "edit", 4) == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                execlp("nano", "nano", LOG_FILE, NULL);
                perror("execlp");
                exit(EXIT_FAILURE);
            }
            wait(NULL); // 편집기 종료 대기
        }
    }

    // 메시지 큐 삭제
    msgctl(server_msgid, IPC_RMID, NULL);
    msgctl(user1_msgid, IPC_RMID, NULL);
    msgctl(user2_msgid, IPC_RMID, NULL);

    return 0;
}
