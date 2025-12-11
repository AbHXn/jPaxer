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

#include "../utils/alloc.h"
#include "../utils/read.h"
#include "../utils/stack.h"
#include "object.h"


#define ERROR(file, fmt, ...) fprintf(file, fmt, ##__VA_ARGS__)


// buffer is int to track of EOF
extern int 		BUFFER[BUFFER_SIZE];
extern int 		buffer_ptr;
extern READER* 	JSON_READER_OBJECT;


typedef enum {
	NO_FLUSH_HAPPENDED,
	COMMA_FLUSH		  ,
	CLOSSING_FLUSH    ,
}FLUSH_TYPE;


typedef enum{
	KEY_ENTERING  = 1,
	VALUE_LICENSE = 2,
	VALUE_ENTERING= 3,
	KEY_VALUE_PAIR= 4
}STATE_FLAGS;


typedef struct _env{
	JSON_NODE* json_node;
	int 	   FLAG;
	int 	   list_index;
	char 	   key[ MAX_KEY_SIZE ];
	char 	   value[ MAX_VALUE_SIZE ];
	size_t	   k_index;
	size_t	   v_index;
	bool 	   is_string_value;
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

STACK_DATA*  get_stack_data_node ( JSON_NODE* j_data )							 ;
bool 		 add_J_NODE_to_jobject ( JSON_NODE** j_node, JSON_NODE* push_node )	 ;
JSON_NODE* 	 get_full_JSON_NODE_pair_from_env ( WORKING_ENV* env, bool )		 ;
bool 		 FLUSH_THE_STACK ( WORKING_ENV** env, STACK** dfs_stack )			 ;
JSON_NODE* 	 parse_JSON_from_FILE ( FILE*  )									 ;


JSON_NODE* parse_JSON_from_str ( const char* json_str );
JSON_NODE* parse_JSON_from_FILE( FILE* json_file );

static void PRINT_SYNTAX_ERROR( size_t line_number, int c_char ){
	int ptr = 0;
	const char* err_string = get_string_some_range( ERR_BUFF_SIZE, &ptr );
	fprintf(stderr, "Json Syntax Error at %ld: \n", line_number);
	if( err_string && strlen(err_string) > 0 ){
		ERROR(stderr, "\033[1;31m%s\033[0m\n", err_string);
		while( --ptr > 0 ) putchar(' ');
	}
	else ERROR(stderr, "\033[1;31m%c\033[0m\n", c_char);
	puts("^");
}
#endif // JSON_PARSER_H