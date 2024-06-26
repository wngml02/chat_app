#include "common.h"

#define SERVER_MSG_QUEUE 1234
#define CLIENT_MSG_QUEUE 5678
#define LOG_FILE "client_log.txt"

// 서버 메시지를 처리하는 함수(시스템 통신)
void handle_server_message(int server_msgid) {
    struct message msg;
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (log_fd == -1) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 서버 메시지 수신
        if (msgrcv(server_msgid, (void *)&msg, sizeof(msg.msg_text), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        // 서버 메시지 출력 및 로그 파일에 저장 (조건 1 파일 입력 해당)
        printf("Server: %s\n", msg.msg_text);
        dprintf(log_fd, "Server: %s\n", msg.msg_text);

        // 서버가 "exit" 메시지를 보낸 경우 채팅 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("Server has exited the chat.\n");
            dprintf(log_fd, "Server has exited the chat.\n");
            break;
        }

        // 서버가 "edit" 메시지를 보낸 경우 텍스트 편집기 실행 : (조건 4 execlp함수 사용)
        if (strncmp(msg.msg_text, "edit", 4) == 0) {
            printf("Server requested to open the editor.\n");
            dprintf(log_fd, "Server requested to open the editor.\n");

            pid_t editor_pid = fork();
            if (editor_pid == 0) {
                execlp("nano", "nano", LOG_FILE, (char *)NULL);
                perror("execlp");
                exit(EXIT_FAILURE);
            }
        }
    }

    close(log_fd);
}

int main() {
    int server_msgid, client_msgid;
    struct message msg;

    // 메시지 큐 생성 (조건 5,6)
    server_msgid = msgget((key_t)SERVER_MSG_QUEUE, 0666 | IPC_CREAT);
    client_msgid = msgget((key_t)CLIENT_MSG_QUEUE, 0666 | IPC_CREAT);

    if (server_msgid == -1 || client_msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // 서버 메시지를 별도의 프로세스에서 처리 (조건 3 프로세스 사용)
    pid_t pid = fork();
    if (pid == 0) {
        handle_server_message(server_msgid);
        exit(EXIT_SUCCESS);
    }

    // 클라이언트 메시지 송신 루프 (조건 5, 6 IPC 시스템 통신)
    while (1) {
        printf("Client: ");
        fgets(msg.msg_text, MAX_TEXT, stdin);

        msg.msg_type = 1;  // 메시지 타입 설정

        // 클라이언트 메시지 송신
        if (msgsnd(client_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        // 클라이언트가 "exit" 메시지를 보낸 경우 채팅 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("Client has exited the chat.\n");
            break;
        }
    }

    // 파일 정보 출력 (조건 1 출력 로그 파일)
    display_file_info(LOG_FILE);

    // 메시지 큐 삭제 및 자식 프로세스 종료 대기
    msgctl(client_msgid, IPC_RMID, NULL);
    wait(NULL);

    return 0;
}
