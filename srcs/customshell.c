#include "customshell.h"

#define MAX_CMD_SIZE    (128)
#define BASE_DIR "/private/tmp/test"

int check_null_pointer(const char *ch) { // malloc 터질 우려
    if (ch == NULL) {
        perror("malloc");
        return 1;
    }
    return 0;
}

int ensure_directory_exists() { // BASE_DIR이 존재하는지 확인하고 없으면 만듦
    struct stat st;

    if (stat(BASE_DIR, &st) != 0)
        if (mkdir(BASE_DIR, 0755) != 0) {
            perror("mkdir");
            return -1;
        }
    return 0;
}

void init_dir() { // 현재 위치가 BASE_DIR 안쪽이 아니면 BASE_DIR로 옮김
    char *current_dir;

    current_dir = getcwd(NULL, 0);
    if (!validate_path(current_dir))
        chdir(BASE_DIR);
}

int init() { // 프로그램 실행 경로가 BASE_DIR이 아닌 경우 해당 경로로 이동. 없으면 파일 만들고. 
    if (ensure_directory_exists() == -1)
        return -1;
    init_dir();
    return 0;
}

int execute_shell(char *command) {
    char *current_dir, *tok_str;

    while (1) {
        current_dir = getcwd(NULL, 0);
        if (check_null_pointer(current_dir))
            return -1;

        printf("%s $ ", current_dir + strlen(BASE_DIR));
        if (fgets(command, MAX_CMD_SIZE-1, stdin) == NULL) {
            free(current_dir);
            return -1;
        }

        tok_str = strtok(command, " \n");
        if (tok_str == NULL)
            continue; // 이거 없애고 싶음

        if (strcmp(tok_str, "quit") == 0)
            return 0;

        else if (strcmp(tok_str, "cd") == 0)
            cmd_cd(current_dir, tok_str);

        else if (strcmp(tok_str, "help") == 0)
            cmd_help();

        else if (strcmp(tok_str, "mkdir") == 0)
            cmd_mkdir(current_dir, tok_str);

        else if (strcmp(tok_str, "rmdir") == 0)
            cmd_rmdir(current_dir, tok_str);

        else if (strcmp(tok_str, "rename") == 0)
            cmd_rename(current_dir, tok_str);

        else if (strcmp(tok_str, "ln") == 0)
            cmd_ln(current_dir, tok_str);

        else if (strcmp(tok_str, "rm") == 0)
            cmd_rm(current_dir, tok_str);

        else if (strcmp(tok_str, "chmod") == 0)
            cmd_chmod(current_dir, tok_str);

        else if (strcmp(tok_str, "ls") == 0)
            cmd_ls();

        else if (strcmp(tok_str, "cat") == 0)
            cmd_cat(current_dir, tok_str);

        else if (strcmp(tok_str, "cp") == 0)
            cmd_cp(current_dir, tok_str);

        else {
            printf("your command: %s\n", tok_str);
            printf("and argument is ");
            tok_str = strtok(NULL, " \n");
            if (tok_str == NULL) {
                printf("NULL\n");
            } else {
                printf("%s\n", tok_str);
            }
        }
        free(current_dir);
    }
    return -1;
}

int main(int argc, char **argv) {
    char *command;

    if (init() == -1) // 초기화
        return -1;

    command = (char*)malloc(MAX_CMD_SIZE);
    if (check_null_pointer(command))
        return -1;

    if (execute_shell(command) == -1)
        return -1;
    free(command);
    return 0;
}
