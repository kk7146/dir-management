#include "customshell.h"

static int ensure_directory_exists() { // BASE_DIR이 존재하는지 확인하고 없으면 만듦
    struct stat st;

    if (stat(BASE_DIR, &st) != 0)
        if (mkdir(BASE_DIR, 0755) != 0) {
            perror("mkdir");
            return -1;
        }
    return 0;
}

static void init_dir() { // 현재 위치가 BASE_DIR 안쪽이 아니면 BASE_DIR로 옮김
    char *current_dir;

    current_dir = getcwd(NULL, 0);
    if (!validate_path(current_dir))
        chdir(BASE_DIR);
}

static void handle_sigint(int sig) {
    printf("Ctrl+C is disabled.");
}

static int print_shell_dir() { // 현재 위치 출력
    char *current_dir;

    current_dir = getcwd(NULL, 0);
    if (check_null_pointer(current_dir))
        return -1;
    printf("%s $ ", current_dir + strlen(BASE_DIR));
    free(current_dir);
    return 0;
}

static void input_argc_argv(int *input_argc, char** input_argv, char *tok_str) { // 쉘 내부 argc, argv로 변경.
    input_argv[0] = tok_str;

    for ((*input_argc) = 1; (*input_argc) < MAX_ARG; (*input_argc)++) {
        if ((tok_str = strtok(NULL, " \n"))) {
            input_argv[(*input_argc)] = tok_str;
        } else {
            return ;
        }
    }
}

int init() { // 프로그램 실행 경로가 BASE_DIR이 아닌 경우 해당 경로로 이동. 없으면 파일 만들고. 
    if (ensure_directory_exists() == -1)
        return -1;
    init_dir();
    return 0;
}

int execute_shell(char *command, cmd_node *const head) {
    char *tok_str;
    char *input_argv[MAX_ARG];
    int  input_argc;
    cmd_node *node;
    optind = 1;

    signal(SIGINT, handle_sigint);
    while (1) {
        optind = 1;
        node = head;
        if (print_shell_dir() != 0)
            return -1;
        if (fgets(command, MAX_CMD_SIZE-1, stdin) == NULL)
            return -1;

        tok_str = strtok(command, " \n");
        if (tok_str == NULL)
            continue ;
        if (strcmp(tok_str, "help") == 0)
            help(node);
        else if (strcmp(tok_str, "quit") == 0)
            return 0;
        else {
            input_argc_argv(&input_argc, input_argv, tok_str);
            node = find_command(node, input_argv[0]);
            if (node != NULL)
                node->cmd_func(input_argc, input_argv);
            else
                printf("%s: command not found\n", input_argv[0]);
        }
    }
    return -1;
}
