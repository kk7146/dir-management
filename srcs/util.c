#include "util.h"

int validate_path(char *path) { // 유효한 위치인지 확인.
    char absolute_path[MAX_CMD_SIZE];
    realpath(path, absolute_path);
    if (strncmp(BASE_DIR, absolute_path, strlen(BASE_DIR)) != 0)
        return 0;
    return 1;
}

char* resolve_path(const char *current_dir, const char *path) { // path를 받고 유효한 위치로 변환해서 리턴.
    char *resolved_path = (char*)malloc(MAX_CMD_SIZE);  // 메모리 할당
    if (resolved_path == NULL) {
        perror("malloc failed");
        return NULL;
    }
    if (path[0] == '/')
        snprintf(resolved_path, MAX_CMD_SIZE, "%s%s", BASE_DIR, path);
    else
        snprintf(resolved_path, MAX_CMD_SIZE, "%s/%s", current_dir, path);
    if (!validate_path(resolved_path))
    {
        free(resolved_path);
        return (char*)malloc(1);
    }
    return resolved_path;
}
