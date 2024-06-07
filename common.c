#include "common.h"

// 파일 정보를 출력하는 함수 구현 (조건 1)
void display_file_info(const char *filename) {
    struct stat file_stat;

    if (stat(filename, &file_stat) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", file_stat.st_size);
    printf("Last modified: %s", ctime(&file_stat.st_mtime));
}
