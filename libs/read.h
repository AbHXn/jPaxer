#ifndef READ_H
#define READ_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "error.h"

#define MAX_READ_SIZE 1024
#define SAFE_SIZE 25
typedef enum _errors READ_ERRORS;

extern unsigned char 	file_reader_buffer[MAX_READ_SIZE + SAFE_SIZE];
extern unsigned char* 	counter;
extern size_t 			byte_read;
extern bool 			HIT_END;

extern const char* _READ_ERR_TYPE;

static const char* read_error_msgs[] = {
	"No error occured\n",
	"JSON reader is null\n",
	"Buffer size is more than provided str size, buffer overflow\n",
	"String is NULL, failed to push\n",
	"Read Error occured\n",
	"Counter is not initialized\n",
	"Buffer is not empty which may overwrite\n",
	"Failed to create Reader object allocation error\n",
	"JSON_FILE/JSON_str is empty\n",
};

typedef enum _errors{
	NO_READ_ERRORS			,
	READ_READER_NULL_ERR	,
	READ_READER_OVERFLOW	,
	READ_READER_UNDERFLOW	,
	READ_PUSH_STRING_NULL	,
	READ_FILE_READ_ERROR	,
	READ_COUNTER_FAILED 	,
	READ_ALLOCATION_ERROR 	,
	READ_SOURCE_FILE_EMTPY	,
	READ_ERR_ENDS
}READ_ERRORS;

typedef enum {
	FILE_MODE, STR_MODE
}READER_TYPE;

typedef struct _reader{
	READER_TYPE reader_type;
	union{
		FILE* 		   JSON_FILE;
		unsigned char* JSON_STR;
	} reader;
}READER;

extern READ_ERRORS READ_ERR_RAISED;

const char* get_read_error_msg( READ_ERRORS err_raised );
READ_ERRORS get_read_error( void );

bool 		push_string 	 	  ( const char* ) ;
bool 		reverse_pointer  	  ( size_t reverse_size ) ;
bool 		push_char_buffer	  ( const char s ) ;
void  		load_next_buffer 	  ( size_t x ) ;
int  		_getc				  ( void ) ;
void 		strip 				  ( char * ) ;
char*       get_string_some_range (size_t, int*);
void 		flush_buffer		  ( void );


READER* create_reader_for_str( const char* json_str );
READER* create_reader_for_FILE( FILE* json_file );

#endif