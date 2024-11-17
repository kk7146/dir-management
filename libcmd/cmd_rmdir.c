#include "libcmd.h"
#include <libgen.h>

// 부모 디렉토리 제거 함수
void remove_parent_directories(char *path, int v_flag) {
    char *parent_path = strdup(path);  // dirname() 함수 사용을 위해 경로 복제
    if (!parent_path) {
        perror("strdup");
        return;
    }

    while (strcmp(parent_path, BASE_DIR) != 0) {
        if (rmdir(parent_path) != 0) {
            perror("rmdir");
            break;
        }
        if (v_flag) {
            printf("removed directory: %s\n", parent_path);
        }
        parent_path = dirname(parent_path);  // 상위 디렉토리로 이동
    }
    free(parent_path);
}

void cmd_rmdir(int argc, char **argv) {
    int opt;
    int p_flag = 0;    // -p 옵션 플래그 (부모 디렉토리 제거)
    int v_flag = 0;   // -v 옵션 플래그 (자세한 출력)

    while ((opt = getopt(argc, argv, "pv")) != -1) {
        switch (opt) {
            case 'p':
                p_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            default:
                usage_rmdir();
                return;
        }
    }

    // 옵션 처리 후 남은 인자는 제거할 디렉토리 경로들
    if (optind >= argc) {
        printf("rmdir: missing operand\n");
        usage_rmdir();
        return;
    }

    // 각 디렉토리 경로에 대해 처리
    for (int i = optind; i < argc; i++) {
        char *new_path = resolve_path(argv[i]);
        if (!new_path) {
            printf("rmdir: failed to resolve path for %s\n", argv[i]);
            continue;
        }

        // 디렉토리 제거
        if (rmdir(new_path) != 0) {
            perror("rmdir");
        } else if (v_flag) {
            printf("removed directory: %s\n", new_path);
        }

        // 부모 디렉토리 제거하는 경우
        if (p_flag) {
            remove_parent_directories(new_path, v_flag);
        }

        free(new_path);
    }
}

// 사용법 출력 함수
void usage_rmdir() {
    printf("Usage: rmdir [-p] [-v] directory ...\n");
    printf("  -p  Remove parent directories if they are empty\n");
    printf("  -v  Verbose mode: print each directory as it is removed\n");
}
