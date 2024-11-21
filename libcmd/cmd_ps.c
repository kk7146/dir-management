#include "libcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <libproc.h>
#include <pwd.h>

static int ps_func(int a_flag, int x_flag, int u_flag) {
    struct proc_bsdinfo proc;
    printf("PID\t");
    if (u_flag) 
        printf("USER\t");
    printf("CMD\n");

    for (int pid = 1; pid < PID_SIZE; ++pid) {  // PID 범위
        if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &proc, sizeof(proc)) > 0) {
            // 간단한 필터링: 옵션에 따라 조건을 설정할 수 있음
            if (proc.pbi_uid != getuid())
                continue;  // -a 옵션이 없으면 현재 사용자만 표시

            // 사용자 이름 가져오기
            if (u_flag) {
                struct passwd *pw = getpwuid(proc.pbi_uid);
                if (pw)
                    printf("%d\t%s\t%s\n", pid, pw->pw_name, proc.pbi_name);
                else
                    printf("%d\t<unknown>\t%s\n", pid, proc.pbi_name);
            }
            else
                printf("%d\t%s\n", pid, proc.pbi_name);
        }
    }
    return 0;
}

int cmd_ps(int argc, char **argv) {
    int opt;
    int a_flag = 0;    // -a 옵션 플래그 (다른 사용자와 터미널에 연결된 프로세스 포함)
    int x_flag = 0;    // -x 옵션 플래그 (터미널에 연결되지 않은 프로세스 포함)
    int u_flag = 0;    // -u 옵션 플래그 (사용자 이름 포함)

    while ((opt = getopt(argc, argv, "axu")) != -1) {
        switch (opt) {
            case 'a':
                a_flag = 1;
                break;
            case 'x':
                x_flag = 1;
                break;
            case 'u':
                u_flag = 1;
                break;
            default:
                usage_ps();
                return -2;
        }
    }
    return ps_func(a_flag, x_flag, u_flag);
}

void usage_ps() {
    printf("Usage: ps [-a] [-x] [-u]\n");
    printf("  -a  Include processes from other users and attached to a terminal(미구현)\n");
    printf("  -x  Include processes not attached to a terminal (미구현)\n");
    printf("  -u  Display the user name associated with each process\n");
}
