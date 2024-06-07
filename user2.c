#include "common.h"

#define SERVER_MSG_QUEUE 1234
#define CLIENT_MSG_QUEUE 5678
#define LOG_FILE "user2_log.txt"

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

        // 서버 메시지 출력 및 로그 파일에 저장
        printf("Server: %s\n", msg.msg_text);
        dprintf(log_fd, "Server: %s\n", msg.msg_text);

        // 서버가 "exit" 메시지를 보낸 경우 채팅 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("Server has exited the chat.\n");
            dprintf(log_fd, "Server has exited the chat.\n");
            break;
        }

        // 서버가 "edit" 메시지를 보낸 경우 텍스트 편집기 실행
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
int main(int argc, char *argv[]) {
    // 사용자 이름을 설정하는 인자가 전달되었는지 확인
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 전달된 사용자 이름을 저장
    char username[MAX_TEXT];
    snprintf(username, sizeof(username), "%s", argv[1]);

    int server_msgid, client_msgid;
    struct message msg;

    // 서버 및 클라이언트 메시지 큐 생성
    server_msgid = msgget((key_t)SERVER_MSG_QUEUE, 0666 | IPC_CREAT);
    client_msgid = msgget((key_t)CLIENT_MSG_QUEUE, 0666 | IPC_CREAT);

    if (server_msgid == -1 || client_msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 사용자 이름과 함께 입력 메시지를 출력
        printf("%s: ", username);
        fgets(msg.msg_text, MAX_TEXT, stdin);

        msg.msg_type = 1;

        // 클라이언트 메시지 큐에 메시지 전송
        if (msgsnd(client_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        // 사용자가 "exit"을 입력하면 채팅 종료
        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("%s has exited the chat.\n", username);
            break;
        }
    }

    // 파일 정보 출력 (로그 파일)
    display_file_info(LOG_FILE);

    // 클라이언트 메시지 큐 제거 및 자식 프로세스 종료 대기
    msgctl(client_msgid, IPC_RMID, NULL);
    wait(NULL);

    return 0;
}
