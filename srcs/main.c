#include "customshell.h"
#include <string.h>

// 명령어 리스트 배열 (cmd_list는 구조체로 정의되어 있어야 함)
extern cmd_list list[]; // list 배열이 다른 파일에 정의되어 있다고 가정합니다.

int main(int argc, char **argv) {
    char *command;
    cmd_node *head = NULL;

    // 초기화 작업
    if (init() == -1) {
        return -1;
    }

    // argv를 통해 명령어 추가
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            int is_found = 0;
            for (int j = 0; j < sizeof(list) / sizeof(list[0]); j++) {
                if (strcmp(argv[i], list[j].name) == 0) {
                    add_command(&head, list[j].name, list[j].cmd_func, list[j].usage_func);
                    is_found = 1;
                    break;
                }
            }
            if (!is_found) {
                printf("Warning: '%s' is not a recognized command and will be ignored.\n", argv[i]);
            }
        }
    }

    // 쉘 명령어 실행
    command = (char*)malloc(MAX_CMD_SIZE);
    if (check_null_pointer(command)) {
        free_all_commands(&head);
        return -1;
    }

    if (execute_shell(command, head) == -1) {
        free(command);
        free_all_commands(&head);
        return -1;
    }

    // 메모리 해제
    free(command);
    free_all_commands(&head);
    return 0;
}
