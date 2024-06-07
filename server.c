#include "common.h"

#define SERVER_MSG_QUEUE 1234
#define CLIENT_MSG_QUEUE 5678
#define LOG_FILE "server_log.txt"

// 클라이언트 메시지를 처리하는 함수
void handle_client_message(int client_msgid) {
    struct message msg;
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (log_fd == -1) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 클라이언트 메시지 수신
        if (msgrcv(client_msgid, (void *)&msg, sizeof(msg.msg_text), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        // 클라이언트 메시지 출력 및 로그 파일에 저장
        printf("Client: %s\n", msg.msg_text);
        dprintf(log_fd, "Client: %s\n", msg.msg_text);

        // 클라이언트가 "exit" 메시지를 보낸 경우 채팅 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("Client has exited the chat.\n");
            dprintf(log_fd, "Client has exited the chat.\n");
            break;
        }

        // 클라이언트가 "edit" 메시지를 보낸 경우 텍스트 편집기 실행
        if (strncmp(msg.msg_text, "edit", 4) == 0) {
            printf("Client requested to open the editor.\n");
            dprintf(log_fd, "Client requested to open the editor.\n");

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

    // 메시지 큐 생성
    server_msgid = msgget((key_t)SERVER_MSG_QUEUE, 0666 | IPC_CREAT);
    client_msgid = msgget((key_t)CLIENT_MSG_QUEUE, 0666 | IPC_CREAT);

    if (server_msgid == -1 || client_msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // 클라이언트 메시지를 별도의 프로세스에서 처리
    pid_t pid = fork();
    if (pid == 0) {
        handle_client_message(client_msgid);
        exit(EXIT_SUCCESS);
    }

    // 서버 메시지 송신 루프
    while (1) {
        printf("Server: ");
        fgets(msg.msg_text, MAX_TEXT, stdin);

        msg.msg_type = 1;  // 메시지 타입 설정

        // 서버 메시지 송신
        if (msgsnd(server_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        // 서버가 "exit" 메시지를 보낸 경우 채팅 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("Server has exited the chat.\n");
            break;
        }
    }

    // 파일 정보 출력 (로그 파일)
    display_file_info(LOG_FILE);

    // 메시지 큐 삭제 및 자식 프로세스 종료 대기
    msgctl(server_msgid, IPC_RMID, NULL);
    wait(NULL);

    return 0;
}
