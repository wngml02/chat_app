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
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h> // 추가

#define MAX_TEXT 512

// 구조체 정의: 메시지 전달용
struct message {
    long msg_type;
    char msg_text[MAX_TEXT];
};

// 파일 정보 출력 함수 선언
void display_file_info(const char *filename);

#endif // COMMON_H
