#include "commandfunc.h"

static void apply_symbolic_permissions(char *permissions, mode_t *mode) {
    int user_flag = 0, group_flag = 0, other_flag = 0;
    char *perm = permissions;
    
    if (*perm == 'u') { user_flag = 1; perm++; }
    else if (*perm == 'g') { group_flag = 1; perm++; }
    else if (*perm == 'o') { other_flag = 1; perm++; }
    else { user_flag = group_flag = other_flag = 1; }

    int add = (*perm == '+') ? 1 : (*perm == '-') ? 0 : -1;
    perm++;
    
    while (*perm) {
        if (*perm == 'r') {
            if (user_flag) *mode = add ? (*mode | S_IRUSR) : (*mode & ~S_IRUSR);
            if (group_flag) *mode = add ? (*mode | S_IRGRP) : (*mode & ~S_IRGRP);
            if (other_flag) *mode = add ? (*mode | S_IROTH) : (*mode & ~S_IROTH);
        } else if (*perm == 'w') {
            if (user_flag) *mode = add ? (*mode | S_IWUSR) : (*mode & ~S_IWUSR);
            if (group_flag) *mode = add ? (*mode | S_IWGRP) : (*mode & ~S_IWGRP);
            if (other_flag) *mode = add ? (*mode | S_IWOTH) : (*mode & ~S_IWOTH);
        } else if (*perm == 'x') {
            if (user_flag) *mode = add ? (*mode | S_IXUSR) : (*mode & ~S_IXUSR);
            if (group_flag) *mode = add ? (*mode | S_IXGRP) : (*mode & ~S_IXGRP);
            if (other_flag) *mode = add ? (*mode | S_IXOTH) : (*mode & ~S_IXOTH);
        }
        perm++;
    }
}

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

static void cat_func(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("cat");
        return;
    }
    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }
    fclose(file);
}

static void chmod_func(char *perm_str, char *filename) {
    struct stat statbuf;
    mode_t mode;

    if (stat(filename, &statbuf) != 0) {
        perror("stat");
        return;
    }
    mode = statbuf.st_mode;

    if (perm_str[0] >= '0' && perm_str[0] <= '7') {

        mode_t new_mode = strtol(perm_str, NULL, 8);
        if (chmod(filename, new_mode) != 0) {
            perror("chmod");
        } else {
            printf("Permissions changed to %o for %s\n", new_mode, filename);
        }
    } else {

        apply_symbolic_permissions(perm_str, &mode);
        if (chmod(filename, mode) != 0) {
            perror("chmod");
        } else {
            printf("Permissions changed for %s\n", filename);
        }
    }
}

static void cp_func(const char *source, const char *destination) {
    FILE *src_file = fopen(source, "r");
    if (src_file == NULL) {
        perror("cp (source)");
        return;
    }
    FILE *dest_file = fopen(destination, "w");
    if (dest_file == NULL) {
        perror("cp (destination)");
        fclose(src_file);
        return;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes, dest_file);
    }
    fclose(src_file);
    fclose(dest_file);
    printf("File copied from %s to %s\n", source, destination);
}

void cmd_ls() {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    struct passwd *pw;
    struct group *gr;
    char timebuf[64];
    char link_target[MAX_DIR_SIZE + 1];

    dir = opendir(".");
    if (dir == NULL) {
        perror("ls");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (lstat(entry->d_name, &statbuf) == -1) {
            perror("lstat");
            continue;
        }
        print_permissions(statbuf.st_mode);
        printf(" %ld ", (long)statbuf.st_nlink);
        pw = getpwuid(statbuf.st_uid);
        gr = getgrgid(statbuf.st_gid);
        printf("%s %s ", pw->pw_name, gr->gr_name);
        printf("%5lld ", (long long)statbuf.st_size);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_atime));
        printf("Access: %s ", timebuf);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_mtime));
        printf("Modify: %s ", timebuf);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_ctime));
        printf("Create: %s ", timebuf);
        printf("%s", entry->d_name);
        if (S_ISLNK(statbuf.st_mode)) {
            ssize_t len = readlink(entry->d_name, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                printf(" -> %s", link_target);
            }
        }
        printf("\n");
    }
    closedir(dir);
}

void cmd_help() {
    printf("Available commands:\n");
    printf("help                   : Show this help message\n");
    printf("cd <path>              : Change directory to <path>\n");
    printf("mkdir <path>           : Create a new directory at <path>\n");
    printf("rmdir <path>           : Remove the directory at <path>\n");
    printf("rename <source> <target> : Rename <source> to <target>\n");
    printf("ln <original> <new>    : Create a hard link from <new> to <original>\n");
    printf("ln -s <original> <new> : Create a symbolic link from <new> to <original>\n");
    printf("chmod <perm> <file>    : Change <file> permissions, e.g., 0644 or u+rwx\n");
    printf("cat <filename>         : Display the contents of <filename>\n");
    printf("cp <source> <dest>     : Copy <source> file to <dest>\n");
    printf("rm <file>              : Remove the specified <file>\n");
    printf("ls                     : List contents of the current directory with details\n");
    printf("quit                   : Exit the shell\n");
}

void cmd_cd(char* current_dir, char *tok_str) {
    char * new_path;

    tok_str = strtok(NULL, " \n");
    if (tok_str == NULL)
    {
        printf("cd: missing argument\n");
        return ;
    }
    if (tok_str[0] == '-')
    {
        if (chdir(new_path) != 0)
            perror("cd");
        return ;
    }
    new_path = resolve_path(current_dir, tok_str);
    if (chdir(new_path) != 0)
        perror("cd");
    free(new_path);
}

