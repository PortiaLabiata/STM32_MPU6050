#pragma once

#include "main.h"
#include <malloc.h>
#include <string.h>

#define CLI_UART huart1
#define MAXLINE 1024
#define CMDS 20
#define MAXARGS 100
#define DELIMITER " "

typedef struct {
    char *cmd;
    void (*fun)(char *argv[]);
} CLI_Cmd_t;

/* Command IO */

void CLI_AddCommand(char *cmd, void (*fun)(char *argv[]));
void CLI_ProcessCommand(void);

/* Basic IO */

HAL_StatusTypeDef println(char *str);
HAL_StatusTypeDef print(char *str);
HAL_StatusTypeDef get_line(char *line);
HAL_StatusTypeDef get_char(char *c);
