#include "customshell.h"

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