void cmd_mkdir(char* current_dir, char *tok_str) {
    char * new_path;

    tok_str = strtok(NULL, " \n");
    if (tok_str == NULL)
    {
        printf("mkdir: missing argument\n");
        return ;
    }
    new_path = resolve_path(current_dir, tok_str);
    if (mkdir(new_path, 0777) != 0)
        printf("mkdir: cannot create directory\n");
    free(new_path);
}

void cmd_rmdir(char* current_dir, char *tok_str) {
    char * new_path;

    tok_str = strtok(NULL, " \n");
    if (tok_str == NULL)
    {
        printf("rmdir: missing argument\n");
        return ;
    }
    new_path = resolve_path(current_dir, tok_str);
    if (rmdir(new_path) != 0)
        printf("rmdir: failed to remove\n");
    free(new_path);
}

void cmd_rename(char* current_dir, char *tok_str) {
    char *src = strtok(NULL, " \n");
    char *dest = strtok(NULL, " \n");
    char * new_src;
    char * new_dest;

    if (src == NULL || dest == NULL) // argument 없는 것 처리
    {
        printf("rename: missing source or target argument\n");
        return ;
    }
    
    new_src = resolve_path(current_dir, src); // 일단 실패하면 null 반환
    if (check_null_pointer(new_src)) // malloc 실패 처리
        return ;
    new_dest = resolve_path(current_dir, dest);
    if (check_null_pointer(current_dir)) // malloc 실패 처리.
    {
        free(new_src);
        printf("rename: failed to rename\n");
        return ;
    }
    if (rename(new_src, new_dest) != 0)
        printf("rename: failed to rename\n");
    free(new_src);
    free(new_dest);
}

void cmd_rm(char* current_dir, char *tok_str) {
    char *file = strtok(NULL, " \n");
    char * new_path;

    if (file == NULL)
    {
        printf("rm: missing argument\n");
        return ;
    }
    new_path = resolve_path(current_dir, tok_str);
    if (unlink(new_path) != 0)
        perror("rm");
    else
        printf("File removed: %s\n", file);
    free(new_path);
}

void cmd_chmod(char* current_dir, char *tok_str) {
    char *perm_str = strtok(NULL, " \n");
    char *filename = strtok(NULL, " \n");
    char * new_path;
    
    if (perm_str == NULL || filename == NULL)
    {
        printf("chmod: missing permission or filename argument\n");
        return ;
    }
    new_path = resolve_path(current_dir, tok_str);
    chmod_func(perm_str, new_path);
    free(new_path);
}

void cmd_cat(char* current_dir, char *tok_str) {
    char *filename = strtok(NULL, " \n");
    char * new_path;

    if (filename == NULL)
    {
        printf("cat: missing filename argument\n");
        return ;
    }
    new_path = resolve_path(current_dir, tok_str);
    cat_func(filename);
    free(new_path);
}

void cmd_cp(char* current_dir, char *tok_str) {
    char *src = strtok(NULL, " \n");
    char *dest = strtok(NULL, " \n");
    char * new_src;
    char * new_dest;

    if (src == NULL || dest == NULL)
    {
        printf("cp: missing source or destination argument\n");
        return ;
    }
    new_src = resolve_path(current_dir, src); // 일단 실패하면 null 반환
    if (check_null_pointer(new_src)) // malloc 실패 처리
        return ;
    new_dest = resolve_path(current_dir, dest);
    if (check_null_pointer(current_dir)) // malloc 실패 처리.
    {
        free(new_src);
        printf("rename: failed to rename\n");
        return ;
    }
    cp_func(new_src, new_dest);
    free(new_src);
    free(new_dest);
}

void cmd_ln(char* current_dir, char *tok_str) {
    char *option = strtok(NULL, " \n");
    char *original = NULL;
    char *new_link = NULL;
    char * new_src;
    char * new_dest;

    if (option && strcmp(option, "-s") == 0) {
        original = strtok(NULL, " \n");
        new_link = strtok(NULL, " \n");

        if (original == NULL || new_link == NULL)
        {
            printf("ln: missing original or new link argument\n");
            return ;
        }
        new_src = resolve_path(current_dir, original); // 일단 실패하면 null 반환
        if (check_null_pointer(new_src)) // malloc 실패 처리
            return ;
        new_dest = resolve_path(current_dir, new_link);
        if (check_null_pointer(current_dir)) // malloc 실패 처리.
        {
            free(new_src);
            return ;
        }
        else if (symlink(new_src, new_dest) != 0)
            perror("ln -s");
        else
            printf("Symbolic link created: %s -> %s\n", new_link, original);
    } else {
        original = option;
        new_link = strtok(NULL, " \n");
        if (original == NULL || new_link == NULL)
        {
            printf("ln: missing original or new link argument\n");
            return ;
        }
        new_src = resolve_path(current_dir, original); // 일단 실패하면 null 반환
        if (check_null_pointer(new_src)) // malloc 실패 처리
            return ;
        new_dest = resolve_path(current_dir, new_link);
        if (check_null_pointer(current_dir)) // malloc 실패 처리.
        {
            free(new_src);
            return ;
        }
        if (link(new_src, new_dest) != 0)
            perror("ln");
        else
            printf("Hard link created: %s -> %s\n", new_link, original);
    }
    free(new_src);
    free(new_dest);
}