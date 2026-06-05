#ifndef __MINISHELL_H__
#define __MINISHELL_H__

#include <unistd.h>
#include <stdio.h>

#define MAX_COMMAND_LENGTH 255

void execute_command(char* cmd);

void miniShell();

#endif