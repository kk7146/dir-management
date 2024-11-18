#include "libcmd.h"

// 파일 권한 출력 함수
static void print_permissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : (S_ISLNK(mode)) ? "l" : (S_ISFIFO(mode)) ? "p" :
           (S_ISCHR(mode)) ? "c" : (S_ISBLK(mode)) ? "b" : (S_ISSOCK(mode)) ? "s" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

static void ls_func(int a_flag, int l_flag, int s_flag, int r_flag){
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    struct passwd *pw;
    struct group *gr;
    char timebuf[64];
    char link_target[MAX_DIR_SIZE + 1];
    char *filename;

    dir = opendir(".");
    if (dir == NULL) {
        perror("ls");
        return;
    }

    // 디렉토리 엔트리 순회
    while ((entry = readdir(dir)) != NULL) {
        if (!a_flag && entry->d_name[0] == '.') {
            continue;  // -a 옵션이 없는 경우 숨김 파일 무시
        }

        if (lstat(entry->d_name, &statbuf) == -1) {
            perror("lstat");
            continue;
        }

        // 상세 정보 표시 (-l 옵션)
        if (l_flag) {
            print_permissions(statbuf.st_mode);
            printf(" %ld ", (long)statbuf.st_nlink);
            pw = getpwuid(statbuf.st_uid);
            gr = getgrgid(statbuf.st_gid);
            printf("%s %s ", pw->pw_name, gr->gr_name);
            printf("%5lld ", (long long)statbuf.st_size);
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_mtime));
            printf("%s ", timebuf);
        }
        printf("%s", entry->d_name);
        // 심볼릭 링크 대상 표시
        if (S_ISLNK(statbuf.st_mode)) {
            ssize_t len = readlink(entry->d_name, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                printf(" -> %s", link_target + strlen(BASE_DIR));
            }
        }
        printf("\n");
    }
    closedir(dir);
}

// ls 명령어 함수
void cmd_ls(int argc, char **argv) {
    int opt;
    int a_flag = 0;     // -a 옵션 플래그 (숨김 파일 표시)
    int l_flag = 0;  // -l 옵션 플래그 (상세 정보 표시)
    int s_flag = 0;    // -t 옵션 플래그 (시간 기준 정렬)
    int r_flag = 0;      // -r 옵션 플래그 (역순 정렬)

    // getopt를 사용하여 옵션 파싱
    while ((opt = getopt(argc, argv, "altr")) != -1) {
        switch (opt) {
            case 'a':
                a_flag = 1;
                break;
            case 'l':
                l_flag = 1;
                break;
            case 't':
                s_flag = 1;
                break;
            case 'r':
                r_flag = 1;
                break;
            default:
                usage_ls();
                return;
        }
    }
    ls_func(a_flag, l_flag, s_flag, r_flag);
}

// ls 사용법 출력 함수
void usage_ls() {
    printf("Usage: ls [-a] [-l] [-t] [-r]\n");
    printf("  -a  List all entries including hidden files\n");
    printf("  -l  Use a long listing format\n");
    printf("  -t  Sort by modification time, newest first (미구현)\n");
    printf("  -r  Reverse the order of the sort (미구현)\n");
}
