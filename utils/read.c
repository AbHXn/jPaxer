#include "read.h"
#include "error.h"

READ_ERRORS READ_ERR_RAISED  = NO_READ_ERRORS;
READER* 	JSON_READER_OBJECT = NULL;
const char* _READ_ERR_TYPE = "Read Error: ";

unsigned char 	file_reader_buffer[MAX_READ_SIZE + SAFE_SIZE];
unsigned char* 	counter 	= NULL;
unsigned char*  end_ptr		= NULL;

size_t 	byte_read 	= 0;

const char* get_read_error_msg( READ_ERRORS err_raised ){
	if ( err_raised >= NO_READ_ERRORS && err_raised < READ_ERR_ENDS ) {
        const char *msg = read_error_msgs[ err_raised ];
        const char *prefix = _READ_ERR_TYPE;
       	return get_full_error_msg( prefix, msg );
    }
    return NULL;
}

READ_ERRORS get_read_error( void ){
	READ_ERRORS err_raised = READ_ERR_RAISED;
	READ_ERR_RAISED = NO_READ_ERRORS;
	return err_raised;
}

char* get_string_some_range( size_t range, int *error_ptr ){
	if( !counter || byte_read == 0 || !error_ptr )
		return NULL;

	ptrdiff_t total_read_left = counter - file_reader_buffer;

	if( total_read_left > range )
		total_read_left = range;

	*error_ptr = 0;
	size_t idx_ctr = 0;
	
	char* error_found_string = _malloc( total_read_left + 1 );
	unsigned char* start = counter - total_read_left;

	while( start != counter ){
		if ( *start == '\n' )
			*error_ptr = 0;
		else (*error_ptr)++;
		error_found_string[ idx_ctr++ ] = *start++;
	}

	error_found_string[idx_ctr] = '\0';
	return error_found_string;
}

bool push_char_buffer( const char s ){
	if( counter == file_reader_buffer ){
		READ_ERR_RAISED = READ_READER_UNDERFLOW;
		return false;
	}
	*--counter = s;
	return true;
}

bool push_string( const char* str ){
	if( !counter ){
		READ_ERR_RAISED = READ_COUNTER_FAILED;
		return false;
	}
	if( !str ){
		READ_ERR_RAISED = READ_PUSH_STRING_NULL;
		return false;
	}
	const size_t str_size = strlen( str );
	const char*	temp_ptr  = str;
	ptrdiff_t avai_size   = counter - file_reader_buffer;

	if( str_size == 0 ) return true;

	if(	avai_size < ( ptrdiff_t ) str_size ){
		READ_ERR_RAISED = READ_READER_OVERFLOW;
		return false;
	}
	str += str_size - 1;
	while( str != temp_ptr )
		*--counter = *str--;
	*--counter = *str;
	return true;
}

void load_next_buffer( size_t size ) {
	if( !JSON_READER_OBJECT ){
		READ_ERR_RAISED = READ_READER_NULL_ERR;
		return;
	}
	if( JSON_READER_OBJECT->reader_type == FILE_MODE ){
		
		FILE* JSON_FILE = JSON_READER_OBJECT->reader.JSON_FILE;
		size_t read_size = ( size != 0 ) ? size: MAX_READ_SIZE;
		byte_read = fread( file_reader_buffer + SAFE_SIZE, 1, read_size, JSON_FILE );

		if( byte_read == 0 && ferror(JSON_FILE) )
			READ_ERR_RAISED = READ_FILE_READ_ERROR;
	}
	else {
		unsigned char* json_str = JSON_READER_OBJECT->reader.JSON_STR;
		size_t sbyte_read = strlcpy( file_reader_buffer + SAFE_SIZE, json_str, MAX_READ_SIZE );
	
		byte_read = ( sbyte_read >= MAX_READ_SIZE ? MAX_READ_SIZE - 1 : sbyte_read );

		JSON_READER_OBJECT->reader.JSON_STR += byte_read;
	}
	counter = file_reader_buffer + SAFE_SIZE;
	end_ptr = file_reader_buffer + SAFE_SIZE + byte_read;
}

void flush_buffer( void ){
	counter 		   = NULL;
	byte_read 		   = 0;
	READ_ERR_RAISED    = NO_READ_ERRORS;
	JSON_READER_OBJECT = NULL;
	memset( file_reader_buffer, 0, MAX_READ_SIZE + SAFE_SIZE );
}

READER* create_reader_for_str( const char* json_str ){
	if( !json_str ){
		fprintf(stderr, "JSON file is null\n" );
		return NULL;
	}
	READER* reader_obj = _malloc( sizeof( READER ) );
	if( !reader_obj )
		return NULL;

	reader_obj->reader_type 	= STR_MODE;

	char* json_cpy = _malloc( strlen( json_str ) + 1 );
	if( !json_cpy ) return NULL;
	strcpy( json_cpy, json_str );

	reader_obj->reader.JSON_STR = json_cpy;
	return reader_obj;
}

READER* create_reader_for_FILE( FILE* json_file ){
	if( !json_file ){
		fprintf(stderr, "JSON file is null\n" );
		return NULL;
	}
	READER* reader_obj = _malloc( sizeof( READER ) );
	if( !reader_obj )
		return NULL;

	reader_obj->reader_type 	= FILE_MODE;
	reader_obj->reader.JSON_FILE = json_file;
	return reader_obj;
}