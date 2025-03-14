#include "cli.h"

static CLI_Cmd_t __cmds[CMDS];
static uint32_t __cmds_num = 0;

/* System functions */

void CLI_AddCommand(char *cmd, void (*fun)(char *argv[]))
{
    CLI_Cmd_t command_struct;
    command_struct.cmd = cmd;
    command_struct.fun = fun;
    __cmds[__cmds_num++] = command_struct;
}

void CLI_ProcessCommand(void)
{
    static char line[MAXLINE];
    get_line(line);
    char *argv[MAXARGS];
    int argc = 0;
    argv[argc++] = strtok(line, DELIMITER);
    while ((argv[argc++] = strtok(NULL, DELIMITER)) != NULL && argc <= MAXARGS) ;

    for (int i = 0; i < CMDS; i++) {
        if (strcmp(__cmds[i].cmd, argv[0]) == 0) {
            __cmds[i].fun(argv);
            return;
        }
    }
    println("Error: command not found");
} 

/* Basic IO */

HAL_StatusTypeDef println(char *str)
{
    char *aux = malloc(sizeof(char) * strlen(str) + 1);
    strcpy(aux, str);
    strcat(aux, "\n");
    HAL_StatusTypeDef status = print(aux);
    free(aux);
    return status;
}

HAL_StatusTypeDef print(char *str)
{
    return HAL_UART_Transmit(&CLI_UART, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

HAL_StatusTypeDef get_line(char *line)
{
    char *c = line;
    HAL_StatusTypeDef status;

    status = get_char(c++);
    if (status != HAL_OK) return status;

    while (c - line + 1 < MAXLINE) {
        status = get_char(c);
        if (status != HAL_OK) return status;
        if (*c == '\n') {
            c--;
            break;
        }

        if (*c != '\b') c++;
        else c--;
    }
    *c = '\0';
    return HAL_OK;
}

HAL_StatusTypeDef get_char(char *c) 
{
    HAL_StatusTypeDef status = HAL_UART_Receive(&CLI_UART, (uint8_t*)c, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) return status;
    return HAL_UART_Transmit(&CLI_UART, (uint8_t*)c, 1, HAL_MAX_DELAY);
}