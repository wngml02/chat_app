#include "common.h"

#define SERVER_MSG_QUEUE 1234
#define CLIENT_MSG_QUEUE 5678

void handle_server_message(int server_msgid) {
    struct message msg;
    while (1) {
        if (msgrcv(server_msgid, (void *)&msg, sizeof(msg.msg_text), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        printf("Server: %s\n", msg.msg_text);

        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("Server has exited the chat.\n");
            break;
        }
    }
}

int main() {
    int server_msgid, client_msgid;
    struct message msg;

    // Create message queues
    server_msgid = msgget((key_t)SERVER_MSG_QUEUE, 0666 | IPC_CREAT);
    client_msgid = msgget((key_t)CLIENT_MSG_QUEUE, 0666 | IPC_CREAT);

    if (server_msgid == -1 || client_msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // Handle server messages in a separate process
    pid_t pid = fork();
    if (pid == 0) {
        handle_server_message(server_msgid);
        exit(EXIT_SUCCESS);
    }

    while (1) {
        printf("Client: ");
        fgets(msg.msg_text, MAX_TEXT, stdin);

        msg.msg_type = 1;

        if (msgsnd(client_msgid, (void *)&msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        if (strncmp(msg.msg_text, "exit", 4) == 0) {
            printf("Client has exited the chat.\n");
            break;
        }
    }

    // Clean up
    msgctl(client_msgid, IPC_RMID, NULL);
    wait(NULL);

    return 0;
}
