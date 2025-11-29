#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#define 	MAX_KEY_SIZE 	 250
#define 	MAX_VALUE_SIZE	 1024
#define 	MAX_KEY_DIGIT	 10
#define 	BUFFER_SIZE 	 250
#define 	MAX_SPACE 		 100
#define 	ERR_BUFF_SIZE    20

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "alloc.h"
#include "read.h"
#include "object.h"
#include "stack.h"

typedef enum status_flags INPUT_STATUS;

#define is_key_filled(flag) 	  ((flag) & KEY_ENTERED)
#define is_value_filled(flag) 	  ((flag) & VALUE_ENTERED)
#define is_in_filling_mode(flag)  ((flag) & SOMETHING_INPUTING)
#define is_list_filling(flag) 	  ((flag) & LIST_INPUTING)
#define is_value_licensed(flag)	  ((flag) & VALUE_LICENSE)
#define is_string_inputing(flag)  ((flag) & STRING_INPUTING)
#define turn_on(flag, mask)		  ((flag) | (mask))
#define turn_off(flag, mask)	  ((flag) & ~(mask))

#define ERROR(file, fmt, ...) fprintf(file, fmt, ##__VA_ARGS__)


// buffer is int to track of EOF
extern int 		BUFFER[BUFFER_SIZE];
extern int 		buffer_ptr;
extern FILE* 	JSON_FILE;

typedef enum {
	OPEN_C 		= '{',
	CLOSE_C		= '}',
	OPEN_S 		= '[',
	CLOSE_S 	= ']',
	APPO 		= '"',
	SEMI	 	= ':',
	COMMA 		= ','
}J_SYNTAX;

typedef enum status_flags{
	SOMETHING_INPUTING		= 1 << 0,
	VALUE_ENTERED			= 1 << 1,
	KEY_ENTERED 			= 1 << 2,
	LIST_INPUTING			= 1 << 3,
	STRING_INPUTING			= 1 << 4,
	VALUE_LICENSE			= 1 << 5,
}INPUT_STATUS;

typedef struct _env{
	JSON_NODE* json_node;
	int 	   FLAG;
	int 	   list_index;
	char 	   key[ MAX_KEY_SIZE ];
	char 	   value[ MAX_VALUE_SIZE ];
	size_t	   k_index;
	size_t	   v_index;
}WORKING_ENV;

typedef struct stack_data{
	size_t list_index;
	int    FLAG;
	JSON_NODE* j_data;
}STACK_DATA;

typedef enum _parser_error{
	NO_PARSER_ERROR			,
	PARSER_INTERNAL_ERROR	,
	PARSER_SYNTAX_ERROR		,
	PARSER_ALLOCATION_ERROR	,
	PARSER_EXTERNAL_ERROR	,
	PARSER_STACK_LIMIT_EXEEDED,
}PARSER_ERROR;

extern PARSER_ERROR PARSER_ERR_RAISED;

STACK_DATA*  get_stack_data ( JSON_NODE* j_data )								 ;
bool 		 add_J_NODE_to_jobject ( JSON_NODE** j_node, JSON_NODE* push_node )	 ;
JSON_NODE* 	 get_full_JSON_NODE_pair_from_env ( WORKING_ENV* env )				 ;
bool 		 FLUSH_THE_STACK ( WORKING_ENV** env, STACK** dfs_stack )			 ;
JSON_NODE* 	 parse_JSON_from_FILE ( FILE*  )									 ;
void 		 add_parent_info_from_stack( JSON_NODE** new_node, STACK* dfs_stack );

static void PRINT_SYNTAX_ERROR( size_t line_number, int c_char ){
	int ptr = 0;
	const char* err_string = get_string_some_range( ERR_BUFF_SIZE, &ptr );
	fprintf(stderr, "Json Syntax Error at %ld: \n", line_number);
	if( strlen(err_string) > 0 ){
		ERROR(stderr, "\033[1;31m%s\033[0m\n", err_string);
		while( --ptr > 0 ) putchar(' ');
	}
	else ERROR(stderr, "\033[1;31m%c\033[0m\n", c_char);
	puts("^");
}
#endif // JSON_PARSER_H