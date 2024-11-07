#ifndef COMMANDFUNC_H
#define COMMANDFUNC_H

#define MAX_CMD_SIZE    (128)

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
#include "util.h"

void cmd_ls();
void cmd_chmod(char *perm_str, char *filename);
void cmd_cat(char* current_dir, char *tok_str);
void cmd_help();
void cmd_cd(char* current_dir, char *tok_str);
void cmd_mkdir(char* current_dir, char *tok_str);
void cmd_rmdir(char* current_dir, char *tok_str);
void cmd_rename(char* current_dir, char *tok_str);
void cmd_ln(char* current_dir, char *tok_str);
void cmd_rm(char* current_dir, char *tok_str);
void cmd_cp(char* current_dir, char *tok_str);

#endif