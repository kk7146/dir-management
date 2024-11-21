#include "libcmd.h"

#define PROC_PATH "/proc"
#define CMDLINE_PATH_FORMAT "/proc/%d/cmdline"
#define STATUS_PATH_FORMAT "/proc/%d/status"

static void print_process_info(int pid, int u_flag) {
    char cmdline_path[256];
    char status_path[256];
    char cmdline[1024];
    FILE *cmdline_file, *status_file;
    struct passwd *pw;
    uid_t uid = -1;
    char username[256] = "<unknown>";

    // Build paths for cmdline and status
    snprintf(cmdline_path, sizeof(cmdline_path), CMDLINE_PATH_FORMAT, pid);
    snprintf(status_path, sizeof(status_path), STATUS_PATH_FORMAT, pid);

    // Read command line
    cmdline_file = fopen(cmdline_path, "r");
    if (!cmdline_file) return; // Skip if the process doesn't exist anymore
    fgets(cmdline, sizeof(cmdline), cmdline_file);
    fclose(cmdline_file);

    // Read status for user ID
    status_file = fopen(status_path, "r");
    if (status_file) {
        char line[256];
        while (fgets(line, sizeof(line), status_file)) {
            if (sscanf(line, "Uid:\t%d", &uid) == 1) break;
        }
        fclose(status_file);
    }

    // Convert UID to username
    if (u_flag && uid != -1) {
        pw = getpwuid(uid);
        if (pw) snprintf(username, sizeof(username), "%s", pw->pw_name);
    }

    // Print process info
    if (u_flag)
        printf("%d\t%s\t%s\n", pid, username, cmdline[0] ? cmdline : "[unknown]");
    else
        printf("%d\t%s\n", pid, cmdline[0] ? cmdline : "[unknown]");
}

static int ps_func(int a_flag, int x_flag, int u_flag) {
    DIR *proc_dir;
    struct dirent *entry;

    printf("PID\t");
    if (u_flag) 
        printf("USER\t");
    printf("CMD\n");

    // Open /proc directory
    proc_dir = opendir(PROC_PATH);
    if (!proc_dir) {
        perror("opendir");
        return -1;
    }

    // Iterate through /proc entries
    while ((entry = readdir(proc_dir))) {
        int pid = atoi(entry->d_name);
        if (pid <= 0) continue; // Skip non-PID entries (like '.', '..', etc.)
        print_process_info(pid, u_flag);
    }

    closedir(proc_dir);
    return 0;
}

int cmd_ps(int argc, char **argv) {
    int opt;
    int a_flag = 0;    // -a option flag (include processes from other users)
    int x_flag = 0;    // -x option flag (include processes not attached to a terminal)
    int u_flag = 0;    // -u option flag (display username)

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
    printf("  -a  Include processes from other users (currently not implemented)\n");
    printf("  -x  Include processes not attached to a terminal (currently not implemented)\n");
    printf("  -u  Display the user name associated with each process\n");
}
