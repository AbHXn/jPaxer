#ifndef DISPLAY_H
#define DISPLAY_H

#include "object.h"
#include <stdio.h>

#define C_RESET   "\033[0m"
#define C_KEY     "\033[1;36m"
#define C_STRING  "\033[1;32m"
#define C_NUMBER  "\033[1;33m"
#define C_BOOL    "\033[1;35m"
#define C_NULL    "\033[1;31m"
#define C_BRACE   "\033[1;34m"

void print_JSON_node( JSON_NODE* root );

#endif 