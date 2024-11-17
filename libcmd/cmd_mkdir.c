#include "libcmd.h"

static void mkdir_func(int argc, char **argv, int p_flag, int v_flag, int mode, int i) {
    char *dir_name = resolve_path(argv[i]);

    // -p 옵션이 있는 경우, 부모 디렉토리도 생성
    if (p_flag) {
        if (mkdir(dir_name, mode) == -1 && errno != EEXIST) { // 파일을 만들지 못하면(파일이 이미 있을 경우 제외)
            perror("mkdir");
            return ;
        }
    }
    else {// -p 옵션이 없는 경우, 디렉토리 생성
        if (mkdir(dir_name, mode) == -1) { // 파일을 만들지 못하는 경우(이미 있는 경우도 오류)
            perror("mkdir");
            return ;
        }
    }
    // -v 옵션이 있는 경우, 생성된 디렉토리 이름 출력
    if (v_flag)
        printf("mkdir: created directory '%s'\n", dir_name);
}


void cmd_mkdir(int argc, char **argv) {
    int opt;
    int p_flag = 0;  // -p 옵션 플래그
    int v_flag = 0;  // -v 옵션 플래그
    mode_t mode = 0777;  // 권한

    while ((opt = getopt(argc, argv, "pvm:")) != -1) {
        switch (opt) {
            case 'p':
                p_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            case 'm':
                mode = strtol(optarg, NULL, 8);  // 8진수
                break;
            default:
                usage_mkdir();
                return ;
        }
    }

    // optind getopt()가 처리하지 않은 첫 번째 인덱스
    for (int i = optind; i < argc; i++) {
        mkdir_func(argc, argv, p_flag, v_flag, mode, i);
    }
}

void usage_mkdir() {
    printf("Usage: mkdir [-pv] [-m mode] directory_name ...\n");
}
