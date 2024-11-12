#ifndef UTILL_H
#define UTILL_H

#define MAX_DIR_SIZE    (128)
#define MAX_CMD_SIZE	(128)
#define BASE_DIR "/private/tmp/test"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int validate_path(char path[MAX_CMD_SIZE]);
char* resolve_path(const char *current_dir, const char *path);
int check_null_pointer(const char *ch);

#endif