#ifndef UTILL_H
#define UTILL_H

#define MAX_CMD_SIZE	(128)
#define BASE_DIR "/private/tmp/test"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int validate_path(char path[MAX_CMD_SIZE]);
char* resolve_path(const char *current_dir, const char *path);

#endif