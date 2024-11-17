#include "libcmd.h"
#include <ctype.h>

static void cat_func(FILE *file, int b_flag, int e_flag, int n_flag, int s_flag, int t_flag, int u_flag, int v_flag) {
    char line[1024];
    int line_number = 1;
    int previous_blank = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        
        // s 플래수 \n가 연속으로 나올 경우 출력하지 않고 다음 분기로
        if (s_flag) {
            if (line[0] == '\n') {
                if (previous_blank)
                    continue;
                previous_blank = 1;
            }
            else
                previous_blank = 0;
        }

        // 줄 수 보이게 처리. b의 경우에는 아무 것도 없는 줄의 경우만 출력.
        if (n_flag || (b_flag && line[0] != '\n'))
            printf("%6d  ", line_number++);

        //이제 전처리 끝. 문자 출력
        // '$' 문자가 끝에 나오게 근데 v도 같이 동작.
        if (e_flag || t_flag || v_flag) {
            for (char *p = line; *p != '\0'; p++) {
            // 제어 문자인 경우 (줄 바꿈과 탭 제외)
                if (iscntrl((unsigned char)*p) && *p != '\n' && *p != '\t') {
                    if ((unsigned char)*p == 127) // 삭제 문자 처리
                        printf("^?");
                    else
                        printf("^%c", *p + '@');
                }
                // 비ASCII 문자 (하이 비트가 1인 경우)
                else if ((unsigned char)*p >= 128)
                    printf("M-%c", (unsigned char)*p & 0x7F);
                // 줄 바꿈 문자 처리 (-e 옵션)
                else if (*p == '\n') {
                    if (e_flag)
                        printf("$\n"); // '$' 기호 추가 후 줄바꿈
                    else
                        putchar(*p); // 그냥 줄바꿈
                }
                // 탭 문자 처리 (-t 옵션)
                else if (*p == '\t') {
                    if (t_flag)
                        printf("^I");
                    else
                        putchar(*p); // 그냥 탭
                }
                // 나머지 인쇄 가능한 문자 처리
                else
                    putchar(*p);
            }
        }
        else
            printf("%s", line);
    }
}

void cmd_cat(int argc, char **argv) {
    char *file_path;
    int opt;
    int b_flag = 0, e_flag = 0, n_flag = 0, s_flag = 0, t_flag = 0, u_flag = 0, v_flag = 0;

    while ((opt = getopt(argc, argv, "benstuv")) != -1) {
        switch (opt) {
            case 'b':
                b_flag = 1;
                n_flag = 0;  // -b의 경우에 -n은 삭제
                break;
            case 'e':
                t_flag = 0;
                v_flag = 0;
                e_flag = 1;
                break;
            case 'n':
                b_flag = 0;
                n_flag = 1;  // -n의 경우에 -b은 삭제
                break;
            case 's':
                s_flag = 1;
                break;
            case 't':
                t_flag = 1;
                v_flag = 0;
                e_flag = 0;
                break;
            case 'u':
                u_flag = 1;
                break;
            case 'v':
                t_flag = 0;
                v_flag = 1;
                e_flag = 0;
                break;
            default:
                usage_cat();
                return;
        }
    }

    if (optind >= argc) {
        usage_cat();
        return;
    }

    for (int i = optind; i < argc; i++) {
        file_path = resolve_path(argv[i]);
        if (file_path == NULL)
        {
            printf("cat: invalid filename argument\n");
            continue;
        }
        FILE *file = fopen(file_path, "r");
        if (file == NULL) {
            perror("cat");
            free(file_path);
            continue;
        }
        cat_func(file, b_flag, e_flag, n_flag, s_flag, t_flag, u_flag, v_flag);
        free(file_path);
        fclose(file);
    }
}

void usage_cat() {
    printf("Usage: cat [-belnstuv] [file ...]\n");
    printf("  -b  Number the non-blank output lines, starting at 1\n");
    printf("  -e  Display non-printing characters and end lines with $\n");
    printf("  -n  Number all output lines, starting at 1\n");
    printf("  -s  Squeeze multiple adjacent empty lines\n");
    printf("  -t  Display non-printing characters and show tabs as ^I\n");
    printf("  -u  Disable output buffering (미구현)\n");
    printf("  -v  Display non-printing characters. \n      Control characters print as \'^X\' for control-X; the delete character (octal 0177) prints as \'^?\'. \n      Non-ASCII characters (with the high bit set) are printed as \'M-\' (for meta) followed by the character for the low 7 bits.\n");
}
