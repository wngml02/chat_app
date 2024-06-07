#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MAX_TEXT 512

struct message {
    long msg_type;
    char msg_text[MAX_TEXT];
};

// 파일 정보를 출력하는 함수 선언
void display_file_info(const char *filename);

#endif // COMMON_H
