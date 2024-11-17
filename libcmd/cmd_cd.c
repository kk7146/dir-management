#include "libcmd.h"

void cmd_cd(int argc, char **argv) {
    char * new_path;

    if (argc > 2)
    {
        printf("cd: too many argument\n");
        return ;
    }
    if (argc == 1)
    {
        chdir(BASE_DIR);
        return ;
    }
    new_path = resolve_path(argv[1]);
    if (chdir(new_path) != 0)
        perror("cd");
    free(new_path);
}

void usage_cd() {
    printf("cd <path>              : Change directory to <path>\n");
}
