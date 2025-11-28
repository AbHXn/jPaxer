#include "object.h"
#include <assert.h>
#include <stdio.h>

/* ANYTHING BEYOND ITS LIMIT IS CONSIDER AS STRING */
OBJECT_ERRORS OBJECT_ERR_RAISED = NO_OBJECT_ERRORS;
const char* _OBJ_ERR_TYPE = "Object Error: ";

const char* get_object_error_msg( OBJECT_ERRORS err_raised ){
	if (err_raised >= NO_OBJECT_ERRORS && err_raised < OBJECT_ERR_ENDS) {
        const char *msg = object_err_msgs[err_raised];
        const char *prefix = _OBJ_ERR_TYPE;
       	return get_full_error_msg( prefix, msg );
    }
    return NULL;
}

OBJECT_ERRORS get_object_error( void ){
	OBJECT_ERRORS err_raised = OBJECT_ERR_RAISED;
	OBJECT_ERR_RAISED = NO_OBJECT_ERRORS;
	return err_raised;
}

static bool parse_long_safe( const char* str, long* out ){
    if ( !str || !*str ) 
    	return false;
    
    bool negative = false;
    if ( *str == '-' ) { 
    	negative = true; 
    	str++; 
    }
    else if ( *str == '+' ) 
    	str++; 
    long result = 0;
    
    while ( *str ) {
        if ( *str < '0' || *str > '9' ) 
        	return false; 
        int digit = *str - '0';
        // if multiply by 10 will it overflow??
        if ( !negative && result > (LONG_MAX - digit) / 10 ) 
        	return false;
        if ( negative && -result < (LONG_MIN + digit) / 10 ) 
        	return false;

        result = result * 10 + digit;
        str++;
    }
    *out = negative ? -result : result;
    return true;
}

static inline JSON_DTYPE test_integer(const char* str, long int** save_ptr) {
    if ( !str || *str == '\0' ) return J_SKIP;

    long val;
    if( !parse_long_safe ( str, &val ) ) return J_SKIP;

	long *temp = _malloc( sizeof( long ) );
	if( !temp ){
		OBJECT_ERR_RAISED = OBJECT_ALLOCATION_ERROR;
		return J_SKIP;
	}

	*temp = val;
	*save_ptr = temp;
	return J_INT;
}

static inline JSON_DTYPE test_double(const char* str, double** save_ptr) {
    if ( !str || *str == '\0' ) return J_SKIP;

    char* endptr;
    double val = strtod(str, &endptr);

    if ( *endptr != '\0' || isnan( val ) || isinf( val ) )
    	return J_SKIP; 

   	double* temp = _malloc( sizeof( double ) );
   	if( !temp ) {
   		OBJECT_ERR_RAISED = OBJECT_ALLOCATION_ERROR;
   		return J_SKIP;
   	}
   	*temp = val;
   	*save_ptr = temp;
    return J_DOUBLE;
}

static inline JSON_DTYPE test_bool(const char* str, bool** save_ptr) {
    if ( !str ) return J_SKIP;

    if ( strcmp(str, "true") == 0 || strcmp(str, "false") == 0 ) {
        bool val = strcmp( str, "true" ) == 0;
       	bool* temp = _malloc( sizeof( bool ) );
    	if( !temp ) {
    		OBJECT_ERR_RAISED = OBJECT_ALLOCATION_ERROR;
    		return J_SKIP;
    	}
    	*temp = val;
    	*save_ptr = temp;
    	return J_BOOL;
    }
    return J_SKIP;
}

JSON_DTYPE test_null(const char* str) {
    if (str && strcmp(str, "null") == 0) {
        return J_NULL;
    }
    return J_SKIP;
}

void*  get_string( const char* str ){
	if( !str ) return NULL;

	char* string_value = _malloc( strlen( str ) + 1 );
	if( string_value == NULL ){
		OBJECT_ERR_RAISED = OBJECT_ALLOCATION_ERROR;
		return NULL;
	}
	strcpy( string_value, str );
	return string_value;
}

void* get_dtype( const char* value, JSON_DTYPE* type ){
	if( !value ){
		OBJECT_ERR_RAISED = OBJECT_VALUE_IS_EMPTY;
		return NULL;
	}
	long int*  int_val 		= NULL;
	bool*  	   bool_val 	= NULL;
	double*    double_val 	= NULL;
	
	*type = J_SKIP;
	
	if ( (*type = test_integer( value, &int_val )) !=  J_SKIP)
		return (void *) int_val;
	else if( (*type = test_bool( value, &bool_val ) )!=  J_SKIP)
		return (void *) bool_val;
	else if((*type = test_double( value, &double_val ) )!=  J_SKIP)
		return (void *) double_val;
	return NULL;
}

JSON_DTYPE non_returned_dtype ( const char *str ){
	JSON_DTYPE json_type;
	if( (json_type = test_null( str )) != J_SKIP )
		return json_type;
	return J_STRING;
}

jobject* get_jobject( JSON_NODE* node ){
	jobject* new_node = _malloc( sizeof( jobject ) );

	if( !new_node ){
		OBJECT_ERR_RAISED = OBJECT_ALLOCATION_ERROR;
		return NULL;
	}
	new_node->j_node = node;
	new_node->next = NULL;
	return new_node;
}

void fill_value_acc( JSON_NODE** new_node, void* value_data, JSON_DTYPE type ){
	if( !new_node || !(*new_node) || !value_data || type == J_SKIP ) 
		return;
	switch( type ){
		case J_DOUBLE:
			(*new_node)->value.double_val 	= (double *) value_data;
			break;
		case J_INT:
			(*new_node)->value.int_val 		= (long int*) value_data;
			break;
		case J_BOOL:
			(*new_node)->value.bool_val 	= (bool *) value_data;
			break;
		case J_STRING:
			(*new_node)->value.string_val 	= (unsigned char*) value_data;
			break;
		case J_NULL:
			memset(&(*new_node)->value, 0, sizeof((*new_node)->value));
			break;
		default: break;
	}
}

JSON_NODE* get_jnode( const char *key ){
	if( !key ){
		OBJECT_ERR_RAISED = OBJECT_KEY_IS_EMPTY;
		return NULL;
	}
	JSON_NODE *nnode = (JSON_NODE *) _malloc( sizeof( JSON_NODE ) );
	if( !nnode ){
		OBJECT_ERR_RAISED = OBJECT_TREE_ALLOC_ERROR;
		return NULL;
	}
	nnode->key = (char *) _malloc( strlen(key) + 1 );
	if( !nnode->key ){
		OBJECT_ERR_RAISED = OBJECT_KEY_VALUE_ALLOC_ERROR;
		//free( nnode );
		return NULL;
	}
	strcpy( nnode->key, key );
	nnode->value.string_val   = NULL;
	nnode->value.object_val   = NULL;
	nnode->value.bool_val	  = NULL;
	nnode->value.double_val   = NULL;
	nnode->value.int_val 	  = NULL;
	nnode->multiple_values 	  = false;
	nnode->dtype 			  = J_SKIP;
	nnode->parent 			  = NULL;

	if( OBJECT_ERR_RAISED == NO_OBJECT_ERRORS )
		return nnode;
	return NULL;
}
