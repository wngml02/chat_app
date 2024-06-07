#include "common.h"

#define USER1_MSG_QUEUE 1234
#define USER2_MSG_QUEUE 5678
#define LOG_FILE "user2_log.txt"

// user1 메시지를 처리하는 함수
void handle_user1_message(int user1_msgid) {
    struct message msg;
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (log_fd == -1) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // user1 메시지 수신
        if (msgrcv(user1_msgid, (void *)&msg, sizeof(msg.msg_text), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        // user1 메시지 출력 및 로그 파일에 저장
        printf("user1: %s\n", msg.msg_text);
        dprintf(log_fd, "user1: %s\n", msg.msg_text);

        // user1이 "exit" 메시지를 보낸 경우 채팅 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("user1 has exited the chat.\n");
            dprintf(log_fd, "user1 has exited the chat.\n");
            break;
        }
    }

    close(log_fd);
}

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

    // user1 메시지를 별도의 프로세스에서 처리
    pid_t pid = fork();
    if (pid == 0) {
        handle_user1_message(user1_msgid);
        exit(EXIT_SUCCESS);
    }

    // user2 메시지 송신 루프
    while (1) {
        printf("user2: ");
        fgets(msg.msg_text, MAX_TEXT, stdin);

        msg.msg_type = 1;  // 메시지 타입 설정

        // user2 메시지 송신
        if (msgsnd(user2_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        // user2가 "exit" 메시지를 보낸 경우 채팅 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("user2 has exited the chat.\n");
            break;
        }
    }

    // 파일 정보 출력 (로그 파일)
    display_file_info(LOG_FILE);

    // 메시지 큐 삭제 및 자식 프로세스 종료 대기
    msgctl(user2_msgid, IPC_RMID, NULL);
    wait(NULL);

    return 0;
}
