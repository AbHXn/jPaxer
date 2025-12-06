#ifndef OBJECT_H
#define OBJECT_H

#define 	MAX_KEY_SIZE 	 250
#define 	MAX_VALUE_SIZE	 1024

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "libs/alloc.h"
#include "libs/error.h"

typedef enum _dtype{
	J_SKIP	 ,
	J_STRING ,
	J_OBJECT ,
	J_BOOL	 ,
	J_NULL	 ,
	J_DOUBLE ,
	J_INT	 ,
}JSON_DTYPE;

typedef struct _jobject{
	struct _jnode*	  j_node;
	struct _jobject*  next;
}jobject;

typedef struct _jnode{
	char*		key;
	JSON_DTYPE  dtype;
	union {
		char*     string_val;
		double*   double_val;
		long* 	  int_val;
		bool* 	  bool_val;
		jobject*  object_val;
	}value;
	bool multiple_values;
	struct _jnode* parent;
}JSON_NODE;

typedef enum _jobject_errors{
	NO_OBJECT_ERRORS				,
	OBJECT_ALLOCATION_ERROR			,
	OBJECT_KEY_VALUE_ALLOC_ERROR	,
	OBJECT_KEY_IS_EMPTY				,
	OBJECT_VALUE_IS_EMPTY			,
	OBJECT_TREE_ALLOC_ERROR			,
	OBJECT_ERR_ENDS		
}OBJECT_ERRORS;

typedef enum {
	OPEN_C 		= '{',
	CLOSE_C		= '}',
	OPEN_S 		= '[',
	CLOSE_S 	= ']',
	APPO 		= '"',
	SEMI	 	= ':',
	COMMA 		= ','
}J_SYNTAX;


extern const char* _OBJ_ERR_TYPE;
extern OBJECT_ERRORS OBJECT_ERR_RAISED;

const char* get_object_error_msg( OBJECT_ERRORS err_raised );
OBJECT_ERRORS get_object_error( void );


static const char* object_err_msgs[] = {
	"no error raised\n",
	"json object allocation failed\n",
	"key value of json object allocation error\n",
	"json object key is null\n",
	"json object values is null\n",
	"json object tree allocation error\n"
};

void* 		get_dtype( const char* , JSON_DTYPE* );
JSON_NODE*  get_jnode( const char * );
JSON_DTYPE	non_returned_dtype( const char * );
void 		fill_value_acc( JSON_NODE** , void* , JSON_DTYPE  );
bool 		is_json_syntax( char c );

void* 		get_J_INT		( long int data );
void* 		get_J_DOUBLE	( double data );
void* 		get_J_BOOL	( bool data );
void* 		get_J_STRING	( char* data );
jobject* 	get_jobject( JSON_NODE*  );

#endif