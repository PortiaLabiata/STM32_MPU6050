#pragma once
#include "main.h"

#include <stdlib.h>
#include <errno.h>

/* Command handlers */

CLI_Status_t getreg_Handler(int argc, char *argv[]);
CLI_Status_t setreg_Handler(int argc, char *argv[]);
